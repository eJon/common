// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/nspio/multi_api.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_QUERYRESPONSE_
#define _H_QUERYRESPONSE_


#include <pthread.h>
#include <string>
#include <nspio/async_api.h>
#include <map>

using namespace std;

namespace nspio {
class Multiio;
class reqresp_ctx;

 
class MultiSender {
 public:
    virtual ~MultiSender() {}
    virtual int Send(int who, const char *data, uint32_t len, reqresp_ctx *h, int to_msec) = 0;
};

class query_result_handler : public ResponseHandler {
 public:
    query_result_handler(int __who);
    ~query_result_handler();
    int HandleError(Error &ed, void *__reqresp_ctx);
    int HandleResponse(const char *data, uint32_t len, void *__reqresp_ctx);

 private:
    int who;
};

class reqresp_ctx {
 public:
    friend class Multiio;
    friend class query_result_handler;
    reqresp_ctx();
    virtual ~reqresp_ctx();

    virtual int request_come(const char *data, uint32_t len, MultiSender *s) = 0;
    virtual int back_response(ResponseWriter &w, bool bad) = 0;
    virtual int one_response_bad(int who, Error &ed) = 0;
    virtual int one_response_done(int who, const char *data, uint32_t len) = 0;

 private:
    int ref;
    bool isbad;
    string route;
    ResponseWriter rw;
    pthread_mutex_t mutex;

    void lock();
    void unlock();
    int __get();
    int __incr();
    int __decr();
    int incr();
    int decr();
};
 
class reqresp_ctxfactor {
 public:
    virtual ~reqresp_ctxfactor() {}
    virtual reqresp_ctx *new_reqresp_ctx() = 0;
};


class Multiio : public MultiSender, public RequestHandler {
 public:
    Multiio();
    ~Multiio();
    int Init(reqresp_ctxfactor *f, int nthreads, async_conf &inapp);
    int AddBackendServer(int who, async_conf &outapp);
    int Start();
    int Stop();

 private:
    bool inited;
    Transport *tp;
    reqresp_ctxfactor *f;

    pthread_mutex_t lock;
    AsyncComsumer *asc;
    map<int, AsyncProducer *> asps;
    map<int, query_result_handler *> qrhs;

    AsyncProducer *find_ap(int who);
    int add_ap_and_handler(int who, AsyncProducer *ap, query_result_handler *qrh);

    friend class reqresp_ctx;
    int Send(int who, const char *data, uint32_t len, reqresp_ctx *h, int to_msec);
    int HandleRequest(const char *data, uint32_t len, ResponseWriter &rw);
};



}
#endif
