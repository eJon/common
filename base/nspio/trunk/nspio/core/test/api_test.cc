// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/test/api_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <gtest/gtest.h>
#include <nspio/errno.h>
#include "os/memalloc.h"
#include "base/crc.h"
#include "log.h"
#include "runner/thread.h"
#include "appctx.h"
#include "proto/icmp.h"
#include "sync_api.h"


using namespace nspio;

static string appname = "testapp";

#define buflen 1024
static char buffer[buflen] = {};
static string apphost = "127.0.0.1:20001";
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
static volatile int cnt = 100;


// test api for AppServer
//    int Recv(string &msg, string &rt);
//    int Send(string &msg, string &rt);
// test api for AppClient
//    int Send(string &msg);
//    int Recv(string &msg);

static volatile int s1_status = 0;
static int app_server1(void *arg_) {
    __comsumer server;
    module_stat *stat = server.Stat();
    
    while (!agstarted || !rmstarted) {
	/* waiting ... */
    }
    if (server.Connect(appname, apphost) < 0) {
	NSPIOLOG_ERROR("connect to testapp");
	return -1;
    }
    s1_status = 1;
    while (s1_status && stat->getkey(API_RECV_PACKAGES) < cnt) {
	string msg, rt;
	if (server.Recv(msg, rt) == 0)
	    server.Send(msg, rt);
    }
    server.Close();
    return 0;
}


static int app_client1(void *arg_) {
    __producer client;
    module_stat *stat = client.Stat();

    while (!agstarted || !rmstarted || !s1_status) {
	/* waiting ... */
    }
    if (client.Connect(appname, apphost) < 0) {
	NSPIOLOG_ERROR("connect to testapp");
	return -1;
    }
    while (stat->getkey(API_SEND_PACKAGES) < cnt) {
	string req;
	client.Send(req);
    }
    while (stat->getkey(API_RECV_PACKAGES) < cnt) {
	string resp;
	if (client.Recv(resp) == 0)
	    EXPECT_TRUE(resp.size() == 0);
    }
    s1_status = 0;
    client.Close();
    return 0;
}


// test api for AppServer
//    int Recv(Msghdr *, string &msg, string &rt);
//    int Send(const Msghdr *, string &msg, string &rt);
// test api for AppClient
//    int Send(const Msghdr *, string &msg);
//    int Recv(Msghdr *, string &msg);

static volatile int s3_status = 0;

static int app_server3(void *arg_) {
    Msghdr hdr = {};
    __comsumer server;
    module_stat *stat = server.Stat();
    
    while (!agstarted || !rmstarted) {
	/* waiting ... */
    }    
    if (server.Connect(appname, apphost) < 0) {
	NSPIOLOG_ERROR("connect to testapp");
	return -1;
    }
    s3_status = 1;
    while (stat->getkey(API_RECV_PACKAGES) < cnt) {
	string msg, rt;
	if (server.Recv(&hdr, msg, rt) == 0)
	    server.Send(&hdr, msg, rt);
    }
    server.Close();
    return 0;
}


static int app_client3(void *arg_) {
    int msglen;
    Msghdr hdr = {};
    string req, resp;
    __producer client;
    module_stat *stat = client.Stat();


    while (!agstarted || !rmstarted || !s3_status) {
	/* waiting ... */
    }
    if (client.Connect(appname, apphost) < 0) {
	NSPIOLOG_ERROR("connect to testapp");
	return -1;
    }

    while ((msglen = stat->getkey(API_SEND_PACKAGES)) < cnt) {
	req.clear();
	if (msglen > buflen)
	    msglen = buflen;
	req.assign(buffer, msglen);
	hdr.checksum = crc16(req.data(), req.size());
	client.Send(&hdr, req);
    }
    while (stat->getkey(API_RECV_PACKAGES) < cnt) {
	hdr.checksum = 0;
	resp.clear();
	if (client.Recv(&hdr, resp) == 0)
	    EXPECT_EQ(hdr.checksum, crc16(resp.data(), resp.size()));
    }
    client.Close();
    return 0;
}


// test api for AppServer
//    int Recv(struct appmsg *);
//    int Send(struct appmsg *);
// test api for AppClient
//    int Send(struct appmsg *);
//    int Recv(struct appmsg *);

static volatile int s5_status = 0;

static int app_server5(void *arg_) {
    __comsumer server;
    module_stat *stat = server.Stat();
    
    while (!agstarted || !rmstarted) {
	/* waiting ... */
    }    
    if (server.Connect(appname, apphost) < 0) {
	NSPIOLOG_ERROR("connect to testapp");
	return -1;
    }
    s5_status = 1;
    while (stat->getkey(API_RECV_PACKAGES) < cnt) {
	struct appmsg msg = {};
	if (server.Recv(&msg) == 0) {
	    server.Send(&msg);
	    if (msg.s.len)
		free(msg.s.data);
	    free(msg.rt);
	}
    }
    server.Close();
    return 0;
}


