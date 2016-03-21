// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/test/thread_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <gtest/gtest.h>
#include <stdio.h>
#include "runner/thread.h"

using namespace nspio;


static int test_pthread_retval(void *arg_) {
    return -5;
}


static int test_pthread_retval_worker() {
    int thread_cnt = 2, i;
    int thread_retval[thread_cnt];
    Thread thread[thread_cnt];

    for (i = 0; i < thread_cnt; i++) {
	thread[i].Start(test_pthread_retval, NULL);
    }

    for (i = 0; i < thread_cnt; i++) {
	thread_retval[i] = thread[i].Stop();
	EXPECT_EQ(-5, thread_retval[i]);
    }
    return 0;
}


TEST(thread, pthread_retval) {
    EXPECT_EQ(0, test_pthread_retval_worker());
}
