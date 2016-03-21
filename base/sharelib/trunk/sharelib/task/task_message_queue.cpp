#include <sharelib/task/task_message_queue.h>

SHARELIB_BS;

TaskMessageQueue::TaskMessageQueue(uint32_t max_queue_size) {
  // Init lock for queue.
  msg_current_queue_.reset(new CurrentQueue<TaskMessage*>
                           (max_queue_size));
}

TaskMessageQueue::~TaskMessageQueue() {
  
}
int TaskMessageQueue::Push(TaskMessage* task_msg_ptr) {
  return msg_current_queue_->Push(task_msg_ptr);
}

TaskMessage* TaskMessageQueue::Pop() {
  TaskMessage* taskmsg_ptr = NULL;
  msg_current_queue_->WaitAndPop(taskmsg_ptr);
  return taskmsg_ptr;
}

void TaskMessageQueue::Stop() {
  msg_current_queue_->Stop();
}

uint32_t TaskMessageQueue::Size() {
  return msg_current_queue_->GetCurrentQueueSize();
}

SHARELIB_ES;
