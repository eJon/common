#ifndef SCRIBESERVER_SCRIBE_WORKER_H_
#define SCRIBESERVER_SCRIBE_WORKER_H_
#include <scribe/common/common.h>
#include <scribe/scribeserver/scribe_worker.h>
#include <scribe/scribeserver/scribe_message_queue.h>
#include <sharelib/util/thread_base.h>
#include "scribe/common/scribe_debug_log.h"
#include "scribe/scribeserver/scribe_define.h"
namespace scribe_server {

  class ScribeWorker: public CThreadBase {
 
    public:
      ScribeWorker() {}
      virtual ~ScribeWorker() {
      }

    public:
      virtual int Start();
      virtual int Init() {
        return 0;
      }

      void SetRuningStat(bool stat) {
        onRunning = stat;
      }
      // End this scribe worker.
      virtual int Stop();
      void SetScribeMessageQueuePtr(ScribeMessageQueuePtr& scribe_message_queue_ptr) {
        scribe_message_queue_ptr_ = scribe_message_queue_ptr;
      }
      virtual int Handle(ScribeMessage* scribe_message_ptr) = 0;
      
    private:
      // Every process worker uses this funtion processor
      // scribe server receive message.
      ScribeMessageQueuePtr scribe_message_queue_ptr_;

    private:
      virtual void* InternalProcess();

  };
  typedef std::tr1::shared_ptr<ScribeWorker> ScribeWorkerPtr;
}

#endif // SCRIBESERVER_SCRIBE_WORKER_H_
