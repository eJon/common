// copyright:
//            (C) SINA Inc.
//
//           file: nspio/api/multi_io.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <string.h>
#include <stdarg.h>
#include <map>
#include <nspio/multi_api.h>
#include "async_api.h"
#include "transport.h"

using namespace std;
NSPIO_DECLARATION_START

query_result_handler::query_result_handler(int __who) :
    who(__who)
{
}

query_result_handler::~query_result_handler()
{
}

int query_result_handler::HandleError(Error &ed, void *cb) {
    reqresp_ctx *h = (reqresp_ctx *)cb;
    h->lock();
    h->isbad = true;
    h->one_response_bad(who, ed);
    h->unlock();
    if (h->decr() == 1) {
	h->back_response(h->rw, h->isbad);
	delete h;
    }
    return 0;
}

int query_result_handler::HandleResponse(const char *data, uint32_t len, void *cb) {
    reqresp_ctx *h = (reqresp_ctx *)cb;
    h->lock();
    h->one_response_done(who, data, len);
    h->unlock();
    if (h->decr() == 1) {
	h->back_response(h->rw, h->isbad);
	delete h;
    }
    return 0;
}




reqresp_ctx::reqresp_ctx() :
    ref(0), isbad(false)
{
    pthread_mutex_init(&mutex, NULL);
}

reqresp_ctx::~reqresp_ctx() {
    pthread_mutex_destroy(&mutex);
}

void reqresp_ctx::lock() {
    pthread_mutex_lock(&mutex);
}

void reqresp_ctx::unlock() {
    pthread_mutex_unlock(&mutex);
}

int reqresp_ctx::__get() {
    return ref;
}

int reqresp_ctx::__incr() {
    int __ref = ref;
    ref++;
    return __ref;
}

int reqresp_ctx::__decr() {
    int __ref = ref;
    ref--;
    return __ref;
}

int reqresp_ctx::incr() {
    int __ref = 0;
    pthread_mutex_lock(&mutex);
    __ref = __incr();
    pthread_mutex_unlock(&mutex);
    return __ref;
}

int reqresp_ctx::decr() {
    int __ref = 0;
    pthread_mutex_lock(&mutex);
    __ref = __decr();
    pthread_mutex_unlock(&mutex);
    return __ref;
}












Multiio::Multiio() :
    inited(false), tp(NULL), f(NULL), asc(NULL)
{
    pthread_mutex_init(&lock, NULL);
    tp = NewTransport();
}

Multiio::~Multiio() {
    async_transport *astp = (async_transport *)tp;
    map<int, AsyncProducer *>::iterator it;
    map<int, query_result_handler *>::iterator qit;

    pthread_mutex_destroy(&lock);
    for (it = asps.begin(); it != asps.end(); ++it) {
	astp->del((__async_producer *)it->second);
	delete it->second;
    }
    astp->del((__async_comsumer *)asc);
    delete asc;

    for (qit = qrhs.begin(); qit != qrhs.end(); ++qit)
	delete qit->second;
    if (astp)
	delete astp;
}

AsyncProducer *Multiio::find_ap(int who) {
    AsyncProducer *ap = NULL;
    map<int, AsyncProducer *>::iterator it;
	
    pthread_mutex_lock(&lock);
    if ((it = asps.find(who)) != asps.end())
	ap = it->second;
    pthread_mutex_unlock(&lock);
    return ap;
}
int Multiio::add_ap_and_handler(int who, AsyncProducer *ap, query_result_handler *qrh) {
    int ret = -1;
    map<int, AsyncProducer *>::iterator it;

    pthread_mutex_lock(&lock);
    if ((it = asps.find(who)) == asps.end()) {
	asps.insert(make_pair(who, ap));
	qrhs.insert(make_pair(who, qrh));
	ret = 0;
    }
    pthread_mutex_unlock(&lock);
    return ret;
}


int Multiio::Init(reqresp_ctxfactor *__f, int nthreads, async_conf &inapp) {
    __async_comsumer *ac = new (std::nothrow) __async_comsumer();
    async_transport *astp = (async_transport *)tp;

    if (inited) {
	errno = SPIO_EDUPOP;
	return -1;
    }
    if (!ac || ac->Setup(inapp, this, tp) < 0)
	return -1;
    astp->Setup(nthreads);
    astp->add(ac);
    asc = ac;
    f = __f;
    inited = true;
    return 0;
}

int Multiio::AddBackendServer(int who, async_conf &outapp) {
    async_transport *astp = (async_transport *)tp;
    __async_producer *asp = new (std::nothrow) __async_producer();
    query_result_handler *qrh = new (std::nothrow) query_result_handler(who);
    map<int, AsyncProducer *>::iterator it;

    if (!asp || !qrh) {
	if (asp)
	    delete asp;
       	if (qrh)
	    delete qrh;
	return -1;
    }
    if (asp->Setup(outapp, qrh, tp) < 0)
	return -1;
    if (add_ap_and_handler(who, asp, qrh) < 0) {
	delete asp;
	delete qrh;
	return -1;
    }
    astp->add(asp);
    return 0;
}

int Multiio::Start() {
    tp->start();
    return 0;
}

int Multiio::Stop() {
    tp->stop();
    return 0;
}

int Multiio::Send(int who, const char *data, uint32_t len, reqresp_ctx *h, int to_msec) {
    int ret = 0;
    AsyncProducer *asp = find_ap(who);

    if (!asp) {
	errno = SPIO_ENETUNREACH;
	return -1;
    }
    h->__incr();
    if ((ret = asp->SendRequest(data, len, h, to_msec)) < 0) {
	h->isbad = true;
	h->__decr();
    }
    return ret;
}

int Multiio::HandleRequest(const char *data, uint32_t len, ResponseWriter &rw) {
    int ref = 0;
    reqresp_ctx *h = f->new_reqresp_ctx();

    if (!h)
	return -1;
    h->incr();
    h->lock();
    h->rw = rw;
    h->request_come(data, len, this);
    ref = h->__get();
    h->unlock();
    if (h->decr() == 1) {
	h->back_response(h->rw, h->isbad);
	delete h;
    }
    return 0;
}




}
