#ifndef SCRIBESERVER_SCRIBE_MESSAGE_QUEUE_H_
#define SCRIBESERVER_SCRIBE_MESSAGE_QUEUE_H_

#include <queue>
#include <scribe/common/common.h>
#include <scribe/scribeserver/scribe_message.h>
#include <scribe/scribeserver/current_queue.h>
#include <sharelib/util/lock.h>

SCRIBESERVER_BEGIN_NAMESPACE_SCRIBE_SERVER;

typedef std::tr1::shared_ptr<CurrentQueue<ScribeMessage*> > CurrentQueuePtr;

class ScribeMessageQueue {
  
  public:
    ScribeMessageQueue(uint32_t max_queue_size);

    virtual ~ScribeMessageQueue();

  public:
    // Push one scribe message into  message_queue_.
    int Push(ScribeMessage* scribe_msg_ptr);
    // Pop one scribe message from message_queue_.
    ScribeMessage* Pop();

    int GetQueueSize() const { 
      return msg_current_queue_->GetCurrentQueueSize();
    }

    void Stop();

    uint32_t Size();
  private:
    // Current Queue store scribe message pointer.
    CurrentQueuePtr msg_current_queue_;
    uint32_t max_queue_size_;
};

typedef std::tr1::shared_ptr<ScribeMessageQueue> ScribeMessageQueuePtr;

SCRIBESERVER_END_NAMESPACE_SCRIBE_SERVER; //end namespace

#endif //end SCRIBESERVER_SCRIBE_MESSAGE_QUEUE_H_
