// copyright:
//            (C) SINA Inc.
//
//           file: nspio/api/mqueue.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _NSPIO_MQUEUE_H_
#define _NSPIO_MQUEUE_H_

#include "sync/tmutex.h"
#include "sync/pcond.h"
#include "sync_api.h"
#include "base/mq.h"

using namespace std;

NSPIO_DECLARATION_START

DEFINE_MQ(base_mq, struct appmsg, node);
typedef void (*waitqueue_cleanup_func)(struct appmsg *);
    
class LockQueue : public base_mq {
 public:
    LockQueue();
    ~LockQueue();
    int Setup(int cap, waitqueue_cleanup_func cfunc);
    void Lock() {
	pmutex_lock(&lock);
    }
    void UnLock() {
	pmutex_unlock(&lock);
    }
    bool Empty() {
	return Size() == 0;
    }

 private:
    pmutex_t lock;
    waitqueue_cleanup_func cleanup_func;
};









}


#endif   // _NSPIO_WAITQUEUE_H_
