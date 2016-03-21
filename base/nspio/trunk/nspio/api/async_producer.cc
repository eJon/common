// copyright:
//            (C) SINA Inc.
//
//           file: nspio/api/async_producer.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <string.h>
#include "os/memalloc.h"
#include "async_api.h"
#include "ed.h"

using namespace std;

NSPIO_DECLARATION_START


AsyncProducer *NewAsyncProducer() {
    __async_producer *mux = new (std::nothrow) __async_producer();
    return mux;
}

static int callbackworker(void *arg_) {
    __async_producer *mux = (__async_producer *)arg_;
    return mux->intern_callbackworker();
}

__async_producer::__async_producer() :
    ach(NULL)
{
}
    
__async_producer::~__async_producer() {
}

inline struct appmsg *__async_producer::pop_req() {
    bool __disable(false);
    struct appmsg *req = NULL;

    req_queue.Lock();
    if (req_queue.Empty())
	__disable = true;
    else
	req = req_queue.Pop();
    if (__disable)
	try_disable_event(EPOLLOUT);
    req_queue.UnLock();
    return req;
}

inline int __async_producer::push_req(struct appmsg *msg) {
    bool __enable(false);
    int ret = 0;
    
    req_queue.Lock();
    if (req_queue.Empty())
	__enable = true;
    if ((ret = req_queue.Push(msg)) == 0 && __enable == true)
	try_enable_event(EPOLLOUT);
    req_queue.UnLock();
    return ret;
}


inline struct appmsg *__async_producer::pop_resp() {
    struct appmsg *msg = NULL;

    resp_queue.Lock();
    msg = resp_queue.Pop();
    resp_queue.UnLock();
    return msg;
}

inline int __async_producer::push_resp(struct appmsg *resp) {
    bool new_task(false);
    struct appmsg *tmp = NULL;

    resp_queue.Lock();
    if (wg.Ref() < conf.max_workers)
	new_task = true;
    while (resp_queue.Push(resp) == -1) {
	if ((tmp = resp_queue.Pop()) != NULL)
	    free_appmsg(tmp); // fix here. we should make icmp back
    }
    resp_queue.UnLock();
    if (new_task) {
	wg.Add();
	astp->run_task(callbackworker, this);
    }
    return 0;
}

int __async_producer::connect_error() {
    interncli.Close();
    astp->disable_eventpoll(this);
    if (connect_to_nspiosvr() == 0)
	astp->enable_eventpoll(this);
    return 0;
}


int __async_producer::__connect_to_nspiosvr() {

    if (interncli.Connect(conf.appname, conf.apphost) < 0)
	return -1;
    ee.SetEvent(interncli.Fd(), EPOLLIN|EPOLLOUT|EPOLLRDHUP, this);
    stats.incrkey(ASAPI_RECONNECT);
    interncli.SetOption(OPT_SOCKCACHE);
    interncli.SetOption(OPT_NONBLOCK, 1);
    return 0;
}

int __async_producer::connect_to_nspiosvr() {
    int ret = 0;
    while (!astp->stopping() && (ret = __connect_to_nspiosvr()) < 0)
	usleep(10);
    return ret;
}    

int __async_producer::recv_massage(int max_recv) {
    struct appmsg msg = {}, *resp = NULL;
    int ret = 0, i;

    for (i = 0; i < max_recv; i++) {
	if ((ret = interncli.Recv(&msg)) == 0) {
	    if ((resp = (struct appmsg *)mem_zalloc(sizeof(*resp)))) {
		resp->hdr = msg.hdr;
		resp->s.len = msg.s.len;
		resp->s.data = msg.s.data;
		push_resp(resp);
	    } else if (msg.s.len)
		mem_free(msg.s.data, msg.s.len);
	    stats.incrkey(ASAPI_RECVBYTES, msg.s.len);
	    stats.incrkey(ASAPI_RECVPACKAGES);
	    memset((char *)&msg, 0, sizeof(msg));
	} else if (ret < 0 && errno != EAGAIN) {
	    stats.incrkey(ASAPI_RECVERRORS);
	    connect_error();
	    break;
	}
    }
    return ret;
}


