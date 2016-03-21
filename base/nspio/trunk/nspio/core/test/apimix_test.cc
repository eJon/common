// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/test/apimix_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <gtest/gtest.h>
#include "log.h"
#include "async_api.h"
#include "base/crc.h"
#include "os/memalloc.h"
#include "appctx.h"
#include "runner/thread.h"


using namespace nspio;
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

struct MyHdr {
    int64_t timestamp;
    uint16_t checksum;
};
#define MYHDRLEN sizeof(struct MyHdr)



#define buflen 1024
static char buffer[buflen] = {};
static string appname = "testapp";
static string apphost = "127.0.0.1:20009";
static AppCtx *ag = NULL;
static Rgm *rgm = NULL;
static volatile int cnt = 20;

static int rgm_thread(void *arg_) {
    Rgm *rgm = (Rgm *)arg_;
    while (rgm->Listen(apphost) < 0)
	{}
    rgm->Start();
    NSPIOLOG_INFO("rgm stop");
    return 0;
}

static int appcontext_thread(void *arg_) {
    while (!rgm->Running()) {
	/* waitting ... */
    }
    ag->Start();
    NSPIOLOG_INFO("ag stop");
    return 0;
}


// test sync client api
static int app_sync_client(void *arg_) {
    __producer client;
    module_stat *stat = client.Stat();

    while (!ag->Running() || !rgm->Running()) {
	/* waiting ... */
    }
    if (client.Connect(appname, apphost) < 0) {
	NSPIOLOG_ERROR("connect to testapp");
	return -1;
    }
    while (stat->getkey(API_SEND_PACKAGES) < cnt) {
	string req;
	EXPECT_TRUE(client.Send(req) == 0);
    }
    while (stat->getkey(API_RECV_PACKAGES) < cnt) {
	string resp;
	if (client.Recv(resp) == 0)
	    EXPECT_TRUE(resp.size() == 0);
    }
    client.Close();
    return 0;
}
class mix_request_handler : public RequestHandler {
public:
    mix_request_handler() {
	mycnt = 0;
	dropped = 0;
    }
    ~mix_request_handler() {}
    int HandleRequest(const char *data, uint32_t len, ResponseWriter &rw);
private:
    int mycnt;
    int dropped;
};


int mix_request_handler::HandleRequest(const char *data, uint32_t len, ResponseWriter &rw) {
    rw.Send(data, len);
    mycnt++;
    return 0;
}

struct client_counter {
    int cnt;
};


class mix_response_handler : public ResponseHandler {
public:
    mix_response_handler() {
	mycnt = 0;
	pmutex_init(&lock);
    }
    ~mix_response_handler() {
	pmutex_destroy(&lock);
    }
    int recvcnt() {
	return mycnt;
    }
    int HandleError(Error &ed, void *pridata) {
	return 0;
    }
    int HandleResponse(const char *data, uint32_t len, void *pridata);

private:
    int mycnt;
    pmutex_t lock;
};

int mix_response_handler::HandleResponse(const char *data, uint32_t len, void *pridata) {
    MyHdr *hdr = NULL;
    char *__data = NULL;
    int __len = 0;
    struct client_counter *cc = (struct client_counter *)pridata;

    cc->cnt++;
    hdr = (MyHdr *)data;
    __data = (char *)data + MYHDRLEN;
    __len = len - MYHDRLEN;
    EXPECT_EQ(hdr->checksum, crc16(__data, __len));
    pmutex_lock(&lock);
    mycnt++;
    pmutex_unlock(&lock);
    return 0;
}


static int app_sync_server(void *arg_) {
    __comsumer server;
    module_stat *stat = server.Stat();

    while (!ag->Running() || !rgm->Running()) {
	/* waiting ... */
    }
    if (server.Connect(appname, apphost) < 0) {
	NSPIOLOG_ERROR("connect to testapp");
	return -1;
    }
    while (stat->getkey(API_RECV_PACKAGES) < cnt) {
	string msg, rt;
	EXPECT_TRUE(server.Recv(msg, rt) == 0);
	EXPECT_TRUE(server.Send(msg, rt) == 0);
    }
    server.Close();
    return 0;
}

static int app_async_client(void *arg_) {
    AsyncProducer *ap = (AsyncProducer *)arg_;
    struct client_counter cc = {};
    string msg;
    MyHdr hdr = {};
    int len = 0, i;

    for (i = 0; i < cnt; i++) {
	msg.clear();
	len = (rand() % (buflen - MYHDRLEN)) + MYHDRLEN;
	hdr.checksum = crc16(buffer, len - MYHDRLEN);
	msg.append((char *)&hdr, MYHDRLEN);
	msg.append(buffer, len - MYHDRLEN);
	ap->SendRequest(msg.data(), msg.size(), &cc);
    }
    while (cc.cnt != cnt)
	usleep(1);
    return 0;
}


static int test_async_sync() {
    mix_request_handler rqh;
    mix_response_handler rph;
    AsyncComsumer *ac = NULL;
    AsyncProducer *ap = NULL;
    async_conf conf;
    Thread t[4];

    conf.set(appname, apphost, 1, 200, 100);
    randstr(buffer, buflen);
    ag = new AppCtx();
    rgm = NewRegisterManager(NULL);
    ac = NewAsyncComsumer();
    ap = NewAsyncProducer();
    app_conf.appid = appname;
    ag->InitFromConf(&app_conf);
    ag->EnableRegistry(rgm);
    t[0].Start(rgm_thread, rgm);
    t[1].Start(appcontext_thread, NULL);
    while (!ag->Running())
	usleep(10);
    while (!rgm->Running())
	usleep(10);

    // async server -- sync client
    ac->Setup(conf, &rqh);
    ac->StartServe();
    usleep(100000);
    t[2].Start(app_sync_client, NULL);
    t[2].Stop();
    ac->Stop();
    delete ac;

    //sync server -- async client
    t[2].Start(app_sync_server, NULL);
    ap->Setup(conf, &rph);
    ap->StartServe();
    usleep(100000);
    t[3].Start(app_async_client, ap);
    t[3].Stop();
    t[2].Stop();
    ap->Stop();
    delete ap;

    ag->Stop();
    rgm->Stop();
    t[0].Stop();
    t[1].Stop();
    delete ag;
    delete rgm;

    return 0;
}


TEST(api, mix) {
     test_async_sync();
}
