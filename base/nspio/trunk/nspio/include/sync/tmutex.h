// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/sync/tmutex.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _TMUTEX_H_INCLUDE_
#define _TMUTEX_H_INCLUDE_

#include <pthread.h>
#include "decr.h"

NSPIO_DECLARATION_START

typedef struct tmutex {
    pthread_mutex_t _mutex;
} tmutex_t;

int tmutex_init(tmutex_t *mutex);
int tmutex_lock(tmutex_t *mutex);
int tmutex_trylock(tmutex_t *mutex);
int tmutex_unlock(tmutex_t *mutex);
int tmutex_destroy(tmutex_t *mutex);

}

#endif //_PMUTEX_H_INCLUDE_
