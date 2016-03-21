// copyright:
//            (C) SINA Inc.
//
//           file: nspio/api/transport.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_NSPIOTRANSPORT_
#define _H_NSPIOTRANSPORT_

#include <set>
#include <nspio/async_api.h>
#include "os/epoll.h"
#include "sync/pmutex.h"
#include "sync/waitgroup.h"
#include "runner/task_pool.h"

using namespace std;
NSPIO_DECLARATION_START


class __async_base;
class async_transport : public Transport {
 public:
    async_transport();
    ~async_transport();

    int Setup(int workers) {
	return tp.Setup(workers);
    }
    int start();
    int stop();
    int stopping() {
	return _stopping;
    }

    int enable_eventpoll(__async_base *ab);
    int disable_eventpoll(__async_base *ab);
    int update_eventpoll(__async_base *ab);
    int add(__async_base *ab);
    int del(__async_base *ab);
    int run_task(task_func tfunc, void *tdata) {
	tp.Run(tfunc, tdata);
	tp.BroadCast();
	return 0;
    }

    int ioloop();
    
 private:
    bool _stopping;
    Epoller *poller;
    WaitGroup wg;
    TaskPool tp;

    pmutex_t lock;
    struct list_head async_base_head;

    int __walkon_epollevents();
};




}
#endif  //  _H_NSPIOTRANSPORT_
