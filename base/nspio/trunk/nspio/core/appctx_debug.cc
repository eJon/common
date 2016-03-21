// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/appctx_debug.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "log.h"
#include "mem_status.h"
#include "os/os.h"
#include "appctx.h"


NSPIO_DECLARATION_START


int AppCtx::process_console_show(int fd) {
    FILE *fp = NULL;
    pid_t pid = gettid();

    if (fd < 0) {
	fp = stdout;
    } else {
	if (!(fp = fdopen(fd, "w"))) {
	    NSPIOLOG_ERROR("fdopen of with errno %d", errno);
	    return -1;
	}
    }
    fprintf(fp, "\nApp status: %s PID(%d)\n", cid(), pid);
    fprintf(fp, "%10s %10s %10s %10s %10s %10s %10s %10s %10s\n",
	    "name", "pollrin", "pollrout", "polldin", "polldout",
	    "rrcvpkg", "rsndpkg", "drcvpkg", "dsndpkg");

#define __output_app_status(op, name)					\
    fprintf(fp, "%10s %10"PRId64" %10"PRId64" %10"PRId64" %10"PRId64	\
	    " %10"PRId64" %10"PRId64" %10"PRId64" %10"PRId64"\n", name,	\
	    astat.getkey_s(RPOLLIN, op), astat.getkey_s(RPOLLOUT, op),	\
	    astat.getkey_s(DPOLLIN, op), astat.getkey_s(DPOLLOUT, op),	\
	    astat.getkey_s(RRCVPKG, op), astat.getkey_s(RSNDPKG, op),	\
	    astat.getkey_s(DRCVPKG, op), astat.getkey_s(DSNDPKG, op));

    __output_app_status(MIN, "min");
    __output_app_status(MAX, "max");
    __output_app_status(AVG, "avg");
    __output_app_status(NOW, "now");
#undef __output_app_status
    
    rom.OutputRoleStatus(fp);
    fprintf(fp, "\n\n\nMQueue status:\n");
    mqp.OutputQueueStatus(fp);

    return 0;
}



extern ctx_stat_recorder *csr;

static int __record_rolemgr_status(const string &app, const struct rgmh_stats *stat) {
    string key;
    if (!csr)
	return -1;

    key = app + "-AT_RECEIVERS";
    csr->add(key, stat->at_receivers);
    key = app + "-AT_DISPATCHERS";
    csr->add(key, stat->at_dispatchers);
    key = app + "-TW_RECEIVERS";
    csr->add(key, stat->tw_receivers);
    key = app + "-TW_DISPATCHERS";
    csr->add(key, stat->tw_dispatchers);
    key = app + "-NR_RECEIVERS";
    csr->add(key, stat->nr_receivers);
    key = app + "-NR_DISPATCHERS";
    csr->add(key, stat->nr_dispatchers);
    key = app + "RECEIVERS";
    csr->add(key, RECEIVER_COUNTER(stat));
    key = app + "DISPATCHERS";
    csr->add(key, DISPATCHER_COUNTER(stat));
    return 0;
}

extern const char *app_stat_module_item[APP_MODULE_STATITEM_KEYRANGE];
static int __record_appctx_status(const string &app, module_stat *self) {
    string key;
    int i = 0;

    if (!csr)
	return -1;
    for (i = 1; i < APP_MODULE_STATITEM_KEYRANGE; i++) {
	key = app + "-" + app_stat_module_item[i];
	csr->add(key, self->getkey_s(i));
    }
    return 0;
}

extern const char *mq_stat_module_item[MQ_MODULE_STATITEM_KEYRANGE];
static int __record_mq_status(const string &app, module_stat *self) {
    string key;
    int i = 0;

    if (!csr)
	return -1;
    for (i = 1; i < MQ_MODULE_STATITEM_KEYRANGE; i++) {
	key = app + "-" + mq_stat_module_item[i];
	csr->add(key, self->getkey_s(i));
    }
    return 0;
}

extern const char *role_stat_module_item[ROLE_MODULE_STATITEM_KEYRANGE];
static int __record_role_status(const string &app, module_stat *self) {
    string key;
    int i = 0;

    if (!csr)
	return -1;
    for (i = 1; i < ROLE_MODULE_STATITEM_KEYRANGE; i++) {
	key = app + "-" + role_stat_module_item[i];
	csr->add(key, self->getkey_s(i));
    }
    return 0;
}


static int __send_role_status_to_monitor_center(Role *r, void *data) {
    string appid;
    MQueue *mq = r->Queue();
    appid = r->cappid();
    __record_mq_status(appid + "-queue-", mq->Stat());
    __record_role_status(appid + "-role-", r->Stat());
    return 0;
}

int AppCtx::record_appctx_status_from_module_stat() {
    struct rgmh_stats romstat = {};

    rom.stats(&romstat);
    __record_appctx_status(appid + "-ctx-", &astat);
    __record_rolemgr_status(appid + "-rolemgr-", &romstat);
    rom.Walk(__send_role_status_to_monitor_center, NULL);
    return 0;
}




}
