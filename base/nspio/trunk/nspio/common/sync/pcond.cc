// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/sync/pcond.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include "sync/pcond.h"


NSPIO_DECLARATION_START

int pcond_init(pcond_t *cond) {
    return pthread_cond_init(&cond->_cond, NULL);
}

int pcond_destroy(pcond_t *cond) {
    return pthread_cond_destroy(&cond->_cond);
}

int pcond_wait(pcond_t *cond, pmutex_t *mutex) {
    return pthread_cond_wait(&cond->_cond, &mutex->_mutex);
}


int pcond_signal(pcond_t *cond) {
    return pthread_cond_signal(&cond->_cond);
}

int pcond_broadcast(pcond_t *cond) {
    return pthread_cond_broadcast(&cond->_cond);
}

}
