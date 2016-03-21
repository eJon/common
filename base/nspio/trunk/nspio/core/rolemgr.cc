// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/rolemgr.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "log.h"
#include "rolemgr.h"
#include "receiver.h"
#include "dispatcher.h"
#include "mem_status.h"

NSPIO_DECLARATION_START


extern spio_mem_status_t spio_mem_stats;
static mem_stat_t *rolemanager_mem_stats = &spio_mem_stats.rolemanager;

RoleManager::RoleManager() :
    cfg(NULL)
{
    pmutex_init(&lock);
    memset(&romstat, 0, sizeof(romstat));
    INIT_LIST_HEAD(&receivers);
    INIT_LIST_HEAD(&dispatchers);
    INIT_LIST_HEAD(&newroles);
    INIT_LIST_HEAD(&tw_receivers);
    INIT_LIST_HEAD(&tw_dispatchers);
    skrb_init(&tw_tree);
    rolemanager_mem_stats->alloc++;
    rolemanager_mem_stats->alloc_size += sizeof(RoleManager);
}

RoleManager::~RoleManager() {
    Role *r = NULL;
    struct list_link *pos = NULL, *next = NULL;

#define __cleanup_all_roles(head)			\
    list_for_each_list_link_safe(pos, next, head) {	\
	r = list_r(pos);				\
	r->detach_from_rolemgr_head();			\
	delete r;					\
    }

    __cleanup_all_roles(&newroles);
    __cleanup_all_roles(&receivers);
    __cleanup_all_roles(&tw_receivers);
    __cleanup_all_roles(&dispatchers);
    __cleanup_all_roles(&tw_dispatchers);
#undef __cleanup_all_roles


    
    pmutex_destroy(&lock);
    rolemanager_mem_stats->alloc--;
    rolemanager_mem_stats->alloc_size -= sizeof(RoleManager);
}

int RoleManager::biglock() {
    return pmutex_lock(&lock);
}
    
int RoleManager::unbiglock() {
    return pmutex_unlock(&lock);
}

    
int RoleManager::stats(struct rgmh_stats *info) {
    biglock();
    clean_time_wait_roles();
    *info = romstat;
    unbiglock();
    return 0;
}    

inline Role *RoleManager::pop_new_role() {
    Role *r = NULL;

    if (!list_empty(&newroles)) {
	r = list_first_r(&newroles);
	r->detach_from_rolemgr_head();
	DECR_NR_COUNTER(r, &romstat);
    }
    return r;
}


inline void RoleManager::push_new_role(Role *r) {
    r->attach_to_rolemgr_head(&newroles);
    INCR_NR_COUNTER(r, &romstat);
}


inline Role *RoleManager::find_exist_role(const string rid, uint32_t rtyp) {
    Role *r = NULL;
    struct list_link *pos = NULL;

    list_for_each_list_link(pos, &newroles) {
	r = list_r(pos);
	if (r->Id() == rid && r->Type() == rtyp) {
	    return r;
	}
    }
    list_for_each_list_link(pos, &receivers) {
	r = list_r(pos);
	if (r->Id() == rid && r->Type() == rtyp) {
	    return r;
	}
    }
    list_for_each_list_link(pos, &dispatchers) {
	r = list_r(pos);
	if (r->Id() == rid && r->Type() == rtyp) {
	    return r;
	}
    }
    return NULL;
}



inline void RoleManager::clean_time_wait_roles() {
    skrb_node_t *node = NULL;
    Role *r = NULL;
    int64_t cur_ms = rt_mstime();

    while (!skrb_empty(&tw_tree)) {
	node = skrb_min(&tw_tree);
	if (node->key > cur_ms)
	    break;
	r = (Role *)node->data;
	r->detach_from_rolemgr_head();
	skrb_delete(&tw_tree, node);
	DECR_TW_COUNTER(r, &romstat);
	NSPIOLOG_ERROR("%s destroy time wait role %s(%s) %s",
		       cappid(), r->cid(), ROLESTR(r), r->remoteip());
	delete r;
    }
}


inline void RoleManager::insert_time_wait_role(Role *r) {
    if (IS_RECEIVER(r->Type()))
	r->attach_to_rolemgr_head(&tw_receivers);
    else if (IS_DISPATCHER(r->Type()))
	r->attach_to_rolemgr_head(&tw_dispatchers);
    r->dd_timeout_node.key = cfg->reconnect_timeout_msec + rt_mstime();
    r->dd_timeout_node.data = r;
    skrb_insert(&tw_tree, &r->dd_timeout_node);
    INCR_TW_COUNTER(r, &romstat);
}


inline Role *RoleManager::__find_tw_r(const string rid, struct list_head *head) {
    Role *r = NULL;
    struct list_link *pos = NULL, *next = NULL;
    
    list_for_each_list_link_safe(pos, next, head) {
	r = list_r(pos);
	if (r->Id() == rid)
	    return r;
    }
    return NULL;
}
    
