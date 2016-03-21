// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/test/apicompat_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <gtest/gtest.h>
#include <nspio/errno.h>
#include <nspio/compat_api.h>
#include "sync_api.h"
#include "log.h"
#include "base/crc.h"
#include "runner/thread.h"
#include "appctx.h"

using namespace nspio;


#define buflen 1024
#define cspioapi_stats(api, __stat) do {			\
	__comsumer *c = (__comsumer *)(api)->get_comsumer();	\
	__producer *p = (__producer *)(api)->get_producer();	\
	if (c)							\
	    __stat =  c->Stat();				\
	else if (p)						\
	    __stat = p->Stat();					\
    } while (0)

static char buffer[buflen] = {};
static string appname = "testapp";
static CtxConf app_conf;


static int randstr(char *buf, int len) {
    int i, idx;
    char token[] = "qwertyuioplkjhgfdsazxcvbnm1234567890";
    for (i = 0; i < len; i++) {
	idx = rand() % strlen(token);
	buf[i] = token[idx];
    }
    return 0;
}

static volatile int agstarted = 0, rmstarted = 0;
static int cnt = 20;

// test compatapi for CSpioApi
// send(const string &msg, int timeout);
// recv(string &msg, int timeout);

static volatile int s1_status = 0;
static int app_server1(void *arg_) {
    CSpioApi app;
    module_stat *stat = NULL;

    while (!agstarted || !rmstarted)
	usleep(10);
    app.init(appname);
    if (app.join_server("127.0.0.1:20010") < 0) {
	NSPIOLOG_ERROR("join server with errno %d", errno);
	return -1;
    }
    s1_status = 1;
    cspioapi_stats(&app, stat);
    while (stat->getkey(API_RECV_PACKAGES) < cnt) {
	string msg;
	if (app.recv(msg, 1000) == 0)
	    app.send(msg, 1000);
    }
    app.terminate();
    s1_status = 0;
    return 0;
}


static int app_client1(void *arg_) {
    int ret = 0;
    int64_t last_recv = 0;
    CSpioApi app;
    module_stat *stat = NULL;

    while (!agstarted || !rmstarted || !s1_status)
	usleep(10);
    app.init(appname);
    if (app.join_client("127.0.0.1:20010") < 0) {
	NSPIOLOG_ERROR("join client with errno %d", errno);
	return -1;
    }
    cspioapi_stats(&app, stat);
    while (stat->getkey(API_SEND_PACKAGES) < cnt) {
	string req, resp;
	while (app.send(req, 1000) != 0)
	    NSPIOLOG_ERROR("compatapi send with errno %d", errno);
	last_recv = stat->getkey(API_RECV_PACKAGES);
	while ((ret = app.recv(resp, 1000)) != 0 && stat->getkey(API_RECV_PACKAGES) == last_recv)
	    NSPIOLOG_ERROR("compatapi recv with errno %d", errno);
	if (ret == 0)
	    EXPECT_TRUE(req == resp);
    }
    app.terminate();
    return 0;
}


// test ramdome msg
static volatile int s2_status = 0;
static int app_server2(void *arg_) {
    CSpioApi app;
    module_stat *stat = NULL;

    while (!agstarted || !rmstarted)
	usleep(10);
    app.init(appname);
    if (app.join_server("127.0.0.1:20010") < 0) {
	NSPIOLOG_ERROR("join server with errno %d", errno);
	return -1;
    }
    s2_status = 1;
    cspioapi_stats(&app, stat);
    while (stat->getkey(API_RECV_PACKAGES) < cnt) {
	string msg;
	if (app.recv(msg, 1000) != 0) {
	    NSPIOLOG_ERROR("compatapi recv with errno %d", errno);
	    continue;
	}
	while (app.send(msg, 1000) != 0)
	    NSPIOLOG_ERROR("compatapi send with errno %d", errno);
    }
    app.terminate();
    s2_status = 0;
    return 0;
}


static int app_client2(void *arg_) {
    int ret = 0;
    int64_t last_recv = 0;
    CSpioApi app;
    module_stat *stat = NULL;

    while (!agstarted || !rmstarted || !s2_status)
	usleep(10);

    app.init(appname);
    if (app.join_client("127.0.0.1:20010") < 0) {
	NSPIOLOG_ERROR("join client with errno %d", errno);
	return -1;
    }
    cspioapi_stats(&app, stat);
    while (stat->getkey(API_SEND_PACKAGES) < cnt) {
	string req, resp;
	req.assign(buffer, rand() % buflen);
	while (app.send(req, 1000) != 0)
	    NSPIOLOG_ERROR("compatapi send with errno %d", errno);
	last_recv = stat->getkey(API_RECV_PACKAGES);
	while ((ret = app.recv(resp, 1000)) != 0 && stat->getkey(API_RECV_PACKAGES) == last_recv)
	    NSPIOLOG_ERROR("compatapi recv with errno %d", errno);
	if (ret == 0)
	    EXPECT_TRUE(req == resp);
    }
    app.terminate();
    return 0;
}