static int app_client5(void *arg_) {
    int ret;
    struct appmsg req = {};
    struct appmsg resp = {};
    __producer client;
    module_stat *stat = client.Stat();

    while (!agstarted || !rmstarted || !s5_status) {
	/* waiting ... */
    }
    if (client.Connect(appname, apphost) < 0) {
	NSPIOLOG_ERROR("connect to testapp");
	return -1;
    }

    while (stat->getkey(API_SEND_PACKAGES) < cnt) {
	req.s.len = 0;
	req.s.data = NULL;
	if ((ret = client.Send(&req)) < 0)
	    NSPIOLOG_ERROR("client send request error");
    }
    while (stat->getkey(API_RECV_PACKAGES) < cnt) {
	if (client.Recv(&resp) == 0) {
	    EXPECT_TRUE(NULL == resp.s.data);
	    EXPECT_TRUE(0 == resp.s.len);
	}
    }
    client.Close();
    return 0;
}




static int app_client6(void *arg_) {
    struct appmsg req = {};
    struct appmsg resp = {};
    __producer client;
    module_stat *stat = client.Stat();
    
    while (!agstarted || !rmstarted) {
	/* waiting ... */
    }
    if (client.Connect(appname, apphost) < 0) {
	NSPIOLOG_ERROR("connect to testapp");
	return -1;
    }
    while (stat->getkey(API_SEND_PACKAGES) < cnt) {
	req.s.len = 0;
	req.s.data = NULL;
	if (client.Send(&req) < 0)
	    NSPIOLOG_ERROR("client send request error");
    }
    while (stat->getkey(API_RECV_PACKAGES) < cnt) {
	memset(&resp, 0, sizeof(resp));
	if (client.Recv(&resp) == 0) {
	    EXPECT_TRUE(IS_APPICMP(&resp.hdr));
	    EXPECT_EQ(resp.s.len, sizeof(struct deliver_status_icmp) + RTLEN);
	    mem_free(resp.s.data, resp.s.len);
	}
    }
    client.Close();
    return 0;
}




// test api for AppServer
//    int Recv(string &msg, string &rt);
//    int Send(const char *data, uint32_t len, string &rt);
// test api for AppClient
//    int Send(string &msg);
//    int Send(const char *data, uint32_t len);

static volatile int s7_status = 0;

static int app_server7(void *arg_) {
    __comsumer server;
    module_stat *stat = server.Stat();
    
    while (!agstarted || !rmstarted) {
	/* waiting ... */
    }    
    if (server.Connect(appname, apphost) < 0) {
	NSPIOLOG_ERROR("connect to testapp");
	return -1;
    }
    s7_status = 1;
    while (stat->getkey(API_RECV_PACKAGES) < cnt) {
	string msg, rt;
	if (server.Recv(msg, rt) == 0)
	    EXPECT_TRUE(server.Send(msg.data(), msg.size(), rt) == 0);
    }
    server.Close();
    return 0;
}


static int app_client7(void *arg_) {
    __producer client;
    module_stat *stat = client.Stat();

    while (!agstarted || !rmstarted || !s7_status) {
	/* waiting ... */
    }
    if (client.Connect(appname, apphost) < 0) {
	NSPIOLOG_ERROR("connect to testapp");
	return -1;
    }

    while (stat->getkey(API_SEND_PACKAGES) < cnt) {
	string req;
	client.Send(req.data(), req.size());
    }
    while (stat->getkey(API_RECV_PACKAGES) < cnt) {
	string resp;
	if (client.Recv(resp) == 0)
	    EXPECT_TRUE(resp.size() == 0);
    }
    client.Close();
    return 0;
}


// test api for OPT_TIMEOUT

static volatile int s8_status = 0;

static int app_server8(void *arg_) {
    __comsumer server;
    int to_msec = 10;
    string req, rt;

    while (!agstarted || !rmstarted) {
	/* waiting ... */
    }
    if (server.Connect(appname, apphost) < 0) {
	NSPIOLOG_ERROR("connect to testapp");
	return -1;
    }
    s8_status = 1;
    EXPECT_TRUE(server.SetOption(OPT_TIMEOUT, to_msec) == 0);
    EXPECT_TRUE((server.Recv(req, rt) == -1 && errno == EAGAIN));
    server.Close();
    return 0;
}


static int app_client8(void *arg_) {
    __producer client;
    int to_msec = 10;
    string resp;

    while (!agstarted || !rmstarted || !s8_status) {
	/* waiting ... */
    }
    if (client.Connect(appname, apphost) < 0) {
	NSPIOLOG_ERROR("connect to testapp");
	return -1;
    }
    EXPECT_TRUE(client.SetOption(OPT_TIMEOUT, to_msec) == 0);
    EXPECT_TRUE((client.Recv(resp) == -1 && errno == EAGAIN));
    client.Close();
    return 0;
}


