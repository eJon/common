#ifndef SHARELIB_TASK_TASK_HANDLER_H_
#define SHARELIB_TASK_TASK_HANDLER_H_
#include <sharelib/util/ini_config.h>
#include "sharelib/task/task_message_queue.h"
#include "sharelib/task/task_worker.h"

SHARELIB_BS;

class TaskHandler {
  
  public:
    TaskHandler() {}
    virtual ~TaskHandler() {
      Stop();
    }

  public:

    int Init(uint32_t max_queue_size, uint32_t worker_num);
    // Runtime update request.
    int Handle(/*bidfeedcomon*/TaskMessage* task_msg);
    // Restart indexserver use this function.
    int Stop();

    virtual TaskMessage* HandleMessage(TaskMessage* task_msg) = 0;

    virtual int CreateWorker(TaskWorkerPtr& task_worker_ptr) = 0;
    int Start();
  private:
    // Disallow copy.
    DISALLOW_COPY_AND_ASSIGN(TaskHandler);
    int CreateWorkers();
  protected:
    std::vector<sharelib::TaskWorkerPtr> workers_;
    TaskMessageQueuePtr task_message_queue_ptr_;
  private:
    uint32_t max_queue_size_;
    uint32_t worker_num_;
};

SHARELIB_ES;

#endif //end SHARELIB_TASK_TASK_HANDLER_H_