// test app_client timeout
static volatile int s3_status = 0;
static int app_server3(void *arg_) {
    CSpioApi app;
    string msg;

    while (!agstarted || !rmstarted)
	usleep(10);
    app.init(appname);
    if (app.join_server("127.0.0.1:20010") < 0) {
	NSPIOLOG_ERROR("join server with errno %d", errno);
	return -1;
    }
    s3_status = 1;
    EXPECT_TRUE(app.recv(msg, 1000) == 0);
    app.terminate();
    s3_status = 0;
    return 0;
}


    

static int app_client3(void *arg_) {
    CSpioApi app;
    string req, resp;
    
    while (!agstarted || !rmstarted || !s3_status)
	usleep(10);

    app.init(appname);
    if (app.join_client("127.0.0.1:20010") < 0) {
	NSPIOLOG_ERROR("join client with errno %d", errno);
	return -1;
    }
    req.assign(buffer, rand() % buflen);
    EXPECT_TRUE(app.send(req, 1) == 0);
    EXPECT_TRUE(app.recv(resp, 1) == -1 && errno == EAGAIN);
    app.terminate();
    return 0;
}


// test app_server timeout
static volatile int s4_status = 0;
static int app_server4(void *arg_) {
    CSpioApi app;
    string msg;

    while (!agstarted || !rmstarted)
	usleep(10);
    app.init(appname);
    if (app.join_server("127.0.0.1:20010") < 0) {
	NSPIOLOG_ERROR("join server with errno %d", errno);
	return -1;
    }
    s4_status = 1;
    EXPECT_TRUE(app.recv(msg, 1000) == 0);
    EXPECT_TRUE(app.recv(msg, 1) == -1 && errno == EAGAIN);
    app.terminate();
    s4_status = 0;
    return 0;
}

static int app_client4(void *arg_) {
    CSpioApi app;
    string req, resp;
    
    while (!agstarted || !rmstarted || !s4_status)
	usleep(10);

    app.init(appname);
    if (app.join_client("127.0.0.1:20010") < 0) {
	NSPIOLOG_ERROR("join client with errno %d", errno);
	return -1;
    }
    req.assign(buffer, rand() % buflen);
    usleep(10);
    EXPECT_TRUE(app.send(req, 1000) == 0);
    EXPECT_TRUE(app.recv(resp, 1) == -1 && errno == EAGAIN);
    app.terminate();
    return 0;
}


// test nspiodown timeout
static volatile int s5_started = 0;
static volatile int c5_started = 0;
static volatile int ag_stopped = 0;
static int app_server5(void *arg_) {
    CSpioApi app;
    string msg;

    while (!agstarted || !rmstarted)
	usleep(10);
    app.init(appname);
    if (app.join_server("127.0.0.1:20010") < 0) {
	NSPIOLOG_ERROR("join server with errno %d", errno);
	return -1;
    }
    s5_started = 1;
    EXPECT_TRUE(app.recv(msg, 1000) == 0);
    while (!ag_stopped)
	usleep(1000);
    while (app.recv(msg, 1) != 0)
	usleep(1000);
    while (app.send(msg, 1000) != 0)
	usleep(1000);
    while (c5_started)
	usleep(1000);
    app.terminate();
    return 0;
}

static int app_client5(void *arg_) {
    CSpioApi app;
    string req, resp;
    
    while (!agstarted || !rmstarted || !s5_started)
	usleep(10);

    app.init(appname);
    if (app.join_client("127.0.0.1:20010") < 0) {
	NSPIOLOG_ERROR("join client with errno %d", errno);
	return -1;
    }
    req.assign(buffer, rand() % buflen);
    EXPECT_TRUE(app.send(req, 1000) == 0);
    c5_started = 1;
    while (!ag_stopped)
	usleep(1000);
    while (app.send(req, 1) != 0)
	usleep(1000);
    while (app.recv(resp) != 0)
	usleep(1000);
    c5_started = 0;
    app.terminate();
    return 0;
}


// test app_server timeout
static volatile int s6_status = 0;
static int app_server6(void *arg_) {
    CSpioApi app;
    string msg;

    while (!agstarted || !rmstarted)
	usleep(10);
    app.init(appname);
    if (app.join_server("127.0.0.1:20010") < 0) {
	NSPIOLOG_ERROR("join server with errno %d", errno);
	return -1;
    }
    s6_status = 1;
    EXPECT_TRUE(app.recv(msg, 50) == 0);
    usleep(100000); // sleep 100ms
    msg.clear();
    EXPECT_TRUE(app.recv(msg, 1000) == 0);
    EXPECT_TRUE(app.send(msg, 1000) == 0);
    EXPECT_TRUE(app.recv(msg, 50) == -1 && errno == EAGAIN);
    app.terminate();
    s6_status = 0;
    return 0;
}

