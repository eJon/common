// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/test/sync_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <gtest/gtest.h>
#include <list>
#include "runner/thread.h"
#include "sync/waitgroup.h"

using namespace std;
using namespace nspio;


static int test_waitgroup_single_thread() {
    WaitGroup wg;
    wg.Add();
    wg.Done();
    wg.Wait();
    wg.Add(2);
    wg.Done(2);
    wg.Wait();
    wg.Add(2);
    wg.Done();
    wg.Done();
    wg.Wait();
    return 0;
}


static int task_func(void *data) {
    int i = 0;
    WaitGroup *wg = (WaitGroup *)data;
    for (i = 0; i < 100; i++)
	wg->Done();
    return 0;
}

static int test_waitgroup_multi_threads() {
    WaitGroup wg;
    int i, nthreads = 0;
    Thread *thread = NULL;
    list<Thread *> workers;
    list<Thread *>::iterator it;
    
    nthreads = rand() % 20;
    for (i = 0; i < nthreads; i++) {
	EXPECT_TRUE((thread = new Thread()) != NULL);
	workers.push_back(thread);
    }
    wg.Add(nthreads * 100);
    for (it = workers.begin(); it != workers.end(); ++it) {
	thread = *it;
	thread->Start(task_func, &wg);
    }
    wg.Wait();
    for (it = workers.begin(); it != workers.end(); ++it) {
	thread = *it;
	thread->Stop();
	delete *it;
    }
    return 0;
}


TEST(sync, waitgroup_single_thread) {
    test_waitgroup_single_thread();
}

TEST(sync, waitgroup_multi_threads) {
    test_waitgroup_multi_threads();
}
