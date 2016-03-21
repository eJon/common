
#include <scribe/scribeserver/scribe_message_queue.h>

SCRIBESERVER_BEGIN_NAMESPACE_SCRIBE_SERVER;

ScribeMessageQueue::ScribeMessageQueue(uint32_t max_queue_size) {
  // Init lock for queue.
  msg_current_queue_.reset(new CurrentQueue<ScribeMessage*>
                           (max_queue_size));
}

ScribeMessageQueue::~ScribeMessageQueue() {
  
}
int ScribeMessageQueue::Push(ScribeMessage* scribe_msg_ptr) {
  return msg_current_queue_->Push(scribe_msg_ptr);
}

ScribeMessage* ScribeMessageQueue::Pop() {
  ScribeMessage* scribemsg_ptr = NULL;
  msg_current_queue_->WaitAndPop(scribemsg_ptr);
  return scribemsg_ptr;
}

void ScribeMessageQueue::Stop() {
  msg_current_queue_->Stop();
}

uint32_t ScribeMessageQueue::Size() {
  return msg_current_queue_->GetCurrentQueueSize();
}

SCRIBESERVER_END_NAMESPACE_SCRIBE_SERVER;
