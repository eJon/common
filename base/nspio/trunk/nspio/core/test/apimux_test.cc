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
#include "log.h"
#include "async_api.h"
#include "base/crc.h"
#include "os/memalloc.h"
#include "appctx.h"
#include "runner/thread.h"


using namespace nspio;
static CtxConf app_conf;

struct MyHdr {
    int64_t timestamp;
    uint16_t checksum;
};
#define MYHDRLEN sizeof(struct MyHdr)



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


static string appname = "testapp";
static string apphost = "127.0.0.1:20009";
static AppCtx *ag = NULL;
static Rgm *rgm = NULL;
static volatile int cnt = 20;

static int rgm_thread(void *arg_) {
    Rgm *rgm = (Rgm *)arg_;
    if (rgm->Listen(apphost) < 0) {
	return -1;
    }
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


static volatile int _stopping;

static int app_server1(void *arg_) {
    __comsumer server;
    struct appmsg req = {};

    while (!rgm->Running() || !ag->Running()) {
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
	    usleep(10000);
	    break;
	case 2:
	    server.Connect(appname, apphost);
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

    while (!rgm->Running() || !ag->Running()) {
	/* waiting ... */
    }
    while (sendcnt < cnt) {
	switch (rand() % 4) {
	case 0:
	    if (sendcnt < cnt) {
		req.s.len = (rand() % buflen) + HDRLEN;
		req.s.data = buffer;
		if (client.Send(&req) == 0)
		    sendcnt++;
	    }
	    break;
	case 1:
	    if (client.Recv(&resp) == 0) {
		if (resp.s.len)
		    free(resp.s.data);
		recvcnt++;
	    }
	    break;
	case 2:
	    client.Close();
	    usleep(10000);
	    break;
	case 3:
	    client.Connect(appname, apphost);
	    client.SetOption(OPT_NONBLOCK, 1);
	    break;
	}
    }
    EXPECT_TRUE(sendcnt > 0);
    _stopping = 1;
    return 0;
}


class my_request_handler : public RequestHandler {
public:
    my_request_handler() {
	mycnt = 0;
	dropped = 0;
    }
    ~my_request_handler() {}
    int HandleRequest(const char *data, uint32_t len, ResponseWriter &rw);
private:
    int mycnt;
    int dropped;
};

int my_request_handler::HandleRequest(const char *data, uint32_t len, ResponseWriter &rw) {
    int i, cnt = 1;
    ResponseWriter rw2 = rw;
    for (i = 0; i < cnt; i++) {
	rw2.Send(data, len);
	mycnt++;
    }
    return 0;
}


struct client_counter {
    int cnt;
};

class my_response_handler : public ResponseHandler {
public:
    my_response_handler() {
	mycnt = 0;
	pmutex_init(&lock);
    }
    ~my_response_handler() {
	pmutex_destroy(&lock);
    }
    int recvcnt() {
	return mycnt;
    }
    int HandleError(Error &ed, void *pridata) {
	struct client_counter *cc = (struct client_counter *)pridata;
	cc->cnt++;
	return 0;
    }
    int HandleResponse(const char *data, uint32_t len, void *pridata);

private:
    int mycnt;
    pmutex_t lock;
};

int my_response_handler::HandleResponse(const char *data, uint32_t len, void *pridata) {
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

// test async client api
static int app_client3(void *arg_) {
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

// test async client api after nspio server down
static int app_client4(void *arg_) {
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
    usleep(200000);
    return 0;
}


static int test_asyncapi() {
    my_request_handler rqh;
    my_response_handler rph;
    AsyncComsumer *ac = NULL;
    AsyncProducer *ap = NULL;
    Thread t[4];

    ag = new AppCtx();
    rgm = NewRegisterManager(NULL);
    ac = NewAsyncComsumer();
    ap = NewAsyncProducer();
    app_conf.appid = appname;
    app_conf.msg_timeout_msec = 200;
    ag->InitFromConf(&app_conf);
    ag->EnableRegistry(rgm);
    t[0].Start(rgm_thread, rgm);
    t[1].Start(appcontext_thread, NULL);
    while (!ag->Running())
	usleep(10);
    while (!rgm->Running())
	usleep(10);

    
    t[2].Start(app_server1, NULL);
    t[3].Start(app_client1, NULL);
    t[2].Stop();
    t[3].Stop();

    async_conf conf1;
    conf1.appname = appname;
    conf1.apphost = apphost;
    ac->Setup(conf1, &rqh);
    ac->StartServe();
    usleep(100000);
    ac->Stop();

    // testing async server and async client
    async_conf conf2;
    conf2.set(appname, apphost, 4, 40, 0);
    ac->Setup(conf2, &rqh);
    EXPECT_TRUE(ac->Setup(conf2, &rqh) == -1 && errno == SPIO_EINTERN);
    ac->StartServe();
    ap->Setup(conf2, &rph);
    ap->StartServe();
    usleep(100000);

    t[2].Start(app_client3, ap);
    t[3].Start(app_client3, ap);
    t[2].Stop();
    t[3].Stop();

    // testing memleak when ~AsyncComsumer()
    usleep(100000);
    t[2].Start(app_client4, ap);
    t[3].Start(app_client4, ap);
    usleep(10000);
    ag->Stop();
    rgm->Stop();
    t[0].Stop();
    t[1].Stop();
    ac->Stop();
    ap->Stop();
    t[2].Stop();
    t[3].Stop();
    delete ac;
    delete ap;

    delete ag;
    delete rgm;
    return 0;
}



static volatile int _stopping9;
static int app_client9(void *arg_) {
    AsyncProducer *ap = (AsyncProducer *)arg_;
    struct client_counter cc = {};
    string msg;
    MyHdr hdr = {};
    int len = 0;

    while (!_stopping9) {
	msg.clear();
	len = (rand() % (buflen - MYHDRLEN)) + MYHDRLEN;
	hdr.checksum = crc16(buffer, len - MYHDRLEN);
	msg.append((char *)&hdr, MYHDRLEN);
	msg.append(buffer, len - MYHDRLEN);
	ap->SendRequest(msg.data(), msg.size(), &cc);
	usleep(1000);
    }
    return 0;
}





static int test_asyncapi_expection() {
    my_request_handler rqh;
    my_response_handler rph;
    AsyncComsumer *ac = NewAsyncComsumer();
    AsyncProducer *ap = NewAsyncProducer();
    Thread t[4];
    
    ag = new AppCtx();
    rgm = NewRegisterManager(NULL);
    app_conf.appid = appname;
    app_conf.msg_timeout_msec = 200;
    ag->InitFromConf(&app_conf);
    ag->EnableRegistry(rgm);
    t[0].Start(rgm_thread, rgm);
    t[1].Start(appcontext_thread, NULL);
    while (!ag->Running())
	usleep(10);
    while (!rgm->Running())
	usleep(10);

    async_conf conf;
    conf.set(appname, apphost, 1, 40, 0);
    ac->Setup(conf, &rqh);
    ac->StartServe();
    ap->Setup(conf, &rph);
    ap->StartServe();

    t[2].Start(app_client9, ap);
    t[3].Start(app_client9, ap);
    int i;
    for (i = 0; i < 10; i++) {
	ag->Stop();
	t[1].Stop();
	delete ag;

	ag = new AppCtx();
	app_conf.appid = appname;
	app_conf.msg_timeout_msec = 200;
	ag->InitFromConf(&app_conf);
	ag->EnableRegistry(rgm);
	t[1].Start(appcontext_thread, NULL);
	while (!ag->Running())
	    usleep(10);
    }
    ag->Stop();
    rgm->Stop();
    t[0].Stop();
    t[1].Stop();
    delete ag;
    delete rgm;
    _stopping9 = 1;


    t[2].Stop();
    t[3].Stop();
    ac->Stop();
    ap->Stop();
    delete ac;
    delete ap;
    return 0;
}



TEST(api, async) {
    randstr(buffer, buflen);
    test_asyncapi();
    test_asyncapi_expection();
}
