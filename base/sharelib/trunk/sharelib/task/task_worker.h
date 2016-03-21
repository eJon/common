#ifndef SHARELIB_TASK_TASK_WORKER_H_
#define SHARELIB_TASK_TASK_WORKER_H_
#include "sharelib/server/thread_base.h"
#include "sharelib/task/task_message_queue.h"
#include "sharelib/task/task_worker.h"
#include "sharelib/util/ini_config.h"

SHARELIB_BS;

class TaskWorker :public sharelib::ThreadBase {
    public:
      TaskWorker() {}
      virtual ~TaskWorker() {
      }

    public:
      virtual int Start();

      void SetRuningStat(bool stat) {
        onRunning = stat;
      }
      // End this aim worker.
      virtual int Stop();

      void SetTaskMessageQueuePtr(TaskMessageQueuePtr& task_message_queue_ptr) {
        task_message_queue_ptr_ = task_message_queue_ptr;
      }

      virtual int HandleOne(TaskMessage* task_message_ptr) = 0;
      
    private:
      // Every process worker uses this funtion processor
      // aim server receive message.
      TaskMessageQueuePtr task_message_queue_ptr_;

    private:
      virtual void* InternalLoop();

  };
  typedef std::tr1::shared_ptr<TaskWorker> TaskWorkerPtr;

SHARELIB_ES;

#endif //end SHARELIB_TASK_TASK_WORKER_H_
