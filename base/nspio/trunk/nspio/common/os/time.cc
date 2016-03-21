// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/os/time.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <time.h>
#include <sys/time.h>
#include "os/time.h"

NSPIO_DECLARATION_START


int64_t clock_mstime() {
    struct timespec tv;
    int64_t ct;

    clock_gettime(CLOCK_MONOTONIC, &tv);
    ct = (int64_t)tv.tv_sec * 1000 + tv.tv_nsec / 1000000;

    return ct;
}

// gettimeofday version
int64_t rt_mstime() {
    struct timeval tv;
    int64_t ct;

    gettimeofday(&tv, NULL);
    ct = (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;

    return ct;
}


int64_t clock_ustime() {
    struct timespec tv;
    int64_t ct;

    clock_gettime(CLOCK_MONOTONIC, &tv);    
    ct = (int64_t)tv.tv_sec * 1000000 + tv.tv_nsec / 1000;
    return ct;
}

int64_t rt_ustime() {
    struct timeval tv;
    int64_t ct;

    gettimeofday(&tv, NULL);
    ct = (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;

    return ct;
}

int64_t clock_nstime() {
    struct timespec tv;
    int64_t ct;

    clock_gettime(CLOCK_MONOTONIC, &tv);    
    ct = (int64_t)tv.tv_sec * 1000000000 + tv.tv_nsec;
    return ct;
}

int64_t rt_nstime() {
    struct timeval tv;
    int64_t ct;

    gettimeofday(&tv, NULL);
    ct = (int64_t)tv.tv_sec * 1000000000 + (int64_t)tv.tv_usec * 1000;

    return ct;
}
    

int rt_usleep(int64_t usec) {
    struct timespec tv;
    tv.tv_sec = usec / 1000000;
    tv.tv_nsec = (usec % 1000000) * 1000;
    return nanosleep(&tv, NULL);
}


}
