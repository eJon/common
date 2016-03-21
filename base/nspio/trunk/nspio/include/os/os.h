// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/os/os.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef __OS_H_INCLUDED__
#define __OS_H_INCLUDED__


#include <inttypes.h>
#include <sys/syscall.h>
#include "decr.h"

NSPIO_DECLARATION_START

extern uint32_t  SLB_PAGESIZE;
extern uint32_t  SLB_PAGESIZE_SHIFT;
extern uint32_t  SLB_CACHELINE_SIZE;

#define gettid() syscall(__NR_gettid)
int nspio_os_init();


}
#endif
