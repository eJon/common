// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/appctx_conf.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "log.h"
#include "mem_status.h"
#include "appctx.h"


NSPIO_DECLARATION_START

static int __update_role_trigger_level(Role *r, void *data) {
    CtxConf *conf = (CtxConf *)data;
    MQueue *mq = r->Queue();

    init_mq_module_stat_trigger(mq->Stat(), conf->mq_trigger_level);
    init_role_module_stat_trigger(r->Stat(), conf->role_trigger_level);
    return 0;
}

int AppCtx::process_conf_update() {
    string roleid;
    CtxConf __nconf;
    map<string, string>::iterator it;

    tmutex_lock(&conf_lock);
    __nconf = nconf;
    tmutex_unlock(&conf_lock);
    if (!__nconf.mtime || __nconf.mtime <= conf.mtime)
	return 0;
    NSPIOLOG_NOTICE("app: %s update config", cid());
    conf.mtime = __nconf.mtime;
    
#define __update_app_config(field)		\
    if (__nconf.field > 0)			\
	conf.field = __nconf.field;

    __update_app_config(msg_queue_size);
    __update_app_config(msg_balance_factor);
    __update_app_config(msg_timeout_msec);
    __update_app_config(epoll_timeout_msec);
    __update_app_config(monitor_record_stats_msec);
    __update_app_config(reconnect_timeout_msec);
#undef __update_app_config

    // close some removed other apps connect
#define __remove_connectto_app(t)					\
    for (it = conf.t##_apps.begin(); it != conf.t##_apps.end(); ) {	\
	if (__nconf.raddr_exist(it->first) == true) {			\
	    it++;							\
	    continue;							\
	}								\
	roleid = it->second;						\
	conf.insert_removed(roleid);					\
	conf.t##_apps.erase(it++);					\
    }

    __remove_connectto_app(at);
    __remove_connectto_app(wt);
    __remove_connectto_app(inat);
#undef __remove_connectto_app

    process_removed_app();

    // conf.at_apps + conf.wt_apps;
    for (it = __nconf.inat_apps.begin(); it != __nconf.inat_apps.end(); ++it) {
	if (conf.raddr_exist(it->first) == true)
	    continue;
	conf.insert_inactive(it->first, it->second);
    }
    if (__nconf.app_trigger_level != conf.app_trigger_level) {
	init_appctx_module_stat_trigger(&astat, __nconf.app_trigger_level);
	__nconf.app_trigger_level = conf.app_trigger_level;
    }

    if (__nconf.role_trigger_level != conf.role_trigger_level
	|| __nconf.mq_trigger_level != conf.mq_trigger_level) {
	rom.Walk(__update_role_trigger_level, &__nconf);
	conf.role_trigger_level = __nconf.role_trigger_level;
	conf.mq_trigger_level = __nconf.mq_trigger_level;
    }
    return 0;
}
    











}
