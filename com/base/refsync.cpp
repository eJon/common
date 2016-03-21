// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/sync/waitgroup.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <stdio.h>
#include "refsync.h"

// NSPIO_DECLARATION_START

RefSync::RefSync() :
    ref(0)
{
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL); 
}

RefSync::~RefSync() {
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}

int RefSync::incr(int refs) {
    pthread_mutex_lock(&mutex);
    ref += refs;
    pthread_mutex_unlock(&mutex);
    return 0;
}

int RefSync::decr(int refs) {
    pthread_mutex_lock(&mutex);
    ref -= refs;
    pthread_mutex_unlock(&mutex);
    return 0;
}

int RefSync::get() {
    int _ref = 0;
    pthread_mutex_lock(&mutex);
    _ref = ref;
    pthread_mutex_unlock(&mutex);
    return _ref;
}

int RefSync::Add(int refs) {
    return incr(refs);
}

int RefSync::Done(int refs) {
    pthread_mutex_lock(&mutex);
    ref -= refs;
    if (ref == 0)
	pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    return 0;
}

int RefSync::Wait() {
    pthread_mutex_lock(&mutex);
    while (ref != 0)
	pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
    return 0;
}








//}
