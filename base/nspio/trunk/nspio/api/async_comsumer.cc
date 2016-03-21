// copyright:
//            (C) SINA Inc.
//
//           file: nspio/api/async_comsumer.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <string.h>
#include "os/time.h"
#include "os/memalloc.h"
#include "proto/icmp.h"
#include "async_api.h"



NSPIO_DECLARATION_START


static int callbackworker(void *arg_) {
    __async_comsumer *mux = (__async_comsumer *)arg_;
    return mux->intern_callbackworker();
}

static void muxs_waitqueue_cleanup_func(struct appmsg *msg) {
    free_appmsg_rt(msg->rt);
    free_appmsg(msg);
}
    
AsyncComsumer *NewAsyncComsumer() {
    __async_comsumer *mux = new (std::nothrow) __async_comsumer();
    return mux;
}

__async_comsumer::__async_comsumer() :
    ash(NULL)
{
}

__async_comsumer::~__async_comsumer() {
}

inline struct appmsg *__async_comsumer::pop_req() {
    struct appmsg *msg = NULL;

    req_queue.Lock();
    msg = req_queue.Pop();
    req_queue.UnLock();
    return msg;
}

inline int __async_comsumer::push_req(struct appmsg *req) {
    bool new_task(false);
    struct appmsg *tmp = NULL, *icmp = NULL;

    req_queue.Lock();
    if (wg.Ref() < conf.max_workers)
	new_task = true;
    while (req_queue.Push(req) == -1) {
	if ((tmp = req_queue.Pop()) != NULL) {
	    if ((icmp = make_deliver_status_icmp(tmp, SPIO_EQUEUEFULL)) != NULL
		&& push_resp(icmp) < 0)
		muxs_waitqueue_cleanup_func(icmp);
	    muxs_waitqueue_cleanup_func(tmp);
	}
    }
    req_queue.UnLock();
    if (new_task) {
	wg.Add();
	astp->run_task(callbackworker, this);
    }
    return 0;
}

inline struct appmsg *__async_comsumer::pop_resp() {
    bool __disable(false);
    struct appmsg *resp = NULL;

    resp_queue.Lock();
    if (resp_queue.Empty())
	__disable = true;
    else
	resp = resp_queue.Pop();
    if (__disable)
	try_disable_event(EPOLLOUT);
    resp_queue.UnLock();
    return resp;
}


inline int __async_comsumer::push_resp(struct appmsg *resp) {
    bool __enable(false);
    int ret = 0;

    resp_queue.Lock();
    if (resp_queue.Empty())
	__enable = true;
    if ((ret = resp_queue.Push(resp)) == 0 && __enable == true)
	try_enable_event(EPOLLOUT);
    resp_queue.UnLock();
    return ret;
}

int __async_comsumer::Setup(async_conf &__conf, RequestHandler *h, Transport *tp) {
    if (inited) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (!h) {
	errno = EINVAL;
	return -1;
    }
    conf = __conf;
    conf.max_workers = conf.max_workers <= 0 ? 1 : conf.max_workers;
    conf.queue_cap = conf.queue_cap <= 0 ? 20000 : conf.queue_cap;
    conf.max_trip_time = conf.max_trip_time <= 0 ? 0 : conf.max_trip_time;
    req_queue.Setup(conf.queue_cap, muxs_waitqueue_cleanup_func);
    resp_queue.Setup(conf.queue_cap, muxs_waitqueue_cleanup_func);
    while (__connect_to_nspiosvr() < 0)
	usleep(10);
    ash = h;
    if (tp)
	astp = (async_transport *)tp;
    inited = 1;
    return 0;
}

int __async_comsumer::connect_error() {
    internsvr.Close();
    astp->disable_eventpoll(this);
    if (connect_to_nspiosvr() == 0)
	astp->enable_eventpoll(this);
    return 0;
}


int __async_comsumer::__connect_to_nspiosvr() {

    if (internsvr.Connect(conf.appname, conf.apphost) < 0)
	return -1;
    ee.SetEvent(internsvr.Fd(), EPOLLIN|EPOLLOUT|EPOLLRDHUP, this);
    stats.incrkey(ASAPI_RECONNECT);
    internsvr.SetOption(OPT_SOCKCACHE);
    internsvr.SetOption(OPT_NONBLOCK, 1);
    return 0;
}

int __async_comsumer::connect_to_nspiosvr() {
    int ret = 0;
    while (!astp->stopping() && (ret = __connect_to_nspiosvr()) < 0)
	usleep(10);
    return ret;
}



