// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/os/time.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_TIME__
#define _H_TIME_


#include <inttypes.h>
#include <sys/syscall.h>
#include "decr.h"

NSPIO_DECLARATION_START

int64_t clock_mstime(); // clock_gettime version
int64_t rt_mstime();    // gettimeofday version
int64_t clock_ustime(); // clock_gettime version
int64_t rt_ustime();    // gettimeofday version
int64_t clock_nstime(); // clock_gettime version
int64_t rt_nstime();    // gettimeofday version
int rt_usleep(int64_t usec);


}
#endif
