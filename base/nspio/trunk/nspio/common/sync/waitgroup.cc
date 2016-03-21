// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/sync/waitgroup.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <stdio.h>
#include "sync/waitgroup.h"

NSPIO_DECLARATION_START


WaitGroup::WaitGroup() :
    ref(0)
{
    pcond_init(&cond);
    pmutex_init(&mutex);
}

WaitGroup::~WaitGroup() {
    pcond_destroy(&cond);
    pmutex_destroy(&mutex);
}

int WaitGroup::incr(int refs) {
    pmutex_lock(&mutex);
    ref += refs;
    pmutex_unlock(&mutex);
    return 0;
}

int WaitGroup::decr(int refs) {
    pmutex_lock(&mutex);
    ref -= refs;
    pmutex_unlock(&mutex);
    return 0;
}

int WaitGroup::get() {
    int _ref = 0;
    pmutex_lock(&mutex);
    _ref = ref;
    pmutex_unlock(&mutex);
    return _ref;
}

int WaitGroup::Add(int refs) {
    return incr(refs);
}

int WaitGroup::Done(int refs) {
    pmutex_lock(&mutex);
    ref -= refs;
    if (ref == 0)
	pcond_broadcast(&cond);
    pmutex_unlock(&mutex);
    return 0;
}

int WaitGroup::Wait() {
    pmutex_lock(&mutex);
    while (ref > 0)
	pcond_wait(&cond, &mutex);
    pmutex_unlock(&mutex);
    return 0;
}








}
