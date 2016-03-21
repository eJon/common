// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/os/epoll.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_EPOLLER_
#define _H_EPOLLER_


#include <sys/epoll.h>
#include <vector>
#include "base/list.h"
#include "base/skrb.h"
#include "sync/pmutex.h"

using namespace std;

NSPIO_DECLARATION_START

#define EPOLLTIMEOUT (1 << 28)


// Fix some platform glibc too old not support EPOLLRDHUP flag
// linux kernel support EPOLLRDHUP since Linux 2.6.17
#ifndef EPOLLRDHUP
#define EPOLLRDHUP 0x2000
#endif


#define list_ev(link) ((EpollEvent *)link->self)
#define list_first_ev(head)						\
    ({struct list_link *__epos =					\
	    list_first(head, struct list_link, node); list_ev(__epos);})

#define list_for_each_poll_link_autodetach(ev, head)		\
    {								\
	struct list_link *__epos = NULL, *__enext = NULL;	\
	list_for_each_list_link_safe(__epos, __enext, head) {	\
	    ev = list_ev(__epos); ev->detach();


/* 
   rules:
   1. all io_head and to_head must be inited before poller->Wait
   2. __ev->detach() in list_for_each_poll_link[_safe](), otherwise,
      if object deleted in list_for_each_poll_link[_safe](),
      so detach_for_each_poll_link will cause coredump
   3. the last, detach_for_each_poll_link after work done
*/ 
#define detach_for_each_poll_link(head)				\
    {								\
	EpollEvent *__ev = NULL;				\
	list_for_each_poll_link_autodetach(__ev, head) {}}}	\
}

 
class Epoller;
class EpollEvent {
 public:
    EpollEvent();
    ~EpollEvent();
    friend class Epoller;

    int udtype;         // user define epollevent type. extend field
    void *ptr;          // user define handler pointer. extend field
    int fd;             // events for fd
    int64_t to_nsec;    // event timeout time
    uint32_t events;    // all epoll events
    uint32_t happened;  // what events happened when epoll_wait return

    // helper function
    void SetEvent(int _fd, uint32_t _events, void *data);
    int attach(struct list_head *head);
    int detach();
    
 private:
    skrb_node_t timer_node;
    struct list_link link;
};


 
class Epoller {
 public:
    Epoller(int efd, int size, int max_io_events);
    ~Epoller();
    int Ctl(int op, EpollEvent *ev);
    int Wait(struct list_head *io_head, struct list_head *to_head, int msec);

    int CtlAdd(EpollEvent *ev);
    int CtlDel(EpollEvent *ev);
    int CtlMod(EpollEvent *ev);

 private:
    skrb_t timer_rbtree;
    pmutex_t lock;
    int epoll_fd, max_io_events;
    int64_t cur_nsec, event_size;

    int64_t find_timer(skrb_t *tree, int64_t to);
};

Epoller *EpollCreate(int size, int max_io_events);







}

















#endif   // _H_EPOLLER_
