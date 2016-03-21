// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/sync/pcond.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _PCOND_H_INCLUDE_
#define _PCOND_H_INCLUDE_

#include <pthread.h>
#include "pmutex.h"

typedef struct pcond {
    pthread_cond_t _cond;
} pcond_t;

int pcond_init(pcond_t *cond);
int pcond_destroy(pcond_t *cond);

int pcond_wait(pcond_t *cond, pmutex_t *mutex);
int pcond_signal(pcond_t *cond);
int pcond_broadcast(pcond_t *cond);


#endif //_PMUTEX_H_INCLUDE_
