// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/sync/pmutex.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include "sync/pmutex.h"


NSPIO_DECLARATION_START

int pmutex_init(pmutex_t *mutex) {
    int ret = 0;
    pthread_mutexattr_t mattr;
    pthread_mutex_t *lock = (pthread_mutex_t *)mutex;

    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);

    ret = pthread_mutex_init(lock, &mattr);

    return ret;
}


int pmutex_lock(pmutex_t *mutex) {
    int ret = 0;
    pthread_mutex_t *lock = (pthread_mutex_t *)mutex;

    ret = pthread_mutex_lock(lock);

    return ret;
}

int pmutex_trylock(pmutex_t *mutex) {
    int ret = 0;
    pthread_mutex_t *lock = (pthread_mutex_t *)mutex;
    ret = pthread_mutex_trylock(lock);
    return ret;
}


int pmutex_unlock(pmutex_t *mutex) {
    int ret = 0;
    pthread_mutex_t *lock = (pthread_mutex_t *)mutex;
    ret = pthread_mutex_unlock(lock);
    return ret;
}


int pmutex_destroy(pmutex_t *mutex) {
    int ret = 0;
    pthread_mutex_t *lock = (pthread_mutex_t *)mutex;    
    ret = pthread_mutex_destroy(lock);
    return ret;
}


}
