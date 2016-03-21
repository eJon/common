// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/role.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "log.h"
#include "role.h"
#include "proto/icmp.h"
#include "os/memalloc.h"



NSPIO_DECLARATION_START

Role::Role(const string &appid, const string &roleid, int rtype) :
    inited(0), roletype(rtype), roleattr(0), rattr_change_h(NULL),
    rstat(ROLE_MODULE_STATITEM_KEYRANGE, &__role_sm), msgev(0), app(appid),
    id(roleid), internconn(NULL), poller(NULL), mqueue(NULL), mqpool(NULL)
{
    memset(&dd_timeout_node, 0, sizeof(dd_timeout_node));
    memset(&loadbalance_data, 0, sizeof(loadbalance_data));
    icmp_snd_queue.Init(1000000);
    icmp_rcv_queue.Init(1000000);
    INIT_LIST_LINK(&rolemgr_node);
    INIT_LIST_LINK(&mqwaiter_node);
    INIT_LIST_LINK(&msgin_node);
    INIT_LIST_LINK(&msgout_node);
    INIT_LIST_LINK(&msgerror_node);
    memset(&tv, 0, sizeof(tv));
}

Role::~Role() {
    struct nspiomsg *icmp_msg = NULL;

    while ((icmp_msg = icmp_snd_queue.Pop()) != NULL)
	mem_free(icmp_msg, MSGPKGLEN(icmp_msg));
    while ((icmp_msg = icmp_rcv_queue.Pop()) != NULL)
	mem_free(icmp_msg, MSGPKGLEN(icmp_msg));
}


int Role::Bind(Conn *conn) {
    if (internconn) {
	errno = SPIO_EINTERN;
	return -1;
    }
    internconn = conn;
    conn->LocalAddr(laddr);
    conn->RemoteAddr(raddr);
    return 0;
}

int Role::unBind(Conn **conn) {
    if (!internconn) {
	errno = SPIO_EINTERN;
	return -1;
    }
    *conn = internconn;
    internconn = NULL;
    return 0;
}

string Role::Id() {
    return id;
}
    
const char *Role::cid() {
    return id.c_str();
}

const char *Role::localip() {
    return laddr.c_str();
}

const char *Role::remoteip() {
    return raddr.c_str();
}

const char *Role::cappid() {
    return app.c_str();
}

uint32_t Role::Type() {
    return roletype;
}
    
Conn *Role::Connect() {
    return internconn;
}

struct iotask_env *Role::IOTask() {
    return &tv;
}

int Role::connect_mode() {
    if (!internconn) {
	errno = SPIO_EINTERN;
	return -1;
    }
    return internconn->OpenMode();
}

int Role::on_rattr_change(rattr_event_change_handler *h) {
    if (!h) {
	errno = EINVAL;
	return -1;
    }
    rattr_change_h = h;
    return 0;
}

int Role::trigger_attr_change_event(int attr, bool enable) {
    if (!rattr_change_h) {
	errno = SPIO_EINTERN;
	return -1;
    }
    switch (attr) {
    case ATTRR:
	if (enable)
	    rattr_change_h->role_enable_r_event(this);
	else
	    rattr_change_h->role_disable_r_event(this);
	break;
    case ATTRW:
	if (enable)
	    rattr_change_h->role_enable_w_event(this);
	else
	    rattr_change_h->role_disable_w_event(this);
	break;
    }
    return 0;
}


uint32_t Role::GetAttr() {
    return roleattr;
}

void Role::SetAttr(uint32_t attr) {
    roleattr = attr;
}

    
uint32_t Role::GetMsgev() {
    return msgev;
}

void Role::SetMsgev(uint32_t ev) {
    msgev = ev;
}

