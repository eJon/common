#ifndef SCRIBESERVER_SCRIBE_SERVER_HANDLER_H_
#define SCRIBESERVER_SCRIBE_SERVER_HANDLER_H_
#include <sharelib/util/ini_config.h>
#include <scribe/common/scribe.h>
#include <scribe/scribeserver/scribe_message_queue.h>
#include <scribe/scribeserver/scribe_worker.h>

SCRIBESERVER_BEGIN_NAMESPACE_SCRIBE_SERVER;

using namespace  scribe::thrift;
class ScribeLogHandler : public scribe::thrift::scribeNull {

  public:
    ScribeLogHandler(){
      log_interval_ = 0;
      last_log_time_ = 0;
    }
    virtual ~ScribeLogHandler() {
      //Stop();
    }

  public:

    ResultCode::type Log(const std::vector<LogEntry> & message);

    virtual int Init(sharelib::CINIConfig& ini_config);

    /*override*/virtual int CreateWorker(ScribeWorkerPtr& scribe_worker) {
        return 0;
    }

    int CreateWorkers();
  
    virtual int Stop();

    virtual int Start();
    // Every log entry from romate scribe client is formated,change log into 
    // ScribeMessage object, and then push this object into message queue.
    // This function need be overloaded to finish these function.
    virtual ScribeMessage* LogEntryHandle(const LogEntry& log_entry) = 0;

    int InitWorkers();

    uint32_t GetQueueSize();
  private:

    //boost::shared_ptr< ::apache::thrift::TProcessor> processor_;

    ScribeMessageQueuePtr scribe_message_queue_ptr_;

    uint32_t worker_num_;

    std::vector<ScribeWorkerPtr> workers_;

    int64_t last_log_time_;
    int64_t log_interval_;

};
typedef boost::shared_ptr<ScribeLogHandler> ScribeLogHandlerPtr;

SCRIBESERVER_END_NAMESPACE_SCRIBE_SERVER;


#endif //end SCRIBESERVER_SCRIBE_SERVER_HANDLER_H_
