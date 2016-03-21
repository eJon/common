// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/os/posix.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <unistd.h>
#include "os/time.h"


NSPIO_DECLARATION_START

uint32_t SLB_PAGESIZE;
uint32_t SLB_PAGESIZE_SHIFT;
uint32_t SLB_CACHELINE_SIZE;
uint64_t nspio_start_timestamp;

int nspio_os_init() {
    SLB_PAGESIZE = getpagesize();
    for (uint32_t n = SLB_PAGESIZE; n >>= 1; SLB_PAGESIZE_SHIFT++)
	{ /* void */ }
    nspio_start_timestamp = rt_mstime();
    return 0;
}


}
