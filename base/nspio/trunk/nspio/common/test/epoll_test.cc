// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/test/epoll_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <gtest/gtest.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "log.h"
#include "os/epoll.h"
#include "runner/thread.h"
#include "net/tcp.h"


using namespace std;
using namespace nspio;


static volatile int stopping1 = 0;
static volatile int status1= 0;

static int tcp_server(void *arg_) {
    TCPListener *listen = (TCPListener *)arg_;
    TCPConn *conn;
    int n, total = 0;
    char buf[1024];

    status1 = 1;
    conn = listen->Accept();
    if (!conn)
	return -1;
    conn->SetSockOpt(SO_NONBLOCK, 1);
    conn->SetSockOpt(SO_NODELAY, 1);
    while (!stopping1) {
	n = conn->Read(buf, 1024);
	total += n;
	conn->Write(buf, n);
    }
    
    delete conn;
    return 0;
}

static void epoll_socket() {
    EpollEvent ev, *ee;
    Epoller *ep;
    TCPListener *listen;
    TCPConn *conn;
    Thread svr;
    char buf[1024] = {};
    struct list_head io_head, to_head;

    INIT_LIST_HEAD(&io_head);
    INIT_LIST_HEAD(&to_head);

    status1 = 0;
    ep = EpollCreate(1000, 500);
    if (!ep)
	return;
    listen = ListenTCP("tcp", "*:18892", 100);
    if (!listen)
	return;
    svr.Start(tcp_server, listen);
    while (!status1) {
	/* waiting... */
    }

    conn = DialTCP("tcp", "", "127.0.0.1:18892");
    if (!conn) {
	stopping1 = 1;
	svr.Stop();
	return;
    }
    conn->SetSockOpt(SO_NONBLOCK, 1);
    conn->SetSockOpt(SO_NODELAY, 1);
    
    ev.fd = conn->sockfd;
    ev.events = EPOLLIN;
    ep->CtlAdd(&ev);

    // the first time won't be timeout
    EXPECT_TRUE(0 == ep->Wait(&io_head, &to_head, 10));
    EXPECT_TRUE(list_empty(&io_head));
    EXPECT_TRUE(list_empty(&to_head));
    detach_for_each_poll_link(&io_head);
    detach_for_each_poll_link(&to_head);

    
    // test epollin
    conn->Write(buf, 4);
    EXPECT_TRUE(0 == ep->Wait(&io_head, &to_head, 100));
    EXPECT_TRUE(!list_empty(&io_head));
    EXPECT_TRUE(list_empty(&to_head));
    if (!list_empty(&io_head)) {
	ee = list_first_ev(&io_head);
	ee->detach();
	conn->Read(buf, 1024);
	EXPECT_TRUE(ee->happened & EPOLLIN);
    }
    detach_for_each_poll_link(&io_head);
    detach_for_each_poll_link(&to_head);

    
    // test timeout
    ep->CtlDel(&ev);
    ev.fd = conn->sockfd;
    ev.events = EPOLLIN;
    ev.to_nsec = 10 * 1000000; // 10ms
    ep->CtlAdd(&ev);

    usleep(1000); // sleep 1ms
    EXPECT_EQ(0, ep->Wait(&io_head, &to_head, 10)); // 10ms
    EXPECT_TRUE(list_empty(&io_head));
    EXPECT_TRUE(list_empty(&to_head));
    detach_for_each_poll_link(&io_head);
    detach_for_each_poll_link(&to_head);

    
    ep->CtlDel(&ev);
    ev.SetEvent(conn->sockfd, EPOLLIN, NULL);
    ev.to_nsec = 10 * 1000000; // 10ms
    ep->CtlAdd(&ev);

    usleep(100000); // sleep 100ms
    EXPECT_EQ(0, ep->Wait(&io_head, &to_head, 100)); 
    EXPECT_TRUE(!list_empty(&to_head)); 
    EXPECT_TRUE(list_empty(&io_head));
    detach_for_each_poll_link(&io_head);
    detach_for_each_poll_link(&to_head);
    
    // close connection
    delete conn;

    stopping1 = 1;
    svr.Stop();
    
    delete ep;
    delete listen;
    return;
}



TEST(epoll, socket) {
    epoll_socket();
}









