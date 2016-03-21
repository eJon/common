// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/appctx.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "log.h"
#include "mem_status.h"
#include "ctx_global.h"
#include "os/os.h"
#include "appctx.h"


NSPIO_DECLARATION_START

extern spio_mem_status_t spio_mem_stats;
static mem_stat_t *app_context_mem_stats = &spio_mem_stats.app_context;

AppCtx::AppCtx() :
    stopping(true), attr(0), now_time(0), first_no_dispatchers(0),
    debugmode(0), inited(0), lbp(NULL), poller(NULL), rgm(NULL),
    iothreadpool(NULL), astat(APP_MODULE_STATITEM_KEYRANGE, &__appctx_sm)
{
    tmutex_init(&conf_lock);
    INIT_LIST_HEAD(&err_roles);
    INIT_LIST_LINK(&apps_link);
    app_context_mem_stats->alloc++;
    app_context_mem_stats->alloc_size += sizeof(AppCtx);
}

AppCtx::~AppCtx() {
    tmutex_destroy(&conf_lock);
    if (poller)
	delete poller;
    if (lbp)
	delete lbp;
    app_context_mem_stats->alloc--;
    app_context_mem_stats->alloc_size -= sizeof(AppCtx);
}

int AppCtx::attach_to_apps_head(struct list_head *head) {
    if (!apps_link.linked) {
	apps_link.linked = 1;
	list_add(&apps_link.node, head);
	return 0;
    }
    return -1;
}

int AppCtx::detach_from_apps_head() {
    if (apps_link.linked) {
	list_del(&apps_link.node);
	apps_link.linked = 0;
	return 0;
    }
    return -1;
}

    
string AppCtx::Id() {
    return appid;
}

int AppCtx::SetDebugMode(int mode) {
    debugmode = mode ? 1 : 0;
    return 0;
}

int AppCtx::Init(const string &id) {
    CtxConf default_conf;

    default_conf.appid = id;
    return InitFromConf(&default_conf);
}

int AppCtx::Update(CtxConf *app_conf) {
    tmutex_lock(&conf_lock);
    nconf = *app_conf;
    tmutex_unlock(&conf_lock);
    return 0;
}

int AppCtx::InitFromConf(CtxConf *app_conf) {

    if (inited) {
	NSPIOLOG_WARN("appid %s is inited", appid.c_str());
	errno = SPIO_EINTERN;
	return -1;
    }
    if (!(poller = EpollCreate(10240, app_conf->epoll_io_events))) {
	NSPIOLOG_ERROR("poller init errno: %d", errno);
	return -1;
    }
    if (!(lbp = MakeLBAdapter(app_conf->msg_balance_algo))) {
	delete poller;
	NSPIOLOG_ERROR("make loadbalance adapter with errno %d", errno);
	return -1;
    }
    if (app_conf)
	conf = *app_conf;
    appid = conf.appid;
    __appctx_sm.setup(appid);
    init_appctx_module_stat_trigger(&astat, conf.app_trigger_level);
    rom.Init(appid, &conf);
    mqp.Setup(appid, &conf);
    inited = 1;
    return 0;
}


int AppCtx::Stop() {
    stopping = true;
    if (rgm)
	rgm->RemoveHandler(&rom);
    return 0;
}

int AppCtx::Start() {
    pid_t pid = gettid();
    int64_t last_update = rt_mstime(), healthcheck = rt_mstime();

    if (rgm && rgm->InsertHandler(&rom) < 0)
	return -1;
    stopping = false;
    first_no_dispatchers = last_update;
    NSPIOLOG_NOTICE("app %s start ok with pid %d", cid(), pid);
    while (!stopping) {
	now_time = rt_mstime();
	astat.update_timestamp(now_time);

	process_massage();

	// Each conf.monitor_record_stats_msec time, flush stats
	if (now_time - last_update > conf.monitor_record_stats_msec) {
	    if (debugmode) {
		system("clear");
		process_console_show(-1);
	    }
	    record_appctx_status_from_module_stat();
	    last_update = now_time;
	}
	if (now_time - healthcheck > conf.role_healthcheck_msec) {
	    broadcast_role_status_icmp();
	    healthcheck = now_time;
	}
	if (nconf.mtime > conf.mtime)
	    process_conf_update();

	process_roles_register();
    }
    NSPIOLOG_NOTICE("app %s stop ok with pid %d", cid(), pid);
    return 0;
}


int AppCtx::Running() {
    return stopping ? false : true;
}

int AppCtx::StopThread() {
    bool __running = Running();
    Stop();
    if (__running)
	thread.Stop();
    return 0;
}

static int appctx_thread_worker(void *arg_) {
    AppCtx *ctx = (AppCtx *)arg_;
    return ctx->Start();
}
    
int AppCtx::StartThread(TaskPool *tp) {
    iothreadpool = tp;
    if (stopping)
	thread.Start(appctx_thread_worker, this);
    return 0;
}    


}
