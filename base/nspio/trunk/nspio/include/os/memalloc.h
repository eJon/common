// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/os/memalloc.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _MEM_ALLOC_H_INCLUDED_
#define _MEM_ALLOC_H_INCLUDED_

#include <inttypes.h>
#include "decr.h"


NSPIO_DECLARATION_START

typedef struct {
    int64_t alloc;
    int64_t alloc_size;
    int64_t memalign;
    int64_t memalign_size;
    int64_t free;
    int64_t free_size;
} mem_stat_t;



void *mem_alloc(uint32_t size);
void *mem_zalloc(uint32_t size);
void *mem_realloc(void *ptr, uint32_t size);
void mem_free(void *ptr);
void mem_free(void *ptr, uint32_t size);
void *mem_align(uint32_t alignment, uint32_t size);
const mem_stat_t *mem_stat();


}


#endif //_MEM_ALLOC_H_INCLUDED_
