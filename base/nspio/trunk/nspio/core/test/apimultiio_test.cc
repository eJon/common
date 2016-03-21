// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/test/apimux_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <gtest/gtest.h>
#include <iostream>
#include "log.h"
#include "appctx.h"
#include <nspio/multi_api.h>
#include <nspio/compat_api.h>
#include "sync_api.h"

using namespace std;
using namespace nspio;
static CtxConf app_conf;

#define buflen 1024
static char buffer[buflen] = {};
static int randstr(char *buf, int len) {
    int i, idx;
    char token[] = "qwertyuioplkjhgfdsazxcvbnm1234567890";
    for (i = 0; i < len; i++) {
	idx = rand() % strlen(token);
	buf[i] = token[idx];
    }
    return 0;
}
static Rgm *rgm = NULL;
static string host = "127.0.0.1:20014";
static string app1 = "ag1";
static AppCtx *ag1 = NULL;
static string app2 = "ag2";
static AppCtx *ag2 = NULL;
static string app3 = "ag3";
static AppCtx *ag3 = NULL;


static int start_network() {
    rgm = NewRegisterManager(NULL);
    ag1 = new AppCtx();
    ag2 = new AppCtx();
    ag3 = new AppCtx();

    if (rgm->Listen(host) < 0)
	return -1;
    app_conf.appid = app1;
    app_conf.msg_timeout_msec = 200;
    ag1->InitFromConf(&app_conf);
    ag1->EnableRegistry(rgm);

    app_conf.appid = app2;
    app_conf.msg_timeout_msec = 200;
    ag2->InitFromConf(&app_conf);
    ag2->EnableRegistry(rgm);

    app_conf.appid = app3;
    app_conf.msg_timeout_msec = 200;
    ag3->InitFromConf(&app_conf);
    ag3->EnableRegistry(rgm);

    rgm->StartThread();
    ag1->StartThread();
    ag2->StartThread();
    ag3->StartThread();
    while (!rgm->Running() || !ag1->Running() || !ag2->Running() || !ag3->Running())
	usleep(1000);
    return 0;
}

static int stop_network() {
    ag1->StopThread();
    ag2->StopThread();
    ag3->StopThread();
    rgm->StopThread();
    delete ag1;
    delete ag2;
    delete ag3;
    delete rgm;
    return 0;
}


enum {
    NET1 = 0x01,
    NET2 = 0x02,
    NET3 = 0x04,
    ALLNET = NET1|NET2|NET3,
};
static string errmsg("Error");
static int cnt = 20;

class query_result : public reqresp_ctx {
public:
    query_result() {
	doneflags = 0;
    }
    ~query_result() {}

    int request_come(const char *data, uint32_t len, MultiSender *s) {
	request.assign(data, len);
	doneflags |= NET1;
	s->Send(NET2, data, len, this, 10);
	s->Send(NET3, data, len, this, 10);
	return 0;
    }
    int back_response(ResponseWriter &w, bool bad) {
	if (!bad && (doneflags == ALLNET))
	    w.Send(response.data(), response.size());
	else {
	    response = request;
	    response.append(request);
	    w.Send(response.data(), response.size());
	}
	return 0;
    }
    int one_response_bad(int who, Error &ed) {
	return 0;
    }
    int one_response_done(int who, const char *data, uint32_t len) {
	doneflags |= who;
	response.append(data, len);
	return 0;
    }

private:
    int doneflags;
    string request, response;
};

class ms_reqresp_ctxfactor : public reqresp_ctxfactor {
public:
    reqresp_ctx *new_reqresp_ctx() {
	return new (std::nothrow) query_result();
    }
};

#define cspioapi_stats(api, __stat) do {			\
	__comsumer *c = (__comsumer *)(api)->get_comsumer();	\
	__producer *p = (__producer *)(api)->get_producer();	\
	if (c)							\
	    __stat =  c->Stat();				\
	else if (p)						\
	    __stat = p->Stat();					\
    } while (0)

static volatile bool client_stopped(false);
static int app_client1(void *arg_) {
    int ret = 0;
    int64_t last_recv = 0;
    CSpioApi client;
    string req, resp;
    module_stat *stat = NULL;

    client.init(app1);
    if (client.join_client(host) < 0) {
	NSPIOLOG_ERROR("client1 connect with errno %d", errno);
	return -1;
    }
    cspioapi_stats(&client, stat);
    while (stat->getkey(API_SEND_PACKAGES) < cnt) {
	req.assign(buffer, rand() % buflen);
	while (client.send(req) != 0)
	    NSPIOLOG_ERROR("client1 send with errno %d", errno);
	last_recv = stat->getkey(API_RECV_PACKAGES);
	while ((ret = client.recv(resp, 10)) != 0 && stat->getkey(API_RECV_PACKAGES) == last_recv)
	    NSPIOLOG_ERROR("client1 recv with errno %d", errno);
	if (ret == 0)
	    EXPECT_EQ(resp.size(), req.size() * 2);
    }
    client_stopped = true;
    return 0;
}


static int app_server2(void *arg_) {
    string msg, rt;
    CSpioApi server;
    module_stat *stat = NULL;

    server.init(app2);
    if (server.join_server(host) < 0) {
	NSPIOLOG_ERROR("server2 connect with errno %d", errno);
	return -1;
    }
    cspioapi_stats(&server, stat);
    while (!client_stopped && stat->getkey(API_RECV_PACKAGES) < cnt) {
	if (server.recv(msg, 10) != 0) {
	    NSPIOLOG_ERROR("server2 recv with errno %d", errno);
	    continue;
	}
	while (server.send(msg) != 0)
	    NSPIOLOG_ERROR("server2 send with errno %d", errno);
    }
    return 0;
}

static int app_server3(void *arg_) {
    string msg, rt;
    __comsumer server;
    module_stat *stat = server.Stat();

    if (server.Connect(app3, host) < 0) {
	NSPIOLOG_ERROR("server3 connect with errno %d", errno);
	return -1;
    }
    server.SetOption(OPT_NONBLOCK, 1);
    while (!client_stopped && stat->getkey(API_RECV_PACKAGES) < cnt) {
	if (server.Recv(msg, rt) != 0) {
	    NSPIOLOG_ERROR("server3 recv with errno %d", errno);
	    continue;
	}
	while (server.Send(msg, rt) != 0)
	    NSPIOLOG_ERROR("server3 send with errno %d", errno);
    }
    return 0;
}


static int test_multiio() {
    Multiio mio;
    ms_reqresp_ctxfactor f;
    async_conf inapp, outapp;
    Thread t[3];

    if (start_network() < 0) {
	NSPIOLOG_ERROR("start network with errno %d", errno);
	return -1;
    }
    inapp.set(app1, host, 1, 1, 10);
    mio.Init(&f, 2, inapp);
    outapp.set(app2, host, 1, 1, 10);
    mio.AddBackendServer(NET2, outapp);
    outapp.set(app3, host, 1, 1, 10);
    mio.AddBackendServer(NET3, outapp);
    mio.Start();

    t[0].Start(app_server2, NULL);
    t[1].Start(app_server3, NULL);
    t[2].Start(app_client1, NULL);
    t[0].Stop();
    t[1].Stop();
    t[2].Stop();
    
    mio.Stop();
    stop_network();
    return 0;
}



TEST(api, multiio) {
    randstr(buffer, buflen);
    test_multiio();
}
