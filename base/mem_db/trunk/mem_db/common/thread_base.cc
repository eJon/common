
#include <mem_db/common/thread_base.h>

namespace mem_db_util {

int ThreadBase::Run () {
  int ret = 0;
  ret = ::pthread_create(&thread_handler_, NULL, ThreadBase::ThreadFun, (void*)this);
  if (0 != ret) {
    return ret;
  }

  is_running_ = true;

  return 0;
}

int ThreadBase::Stop() {
  int ret = 0;
  if (false == is_running_) {
    return ret;
  }

  ret = pthread_join(thread_handler_, NULL);
  if (0 != ret) {
    return ret;
  }

  is_running_ = false;

  return 0;
}


void *ThreadBase::ThreadFun(void *arg) {
  ThreadBase *self = (ThreadBase*)arg;

  return self->InternalProcess();
}

}  // namespace mem_db
