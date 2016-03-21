#include "sharelib/task/task_handler.h"
#include "sharelib/util/sharelib_log.h"

using namespace std;

SHARELIB_BS;

int TaskHandler::Init(uint32_t max_queue_size, 
                      uint32_t worker_num) {
  max_queue_size_ = max_queue_size;
  worker_num_ = worker_num;
  task_message_queue_ptr_.reset(new TaskMessageQueue(max_queue_size));

  if (0 != CreateWorkers()) {
    SHARELIB_LOG_ERROR("CreateWorkers failed!");
    return -1;
  }
  return 0;
}

int TaskHandler::Handle(TaskMessage* task_message) {
  TaskMessage* task_msg = HandleMessage(task_message);
  if (NULL == task_msg) {
    SHARELIB_LOG_ERROR("Handle, new AdTaskContent failed!");
    return -1;
  }
  if (0 != task_message_queue_ptr_->Push(task_msg)) {
    SHARELIB_LOG_ERROR("task Queue Push is failed");
    return -1;
  }
  return 0;
}

int TaskHandler::CreateWorkers() {
  uint32_t i = 0;
  TaskWorkerPtr task_worker_ptr;
  for (i = 0; i < worker_num_; ++i) {
    if (0 != CreateWorker(task_worker_ptr)) {
      SHARELIB_LOG_ERROR("Create worker failed!");
      continue;
    }
    task_worker_ptr->SetTaskMessageQueuePtr(task_message_queue_ptr_);
    workers_.push_back(task_worker_ptr);
  }
  return 0;
}

int TaskHandler::Start() {
  size_t i = 0;
  for (i = 0; i < workers_.size(); ++i) {
    (workers_[i])->Start();
  }
  return 0;
}

int TaskHandler::Stop() {                                                                         
  size_t i = 0;                                                                                        
  for (i = 0; i < workers_.size(); ++i) {                                                              
    (workers_[i])->SetRuningStat(false);                                                               
  }                                                                                                    
  task_message_queue_ptr_->Stop();                                                                   
  for (i = 0; i < workers_.size(); ++i) {                                                              
    (workers_[i])->Stop();                                                                             
  }                                                                                                    

  return 0;                                                                                            
}

SHARELIB_ES;

