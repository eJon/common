// copyright:
//            (C) SINA Inc.
//
//           file: nspio/service/nspiod/monitor.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include <monitor/client/client_agent.h>
#include "log.h"
#include "os/os.h"
#include "appctx.h"
#include "ctx_global.h"

using namespace std;
using namespace nspio;

extern SpioConfig spio_conf;
ctx_stat_recorder *monitor_client = NULL;

class my_appctx_monitor : public ctx_stat_recorder {
public:
    my_appctx_monitor();
    ~my_appctx_monitor();
    int init(string &monitor_conf);
    int add(string &key, int64_t val);
    
private:
    int inited;
    monitor::ClientAgent *intern_monitor;
};


my_appctx_monitor::my_appctx_monitor() :
    inited(0), intern_monitor(NULL)
{

}

my_appctx_monitor::~my_appctx_monitor() {
    if (intern_monitor)
	delete intern_monitor;
}

int my_appctx_monitor::init(string &monitor_conf) {
    monitor::ClientAgent *__monitor = NULL;

    if (inited) {
	errno = SPIO_EINTERN;
	return -1;
    }
    __monitor = new (std::nothrow) monitor::ClientAgent();
    if (!__monitor) {
	errno = ENOMEM;
	return -1;
    }
    if (__monitor->Init(monitor_conf) < 0) {
	delete __monitor;
	return -1;
    }
    intern_monitor = __monitor;
    inited = 1;
    return 0;
}

int my_appctx_monitor::add(string &key, int64_t val) {
    intern_monitor->Add(key, val);
    return 0;
}


int nspiod_start_monitor() {
    pid_t pid = gettid();
    my_appctx_monitor *mymonitor_client = NULL;

    mymonitor_client = new (std::nothrow) my_appctx_monitor();
    if (!mymonitor_client || mymonitor_client->init(spio_conf.monitor) < 0) {
	NSPIOLOG_NOTICE("monitor start error with errno %d", errno);
	delete mymonitor_client;
	mymonitor_client = NULL;
	return -1;
    }
    monitor_client = mymonitor_client;
    set_ctx_global_stat_recorder(mymonitor_client);
    NSPIOLOG_NOTICE("monitor start ok with pid %d", pid);
    return 0;
}


int nspiod_stop_monitor() {
    pid_t pid = gettid();

    if (monitor_client) {
	NSPIOLOG_NOTICE("monitor stop ok with pid %d", pid);
	delete monitor_client;
    }
    monitor_client = NULL;
    return 0;
}