int Role::HandleMsgev() {
    if (ee.happened & EPOLLERR) {
	NSPIOLOG_ERROR("%s %s(%s) EPOLLERR", cappid(), remoteip(), ROLESTR(this));
    } else if (ee.happened & EPOLLTIMEOUT) {
	NSPIOLOG_ERROR("%s %s(%s) EPOLLTIMEOUT", cappid(), remoteip(), ROLESTR(this));
    } else if (ee.happened & EPOLLRDHUP) {
	NSPIOLOG_ERROR("%s %s(%s) EPOLLRDHUP", cappid(), remoteip(), ROLESTR(this));
    }

    msgev = (ee.happened & (EPOLLERR|EPOLLTIMEOUT)) ? MSGERROR : 0;
    if (msgev & MSGERROR)
	return 0;

    msgev |= ((ee.happened & EPOLLIN) && (GetAttr() & ATTRR)) ? MSGIN : 0;
    msgev |= ((ee.happened & EPOLLOUT) && (GetAttr() & ATTRW)) ? MSGOUT : 0;
    msgev |= (ee.happened & EPOLLRDHUP) ? MSGERROR : 0;

    return 0;
}    

int Role::AttachToMsgevHead(struct list_head *in, struct list_head *out, struct list_head *err) {

    if (!in || !out || !err) {
	errno = EINVAL;
	return -1;
    }

    HandleMsgev();
    if (msgev & MSGIN)
	attach_to_msgin_head(in);
    if (msgev & MSGERROR)
	attach_to_msgerror_head(err);
    else if (msgev & MSGOUT)
	attach_to_msgout_head(out);
    return 0;
}    

    
module_stat *Role::Stat() {
    return &rstat;
}

module_stat_trigger *Role::StatTrigger() {
    return &__role_sm;
}


char *Role::LoadBalance_data() {
    return (char *)&loadbalance_data;
}


int Role::resource_stat_module_timestamp_update(int64_t timestamp) {
    module_stat *qstat = NULL;

    rstat.update_timestamp(timestamp);
    if (mqueue) {
	qstat = mqueue->Stat();
	qstat->update_timestamp(timestamp);
    }
    return 0;
}

    
int Role::AttachPoller(Epoller *eplr, int to_msec) {
    int ret = 0;
    if (!eplr || to_msec < 0) {
	errno = SPIO_EINTERN;
	return -1;
    }
    
    ee.ptr = this;
    if (to_msec > 0)
	ee.to_nsec = to_msec * 1000000;
    if ((ee.fd = internconn->Fd()) >= 0)
	ee.events = EPOLLIN|EPOLLRDHUP;
    if ((ret = eplr->CtlAdd(&ee)) == 0)
	poller = eplr;
    return ret;
}


int Role::DetachPoller() {
    int ret;
    if (!poller) {
	errno = SPIO_EINTERN;
	return -1;
    }

    if ((ret = poller->CtlDel(&ee)) == 0)
	poller = NULL;
    return ret;
}


int Role::EnablePOLLIN() {
    int ret = 0;

    if (!poller)
	return -1;
    if (!(ee.events & EPOLLIN)) {
	ee.events |= EPOLLIN;
	ret = poller->CtlMod(&ee);
    }
    return ret;
}

int Role::DisablePOLLIN() {
    int ret = 0;
    if (!poller)
	return -1;

    if (ee.events & EPOLLIN) {
	ee.events &= ~EPOLLIN;
	ret = poller->CtlMod(&ee);
    }
    return ret;
}

int Role::EnablePOLLOUT() {
    int ret = 0;

    if (!poller)
	return -1;
    if (!(ee.events & EPOLLOUT)) {
	ee.events |= EPOLLOUT;
	ret = poller->CtlMod(&ee);
    }
    return ret;
}

int Role::DisablePOLLOUT() {
    int ret = 0;

    if (!poller) 
	return -1;

    if (ee.events & EPOLLOUT) {
	ee.events &= ~EPOLLOUT;
	ret = poller->CtlMod(&ee);
    }
    return ret;
}

int Role::attach_to_mqwaiter_head(struct list_head *head) {
    if (!mqwaiter_node.linked) {
	mqwaiter_node.linked = 1;
	list_add(&mqwaiter_node.node, head);
	return 0;
    }
    return -1;
}

