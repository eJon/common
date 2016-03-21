// copyright:
//            (C) SINA Inc.
//
//           file: nspio/api/mqueue.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "mqueue.h"


NSPIO_DECLARATION_START

LockQueue::LockQueue() :
    cleanup_func(NULL)
{
    pmutex_init(&lock);
}


LockQueue::~LockQueue() {
    struct appmsg *raw = NULL;

    while (!Empty()) {
	raw = Pop();
	if (cleanup_func)
	    cleanup_func(raw);
    }
    pmutex_destroy(&lock);
}


int LockQueue::Setup(int cap, waitqueue_cleanup_func cfunc) {
    Init(cap);
    cleanup_func = cfunc;
    return 0;
}

}    
