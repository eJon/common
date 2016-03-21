// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/runner/thread.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_OSTHREAD_
#define _H_OSTHREAD_

#include <pthread.h>
#include <sys/syscall.h>

static inline pid_t gettid()
{
    return syscall(SYS_gettid);
}


typedef int (Thread_fn) (void*);
class Thread {
 public:
    Thread () : tfn(NULL), m_arg(NULL), tret(0),
	tid(0) 
    {
    }

    int Start (Thread_fn *tfn_, void *arg_);
    int Stop ();

    Thread_fn *tfn;
    void * m_arg;
    int tret;
    pid_t tid;
 private:
    pthread_t m_descriptor;
    Thread (const Thread&);
    const Thread &operator = (const Thread&);
};

#endif  // _H_OSTHREAD_
