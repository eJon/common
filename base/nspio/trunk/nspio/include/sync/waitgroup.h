// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/sync/waitgroup.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_WAITGROUP_
#define _H_WAITGROUP_

#include "pmutex.h"
#include "pcond.h"

NSPIO_DECLARATION_START


class WaitGroup {
 public:
    WaitGroup();
    ~WaitGroup();

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

    pcond_t cond;
    pmutex_t mutex;
};


}
#endif  // _H_WAITGROUP_