// test api for socket error
static volatile int s9_stopped = 0;
static volatile int c9_stopped = 0;
static volatile int s9_status = 0;

static int app_server9(void *arg_) {
    __comsumer server;
    int ret = 0, to_msec = 1000;
    string req, rt;

    while (!agstarted || !rmstarted) {
	/* waiting ... */
    }
    if (server.Connect(appname, apphost) < 0) {
	NSPIOLOG_ERROR("connect to testapp");
	return -1;
    }
    s9_status = 1;
    EXPECT_TRUE((ret = server.SetOption(OPT_TIMEOUT, to_msec)) == 0);
    if (ret != 0)
	NSPIOLOG_ERROR("errno %d", errno);
    EXPECT_TRUE((ret = server.Recv(req, rt)) == 0);
    if (ret != 0)
	NSPIOLOG_ERROR("errno %d", errno);
    EXPECT_TRUE((ret = server.Send(req, rt)) == 0);
    if (ret != 0)
	NSPIOLOG_ERROR("errno %d", errno);

    randstr(buffer, buflen);
    write(server.Fd(), buffer, buflen);
    s9_stopped = 1;
    while (agstarted)
	usleep(1);

    // fix here. testing bad socket error
    server.Close();
    return 0;
}


static int app_client9(void *arg_) {
    __producer client;
    string req, resp;
    int ret = 0, to_msec = 10000;


    while (!agstarted || !rmstarted || !s9_status) {
	/* waiting ... */
    }
    if (client.Connect(appname, apphost) < 0) {
	NSPIOLOG_ERROR("connect to testapp");
	return -1;
    }
    EXPECT_TRUE(client.SetOption(OPT_TIMEOUT, to_msec) == 0);
    req.assign(buffer, rand() % buflen);
    EXPECT_TRUE((ret = client.Send(req)) == 0);
    if (ret != 0)
	NSPIOLOG_ERROR("errno %d", errno);
    EXPECT_TRUE((ret = client.Recv(resp)) == 0);
    if (ret != 0)
	NSPIOLOG_ERROR("errno %d", errno);

    write(client.Fd(), buffer, buflen);
    c9_stopped = 1;
    while (agstarted)
	usleep(1);

    // fix here. testing bad socket error
    client.Close();
    return 0;
}


static int test_rawapi(void *arg) {
    AppCtx * ag = new AppCtx();
    Rgm *rgm = NewRegisterManager(NULL);
    Thread t[5];
    TaskPool tp;

    randstr(buffer, buflen);
    app_conf.appid = appname;
    app_conf.app_trigger_level = "RRCVPKG:s:1;RSNDPKG:s:1";
    tp.Setup(2);
    tp.Start();
    ag->InitFromConf(&app_conf);
    ag->Update(&app_conf);
    ag->EnableRegistry(rgm);
    rgm->Listen(apphost);
    rgm->StartThread();
    rmstarted = 1;
    ag->StartThread(&tp);
    agstarted = 1;
    
    t[2].Start(app_server1, NULL);
    t[3].Start(app_client1, NULL);
    t[2].Stop();
    t[3].Stop();
    NSPIOLOG_NOTICE("PASSED");

    t[2].Start(app_server3, NULL);
    t[3].Start(app_client3, NULL);
    t[2].Stop();
    t[3].Stop();
    NSPIOLOG_NOTICE("PASSED");
    
    t[2].Start(app_server5, NULL);
    t[3].Start(app_client5, NULL);
    t[2].Stop();
    t[3].Stop();
    NSPIOLOG_NOTICE("PASSED");
    
    t[2].Start(app_client6, NULL);
    t[2].Stop();
    NSPIOLOG_NOTICE("PASSED");
    
    t[2].Start(app_server7, NULL);
    t[3].Start(app_client7, NULL);
    t[2].Stop();
    t[3].Stop();
    NSPIOLOG_NOTICE("PASSED");
    
    t[2].Start(app_server8, NULL);
    t[3].Start(app_client8, NULL);
    t[2].Stop();
    t[3].Stop();
    NSPIOLOG_NOTICE("PASSED");

    t[2].Start(app_server9, NULL);
    t[3].Start(app_client9, NULL);

    while (!s9_stopped || !c9_stopped)
	usleep(1);
    rgm->StopThread();
    rmstarted = 0;
    ag->StopThread();

    delete ag;
    delete rgm;
    agstarted = 0;

    t[2].Stop();
    t[3].Stop();
    tp.Stop();
    NSPIOLOG_NOTICE("PASSED");

    return 0;
}




TEST(api, raw) {
    test_rawapi(NULL);
}

