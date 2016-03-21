// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/appctx_role.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "log.h"
#include "appctx.h"

NSPIO_DECLARATION_START


int AppCtx::EnableRegistry(Rgm *_rgm) {
    rgm = _rgm;
    return 0;
}

int AppCtx::process_single_error(Role *r) {
    int connect_mode = 0;
    string raddr, roleid;
    map<string, string>::iterator it;

    CLEAR_MSGERROR(r);
    r->Reset();
    roleid = r->Id();
    raddr = r->remoteip();
    connect_mode = r->connect_mode();

    // release corresponse appctx resource
    r->ReleaseResource();
    if (IS_DISPATCHER(r->Type()))
	lbp->del(r);

    // TODO: fix here
    // !importance. maybe the same role have been registed in
    // new_roles queue at the same time (ooh ...)
    rom.TimeWait(r);
    
    // Announce death and try reconnect if needed
    if (connect_mode == O_ACTIVE) {
	if ((it = conf.at_apps.find(raddr)) != conf.at_apps.end()) {
	    conf.at_apps.erase(it);
	    conf.insert_inactive(raddr, roleid);
	} else {
	    const char *rip = raddr.c_str();
	    NSPIOLOG_NOTICE("appid %s remote node %s was removed", cid(), rip);
	}
    }

    return 0;
}


int AppCtx::process_roles_error() {
    Role *r = NULL;
    struct rgmh_stats romstat = {};
    struct list_link *pos = NULL, *next = NULL;

    list_for_each_list_link_safe(pos, next, &err_roles) {
	r = list_r(pos);
	r->detach_from_msgerror_head();
	process_single_error(r);
    }
    rom.stats(&romstat);
    if (romstat.at_dispatchers == 0 && first_no_dispatchers == -1)
	first_no_dispatchers = rt_mstime();
    if (!list_empty(&err_roles))
	NSPIOLOG_ERROR("impossible reach here: %d", errno);
    return 0;
}

int AppCtx::process_removed_app() {
    string rid;
    Role *r = NULL;
    set<string>::iterator it;

    for (it = conf.rm_apps.begin(); it != conf.rm_apps.end(); ++it) {
	rid = *it;
	if ((r = rom.Find(rid)) != NULL)
	    process_single_error(r);
    }
    conf.rm_apps.clear();
    return 0;
}


int AppCtx::process_connectto_app() {
    map<string, string>::iterator it;
    
    for (it = conf.inat_apps.begin(); it != conf.inat_apps.end(); ++it) {
	rgm->ConnectTo(it->second, appid, it->first, &rom);
	conf.insert_waiting(it->first, it->second);
    }
    conf.inat_apps.clear();
    return 0;
}


bool AppCtx::is_time_to_close_all_receivers() {
    struct rgmh_stats romstat = {};

    // disable this feature ...
    return false;

    rom.stats(&romstat);
    if (romstat.at_dispatchers == 0
	&& rt_mstime() - first_no_dispatchers >= conf.reconnect_timeout_msec)
	return true;
    return false;
}



int AppCtx::init_r_module_stat_trigger_level(Role *r) {
    MQueue *mq = r->Queue();
    init_mq_module_stat_trigger(mq->Stat(), conf.mq_trigger_level);
    init_role_module_stat_trigger(r->Stat(), conf.role_trigger_level);
    return 0;
}


//  close all receivers when has no dispatchers 
static int __close_receiver(Role *r, void *data) {
    struct list_head *head = (struct list_head *)data;
    r->attach_to_msgerror_head(head);
    return 0;
}

int AppCtx::process_roles_register() {
    Role *r = NULL;
    int connect_mode = 0;
    map<string, string>::iterator it;

    while ((r = rom.PopNewer()) != NULL) {
	// monitor role attr change events
	r->on_rattr_change(&ratm);
	connect_mode = r->connect_mode();

	if (r->InitResource(&mqp, poller, conf.role_timeout_msec) < 0) {
	    NSPIOLOG_ERROR("new role init resource errno: %d", errno);
	    continue;
	}
	init_r_module_stat_trigger_level(r);
	if (connect_mode == O_ACTIVE) {
	    if (conf.raddr_exist(r->remoteip()) == false) {
		process_single_error(r);
		continue;
	    }
	    if ((it = conf.wt_apps.find(r->remoteip())) != conf.wt_apps.end()) {
		conf.wt_apps.erase(it);
		conf.insert_active(r->remoteip(), r->Id());
	    }
	}
	if (IS_DISPATCHER(r->Type()))
	    lbp->add(r);
	if (r->Type() == ROLE_DISPATCHER && first_no_dispatchers != -1)
	    first_no_dispatchers = -1;
    }
    if (is_time_to_close_all_receivers()) {
	rom.WalkReceivers(__close_receiver, &err_roles);
	if (!list_empty(&err_roles)) {
	    first_no_dispatchers = rt_mstime();
	    process_roles_error();
	    NSPIOLOG_ERROR("close all receivers because of no dispatchers");
	}
    }

    process_connectto_app();

    return 0;
}



}
