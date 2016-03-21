// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/runner/task_pool.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_TASK_POOL_
#define _H_TASK_POOL_

#include <list>
#include "pmutex.h"
#include "pcond.h"
#include "thread.h"

using namespace std;

typedef int (*task_func) (void *data);

struct _task {
	task_func func;
	void *data;
};

class TaskPool {
 public:
    TaskPool();
    ~TaskPool();

    int Setup(int workers);
    void BroadCast() {
	Lock();
	pcond_broadcast(&cond);
	UnLock();
    }
    int Run(task_func tfunc, void *tdata) {
	int ret = 0;
	Lock();
	ret = push_task(tfunc, tdata);
	UnLock();
	return ret;
    }
    int Start();
    int Stop();

    int intern_worker();
    
 private:
    bool stopping;
    int inited;
    int tasks, workers;
    pmutex_t lock;
    pcond_t cond;
    list<struct _task> task_head;
    list<Thread *> mpthreads;

    void Lock() {
	pmutex_lock(&lock);
    }
    void UnLock() {
	pmutex_unlock(&lock);
    }
    void Wait() {
	pcond_wait(&cond, &lock);
    }

    int push_task(task_func tfunc, void *tdata);
    int pop_task(task_func *tfunc, void **tdata);
    int take(task_func *tfunc, void **tdata);
};













#endif   // _H_TASK_POOL_