inline Role *RoleManager::find_time_wait_role(const string rid, uint32_t typ) {
    Role *r = NULL;
    if (IS_RECEIVER(typ)) {
	if ((r = __find_tw_r(rid, &tw_receivers)) != NULL) {
	    r->detach_from_rolemgr_head();
	    skrb_delete(&tw_tree, &r->dd_timeout_node);
	}
    } else if (IS_DISPATCHER(typ)) {
	if ((r = __find_tw_r(rid, &tw_dispatchers)) != NULL) {
	    r->detach_from_rolemgr_head();
	    skrb_delete(&tw_tree, &r->dd_timeout_node);
	}
    }
    if (r)
	DECR_TW_COUNTER(r, &romstat);
    return r;
}


Role *RoleManager::PopNewer() {
    Role *r = NULL;
    biglock();
    if ((r = pop_new_role()) != NULL) {
	if (IS_RECEIVER(r->Type()))
	    r->attach_to_rolemgr_head(&receivers);
	else if (IS_DISPATCHER(r->Type()))
	    r->attach_to_rolemgr_head(&dispatchers);
	INCR_AT_COUNTER(r, &romstat);
	active_roles.insert(make_pair(r->Id(), r));
    }
    unbiglock();
    return r;
}


Role *RoleManager::Find(const string &id) {
    Role *r = NULL;
    map<string, Role *>::iterator it;

    biglock();
    if ((it = active_roles.find(id)) != active_roles.end())
	r = it->second;
    unbiglock();
    return r;
}


int RoleManager::__walk_receivers(rwalkfn walkfn, void *data) {
    Role *r = NULL;
    struct list_link *pos = NULL, *next = NULL;

    list_for_each_list_link_safe(pos, next, &receivers) {
	r = list_r(pos);
	walkfn(r, data);
    }
    return 0;
}
    

int RoleManager::__walk_dispatchers(rwalkfn walkfn, void *data) {
    Role *r = NULL;
    struct list_link *pos = NULL, *next = NULL;

    list_for_each_list_link_safe(pos, next, &dispatchers) {
	r = list_r(pos);
	walkfn(r, data);
    }
    return 0;
}


int RoleManager::WalkReceivers(rwalkfn walkfn, void *data) {

    biglock();
    __walk_receivers(walkfn, data);
    unbiglock();
    return 0;
}
    

int RoleManager::WalkDispatchers(rwalkfn walkfn, void *data) {

    biglock();
    __walk_dispatchers(walkfn, data);
    unbiglock();
    return 0;
}
    

int RoleManager::Walk(rwalkfn walkfn, void *data) {

    biglock();
    __walk_receivers(walkfn, data);
    __walk_dispatchers(walkfn, data);
    unbiglock();
    return 0;
}




int RoleManager::TimeWait(Role *r) {
    Conn *internconn = NULL;
    map<string, Role *>::iterator it;

    if (!r) {
	NSPIOLOG_NOTICE("%s unexpect null error role", cappid());
	return -1;
    }
    r->unBind(&internconn);
    delete internconn;

    biglock();
    clean_time_wait_roles();
    DECR_AT_COUNTER(r, &romstat);
    r->detach_from_rolemgr_head();
    if ((it = active_roles.find(r->Id())) != active_roles.end())
	active_roles.erase(it);
    else
	NSPIOLOG_ERROR("crit here of %s %s", r->cid(), r->remoteip());

    if (cfg->reconnect_timeout_msec == 0) {
	NSPIOLOG_NOTICE("%s role %s(%s) %s destroy now",
			cappid(), r->cid(), ROLESTR(r), r->remoteip());
	delete r;
    } else {
	NSPIOLOG_NOTICE("%s role %s(%s) %s enter time_wait",
			cappid(), r->cid(), ROLESTR(r), r->remoteip());
	insert_time_wait_role(r);
    }
    unbiglock();
    return 0;
}

bool RoleManager::reg_keep_session(const struct spioreg *rgh) {
    Role *r = NULL;
    bool keep = false;
    string rid;

    if (!rgh) {
	errno = EINVAL;
	return -1;
    }
    biglock();
    rgh_getid(rgh, rid);
    if ((r = __find_tw_r(rid, &tw_receivers)) != NULL ||
	(r = __find_tw_r(rid, &tw_dispatchers)) != NULL)
	keep = true;
    unbiglock();
    return keep;
}


