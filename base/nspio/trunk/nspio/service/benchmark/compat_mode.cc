// copyright:
//            (C) SINA Inc.
//
//           file: nspio/service/benchmark/compat_mode.cc
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
#include <nspio/compat_api.h>
#include "os/time.h"
#include "log.h"
#include "base/crc.h"
#include "runner/thread.h"
#include "os/epoll.h"
#include "benchmark.h"

using namespace nspio;

int nspio_compat_server(void *arg_) {
    CSpioApi app;
    string msg;
    uint32_t cur_recv = 0;
    int64_t start_tt = 0, cur_tt = 0;

    start_tt = rt_mstime();
    app.init(appname);
    if (app.join_server(nspiosvrhost) < 0) {
	printf("server connect nspio svr failed\n");
	return -1;
    }
    while (!stopping) {
	if (app.recv(msg) == 0) {
	    cur_recv++;
	    if (cur_recv % 10000 == 0 || (cur_tt = rt_mstime() - start_tt) > 1000) {
		printf("server cur recv %d\n", cur_recv);
		if (cur_tt > 1000)
		  start_tt = rt_mstime();
	    }
	    usleep(20000);
	    if (app.send(msg) != 0)
		printf("server send: %d\n", errno);
	    msg.clear();
	} else
	    printf("server recv: %d\n", errno);
    }
    app.terminate();
    return 0;
}






int nspio_compat_client(void *arg_) {
    CSpioApi app;
    int64_t start_tt = 0, end_tt = 0;
    int cur_send = 0, cur_recv = 0;

    app.init(appname);
    if (app.join_client(nspioclihost) < 0) {
	printf("client connect nspio svr failed\n");
	return 0;
    }
    end_tt = rt_mstime() + g_time;
    start_tt = rt_mstime();

    while (rt_mstime() < end_tt) {
	string req, resp;
	req.assign(buffer, rand() % g_size);
	if (app.send(req, 100) != 0) {
	    printf("client send errno: %d\n", errno);
	    continue;
	}
	cur_send++;
	if (app.recv(resp, 100) != 0) {
	    printf("client recv errno: %d\n", errno);
	    continue;
	}
	cur_recv++;
	if (req != resp) {
	    printf("client recv wrong massage\n");
	}
    }
    sleep(2);
    app.terminate();
    return 0;
}

