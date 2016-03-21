// copyright:
//            (C) SINA Inc.
//
//           file: nspio/service/benchmark/sync_mode.cc
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


struct my_msg {
    int64_t msg_timestamp;
    int64_t msg_checksum;
};

class MyApp {
public:
    MyApp() {}
    ~MyApp() {}

    __producer client;
    EpollEvent ee;
};


int nspio_sync_server(void *arg_) {

    Epoller *poller = NULL;
    EpollEvent ee;
    struct list_head io_head, to_head;
    
    __comsumer server;
    struct appmsg req = {};
    struct my_msg *_my_msg = NULL;
    int ret = 0;
    
    INIT_LIST_HEAD(&io_head);
    INIT_LIST_HEAD(&to_head);

    if (!(poller = EpollCreate(1024, 500))) {
	NSPIOLOG_ERROR("epoll create with errno %d\n", errno);
	return -1;
    }
    if ((ret = server.Connect(appname, nspiosvrhost)) < 0) {
	delete poller;
	return -1;
    }
    ee.SetEvent(server.Fd(), EPOLLIN|EPOLLRDHUP, &server);
    poller->CtlAdd(&ee);
    server.SetOption(OPT_NONBLOCK, 1);

    while (!stopping) {
	ee.happened = 0;
	if ((ret = poller->Wait(&io_head, &to_head, 1)) < 0) {
	    printf("epoll wait with errno %d\n", errno);
	    break;
	}
	if (ee.happened & EPOLLRDHUP) {
	    printf("nspio benchmark appserver EPOLLRDHUP\n");
	    poller->CtlDel(&ee);
	    server.Close();
	    while (server.Connect(appname, nspiosvrhost) < 0)
		usleep(100000);
	    ee.SetEvent(server.Fd(), EPOLLIN|EPOLLRDHUP, &server);
	    poller->CtlAdd(&ee);
	}
	if (ee.happened & EPOLLIN) {
	    if ((ret = server.Recv(&req)) == 0) {
		_my_msg = (struct my_msg *)req.s.data;
		server.Send(&req);
		free(req.s.data);
		free(req.rt);
	    } else if (ret < 0 && errno != EAGAIN) {
		poller->CtlDel(&ee);
		server.Close();
		while (server.Connect(appname, nspiosvrhost) < 0)
		    usleep(100000);
		ee.SetEvent(server.Fd(), EPOLLIN|EPOLLRDHUP, &server);
		poller->CtlAdd(&ee);
	    }
	}
	detach_for_each_poll_link(&io_head);
	detach_for_each_poll_link(&to_head);
    }
    delete poller;
    return 0;
}






