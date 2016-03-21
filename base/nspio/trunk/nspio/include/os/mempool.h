// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/os/mempool.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _MEMPOOL_H_INCLUDED_
#define _MEMPOOL_H_INCLUDED_


#include <inttypes.h>
#include <malloc.h>
#include "os/os.h"
#include "decr.h"


NSPIO_DECLARATION_START


/*
* MAX_ALLOC_FROM_POOL should be (pagesize - 1), i.e. 4095 on x86.
* On Windows NT it decreases a number of locked pages in a kernel.
*/
#define MAX_ALLOC_FROM_POOL  (SLB_PAGESIZE - 1)

#define DEFAULT_POOL_SIZE    (16 * 1024)

#define POOL_ALIGNMENT       16
#define MIN_POOL_SIZE                                                     \
    mem_align((sizeof(mempool_t) + 2 * sizeof(mempool_large_t)),            \
    POOL_ALIGNMENT)

typedef void (*mempool_cleanup_pt)(void *data);

struct mempool_cleanup_t {
    mempool_cleanup_pt   handler;
    void                  *data;
    mempool_cleanup_t   *next;
};

struct mempool_large_t{
    mempool_large_t     *next;
    void                  *alloc;
};

struct mempool_t;

struct mempool_data_t{
    int8_t                *last;
    int8_t               *end;
    mempool_t           *next;
    uint32_t            failed;
};

struct mempool_t {
    mempool_data_t      d;
    uint32_t                max;
    mempool_t           *current;
    mempool_large_t     *large;
    mempool_cleanup_t   *cleanup;
};

mempool_t *mp_create(uint32_t size);
void mp_destroy(mempool_t *pool);
void mp_reset(mempool_t *pool);

void *mp_alloc(mempool_t *pool, uint32_t size);
void *mp_nalloc(mempool_t *pool, uint32_t size);
void *mp_calloc(mempool_t *pool, uint32_t size);
void *mp_align(mempool_t *pool, uint32_t size, uint32_t alignment);
int mp_free(mempool_t *pool, void *p);

mempool_cleanup_t *mempool_cleanup_add(mempool_t *p, uint32_t size);

}
 
#endif //_SPIO_PALLOC_H_INCLUDED_
