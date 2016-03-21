#include <iostream>
#include <sharelib/task/task_worker.h>
#include <sharelib/util/sharelib_log.h>
#include <sharelib/util/log.h>
SHARELIB_BS;
using namespace std;

int TaskWorker::Start() {
  Run();
  return 0;
}

int TaskWorker::Stop() {
  ThreadBase::Stop();
  return 0;
}

void* TaskWorker::InternalLoop() {
  TaskMessage* task_msg_ptr = NULL;
  int ret = 0;
  while (onRunning) {
    task_msg_ptr = task_message_queue_ptr_->Pop();
    if (NULL == task_msg_ptr) {
      SHARELIB_LOG_ERROR("Pop message is failed,task_msg_ptr is null");
      continue;
    }
    ret = HandleOne(task_msg_ptr);
    if (0 != ret) {
      SHARELIB_LOG_INFO("Handle is failed, ret=%d", ret);
    }
  }
  return NULL;
}

SHARELIB_ES;
