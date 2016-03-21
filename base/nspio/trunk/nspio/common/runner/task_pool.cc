// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/runner/task_pool.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "os/memalloc.h"
#include "runner/task_pool.h"


NSPIO_DECLARATION_START


struct _task {
    task_func func;
    void *data;
    struct list_head node;
};


TaskPool::TaskPool() :
    stopping(true), inited(0), tasks(0), workers(0)
{
    pmutex_init(&lock);
    pcond_init(&cond);
    INIT_LIST_HEAD(&task_head);
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
    struct _task *task = NULL;

    if ((task = (struct _task *)mem_zalloc(sizeof(*task))) != NULL) {
	task->func = tfunc;
	task->data = tdata;
	list_add_tail(&task->node, &task_head);
	tasks++;
	return 0;
    }
    return -1;
}


int TaskPool::pop_task(task_func *tfunc, void **tdata) {
    struct _task *task = NULL;
    
    if (!list_empty(&task_head)) {
	task = list_first(&task_head, struct _task, node);
	list_del(&task->node);
	*tfunc = task->func;
	*tdata = task->data;
	mem_free(task, sizeof(*task));
	tasks--;
	return 0;
    }

    return -1;
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

    if (!stopping) {
	errno = SPIO_EDUPOP;
	return -1;
    }
    if (!inited) {
	errno = SPIO_EINTERN;
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

    if (stopping) {
	errno = SPIO_EDUPOP;
	return -1;
    }
    if (!inited) {
	errno = SPIO_EINTERN;
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






}
