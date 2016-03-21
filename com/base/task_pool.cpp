// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/runner/task_pool.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <errno.h>
#include <inttypes.h>
#include "task_pool.h"

TaskPool::TaskPool() :
    stopping(true), inited(0), tasks(0), workers(0)
{
    pmutex_init(&lock);
    pcond_init(&cond);
}


TaskPool::~TaskPool() {
    Thread *thread = NULL;
    list<Thread *>::iterator it;

    pmutex_destroy(&lock);
    pcond_destroy(&cond);
    for (it = mpthreads.begin(); it != mpthreads.end(); ++it) {
	thread = *it;
	delete thread;
    }
}

int TaskPool::Setup(int workers) {
    int i = 0;
    Thread *thread = NULL;

    if (workers <= 0)
	workers = 1;
    for (i = 0; i < workers; i++) {
	if ((thread = new (std::nothrow) Thread()) == NULL) {
	    errno = ENOMEM;
	    break;
	}
	mpthreads.push_back(thread);
    }
    inited = 1;
    return 0;
}


int TaskPool::push_task(task_func tfunc, void *tdata) {
    struct _task task = {};
    task.func = tfunc;
    task.data = tdata,
    task_head.push_back(task);
    tasks++;
    return 0;
}


int TaskPool::pop_task(task_func *tfunc, void **tdata) {
    struct _task task = {};

    if (task_head.empty())
	   return -1;

    task = task_head.front();
    task_head.pop_front();
    *tfunc = task.func;
    *tdata = task.data;
    return 0;
}

int TaskPool::take(task_func *tfunc, void **tdata) {
    int ret = 0;
    Lock();
    if ((ret = pop_task(tfunc, tdata)) < 0)
	Wait();
    UnLock();
    return ret;
}


int TaskPool::intern_worker() {
    int ret = 0;
    task_func tfunc = NULL;
    void *tdata = NULL;
    
    while (!stopping) {
	if ((ret = take(&tfunc, &tdata)) < 0)
	    continue;
	tfunc(tdata);
	tfunc = 0;
	tdata = 0;
    }
    return 0;
}

static int tasks_worker(void *data) {
    TaskPool *tp = (TaskPool *)data;
    return tp->intern_worker();
}

int TaskPool::Start() {
    Thread *thread = NULL;
    list<Thread *>::iterator it;

    if (!stopping || !inited) {
	return -1;
    }
    stopping = false;
    for (it = mpthreads.begin(); it != mpthreads.end(); ++it) {
	thread = *it;
	thread->Start(tasks_worker, this);
    }
    return 0;
}


int TaskPool::Stop() {
    Thread *thread = NULL;
    list<Thread *>::iterator it;

    if (stopping || !inited) {
	return -1;
    }
    stopping = true;
    BroadCast();
    for (it = mpthreads.begin(); it != mpthreads.end(); ++it) {
	thread = *it;
	thread->Stop();
    }
    return 0;
}