int nspio_sync_client(void *arg_) {

    __producer *client = NULL;
    benchmark_module_stat_trigger mstrigger;
    module_stat stat(BENCHMARK_MODULE_STAT_KEYRANGE, &mstrigger);
    MyApp *app = NULL;
    Epoller *poller = NULL;
    EpollEvent *ev = NULL;
    struct list_head io_head, to_head;
    struct list_link *elpos = NULL, *elnxt = NULL;
    
    vector<MyApp *> apps;
    vector<MyApp *>::iterator it;
    
    string req, resp;
    Msghdr hdr = {};
    int i, ret = 0;
    int64_t start_tt = 0, end_tt = 0;
    
    INIT_LIST_HEAD(&io_head);
    INIT_LIST_HEAD(&to_head);
    stat.batch_set_threshold(1);
    if (!(poller = EpollCreate(1024, 500))) {
	NSPIOLOG_ERROR("epoll create with errno %d\n", errno);
	return -1;
    }

    for (i = 0; i < g_clients; i++) {
	app = new MyApp();
	apps.push_back(app);
	client = &app->client;
	while (client->Connect(appname, nspioclihost) < 0)
	    usleep(10);
	client->SetOption(OPT_NONBLOCK, 1);
    }
    start_tt = rt_mstime();
    end_tt = start_tt + g_time;
    for (it = apps.begin(); it != apps.end(); ++it) {
	app = *it;
	ev = &app->ee;
	client = &app->client;
	ev->SetEvent(client->Fd(), EPOLLIN|EPOLLRDHUP, app);	    
	poller->CtlAdd(ev);

	req.clear();
	req.assign(buffer, rand() % g_size);
	hdr.timestamp = rt_mstime();
	if (g_check == "yes")
	    hdr.checksum = crc16(req.data(), req.size());
	client->Send(&hdr, req);
	stat.incrkey(BC_SENDPKG);
    }
    
    while (rt_mstime() < end_tt) {
	
	if ((poller->Wait(&io_head, &to_head, 1)) < 0 || list_empty(&io_head)) {
	    detach_for_each_poll_link(&to_head);
	    continue;
	}

	list_for_each_list_link_safe(elpos, elnxt, &io_head) {
	    ev = list_ev(elpos);
	    ev->detach();
	    app = (MyApp *)ev->ptr;
	    client = &app->client;

	    if (ev->happened & EPOLLRDHUP) {
		printf("client found nspio EPOLLRDHUP\n");
		poller->CtlDel(ev);
		client->Close();
		while (client->Connect(appname, nspioclihost) < 0)
		    rt_usleep(10000);
		client->SetOption(OPT_NONBLOCK, 1);
		continue;
	    }
	    if (ev->happened & EPOLLIN) {
		resp.clear();
		if ((ret = client->Recv(&hdr, resp)) == 0) {
		    stat.incrkey(BC_RECVPKG);
		    stat.incrkey(BC_RTT, rt_mstime() - hdr.timestamp);
		    if (g_check == "yes" && hdr.checksum != crc16(resp.data(), resp.size()))
			printf("client recv error checksum response of line:%d\n", __LINE__);
		    hdr.timestamp = rt_mstime();		    
		    client->Send(&hdr, resp);
		    stat.incrkey(BC_SENDPKG);
		    resp.clear();
		} else if (ret < 0 && errno != EAGAIN) {
		    poller->CtlDel(ev);
		    continue;
		}
	    }
	}
	stat.update_timestamp(rt_mstime());
	detach_for_each_poll_link(&io_head);
	detach_for_each_poll_link(&to_head);
    }

    // waiting rest package
    end_tt = rt_mstime() + 1000;

    while (rt_mstime() < end_tt) {
	if ((poller->Wait(&io_head, &to_head, 1)) < 0 || list_empty(&io_head)) {
	    detach_for_each_poll_link(&to_head);
	    continue;
	}

	list_for_each_poll_link_autodetach(ev, &io_head) {
	    app = (MyApp *)ev->ptr;
	    client = &app->client;

	    if (ev->happened & EPOLLRDHUP) {
		poller->CtlDel(ev);
		continue;
	    }
	    if (ev->happened & EPOLLIN) {
		resp.clear();
		if ((ret = client->Recv(&hdr, resp)) == 0) {
		    stat.incrkey(BC_RECVPKG);
		    stat.incrkey(BC_RTT, rt_mstime() - hdr.timestamp);
		    if (g_check == "yes" && hdr.checksum != crc16(resp.data(), resp.size())) {
			printf("client recv error checksum response of line:%d\n", __LINE__);
		    }
		    resp.clear();
		} else if (ret < 0 && errno != EAGAIN) {
		    poller->CtlDel(ev);
		    continue;
		}
	    }
	}}}

	detach_for_each_poll_link(&io_head);
	detach_for_each_poll_link(&to_head);
    }

    for (it = apps.begin(); it != apps.end(); ++it) {
	app = *it;
	ev = &app->ee;
	poller->CtlDel(ev);
    }

    for (it = apps.begin(); it != apps.end(); ++it)
	delete *it;
    delete poller;

    return 0;
}

