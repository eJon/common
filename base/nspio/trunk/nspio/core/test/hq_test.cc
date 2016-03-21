// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/test/hq_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <gtest/gtest.h>
#include "ht.h"
#include "runner/thread.h"


using namespace nspio;


static int cnt = 100;

class MyRpp : public ResponseHandler {
public:
    MyRpp() {
	cnt = to_cnt = 0;
    }
    ~MyRpp() {}
    int HandleError(Error &ed, void *cb) {
	to_cnt++;
	return 0;
    }
    int HandleResponse(const char *data, uint32_t len, void *yours) {
	cnt++;
	return 0;
    }
    int cnt, to_cnt;
};

static int callback_worker(void *arg_) {
    HandlerTable *ht = (HandlerTable *)arg_;
    vector<int> hids;
    vector<int>::iterator it;
    MyRpp mr, *cb = NULL;
    void *yours = NULL;
    int i = 0, hid = 0;
    struct hlist_head to_head = {};

    INIT_HLIST_HEAD(&to_head);
    for (i = 0; i < cnt; i++) {
	if ((hid = ht->GetHid()) < 0)
	    continue;
	hids.push_back(hid);
	EXPECT_EQ(0, ht->InsertHandler(hid, &mr, rand() % 50));
	if (i == cnt/2)
	    usleep(250000);
    }
    for (it = hids.begin(); it != hids.end(); ++it) {
	hid = *it;
	if (ht->FindHandler(hid, &yours) == 0) {
	    cb = (MyRpp *)yours;
	    cb->cnt++;
	}
    }

    EXPECT_NE(0, mr.cnt);
    EXPECT_EQ(cnt, mr.cnt + mr.to_cnt);
    return 0;
}


static int ht_test() {
    int w = 4, i;
    Thread t[4];
    HandlerTable ht;

    ht.Init(100);
    for (i = 0; i < w; i++)
	t[i].Start(callback_worker, &ht);
    for (i = 0; i < w; i++)
	t[i].Stop();
    return 0;
}


TEST(ht, handlertable) {
    ht_test();
}