static void epoll_pipe() {
    EpollEvent *ev;
    Epoller *ep;
    int cnt = 0;
    int pipefd[2], rfd, wfd;
    struct list_head io_head, to_head;

    INIT_LIST_HEAD(&io_head);
    INIT_LIST_HEAD(&to_head);

    ep = EpollCreate(1000, 500);
    if (!ep)
	return;
    if (-1 == pipe(pipefd) || pipefd[0] <= 0 || pipefd[1] <= 0)
	return;
    rfd = pipefd[0];
    wfd = pipefd[1];
    
    ev = new (std::nothrow) EpollEvent();
    ev->SetEvent(rfd, EPOLLIN, NULL);
    ev->to_nsec = 1000000;
    ep->CtlAdd(ev);
    
    ev = new (std::nothrow) EpollEvent();
    ev->SetEvent(wfd, EPOLLOUT, NULL);
    ev->to_nsec = 1000000; // 1ms
    ep->CtlAdd(ev);

    write(wfd, "haha", 4);
    usleep(100000); // sleep 100ms

    // test timeout
    EXPECT_EQ(0, ep->Wait(&io_head, &to_head, 200));
    list_for_each_poll_link_autodetach(ev, &to_head) {
	cnt++;
	delete ev;
    }}}
    EXPECT_EQ(2, cnt);
    EXPECT_TRUE(list_empty(&io_head));
    detach_for_each_poll_link(&io_head);
    detach_for_each_poll_link(&to_head);


    
    close(rfd);
    close(wfd);
    delete ep;
    return;
}



TEST(epoll, pipe) {
    epoll_pipe();
}






static volatile int status2 = 0;

static int tcp_server2(void *arg_) {
    TCPListener *listen;
    TCPConn *conn;
    int n;
    char buf[1024] = "haha";

    listen = ListenTCP("tcp", "*:20002", 100);
    listen->SetReuseAddr(true);
    if (listen == NULL)
	return -1;
    status2 = 1;
    conn = listen->Accept();
    conn->SetSockOpt(SO_NODELAY, 1);
    n = conn->Read(buf, 1024);
    conn->Write(buf, n);
    conn->Close();
    delete conn;
    delete listen;

    return 0;
}


static void epoll_test_rdhup(void *arg_) {
    TCPConn *conn;
    Thread svr;
    char buf[5] = {};
    Epoller *poller;
    EpollEvent ev, *ee = NULL;
    struct list_head io_head, to_head;

    INIT_LIST_HEAD(&io_head);
    INIT_LIST_HEAD(&to_head);

    status2 = 0;
    svr.Start(tcp_server2, NULL);
    while (!status2) {
	/* void */
    }
    
    poller = EpollCreate(500, 10);
    ASSERT_TRUE(poller != NULL);
    conn = DialTCP("tcp", "", "127.0.0.1:20002");
    conn->SetSockOpt(SO_NODELAY, 1);
    ASSERT_TRUE(conn != NULL);

    ev.SetEvent(conn->Fd(), EPOLLIN|EPOLLRDHUP, NULL);
    poller->CtlAdd(&ev);

    conn->Write(buf, 5);
    poller->Wait(&io_head, &to_head, 100);
    EXPECT_TRUE(!list_empty(&io_head));
    EXPECT_TRUE(list_empty(&to_head));

    if (!list_empty(&io_head)) {
	ee = list_first_ev(&io_head);
	ee->detach();
	conn->Read(buf, 1024);
	EXPECT_TRUE(ee->happened & EPOLLIN);
    }
    detach_for_each_poll_link(&io_head);
    detach_for_each_poll_link(&to_head);

    
    if (!(ee->happened & EPOLLRDHUP)) {
	poller->Wait(&io_head, &to_head, 100);
	EXPECT_TRUE(!list_empty(&io_head));
	EXPECT_TRUE(list_empty(&to_head));
	if (!list_empty(&io_head)) {
	    ee = list_first_ev(&io_head);
	    ee->detach();
	    EXPECT_TRUE(ee->happened & EPOLLRDHUP);
	}
	detach_for_each_poll_link(&io_head);
	detach_for_each_poll_link(&to_head);
    }

    svr.Stop();
    poller->CtlDel(&ev);
    delete poller;
    delete conn;
    return;
}


TEST(epoll, rdhup) {
    epoll_test_rdhup(NULL);
}


static volatile int status4 = 0;
static volatile int canexit = 0;


