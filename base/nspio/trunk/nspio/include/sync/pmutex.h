// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/sync/pmutex.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _PMUTEX_H_INCLUDE_
#define _PMUTEX_H_INCLUDE_

#include <pthread.h>
#include "decr.h"

NSPIO_DECLARATION_START

typedef struct pmutex {
    pthread_mutex_t _mutex;
} pmutex_t;

int pmutex_init(pmutex_t *mutex);
int pmutex_lock(pmutex_t *mutex);
int pmutex_trylock(pmutex_t *mutex);
int pmutex_unlock(pmutex_t *mutex);
int pmutex_destroy(pmutex_t *mutex);

}

#endif //_PMUTEX_H_INCLUDE_
