// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/test/timer_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <gtest/gtest.h>
#include "regmgr/timer.h"

using namespace nspio;

static volatile int timer_cnt = 0;

static int mytimer_func(void *data) {
    timer_cnt++;
    return 0;
}


static int timer_test() {
    Timer tt;
    tt.Setup(100);
    tt.AddTimerEvent(mytimer_func, NULL, 1);
    tt.AddTimerEvent(mytimer_func, NULL, 50);
    tt.AddTimerEvent(mytimer_func, NULL, 200);
    tt.AddTimerEvent(mytimer_func, NULL, 200);
    usleep(1000);
    tt.Wait(1);
    EXPECT_EQ(1, timer_cnt);
    usleep(100000);
    tt.Wait(1);
    EXPECT_EQ(2, timer_cnt);
    usleep(500000);
    tt.Wait(1);
    EXPECT_EQ(4, timer_cnt);
    return 0;
}

static int timer_data_queue_test() {
    TDQueue tq;
    char buf[33] = {};
    void *ptr = NULL;

    tq.PushTD(buf, 10);
    tq.PushTD(buf, 15);
    usleep(1000);
    ptr = tq.PopTD();
    EXPECT_TRUE(ptr == NULL);
    usleep(20000);
    ptr = tq.PopTD();
    EXPECT_TRUE(ptr != NULL);
    ptr = tq.PopTD();
    EXPECT_TRUE(ptr != NULL);
    return 0;
}




TEST(timer, timer) {
    timer_test();
}


TEST(timer, tdqueue) {
    timer_data_queue_test();
}
