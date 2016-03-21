// copyright:
//            (C) SINA Inc.
//
//           file: nspio/api/transport.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "transport.h"
#include "async_api.h"


NSPIO_DECLARATION_START

#define list_async_base(link) ((__async_base *)link->self) 

Transport *NewTransport() {
    return new (std::nothrow) async_transport();
}


async_transport::async_transport() :
    _stopping(true)
{
    poller = EpollCreate(1024, 100);
    pmutex_init(&lock);
    INIT_LIST_HEAD(&async_base_head);
}

async_transport::~async_transport() {
    pmutex_destroy(&lock);
    if (poller)
	delete poller;
}

int async_transport::enable_eventpoll(__async_base *ab) {
    EpollEvent *ev = NULL;

    if (!poller) {
	errno = SPIO_EINTERN;
	return -1;
    }
    ev = ab->epollevent();
    return poller->CtlAdd(ev);
}

int async_transport::disable_eventpoll(__async_base *ab) {
    EpollEvent *ev = NULL;

    if (!poller) {
	errno = SPIO_EINTERN;
	return -1;
    }
    ev = ab->epollevent();
    return poller->CtlDel(ev);
}

int async_transport::update_eventpoll(__async_base *ab) {
    EpollEvent *ev = NULL;

    if (!poller) {
	errno = SPIO_EINTERN;
	return -1;
    }
    ev = ab->epollevent();
    return poller->CtlMod(ev);
}

int async_transport::add(__async_base *ab) {
    int ret = 0;

    if ((ret = enable_eventpoll(ab)) == 0) {
	pmutex_lock(&lock);
	ab->attach_to_transport_head(&async_base_head);
	pmutex_unlock(&lock);
    }
    return ret;
}

int async_transport::del(__async_base *ab) {
    int ret = 0;

    if ((ret = disable_eventpoll(ab)) == 0) {
	pmutex_lock(&lock);
	ab->detach_from_transport_head();
	pmutex_unlock(&lock);
    }
    return ret;
}

int async_transport::__walkon_epollevents() {
    __async_base *ab = NULL;
    struct list_link *pos = NULL, *next = NULL;

    pmutex_lock(&lock);
    list_for_each_list_link_safe(pos, next, &async_base_head) {
	ab = list_async_base(pos);
	ab->handle_events();
    }
    pmutex_unlock(&lock);
    return 0;
}

int async_transport::ioloop() {
    struct list_head io_head = {}, to_head = {};

    INIT_LIST_HEAD(&io_head);
    INIT_LIST_HEAD(&to_head);

    while (!stopping()) {
	if (poller->Wait(&io_head, &to_head, 1) >= 0) {
	    __walkon_epollevents();
	    detach_for_each_poll_link(&io_head);
	    detach_for_each_poll_link(&to_head);
	}
    }
    wg.Done();
    return 0;
}



static int ioworker(void *arg_) {
    async_transport *at = (async_transport *)arg_;
    return at->ioloop();
}

int async_transport::start() {
    if (!poller) {
	errno = SPIO_EINTERN;
	return -1;
    }
    _stopping = false;
    tp.Start();
    wg.Add();
    tp.Run(ioworker, this);
    tp.BroadCast();
    return 0;
}

int async_transport::stop() {
    if (!poller) {
	errno = SPIO_EINTERN;
	return -1;
    }
    _stopping = true;
    wg.Wait();
    tp.Stop();
    return 0;
}











}
