// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/nspio/async_api.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_NSPIOASYNC_
#define _H_NSPIOASYNC_


#include <nspio/api.h>

namespace nspio {
class AsyncComsumer;
class AsyncProducer;



class Transport {
 public:
    virtual ~Transport() {}
    virtual int Setup(int workers) = 0;
    virtual int start() = 0;
    virtual int stop() = 0;
}; 
Transport *NewTransport();

 
 
class async_conf {
 public:
    async_conf() {
	max_workers = queue_cap = max_trip_time = 0;
    }
    void set(string &app, string &host, int w, int cap, int rtt) {
	appname = app;
	apphost = host;
	max_workers = w;
	queue_cap = cap;
	max_trip_time = rtt;
    }

    string appname, apphost;
    int max_workers, queue_cap, max_trip_time;
};
    


class ResponseWriter {
 public:
    int Send(const char *data, uint32_t len);
    string route;
    AsyncComsumer *async_comsumer;
};

class RequestHandler {
 public:
    virtual ~RequestHandler() {}
    virtual int HandleRequest(const char *data, uint32_t len, ResponseWriter &rw) = 0;
};

class AsyncComsumer {
 public:
    virtual ~AsyncComsumer() {}
    virtual int Setup(async_conf &conf, RequestHandler *h, Transport *tp = NULL) = 0;
    virtual int Stop() = 0;
    virtual int StartServe() = 0;
    virtual int SendResponse(const char *data, int len, const string &rt) = 0;
};
AsyncComsumer *NewAsyncComsumer();

 
class Error {
 public:
    virtual ~Error() {}
    virtual string Str() = 0;
};

class ResponseHandler {
 public:
    virtual ~ResponseHandler() {}
    virtual int HandleError(Error &ed, void *cb) = 0;
    virtual int HandleResponse(const char *data, uint32_t len, void *cb) = 0;
};

class AsyncProducer {
 public:
    virtual ~AsyncProducer() {}
    virtual int Setup(async_conf &conf, ResponseHandler *h, Transport *tp = NULL) = 0;
    virtual int Stop() = 0;
    virtual int StartServe() = 0;
    virtual int SendRequest(const char *data, int len, void *cb, int to_msec = 0) = 0;
};
AsyncProducer *NewAsyncProducer();


}










#endif  // _H_NSPIOASYNC_
