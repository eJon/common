// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/test/log4cpp_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <gtest/gtest.h>
#include <stdlib.h>
#include "log.h"
#include "runner/thread.h"

using namespace nspio;


static int randstr(char *buf, int len) {
    int i, idx;
    char token[] = "qwertyuioplkjhgfdsazxcvbnm1234567890";
    for (i = 0; i < len; i++) {
	idx = rand() % strlen(token);
	buf[i] = token[idx];
    }
    return 0;
}


static int log_gener(void *arg) {
    int level = 0, cnt = 10, i;
    char buf[50] = {};

    
    for (i = 0; i < cnt; i++) {
	level = rand() % 4;
	randstr(buf, 49);
	switch(level) {
	case 0:
	    NSPIOLOG_INFO("%s", buf);
	    break;
	case 1:
	    NSPIOLOG_DEBUG("%s", buf);
	    break;
	case 2:
	    NSPIOLOG_WARN("%s", buf);
	    break;
	case 3:
	    NSPIOLOG_ERROR("%s", buf);
	    break;
	}
    }
    return 0;
}


static int log_test(void *arg) {
    int thread_cnt = 10, i;
    Thread thread[thread_cnt];
    
    for (i = 0; i < thread_cnt; i++) {
	thread[i].Start(log_gener, NULL);
    }
    for (i = 0; i < thread_cnt; i++) {
	thread[i].Stop();
    }
    return 0;
}


TEST(log4cpp, multiloger) {
    EXPECT_EQ(0, log_test(NULL));
}
