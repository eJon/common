// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/sync/pspinlock.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_PSPINLOCK_
#define _H_PSPINLOCK_

#include <pthread.h>
#include "decr.h"

NSPIO_DECLARATION_START

typedef struct _spinlock {
    pthread_spinlock_t _spin;
} pspin_t;

int pspin_init(pspin_t *spin);
int pspin_lock(pspin_t *spin);
int pspin_unlock(pspin_t *spin);
int pspin_destroy(pspin_t *spin);

}

#endif //_PSPIN_H_INCLUDE_
