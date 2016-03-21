// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/test/mqp_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <gtest/gtest.h>
#include "os/time.h"
#include "receiver.h"
#include "mqp.h"


using namespace nspio;

static int msgqueueex_normal(void *arg_) {
    MQueuePool mqp;
    MQueue *mq;
    struct nspiomsg *hdr;
    int cnt = 1000, i, idx;
    string rid;
    CtxConf cfg;
    vector<string> uuid;

    // first. create queue
    cfg.msg_queue_size = 102400;
    cfg.reconnect_timeout_msec = 0;
    cfg.msg_timeout_msec = 0;
    mqp.Setup("mockapp", &cfg);
    for (i = 0; i < cnt; i++) {
	route_genid(rid);
	uuid.push_back(rid);
	mqp.Set(rid);
    }


    // second. random push data from queue
    for (i = 0; i < cnt; i++) {
	idx = rand() % cnt;
	mq = mqp.Find(uuid.at(idx));
	if (mq) {
	    hdr = (struct nspiomsg *)malloc(MSGHDRLEN);
	    memset(hdr, 0, MSGHDRLEN);
	    mq->PushMsg(hdr);
	}
    }

    // third. random pop data from queue
    for (i = 0; i < cnt; i++) {
	idx = rand() % cnt;
	mq = mqp.Find(uuid.at(idx));
	if (mq) {
	    if ((hdr = mq->PopMsg()) != NULL)
		free(hdr);
	}
    }
    return 0;
}


static int msgqueueex_timeout() {
    MQueuePool mqp, mqp2;
    MQueue *mq;
    struct nspiomsg *msg;
    string rid;
    CtxConf cfg1, cfg2;

    cfg1.msg_queue_size = 102400;
    cfg1.reconnect_timeout_msec = 0;
    cfg1.msg_timeout_msec = 0;
    mqp.Setup("mockapp", &cfg1);

    cfg2.msg_queue_size = 102400;
    cfg2.reconnect_timeout_msec = 0;
    cfg2.msg_timeout_msec = 10;
    mqp2.Setup("mockapp", &cfg2);

    route_genid(rid);
    mqp.Set(rid);
    mqp2.Set(rid);
    
    // second. random push data from queue
    mq = mqp.Find(rid);

    msg = (struct nspiomsg *)malloc(MSGHDRLEN);
    memset(msg, 0, MSGHDRLEN);
    msg->hdr.timestamp = rt_mstime();
    mq->PushMsg(msg);
    usleep(500);
    msg = mq->PopMsg();
    EXPECT_TRUE(msg != NULL);


    msg->hdr.timestamp = rt_mstime();
    mq = mqp2.Find(rid);
    mq->PushMsg(msg);
    usleep(100000);
    // package is timeout and dropped by queue
    msg = mq->PopMsg();
    EXPECT_TRUE(msg == NULL);

    mqp.unSet(rid);
    return 0;
}



TEST(msgqueueex, normal) {
    msgqueueex_normal(NULL);
}


TEST(msgqueueex, timeout) {
    msgqueueex_timeout();
}
