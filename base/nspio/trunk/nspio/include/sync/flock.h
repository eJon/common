// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/sync/flock.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _FLOCK_H_INCLUDE_
#define _FLOCK_H_INCLUDE_

#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "decr.h"


NSPIO_DECLARATION_START


typedef struct __flock {
    int     fd;
    char    *name;
} flock_t;

int flock_create(flock_t *fl, const char *name);
int flock_destroy(flock_t *fl);
int flock_trylock(flock_t *fl);
int flock_lock(flock_t *fl);
int flock_unlock(flock_t *fl);


}
 
#endif //_SHMTX_H_INCLUDE_
