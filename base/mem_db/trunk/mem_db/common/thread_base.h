
#ifndef _THREADBASE_H
#define _THREADBASE_H

#include <pthread.h>

namespace mem_db_util {

class ThreadBase {
  public:
    ThreadBase () : thread_handler_(0), is_running_(false) {};
    virtual ~ThreadBase () {};

    int Run ();
    int Stop ();

  private:
    static void *ThreadFun (void *arg);
    virtual void *InternalProcess() = 0;

    pthread_t thread_handler_;
    bool is_running_;
};

}  // namespace mem_db

#endif
