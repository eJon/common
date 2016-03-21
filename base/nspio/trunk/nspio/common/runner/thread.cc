// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/runner/thread.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <unistd.h>
#include <nspio/errno.h>
#include "os/os.h"
#include "runner/thread.h"


NSPIO_DECLARATION_START

static void *thread_routine (void *arg_)
{
    Thread *self = (Thread*) arg_;

    self->tid = gettid();
    self->tret = self->tfn (self->m_arg);

    return NULL;
}

int Thread::Start (Thread_fn *tfn_, void *arg_)
{
    tfn = tfn_;
    m_arg =arg_;
    int rc = pthread_create (&m_descriptor, NULL, thread_routine, this);
    return rc;
}

int Thread::Stop ()
{
    void *status;

    pthread_join (m_descriptor, &status);
    return tret;
}


}