int RoleManager::Register(struct spioreg *rgh, Conn *conn) {
    Role *r = NULL;
    Receiver *rb;
    Dispatcher *db;
    string rid, raddr;
    module_stat *rstat = NULL;

    if (!(rgh->rtype & (ROLE_RECEIVER|ROLE_DISPATCHER)) || !conn ||
	strlen(rgh->appname) == 0 || strlen(rgh->appname) > MAX_APPNAME_LEN) {
	NSPIOLOG_NOTICE("%s unexpect rgh[type:%u appname:%s conn:%p]", cappid(),
			rgh->rtype, rgh->appname, conn);
	return -1;
    }
    biglock();
    conn->SetSockOpt(SO_READCACHE, 0);
    conn->SetSockOpt(SO_WRITECACHE, 0);
    rgh_getid(rgh, rid);
    if ((r = find_exist_role(rid, rgh->rtype)) != NULL) {
	conn->RemoteAddr(raddr);
	NSPIOLOG_NOTICE("%s found new register %s [%d:%s] conflict of an exist role [%s:%s]",
			cappid(), rid.c_str(), rgh->rtype, raddr.c_str(), ROLESTR(r), r->remoteip());
	unbiglock();
	return -1;
    }
    if ((r = find_time_wait_role(rid, rgh->rtype)) != NULL) {
	// A reconnect connection
	r->Bind(conn);
	push_new_role(r);
	rstat = r->Stat();
	rstat->incrkey(RECONNECT);
	NSPIOLOG_NOTICE("%s found an time_wait role %s(%s) %s",
			cappid(), r->cid(), ROLESTR(r), r->remoteip());
	unbiglock();
	return 0;
    }

    if (IS_RECEIVER(rgh->rtype)) {
	if ((rb = new (std::nothrow) Receiver(rgh->rtype, appid, rid)) != NULL) {
	    rb->Bind(conn);
	    push_new_role(rb);
	    NSPIOLOG_NOTICE("%s register new receiver %s %s",
			    cappid(), rb->cid(), rb->remoteip());
	} else {
	    NSPIOLOG_ERROR("%s out of memory when register %s(r)", cappid(), rid.c_str());
	}
    } else if (IS_DISPATCHER(rgh->rtype)) {
	if ((db = new (std::nothrow) Dispatcher(rgh->rtype, appid, rid)) != NULL) {
	    db->Bind(conn);
	    push_new_role(db);
	    NSPIOLOG_NOTICE("%s register new dispatcher %s %s",
			    cappid(), db->cid(), db->remoteip());
	} else {
	    NSPIOLOG_ERROR("%s out of memory when register %s(d)", cappid(), rid.c_str());
	}
    }
    unbiglock();
    return 0;
}




// Output all role status
int RoleManager::OutputRoleStatus(FILE *fp) {

    Role *r = NULL;
    string uid;
    int64_t rtt = 0;
    module_stat *rstat = NULL;
    module_stat_trigger *tg = NULL;
    struct list_link *pos = NULL;

    biglock();
    fprintf(fp, "\n\nRole status: \n");
    fprintf(fp,
	    "%20s %15s %10s %10s %10s %10s %10s %10s %10s %10s %10s\n",
	    "host", "uuid", "reconn", "rcv", "snd", "rcvpkg",
	    "sndpkg", "rcverr", "snderr", "chkerr", "rtt");			
    fprintf(fp, "-----------------------------------------------------\n");

#define __output_role_status(head, t)					\
    list_for_each_list_link(pos, head) {				\
	r = list_r(pos);						\
	rstat = r->Stat();						\
	tg = r->StatTrigger();						\
	uid.clear();							\
	uid.assign(r->cid(), 8);					\
	uid.append(t);							\
	rtt = 0;							\
	if (rstat->getkey_s(RECV_PACKAGES)) {				\
	    if (IS_RECEIVER(r->Type()))					\
		rtt = rstat->getkey_s(RRTT)				\
		    / rstat->getkey_s(RECV_PACKAGES);			\
	    else if (IS_DISPATCHER(r->Type()))				\
		rtt = rstat->getkey_s(DRTT)				\
		    / rstat->getkey_s(RECV_PACKAGES);			\
	}								\
	fprintf(fp, "%20s %15s"						\
		" %10"PRId64" %10"PRId64" %10"PRId64" %10"PRId64	\
		" %10"PRId64" %10"PRId64" %10"PRId64" %10"PRId64	\
		" %10"PRId64"\n",					\
		r->remoteip(), uid.c_str(),				\
		tg->getlast_s(RECONNECT),				\
		tg->getlast_s(RECV_BYTES),				\
		tg->getlast_s(SEND_BYTES),				\
		tg->getlast_s(RECV_PACKAGES),				\
		tg->getlast_s(SEND_PACKAGES),				\
		tg->getlast_s(RECV_ERRORS),				\
		tg->getlast_s(SEND_ERRORS),				\
		tg->getlast_s(CHECKSUM_ERRORS), rtt);			\
    }

    __output_role_status(&receivers, "(r)");
    __output_role_status(&dispatchers, "(d)");

    __output_role_status(&tw_receivers, "(tw r)");
    __output_role_status(&tw_dispatchers, "(tw d)");
    fprintf(fp, "----------------------------------------------------\n");
#undef __output_role_status

    unbiglock();
    return 0;
}    


}