int __async_comsumer::recv_massage(int max_recv) {
    int ret = 0, cnt = 0;
    bool istimeout = false;
    struct appmsg pkg = {}, *req = NULL, *icmp = NULL;

    while (max_recv > 0) {
	if ((ret = internsvr.Recv(&pkg)) < 0)
	    break;
	if (!(istimeout = __package_is_timeout(&pkg, conf.max_trip_time))
	    && ((req = (struct appmsg *)mem_zalloc(sizeof(*req))) != NULL)) {
	    memcpy(req, (char *)&pkg, sizeof(pkg));
	    push_req(req);
	} else {
	    if (istimeout) {
		if ((icmp = make_deliver_status_icmp(&pkg, SPIO_ETIMEOUT)) != NULL
		    && push_resp(icmp) < 0)
		    muxs_waitqueue_cleanup_func(icmp);
	    }
	    mem_free(pkg.rt);
	    if (pkg.s.len > 0)
		mem_free(pkg.s.data, pkg.s.len);
	}
	max_recv--;
	cnt++;
	stats.incrkey(ASAPI_RECVPACKAGES);
	stats.incrkey(ASAPI_RECVBYTES, pkg.s.len);
	memset((char *)&pkg, 0, sizeof(pkg));
    }
    if (ret < 0 && errno != EAGAIN) {
	stats.incrkey(ASAPI_RECVERRORS);
	connect_error();
    }
    return cnt;
}

int __async_comsumer::send_massage(int max_send) {
    int ret = 0, cnt = 0;
    uint32_t snd_bytes = 0;
    struct appmsg *resp = NULL;

    while (max_send > 0 && (resp = pop_resp()) != NULL) {
	cnt++;
	max_send--;
	snd_bytes = resp->s.len + appmsg_rtlen(resp->rt->ttl);
	if ((ret = internsvr.Send(resp)) == 0) {
	    stats.incrkey(ASAPI_SENDBYTES, snd_bytes);
	    stats.incrkey(ASAPI_SENDPACKAGES);
	} else if (ret < 0 && errno != EAGAIN) {
	    stats.incrkey(ASAPI_SENDERRORS);
	    connect_error();
	}
	muxs_waitqueue_cleanup_func(resp);
    }
    if (cnt > 0)
	internsvr.FlushCache();
    return 0;
}

int __async_comsumer::handle_events() {
    int rcachesize = internsvr.CacheSize();

    if (!ee.happened && rcachesize <= 0)
	return -1;
    if ((ee.happened & EPOLLIN) || rcachesize > 0)
	recv_massage(conf.max_workers * 10);
    if (ee.happened & EPOLLOUT)
	send_massage(conf.max_workers);
    if (ee.happened & (EPOLLRDHUP|EPOLLERR))
	connect_error();
    ee.happened = 0;
    return 0;
}


static inline string build_route(struct appmsg *req) {
    string rt;
    rt.append((char *)&req->hdr, sizeof(req->hdr));
    rt.append((char *)req->rt, appmsg_rtlen(req->rt->ttl));
    return rt;
}


int ResponseWriter::Send(const char *data, uint32_t len) {
    __async_comsumer *ascr = (__async_comsumer *)async_comsumer;
    return ascr->SendResponse(data, len, route);
}


int __async_comsumer::intern_callbackworker() {
    struct appmsg *req = NULL;
    ResponseWriter rw;

    while (!astp->stopping()) {
	if ((req = pop_req()) == NULL)
	    break;
	rw.route = build_route(req);
	rw.async_comsumer = this;
	ash->HandleRequest(req->s.data, req->s.len, rw);
	muxs_waitqueue_cleanup_func(req);
    }
    wg.Done();
    return 0;
}

int __async_comsumer::Stop() {
    if (!astp) {
	errno = SPIO_EINTERN;
	return -1;
    }
    astp->stop();
    wg.Wait();
    astp->del(this);
    delete astp;
    astp = NULL;
    return 0;
}


int __async_comsumer::StartServe() {
    if (!astp) {
	astp = new async_transport();
	astp->Setup(conf.max_workers + 1);
    }
    if (!astp->stopping()) {
	errno = SPIO_EDUPOP;
	return -1;
    }
    astp->add(this);
    astp->start();
    return 0;
}

int __async_comsumer::SendResponse(const char *data, int len, const string &rt) {
    struct appmsg *resp = NULL;

    if (!astp || astp->stopping()) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (len < 0) {
	errno = EINVAL;
	return -1;
    }
    if (!(resp = new_appmsg(data, len))) {
	errno = ENOMEM;
	return -1;
    }
    resp->hdr = *(struct spiohdr *)rt.data();
    if (!(resp->rt = appmsg_rtdup((struct appmsg_rt *)(rt.data() + sizeof(resp->hdr))))) {
	free_appmsg(resp);
	errno = ENOMEM;
	return -1;
    }
    if (push_resp(resp) < 0) {
	muxs_waitqueue_cleanup_func(resp);
	errno = SPIO_EQUEUEFULL;
	return -1;
    }
    return 0;
}




}
