// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/sync/tmutex.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include "sync/tmutex.h"

NSPIO_DECLARATION_START

int tmutex_init(tmutex_t *mutex) {
    int ret = 0;
    pthread_mutexattr_t mattr;
    pthread_mutex_t *lock = (pthread_mutex_t *)mutex;

    pthread_mutexattr_init(&mattr);
    ret = pthread_mutex_init(lock, &mattr);

    return ret;
}


int tmutex_lock(tmutex_t *mutex) {
    int ret = 0;
    pthread_mutex_t *lock = (pthread_mutex_t *)mutex;

    ret = pthread_mutex_lock(lock);

    return ret;
}

int tmutex_trylock(tmutex_t *mutex) {
    int ret = 0;
    pthread_mutex_t *lock = (pthread_mutex_t *)mutex;
    ret = pthread_mutex_trylock(lock);
    return ret;
}


int tmutex_unlock(tmutex_t *mutex) {
    int ret = 0;
    pthread_mutex_t *lock = (pthread_mutex_t *)mutex;
    ret = pthread_mutex_unlock(lock);
    return ret;
}


int tmutex_destroy(tmutex_t *mutex) {
    int ret = 0;
    pthread_mutex_t *lock = (pthread_mutex_t *)mutex;    
    ret = pthread_mutex_destroy(lock);
    return ret;
}


}
