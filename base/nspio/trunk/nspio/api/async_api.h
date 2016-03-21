// copyright:
//            (C) SINA Inc.
//
//           file: nspio/api/async_api.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_NSPIO_R_
#define _H_NSPIO_R_

#include <nspio/errno.h>
#include <nspio/async_api.h>
#include "os/time.h"
#include "base/list.h"
#include "ht.h"
#include "module_stats.h"
#include "mqueue.h"
#include "sync_api.h"
#include "os/epoll.h"
#include "sync/waitgroup.h"
#include "runner/thread.h"
#include "runner/task_pool.h"
#include "transport.h"



NSPIO_DECLARATION_START

enum ASAPI_MODULE_STATITEM {
    ASAPI_RECONNECT = 1,
    ASAPI_RECVBYTES,
    ASAPI_SENDBYTES,
    ASAPI_RECVPACKAGES,
    ASAPI_SENDPACKAGES,
    ASAPI_RECVERRORS,
    ASAPI_SENDERRORS,
    ASAPI_CHECKSUMERRORS,
    ASAPI_MODULE_STATITEM_KEYRANGE,
};

static inline int __package_is_timeout(struct appmsg *msg, int64_t stt) {
    return stt > 0 && (rt_mstime() - msg->hdr.timestamp) > stt;
}

class __async_comsumer;
class __async_producer;
class __async_base {
 public:
    friend class __async_comsumer;
    friend class __async_producer;
    __async_base();
    virtual ~__async_base();

    EpollEvent *epollevent();
    virtual int handle_events() = 0;
    int try_disable_event(int ev);
    int try_enable_event(int ev);

    int start();
    int stop();
    module_stat *stat() {
	return &stats;
    }
    int attach_to_transport_head(struct list_head *head);
    int detach_from_transport_head();

 private:
    int inited;
    async_conf conf;
    EpollEvent ee;
    WaitGroup wg;
    module_stat stats;
    LockQueue req_queue, resp_queue;
    async_transport *astp;
    struct list_link astp_link;
};



class __async_comsumer: public __async_base, public AsyncComsumer {
 public:
    __async_comsumer();
    ~__async_comsumer();

    int Setup(async_conf &__conf, RequestHandler *h, Transport *tp = NULL);
    int Stop();
    int StartServe();
    int SendResponse(const char *data, int len, const string &rt);

    int handle_events();
    int intern_ioworker();
    int intern_callbackworker();

 private:
    RequestHandler *ash;
    __comsumer internsvr;
    
    inline int recv_massage(int max_recv);
    inline int send_massage(int max_send);

    inline struct appmsg *pop_req();
    inline int push_req(struct appmsg *req);
    inline struct appmsg *pop_resp();
    inline int push_resp(struct appmsg *resp);
    
    inline int connect_error();
    inline int __connect_to_nspiosvr();
    inline int connect_to_nspiosvr();
};



class __async_producer : public __async_base, public AsyncProducer {
 public:
    __async_producer();
    ~__async_producer();

    int Setup(async_conf &__conf, ResponseHandler *h, Transport *tp = NULL);
    int Stop();
    int StartServe();
    int SendRequest(const char *data, int len, void *callback, int to_msec);

    int handle_events();
    int intern_ioworker();
    int intern_callbackworker();
    
 private:
    ResponseHandler *ach;
    __producer interncli;
    HandlerTable ht;
    
    inline int send_massage(int max_send);
    inline int recv_massage(int max_recv);

    inline struct appmsg *pop_req();
    inline int push_req(struct appmsg *req);
    inline struct appmsg *pop_resp();
    inline int push_resp(struct appmsg *resp);

    inline int connect_error();
    inline int __connect_to_nspiosvr();
    inline int connect_to_nspiosvr();
    inline int cleanup_tw_callbacks();
};










 

}



#endif  // _H_NSPIO_R_
