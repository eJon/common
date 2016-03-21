// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/os/epoll.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <sys/epoll.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <nspio/errno.h>
#include "os/time.h"
#include "os/epoll.h"
#include "base/skrb_sync.h"

NSPIO_DECLARATION_START

EpollEvent::EpollEvent() :
    udtype(0), ptr(NULL), fd(-1), to_nsec(0), events(0), happened(0)
{
    memset(&timer_node, 0, sizeof(timer_node));
    timer_node.data = this;
    INIT_LIST_LINK(&link);
}

int EpollEvent::attach(struct list_head *head) {
    if (!link.linked) {
	link.linked = 1;
	list_add(&link.node, head);
    }
    return 0;
}
    
int EpollEvent::detach() {
    if (link.linked) {
	list_del(&link.node);
	link.linked = 0;
    }
    return 0;
}
    
EpollEvent::~EpollEvent() {

}


void EpollEvent::SetEvent(int _fd, uint32_t _events, void *data) {
    fd = _fd;
    events = _events;
    ptr = data;
}

Epoller::Epoller(int efd, int size, int _max_io_evs) :
    epoll_fd(efd), max_io_events(_max_io_evs), cur_nsec(0), event_size(0)
{
    skrb_init(&timer_rbtree);
    pmutex_init(&lock);
}

Epoller::~Epoller() {
    if (epoll_fd >= 0)
	close(epoll_fd);
    pmutex_destroy(&lock);
}


int Epoller::Ctl(int op, EpollEvent *ev) {
    switch (op) {
    case EPOLL_CTL_ADD:
	return CtlAdd(ev);
    case EPOLL_CTL_MOD:
	return CtlMod(ev);
    case EPOLL_CTL_DEL:
	return CtlDel(ev);
    }
    return -1;
}

int Epoller::CtlAdd(EpollEvent *ev) {
    int ret = 0;
    int64_t _cur_nsec = rt_nstime();
    epoll_event ee;

    if (ev->to_nsec) {
	ev->timer_node.key = _cur_nsec + ev->to_nsec;
	skrb_mutex_insert(&timer_rbtree, &ev->timer_node, &lock);
    }

    if (ev->fd >= 0) {
	ee.events = ev->events;
	ee.data.ptr = ev;
	if ((ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ev->fd, &ee)) != -1)
	    event_size++;
    }
    return ret;
}

int Epoller::CtlMod(EpollEvent *ev) {
    int ret = 0;
    int64_t _cur_nsec = rt_nstime();
    epoll_event ee;

    if (ev->to_nsec) {
	if (ev->timer_node.key)
	    skrb_mutex_delete(&timer_rbtree, &ev->timer_node, &lock);
	ev->timer_node.key = _cur_nsec + ev->to_nsec;
	skrb_mutex_insert(&timer_rbtree, &ev->timer_node, &lock);
    }

    if (ev->fd >= 0) {
	ee.events = ev->events;
	ee.data.ptr = ev;
	ret = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, ev->fd, &ee);
    }
    return ret;
}

int Epoller::CtlDel(EpollEvent *ev) {
    int ret = 0;
    epoll_event ee;

    if (ev->to_nsec && ev->timer_node.key) {
	skrb_mutex_delete(&timer_rbtree, &ev->timer_node, &lock);
	ev->timer_node.key = 0;
    }

    if (ev->fd >= 0) {
	ee.events = ev->events;
	ee.data.ptr = ev;
	if ((ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, ev->fd, &ee)) != -1)
	    event_size--;
    }
    return ret;
}


int64_t Epoller::find_timer(skrb_t *tree, int64_t to) {
    skrb_node_t *node = NULL;
    
    if (skrb_mutex_empty(tree, &lock))
	return to;
    node = skrb_mutex_min(tree, &lock);
    if (node->key < to)
	return node->key;
    return to;
}


int Epoller::Wait(struct list_head *io_head, struct list_head *to_head, int msec) {
    EpollEvent *ev = NULL;
    epoll_event ev_buf[max_io_events];
    int i = 0, n = 0;
    skrb_node_t *node = NULL;
    int64_t timeout = 0, to = 0, _cur_nsec = rt_nstime();

    if (!io_head || !to_head) {
	errno = EINVAL;
	return -1;
    }
    to = _cur_nsec + msec * 1000000;
    timeout = find_timer(&timer_rbtree, to);
    timeout = timeout > _cur_nsec ? timeout - _cur_nsec : 0;
    if (event_size <= 0)
	usleep(timeout/1000);
    else if ((n = epoll_wait(epoll_fd, ev_buf, max_io_events, timeout/1000000)) < 0)
	n = 0;

    for (i = 0; i < n; i++) {
	ev = (EpollEvent *)ev_buf[i].data.ptr;
	if (ev->timer_node.key && ev->timer_node.key < _cur_nsec) {
	    skrb_mutex_delete(&timer_rbtree, &ev->timer_node, &lock);
	    ev->timer_node.key = 0;
	    ev->happened = EPOLLTIMEOUT;
	    ev->attach(to_head);
	} else {
	    // shoud here update timer_rb timestamp ?
	    ev->happened = ev_buf[i].events;
	    ev->attach(io_head);
	}
    }

    n = max_io_events - n;
    while (n && !skrb_mutex_empty(&timer_rbtree, &lock)) {
	node = skrb_mutex_min(&timer_rbtree, &lock);
	if (node->key > _cur_nsec)
	    break;
	ev = (EpollEvent *)node->data;
	skrb_mutex_delete(&timer_rbtree, &ev->timer_node, &lock);
	ev->timer_node.key = 0;
	ev->happened = EPOLLTIMEOUT;
	ev->attach(to_head);
	n--;
    }
    return 0;
}


Epoller *EpollCreate(int size, int max_io_events) {
    int efd = 0;
    Epoller *ep = NULL;

    if (-1 == (efd = epoll_create(size)))
	return NULL;
    if (!(ep = new (std::nothrow) Epoller(efd, size, max_io_events)))
	close(efd);
    return ep;
}


}
