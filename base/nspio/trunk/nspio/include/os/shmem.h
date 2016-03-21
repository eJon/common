// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/os/shmem.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _SHMEM_H_INCLUDE_
#define _SHMEM_H_INCLUDE_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mempool.h"

NSPIO_DECLARATION_START


typedef struct shm_t shm_t;

struct shm_t
{
    char   *name;
    char         *addr;
    uint64_t     size;
    int          fd;
    mempool_t *pool;
};
shm_t *shm_create(uint64_t size, char *name, mempool_t *p);
int shm_alloc(shm_t *shm, char *addr, int is_create_or_clear);
int shm_free(shm_t *shm);


}

#endif // _SHMEM_H_INCLUDED_
