// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/sync/waitgroup.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_WAITGROUP_
#define _H_WAITGROUP_

// NSPIO_DECLARATION_START
#include <pthread.h>

class RefSync {
 public:
    RefSync();
    ~RefSync();

    int Add(int refs = 1);
    int Done(int refs = 1);
    int Wait();
    int Ref() {
	return get();
    }

 private:
    int ref;
    int get();
    int incr(int refs);
    int decr(int refs);

    pthread_cond_t cond;
    pthread_mutex_t mutex;
};


//}
#endif  // _H_WAITGROUP_
