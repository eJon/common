// copyright:
//            (C) SINA Inc.
//
//           file: nspio/service/benchmark/exp_mode.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <nspio/errno.h>
#include "os/time.h"
#include "log.h"
#include "base/crc.h"
#include "runner/thread.h"
#include "os/epoll.h"
#include "sync_api.h"
#include "benchmark.h"
#include "benchmark_modstat.h"


using namespace nspio;


int nspio_except_server(void *arg_) {
    Epoller *poller = NULL;
    EpollEvent ee;
    struct list_head io_head, to_head;
    __comsumer server;
    string req, rt;
    int fd, i = 0, ret = 0, pkgs = 1000;

    INIT_LIST_HEAD(&io_head);
    INIT_LIST_HEAD(&to_head);

    if (!(poller = EpollCreate(1024, 500))) {
	NSPIOLOG_ERROR("epoll create with errno %d\n", errno);
	return -1;
    }
    if ((ret = server.Connect(appname, nspiosvrhost)) < 0) {
	delete poller;
	NSPIOLOG_ERROR("server connect to %s with errno %d\n", nspiosvrhost.c_str(), errno);
	return -1;
    }
    fd = server.Fd();
    ee.SetEvent(fd, EPOLLIN|EPOLLRDHUP, &server);
    poller->CtlAdd(&ee);

    while (1) {
	for (i = 0; i < pkgs; i++) {
	    if (poller->Wait(&io_head, &to_head, 1) < 0)
		continue;
	    if (!list_empty(&io_head) && (0 == server.Recv(req, rt))) {
		detach_for_each_poll_link(&io_head);
		detach_for_each_poll_link(&to_head);
		server.Send(req, rt);
		req.clear();
		rt.clear();
	    }
	    if (!list_empty(&io_head) && (ee.happened & (EPOLLERR|EPOLLRDHUP))) {
		poller->CtlDel(&ee);
		server.Close();
		while (server.Connect(appname, nspiosvrhost) < 0)
		    usleep(100000);
		ee.SetEvent(server.Fd(), EPOLLIN|EPOLLRDHUP, &server);
		poller->CtlAdd(&ee);
		detach_for_each_poll_link(&io_head);
		detach_for_each_poll_link(&to_head);
	    }
	}
	if (rand() % 239 == 0)
	    write(fd, buffer, rand() % buflen);
    }

    delete poller;
    return 0;
}



int nspio_except_client(void *arg_) {
    Epoller *poller = NULL;
    EpollEvent ee;
    struct list_head io_head, to_head;
    __producer client;

    int i, fd = 0;
    Msghdr hdr = {};
    string req, resp;
    int64_t stt, ett, randcnt = 0;
    benchmark_module_stat_trigger mstrigger;
    module_stat stat(BENCHMARK_MODULE_STAT_KEYRANGE, &mstrigger);

    INIT_LIST_HEAD(&io_head);
    INIT_LIST_HEAD(&to_head);

    if (!(poller = EpollCreate(1024, 500))) {
	NSPIOLOG_ERROR("epoll create with errno %d\n", errno);
	return -1;
    }
    if (client.Connect(appname, nspioclihost) < 0) {
	delete poller;
	NSPIOLOG_ERROR("client connect to %s with errno %d\n", nspioclihost.c_str(), errno);
	return -1;
    }
    client.SetOption(OPT_NONBLOCK, 1);
    stt = rt_mstime();
    ett = stt + g_time * 1000;

    fd = client.Fd();
    ee.SetEvent(fd, EPOLLIN|EPOLLRDHUP, &client);
    poller->CtlAdd(&ee);

    client.Recv(&hdr, resp);
    while (1) {
	randcnt = rand() % 100;
	for (i = 0; i < randcnt; i++) {
	    req.clear();
	    req.assign(buffer, rand() % g_size);
	    hdr.timestamp = rt_mstime();
	    if (g_check == "yes")
		hdr.checksum = crc16(req.data(), req.size());
	    if (client.Send(&hdr, req) == 0)
		stat.incrkey(BC_SENDPKG);
	}
	for (i = 0; i < randcnt; i++) {
	    ee.happened = 0;
	    if (poller->Wait(&io_head, &to_head, 1) < 0)
		continue;
	    if (!list_empty(&io_head))
		ee.detach();
	    if ((ee.happened & (EPOLLERR|EPOLLRDHUP))) {
		poller->CtlDel(&ee);
		client.Close();
		while (client.Connect(appname, nspioclihost) < 0)
		    usleep(100000);
		ee.SetEvent(client.Fd(), EPOLLIN|EPOLLRDHUP, &client);
		poller->CtlAdd(&ee);
		stat.incrkey(BC_RECONNECT);
		break;
	    }
	    if (ee.happened & EPOLLIN && client.Recv(&hdr, resp) == 0) {
		if (g_check == "yes" && hdr.checksum != crc16(resp.data(), resp.size()))
		    stat.incrkey(BC_CHECKSUM_ERROR);
		stat.incrkey(BC_RECVPKG);
		stat.incrkey(BC_RTT, rt_mstime() - hdr.timestamp);
		resp.clear();
	    }
	    if (rand() % 239 == 0)
		write(fd, buffer, rand() % buflen);
	}
	stat.update_timestamp(rt_mstime());
    }

    delete poller;
    return 0;
}



