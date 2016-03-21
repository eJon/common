// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/test/appctx_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <gtest/gtest.h>
#include "appctx.h"
#include "config.h"
#include "sync_api.h"

using namespace nspio;

#define buflen 1024
static char buffer[buflen] = {};
static string appname = "testapp";
static string apphost = "127.0.0.1:20002";
static string apphost2 = "127.0.0.1:20003";
static volatile int agstarted = 0, rmstarted = 0;
static volatile int cnt = 100;


static int randstr(char *buf, int len) {
    int i, idx;
    char token[] = "qwertyuioplkjhgfdsazxcvbnm1234567890";
    for (i = 0; i < len; i++) {
	idx = rand() % strlen(token);
	buf[i] = token[idx];
    }
    return 0;
}


static volatile int _stopping;
static int app_server1(void *arg_) {
    __comsumer server;
    struct appmsg req = {};

    while (!agstarted || !rmstarted) {
	/* waiting ... */
    }
    while (!_stopping) {
	switch (rand() % 4) {
	case 0:
	    if (server.Recv(&req) == 0) {
		server.Send(&req);
		if (req.s.len)
		    free(req.s.data);
		free(req.rt);
	    }
	    break;
	case 1:
	    server.Close();
	    break;
	case 2:
	    server.Connect(appname, apphost2);
	    server.SetOption(OPT_NONBLOCK, 1);
	    break;
	}
    }
    return 0;
}



static int app_client1(void *arg_) {
    __producer client;
    struct appmsg req = {};
    struct appmsg resp = {};
    int sendcnt = 0, recvcnt = 0;

    while (!agstarted || !rmstarted) {
	/* waiting ... */
    }
    while (sendcnt < cnt) {
	switch (rand() % 3) {
	case 0:
	    if (sendcnt < cnt) {
		req.s.len = (rand() % buflen) + HDRLEN;
		req.s.data = buffer;
		if (client.Send(&req) == 0)
		    sendcnt++;
	    }
	    break;
	case 1:
	    client.Close();
	    break;
	case 2:
	    client.Connect(appname, apphost);
	    client.SetOption(OPT_NONBLOCK, 1);
	    break;
	}
	if (client.Recv(&resp) == 0) {
	    if (resp.s.len)
		free(resp.s.data);
	    recvcnt++;
	}
    }
    _stopping = 1;
    return 0;
}


static CtxConf app_conf;

static int test_appcontext(void *arg) {
    AppCtx *ag = new AppCtx();
    AppCtx *ag2 = new AppCtx();
    Rgm *rgm = NewRegisterManager(NULL);
    Rgm *rgm2 = NewRegisterManager(NULL);
    Thread t[2];

    randstr(buffer, buflen);
    app_conf.appid = appname;
    app_conf.msg_timeout_msec = 50;
    ag->InitFromConf(&app_conf);
    ag2->InitFromConf(&app_conf);    
    ag2->EnableRegistry(rgm2);
    rgm2->Listen(apphost2);
    rgm2->StartThread();
    ag2->StartThread();

    ag->EnableRegistry(rgm);
    rgm->Listen(apphost);
    rgm->StartThread();
    ag->StartThread();
    rmstarted = 1;
    agstarted = 1;

    t[0].Start(app_server1, NULL);
    t[1].Start(app_client1, NULL);
    t[0].Stop();
    t[1].Stop();

    rgm->StopThread();
    rgm2->StopThread();
    rmstarted = 1;
    ag->StopThread();
    ag2->StopThread();
    agstarted = 1;
    delete ag;
    delete ag2;
    delete rgm;
    delete rgm2;
    return 0;
}


TEST(ctx, appcontext) {
    test_appcontext(NULL);
}

static int test_module_stat_itemkey() {
    EXPECT_EQ(RPOLLIN, 1);
    EXPECT_EQ(RPOLLOUT, 2);
    EXPECT_EQ(DPOLLIN, 3);
    EXPECT_EQ(DPOLLOUT, 4);
    EXPECT_EQ(RRCVPKG, 5);
    EXPECT_EQ(RSNDPKG, 6);
    EXPECT_EQ(DRCVPKG, 7);
    EXPECT_EQ(DSNDPKG, 8);

    EXPECT_EQ(SIZE, 1);
    EXPECT_EQ(PASSED, 2);
    EXPECT_EQ(TIMEOUTED, 3);
    EXPECT_EQ(RESI_MSEC, 4);

    EXPECT_EQ(RRTT, 1);
    EXPECT_EQ(DRTT, 2);
    EXPECT_EQ(POLLIN, 3);
    EXPECT_EQ(POLLOUT, 4);
    EXPECT_EQ(POLLERR, 5);
    EXPECT_EQ(RECONNECT, 6);
    EXPECT_EQ(RECV_BYTES, 7);
    EXPECT_EQ(SEND_BYTES, 8);
    EXPECT_EQ(RECV_PACKAGES, 9);
    EXPECT_EQ(SEND_PACKAGES, 10);
    EXPECT_EQ(RECV_ICMPS, 11);
    EXPECT_EQ(SEND_ICMPS, 12);
    EXPECT_EQ(RECV_ERRORS, 13);
    EXPECT_EQ(SEND_ERRORS, 14);
    EXPECT_EQ(CHECKSUM_ERRORS, 15);
    return 0;
}

TEST(ctx, module_stat_itemkey) {
    test_module_stat_itemkey();
}