static int tcp_server4(void *arg_) {
    TCPListener *listen;
    TCPConn *conn;

    listen = ListenTCP("tcp", "*:20003", 100);
    listen->SetReuseAddr(true);
    if (listen == NULL)
	return -1;
    status4 = 1;
    conn = listen->Accept();
    while (!canexit) {
	/* void */
    }
    
    delete listen;
    delete conn;
    return 0;
}



static void epoll_test_epollctl(void *arg_) {
    TCPConn *conn;
    Thread svr;
    int cnt = 2;
    Epoller *poller;
    EpollEvent ev, *ee;
    struct list_head io_head, to_head;

    INIT_LIST_HEAD(&io_head);
    INIT_LIST_HEAD(&to_head);

    status4 = 0;
    svr.Start(tcp_server4, NULL);
    while (!status4) {
	/* waiting... */
    }
    
    poller = EpollCreate(500, 10);
    ASSERT_TRUE(poller != NULL);
    conn = DialTCP("tcp", "", "127.0.0.1:20003");
    ASSERT_TRUE(conn != NULL);


    cnt = 2;
    ev.fd = conn->Fd();
    ev.events = EPOLLIN|EPOLLOUT;
    poller->CtlAdd(&ev);
    while (cnt--) {
	poller->Wait(&io_head, &to_head, 100);
	EXPECT_TRUE(!list_empty(&io_head));
	if (!list_empty(&io_head)) {
	    ee = list_first_ev(&io_head);
	    ee->detach();
	    EXPECT_TRUE(ee->happened & EPOLLOUT);
	}
	detach_for_each_poll_link(&io_head);
	detach_for_each_poll_link(&to_head);
    }

    cnt = 2;
    ev.events &= ~EPOLLOUT;
    poller->CtlMod(&ev);
    while (cnt--) {
	poller->Wait(&io_head, &to_head, 100);
	EXPECT_TRUE(list_empty(&io_head));
	detach_for_each_poll_link(&io_head);
	detach_for_each_poll_link(&to_head);
    }

    cnt = 2;
    ev.events |= EPOLLOUT;
    poller->CtlMod(&ev);
    while (cnt--) {
	poller->Wait(&io_head, &to_head, 100);
	EXPECT_TRUE(!list_empty(&io_head));
	if (!list_empty(&io_head)) {
	    ee = list_first_ev(&io_head);
	    ee->detach();
	    EXPECT_TRUE(ee->happened & EPOLLOUT);
	}
	detach_for_each_poll_link(&io_head);
	detach_for_each_poll_link(&to_head);
    }

    canexit = 1;

    EXPECT_TRUE(0 == poller->CtlDel(&ev));
    svr.Stop();
    delete conn;
    delete poller;
    return;
}


TEST(epoll, modevents) {
    epoll_test_epollctl(NULL);
}


static void epoll_test_puretimeout(void *arg_) {
    Thread svr;
    Epoller *poller;
    EpollEvent ev, *ee;
    struct list_head io_head, to_head;

    INIT_LIST_HEAD(&io_head);
    INIT_LIST_HEAD(&to_head);

    poller = EpollCreate(500, 10);
    ASSERT_TRUE(poller != NULL);

    ev.fd = -1;
    ev.to_nsec = (int64_t)100 * 1000000; // 100ms
    poller->CtlAdd(&ev);
    poller->Wait(&io_head, &to_head, 1); // here wait 10ms
    EXPECT_TRUE(list_empty(&to_head));
    EXPECT_TRUE(list_empty(&io_head));
    detach_for_each_poll_link(&io_head);
    detach_for_each_poll_link(&to_head);

    
    usleep(500000); // 100ms
    poller->Wait(&io_head, &to_head, 1);
    EXPECT_TRUE(list_empty(&io_head));
    EXPECT_TRUE(!list_empty(&to_head));
    if (!list_empty(&to_head)) {
	ee = list_first_ev(&to_head);
	ee->detach();
	EXPECT_TRUE(ee->happened & EPOLLTIMEOUT);
    }
    detach_for_each_poll_link(&io_head);
    detach_for_each_poll_link(&to_head);


    
    poller->CtlDel(&ev);
    delete poller;
    return;
}


TEST(epoll, puretimeout) {
    epoll_test_puretimeout(NULL);
}

