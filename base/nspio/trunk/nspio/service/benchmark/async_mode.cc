// copyright:
//            (C) SINA Inc.
//
//           file: nspio/service/benchmark/async_mode.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <nspio/errno.h>
#include "os/time.h"
#include "log.h"
#include "base/crc.h"
#include "sync/pspinlock.h"
#include "runner/thread.h"
#include "os/epoll.h"
#include "async_api.h"
#include "benchmark.h"
#include "module_stats.h"
#include "benchmark_modstat.h"


using namespace nspio;

class lmodule_stat : public module_stat {
public:
    lmodule_stat() :
	module_stat(BENCHMARK_MODULE_STAT_KEYRANGE, &mstrigger)
    {
	pspin_init(&__lock);
    }
    ~lmodule_stat() {
	pspin_destroy(&__lock);
    }

    void lock() {
	pspin_lock(&__lock);
    }
    void unlock() {
	pspin_unlock(&__lock);
    }
    
private:
    pspin_t __lock;
    benchmark_module_stat_trigger mstrigger;
};


struct MyHdr {
    int64_t timestamp;
    uint16_t checksum;
};
#define MYHDRLEN sizeof(struct MyHdr)

class my_request_handler : public RequestHandler {
public:
    my_request_handler() {}
    ~my_request_handler() {}
    int HandleRequest(const char *data, uint32_t len, ResponseWriter &rw);
};

int my_request_handler::HandleRequest(const char *data, uint32_t len, ResponseWriter &rw) {
    rw.Send(data, len);
    return 0;
}


class my_response_handler : public ResponseHandler {
public:
    my_response_handler(lmodule_stat *s) :
	stat(s)
    {}
    ~my_response_handler() {}
    int HandleError(Error &ed, void *cb);
    int HandleResponse(const char *data, uint32_t len, void *cb);

private:
    lmodule_stat *stat;
};

int my_response_handler::HandleError(Error &ed, void *cb) {
    stat->incrkey(BC_DELIVER_ERROR);
    NSPIOLOG_ERROR("async deliver with %s", ed.Str().c_str());
    return 0;
}

int my_response_handler::HandleResponse(const char *data, uint32_t len, void *cb) {
    int error = 0;
    MyHdr *hdr = (MyHdr *)data;
    const char *__data = data + MYHDRLEN;
    int __len = len - MYHDRLEN;

    if (g_check == "yes" && hdr->checksum != crc16(__data, __len))
	error = 1;
    stat->lock();
    if (error)
	stat->incrkey(BC_CHECKSUM_ERROR);
    stat->incrkey(BC_RECVPKG);
    stat->incrkey(BC_RTT, rt_mstime() - hdr->timestamp);
    stat->unlock();
    return 0;
}

static volatile int _stopping = 0;

int nspio_async_server(void *arg_) {
    my_request_handler mrh;
    async_conf conf;
    __async_comsumer *asio = new __async_comsumer();

    if (!asio)
	return -1;
    conf.set(appname, nspiosvrhost, g_servers, 1000 * g_servers, 100);
    asio->Setup(conf, &mrh);
    asio->StartServe();
    while (!_stopping)
	usleep(1000);
    asio->Stop();
    delete asio;
    return 0;
}

struct async_env {
    __async_producer *asio;
    lmodule_stat *async_stat;
};


int async_send_worker(void *arg_) {
    async_env *env = (struct async_env *)(arg_);
    __async_producer *asio = env->asio;
    lmodule_stat *stat = env->async_stat;
    MyHdr hdr = {};
    int len = 0, i;
    int64_t start_tt = 0, end_tt = 0, sleep_tt = 0, one_s = 0;    

    one_s = start_tt = rt_mstime();
    end_tt = start_tt + g_time;
    sleep_tt = 1000000 / g_freq;
    while (rt_mstime() <= end_tt) {
	for (i = 0; i < g_pkgs; i++) {
	    len = rand() % (g_size - MYHDRLEN) + MYHDRLEN;
	    hdr.timestamp = rt_mstime();
	    if (g_check == "yes")
		hdr.checksum = crc16(buffer, len);
	    memcpy(buffer, &hdr, MYHDRLEN);
	    if (asio->SendRequest(buffer, len, NULL, 10) == 0) {
		stat->lock();
		stat->incrkey(BC_SENDPKG);
		stat->unlock();
	    }
	}
	rt_usleep(sleep_tt);
    }
    return 0;
}


int nspio_async_client(void *arg_) {
    async_env env = {};
    async_conf conf;
    lmodule_stat async_stat;
    __async_producer *asio = NULL;
    my_response_handler mrh(&async_stat);
    Thread *thread = NULL;
    vector<Thread *> send_workers;
    vector<Thread *>::iterator it;
    int64_t end_tt = rt_mstime() + g_time;

    conf.set(appname, nspioclihost, g_clients, 1000 * g_clients, 0);
    async_stat.batch_set_threshold(1);
    asio = new __async_producer();
    if (!asio)
	return -1;
    asio->Setup(conf, &mrh);
    asio->StartServe();

    env.asio = asio;
    env.async_stat = &async_stat;
    for (int i = 0; i < g_clients; i++) {
	thread = new Thread();
	send_workers.push_back(thread);
	thread->Start(async_send_worker, &env);
    }
    while (rt_mstime() < end_tt) {
	usleep(1000);
	async_stat.lock();
	async_stat.update_timestamp(rt_mstime());
	async_stat.unlock();
    }
    for (int i = 0; i < g_clients; i++) {
	thread = send_workers.at(i);
	thread->Stop();
    }
    asio->Stop();
    _stopping = 1;

    delete asio;
    return 0;
}