static int app_client6(void *arg_) {
    uint16_t checksum = 0;
    CSpioApi app;
    string req, resp;
    
    while (!agstarted || !rmstarted || !s6_status)
	usleep(10);

    app.init(appname);
    if (app.join_client("127.0.0.1:20010") < 0) {
	NSPIOLOG_ERROR("join client with errno %d", errno);
	return -1;
    }
    req.assign(buffer, rand() % buflen);
    EXPECT_TRUE(app.send(req, 50) == 0);
    EXPECT_TRUE(app.recv(resp, 50) == -1 && errno == EAGAIN);
    req.assign(buffer, rand() % buflen);
    checksum = crc16(req.data(), req.size());
    EXPECT_TRUE(app.send(req, 50) == 0);
    EXPECT_TRUE(app.recv(resp, 200) == 0);
    EXPECT_TRUE(checksum == crc16(resp.data(), resp.size()));
    app.terminate();
    return 0;
}


// test OPT_KEEPORDER
static volatile int s7_status = 0;
static int app_server7(void *arg_) {
    CSpioApi app;
    string msg;

    while (!agstarted || !rmstarted)
	usleep(10);
    app.init(appname);
    if (app.join_server("127.0.0.1:20010") < 0) {
	NSPIOLOG_ERROR("join server with errno %d", errno);
	return -1;
    }
    s7_status = 1;
    while (s7_status) {
	if (app.recv(msg, 1000) == 0)
	    app.send(msg, 1000);
    }
    app.terminate();
    return 0;
}

static int app_client7(void *arg_) {
    int i, snd, ret;
    stringstream ss;
    CSpioApi app;
    string req, resp, tmp;
    
    while (!agstarted || !rmstarted || !s7_status)
	usleep(10);

    app.init(appname);
    if (app.join_client("127.0.0.1:20010") < 0) {
	NSPIOLOG_ERROR("join client with errno %d", errno);
	return -1;
    }
    for (i = 0; i < cnt; i++) {
	snd = (rand() % 4) + 1;
	while (snd) {
	    tmp.assign(buffer, rand() % 10);
	    if (app.send(tmp) == 0)
		req = tmp;
	    snd--;
	}
	while ((ret = app.recv(resp)) < 0 && errno == EAGAIN)
	    {}
	if (ret == 0)
	    EXPECT_TRUE(req == resp);
    }
    s7_status = 0;
    app.terminate();
    return 0;
}


static int test_compatapi(void *arg) {
    AppCtx *ag = new AppCtx();
    Rgm *rgm = NewRegisterManager(NULL);
    Thread t[4];

    app_conf.appid = appname;
    ag->InitFromConf(&app_conf);
    ag->EnableRegistry(rgm);
    rgm->Listen("*:20010");
    rgm->StartThread();
    ag->StartThread();
    while (!ag->Running())
	usleep(10);
    while (!rgm->Running())
	usleep(10);
    agstarted = rmstarted = 1;

    t[2].Start(app_server1, NULL);
    t[3].Start(app_client1, NULL);
    t[2].Stop();
    t[3].Stop();

    t[2].Start(app_server2, NULL);
    t[3].Start(app_client2, NULL);
    t[2].Stop();
    t[3].Stop();

    t[2].Start(app_server3, NULL);
    t[3].Start(app_client3, NULL);
    t[2].Stop();
    t[3].Stop();

    t[2].Start(app_server4, NULL);
    t[3].Start(app_client4, NULL);
    t[2].Stop();
    t[3].Stop();

    t[2].Start(app_server6, NULL);
    t[3].Start(app_client6, NULL);
    t[2].Stop();
    t[3].Stop();

    t[2].Start(app_server7, NULL);
    t[3].Start(app_client7, NULL);
    t[2].Stop();
    t[3].Stop();
    
    t[2].Start(app_server5, NULL);
    t[3].Start(app_client5, NULL);
    
    while (!c5_started || !s5_started)
	usleep(1);
    rgm->StopThread();
    ag->StopThread();
    delete ag;
    delete rgm;
    ag_stopped = 1;

    // ag restart
    rgm = NewRegisterManager(NULL);
    ag = new AppCtx();
    ag->InitFromConf(&app_conf);
    ag->EnableRegistry(rgm);
    rgm->Listen("*:20010");
    rgm->StartThread();
    ag->StartThread();

    t[2].Stop();
    t[3].Stop();

    rgm->StopThread();
    ag->StopThread();
    delete ag;    
    delete rgm;

    return 0;
}


TEST(api, compat) {
    randstr(buffer, buflen);
    test_compatapi(NULL);
}
