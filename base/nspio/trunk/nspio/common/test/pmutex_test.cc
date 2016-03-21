// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/test/pmutex_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <gtest/gtest.h>
#include <unistd.h>
#include "sync/pmutex.h"

using namespace nspio;


static int test_pmutex() {
    pmutex_t lock;
    int ret;

    pmutex_init(&lock);
    if ((ret = pmutex_lock(&lock)) != 0) {
	printf("lock failed\n");
	return -1;
    }
    if ((ret = pmutex_trylock(&lock)) == 0) {
	printf("lock shuld be EBUSY\n");
	return -1;
    }
    if ((ret = pmutex_unlock(&lock)) != 0) {
	printf("lock can't unlock\n");
	return -1;
    }
    pmutex_destroy(&lock);

    return 0;
}

static int test_pmutex_multiprocess() {
    pmutex_t lock;
    int ret;
    pid_t pid;

    pmutex_init(&lock);
    if ((ret = pmutex_lock(&lock)) != 0) {
	printf("lock failed\n");
	pmutex_destroy(&lock);
	return -1;
    }
    if ((pid = fork()) == 0) {
	// child
	if ((ret = pmutex_trylock(&lock)) == 0) {
	    printf("lock should be EBUSY\n");
	    exit(-1);
	}
	pmutex_destroy(&lock);
	exit(0);
    }
    waitpid(pid, &ret, 0);
    if (ret != 0) {
	printf("child failed... %d\n", ret);
	return ret;
    }
    pmutex_unlock(&lock);
    pmutex_destroy(&lock);
    return ret;
}

static int benchmark_pmutex_multithread() {
    pmutex_t lock;
    int i = 0, cnt = 1000;

    pmutex_init(&lock);

    for (i = 0; i < cnt; i++) {
	pmutex_lock(&lock);
	pmutex_unlock(&lock);
    }
    
    pmutex_destroy(&lock);
    return 0;
}


static int benchmark_pmutex_multiprocess() {
    pmutex_t lock;

    pmutex_init(&lock);

    pmutex_destroy(&lock);
    return 0;
}



TEST(pmutex, single) {
    EXPECT_EQ(0, test_pmutex());
}

TEST(pmutex, multiprocess) {
    return;
    EXPECT_EQ(0, test_pmutex_multiprocess());
}


TEST(pmutex, multipthread_benchmark) {
    EXPECT_EQ(0, benchmark_pmutex_multithread());
}

TEST(pmutex, multiprocess_benchmark) {
    return;
    EXPECT_EQ(0, benchmark_pmutex_multiprocess());
}
