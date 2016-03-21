#ifndef SHARELIB_TASK_TASK_MESSAGE_QUEUE_H_
#define SHARELIB_TASK_TASK_MESSAGE_QUEUE_H_

#include <queue>
#include <tr1/memory>
#include "sharelib/common.h"
#include "sharelib/task/concurrent_queue.h"
#include "sharelib/task/task_message.h"
#include <sharelib/util/lock.h>

SHARELIB_BS;

typedef std::tr1::shared_ptr<CurrentQueue<TaskMessage*> > CurrentQueuePtr;

class TaskMessageQueue {
  
  public:
    TaskMessageQueue(uint32_t max_queue_size);

    virtual ~TaskMessageQueue();

  public:
    // Push one aim message into  message_queue_.
    int Push(TaskMessage* aim_msg_ptr);
    // Pop one aim message from message_queue_.
    TaskMessage* Pop();

    int GetQueueSize() const { 
      return msg_current_queue_->GetCurrentQueueSize();
    }

    void Stop();

    uint32_t Size();
  private:
    // Current Queue store aim message pointer.
    CurrentQueuePtr msg_current_queue_;
    uint32_t max_queue_size_;
};

typedef std::tr1::shared_ptr<TaskMessageQueue> TaskMessageQueuePtr;

SHARELIB_ES;

#endif //end SHARELIB_TASK_TASK_MESSAGE_QUEUE_H_