static int __set_async_api_flags(struct spiohdr *hdr) {
    hdr->flags = hdr->flags | ASYNCAPIMSG;
    return 0;
}

int __async_producer::cleanup_tw_callbacks() {
    simple_timeout_ed toed;
    struct hlist_head callback_to_head = {};
    nspio_respcallback_t *pos = NULL, *next = NULL;

    INIT_HLIST_HEAD(&callback_to_head);    
    ht.cleanup_tw_callbacks(&callback_to_head);    
    hlist_for_each_entry_safe(pos, next, &callback_to_head, nspio_respcallback_t, cb_link) {
	hlist_del(&pos->cb_link);
	toed.parsefrom(pos->hid, pos->ctime);
	ach->HandleError(toed, pos->callback);
	mem_free(pos, sizeof(*pos));
    }
    return 0;
}

int __async_producer::send_massage(int max_send) {
    int ret = 0, cnt = 0;
    uint32_t snd_bytes = 0;
    struct appmsg *req = NULL;

    while (max_send > 0 && (req = pop_req()) != NULL) {
	cnt++;
	max_send--;
	__set_async_api_flags(&req->hdr);
	snd_bytes = sizeof(*req) + req->s.len;
	if ((ret = interncli.Send(req)) == 0) {
	    stats.incrkey(ASAPI_SENDBYTES, snd_bytes);
	    stats.incrkey(ASAPI_SENDPACKAGES);
	} else if (ret < 0 && errno != EAGAIN) {
	    stats.incrkey(ASAPI_SENDERRORS);
	    connect_error();
	}
	free_appmsg(req);
    }
    if (cnt > 0)
	interncli.FlushCache();
    return 0;
}

int __async_producer::handle_events() {
    int rcachesize = interncli.CacheSize();;

    cleanup_tw_callbacks();

    if (!ee.happened && rcachesize <= 0)
	return -1;
    if ((ee.happened & EPOLLIN) || rcachesize > 0)
	recv_massage(conf.max_workers * 10);
    if ((ee.happened & EPOLLOUT))
	send_massage(conf.max_workers);
    if (ee.happened & (EPOLLRDHUP|EPOLLERR))
	connect_error();
    ee.happened = 0;
    return 0;
}

int __async_producer::intern_callbackworker() {
    deicmp_ed deed;
    struct appmsg *resp = NULL;
    void *cb = NULL;

    while (!astp->stopping()) {
	if ((resp = pop_resp()) == NULL)
	    break;
	if (ht.FindHandler(resp->hdr.seqid, &cb) == 0) {
	    if (resp->hdr.flags == APPICMP_DELIVERERROR) {
		deed.parsefrom(resp);
		ach->HandleError(deed, cb);
	    } else {
		ach->HandleResponse(resp->s.data, resp->s.len, cb);
	    }
	}
	cb = NULL;
	free_appmsg(resp);
    }
    wg.Done();
    return 0;
}

int __async_producer::Setup(async_conf &__conf, ResponseHandler *h, Transport *tp) {
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
    ht.Init(conf.queue_cap);
    req_queue.Setup(conf.queue_cap, free_appmsg);
    resp_queue.Setup(conf.queue_cap, free_appmsg);
    while (__connect_to_nspiosvr() < 0)
	usleep(10);
    ach = h;
    if (tp)
	astp = (async_transport *)tp;
    inited = 1;
    return 0;
}


int __async_producer::Stop() {
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

int __async_producer::StartServe() {
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

int __async_producer::SendRequest(const char *data, int len, void *callback, int to_msec) {
    int64_t hid = 0;
    struct appmsg *msg = NULL;

    if (!astp || astp->stopping()) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (len < 0) {
	errno = EINVAL;
	return -1;
    }
    if ((msg = new_appmsg(data, len)) == NULL) {
	errno = ENOMEM;
	return -1;
    }
    hid = ht.GetHid();
    ht.InsertHandler(hid, callback, to_msec);
    msg->hdr.seqid = hid;
    if (push_req(msg) < 0) {
	ht.FindHandler(hid, &callback);
	free_appmsg(msg);
	errno = SPIO_EQUEUEFULL;
	return -1;
    }
    return 0;
}

}
