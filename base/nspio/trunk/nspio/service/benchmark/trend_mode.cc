// copyright:
//            (C) SINA Inc.
//
//           file: nspio/service/benchmark/trend_mode.cc
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
#include "sync/pmutex.h"
#include "runner/thread.h"
#include "os/epoll.h"
#include "async_api.h"
#include "benchmark.h"


using namespace nspio;

static int sleeptime = 20;

struct MyHdr {
    int64_t timestamp;
    uint16_t checksum;
};
#define MYHDRLEN sizeof(struct MyHdr)

class trend_request_handler : public RequestHandler {
public:
    trend_request_handler() : drop(0), recv_packages(0)
    {
    }
    ~trend_request_handler() {}
    int HandleRequest(const char *data, uint32_t len, ResponseWriter &rw);
private:
    int drop;
    int recv_packages;
};

int trend_request_handler::HandleRequest(const char *data, uint32_t len, ResponseWriter &rw) {
    int i = 0;
    int64_t ett = 0;
    ett = rt_mstime() + sleeptime;
    while (rt_mstime() < ett)
	i++;
    rw.Send(data, len);
    recv_packages++;
    return 0;
}


class trend_response_handler : public ResponseHandler {
public:
    trend_response_handler() : rtt(0), recv_packages(0), error_packages(0) {
	pmutex_init(&lock);
    }
    ~trend_response_handler() {
	pmutex_destroy(&lock);
    }
    int Recvs() {
	return recv_packages;
    }
    int Rtt() {
	return rtt;
    }
    int HandleError(Error &ed, void *pridata);
    int HandleResponse(const char *data, uint32_t len, void *pridata);

private:
    int rtt;
    int recv_packages, error_packages;
    pmutex_t lock;
};

int trend_response_handler::HandleError(Error &ed, void *pridata) {
    printf("trend error %s\n", ed.Str().c_str());
    return 0;
}

int trend_response_handler::HandleResponse(const char *data, uint32_t len, void *pridata) {
    int error = 0;
    int ett = rt_mstime();
    MyHdr *hdr = (MyHdr *)data;
    const char *__data = data + MYHDRLEN;
    int __len = len - MYHDRLEN;

    if (g_check == "yes" && hdr->checksum != crc16(__data, __len))
	error = 1;
    pmutex_lock(&lock);
    recv_packages++;
    if (error)
	error_packages++;
    rtt += ett - hdr->timestamp;
    pmutex_unlock(&lock);
    return 0;
}

static volatile int _stopping = 0;

int nspio_trend_server(void *arg_) {
    trend_request_handler trh;
    async_conf conf;
    AsyncComsumer *asio = NewAsyncComsumer();

    conf.set(appname, nspiosvrhost, g_servers, g_servers * 200, 100);
    if (!asio)
	return -1;
    asio->Setup(conf, &trh);
    asio->StartServe();
    while (!_stopping)
	sleep(1);
    asio->Stop();
    delete asio;
    return 0;
}


static int trend_send_worker(void *arg_) {
    AsyncProducer *asio = (AsyncProducer *)arg_;
    MyHdr hdr = {};
    int64_t start_tt = 0, end_tt = 0, intern_tt = 0;    
    int len = 0, sendcnt = 0, i;

    start_tt = rt_mstime();
    end_tt = start_tt + g_time;
    intern_tt = 1000000 / g_freq;
    while (rt_mstime() <= end_tt) {
	for (i = 0; i < g_pkgs; i++) {
	    len = rand() % (g_size - MYHDRLEN) + MYHDRLEN;
	    hdr.timestamp = rt_mstime();
	    if (g_check == "yes")
		hdr.checksum = crc16(buffer + MYHDRLEN, len - MYHDRLEN);
	    sendcnt++;
	    memcpy(buffer, &hdr, MYHDRLEN);
	    asio->SendRequest(buffer, len, NULL, 100);
	}
	rt_usleep(intern_tt);
    }
    return sendcnt;
}


int nspio_trend_client(void *arg_) {
    async_conf conf;
    AsyncProducer *asio = NULL;
    trend_response_handler trh;
    int i, sendcnt = 0, recvcnt = 0, lost;
    Thread *thread = NULL;
    vector<Thread *> send_workers;
    vector<Thread *>::iterator it;
    
    conf.set(appname, nspioclihost, g_clients, 20 * g_clients, 0);
    asio = NewAsyncProducer();
    if (!asio)
	return -1;
    asio->Setup(conf, &trh);
    asio->StartServe();

    for (i = 0; i < g_clients; i++) {
	thread = new Thread();
	send_workers.push_back(thread);
	thread->Start(trend_send_worker, asio);
    }
    for (it = send_workers.begin(); it != send_workers.end(); ++it) {
	thread = *it;
	sendcnt += thread->Stop();
	delete thread;
    }
    sleep(2);
    asio->Stop();
    _stopping = 1;

    recvcnt = trh.Recvs();
    lost = sendcnt - recvcnt;
    if (recvcnt)
	printf("client send: %10d recv: %10d rtt: %2d lost: %.6f tp: %d\n",
	       sendcnt, recvcnt, trh.Rtt() / recvcnt,
	       (float)lost / (float)sendcnt, recvcnt * 1000 / g_time);
    delete asio;
    return 0;
}