int Role::detach_from_mqwaiter_head() {
    if (mqwaiter_node.linked) {
	list_del(&mqwaiter_node.node);
	mqwaiter_node.linked = 0;
	return 0;
    }
    return -1;
}

int Role::mq_empty_callback_func() {
    return DisablePOLLOUT();
}    

int Role::mq_nonempty_callback_func() {
    return EnablePOLLOUT();
}    

    
int Role::attach_to_rolemgr_head(struct list_head *head) {
    if (!rolemgr_node.linked) {
	rolemgr_node.linked = 1;
	list_add(&rolemgr_node.node, head);
	return 0;
    }
    return -1;
}

int Role::detach_from_rolemgr_head() {
    if (rolemgr_node.linked) {
	list_del(&rolemgr_node.node);
	rolemgr_node.linked = 0;
	return 0;
    }
    return -1;
}


int Role::attach_to_msgin_head(struct list_head *head) {
    if (!msgin_node.linked) {
	msgin_node.linked = 1;
	list_add(&msgin_node.node, head);
	return 0;
    }
    return -1;
}

int Role::detach_from_msgin_head() {
    if (msgin_node.linked) {
	list_del(&msgin_node.node);
	msgin_node.linked = 0;
	return 0;
    }
    return -1;
}



int Role::attach_to_msgout_head(struct list_head *head) {
    if (!msgout_node.linked) {
	msgout_node.linked = 1;
	list_add(&msgout_node.node, head);
	return 0;
    }
    return -1;
}

int Role::detach_from_msgout_head() {
    if (msgout_node.linked) {
	list_del(&msgout_node.node);
	msgout_node.linked = 0;
	return 0;
    }
    return -1;
}


int Role::attach_to_msgerror_head(struct list_head *head) {
    if (!msgerror_node.linked) {
	msgerror_node.linked = 1;
	list_add(&msgerror_node.node, head);
	return 0;
    }
    return -1;
}

int Role::detach_from_msgerror_head() {
    if (msgerror_node.linked) {
	list_del(&msgerror_node.node);
	msgerror_node.linked = 0;
	return 0;
    }
    return -1;
}


int Role::PushSNDICMP(struct nspiomsg *icmp_msg) {
    return icmp_snd_queue.Push(icmp_msg);
}


struct nspiomsg *Role::PopSNDICMP() {
    return icmp_snd_queue.Pop();
}

int Role::PushRCVICMP(struct nspiomsg *icmp_msg) {
    return icmp_rcv_queue.Push(icmp_msg);
}


struct nspiomsg *Role::PopRCVICMP() {
    return icmp_rcv_queue.Pop();
}

MQueue *Role::Queue() {
    return mqueue;
}

int Role::InitResource(MQueuePool *mqp, Epoller *elpr, int to_msec) {
    if (inited) {
	errno = SPIO_EINTERN;
	return -1;
    }
    CLEAR_MSGEV(this);
    if (AttachPoller(elpr, to_msec) < 0) {
	NSPIOLOG_ERROR("%s [%d %s %s] attach poller with errno %d",
		       cappid(), roletype, cid(), remoteip(), errno);
	return -1;
    }
    if (!(mqueue = mqp->Set(id))) {
	if (DetachPoller() < 0)
	    NSPIOLOG_ERROR("%s [%d %s %s] detach poller with errno %d",
			   cappid(), roletype, cid(), remoteip(), errno);
	return -1;
    }
    inited = 1;
    mqpool = mqp;
    SETROLE_RW(this);
    mqueue->AddMonitor(this);
    __role_sm.setup(app, id, raddr);
    return 0;
}

int Role::ReleaseResource() {
    if (!inited) {
	errno = SPIO_EINTERN;
	return -1;
    }

    if (DetachPoller() < 0)
	NSPIOLOG_ERROR("%s [%d %s %s] detach poller with errno %d",
		       cappid(), roletype, cid(), remoteip(), errno);
    UNSETROLE_RW(this);
    mqueue->DelMonitor(this);
    mqpool->unSet(cid());
    mqpool = NULL;
    mqueue = NULL;
    inited = 0;
    return 0;
}    



}
