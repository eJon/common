#include <istream>
#include <scribe/scribeserver/scribe_log_handler.h>
#include "scribe/scribeserver/scribe_define.h"
#include "scribe/common/scribe_debug_log.h"

SCRIBESERVER_BEGIN_NAMESPACE_SCRIBE_SERVER

using namespace  scribe::thrift;
using namespace  sharelib;
using namespace  std;

int ScribeLogHandler::Init(CINIConfig& ini_config) {

  ini_config.SetAreaName(ScribeDefine::kScribeLogHandlerStr.c_str());

  worker_num_ = ini_config.GetUInt(ScribeDefine::kWorkerNum, 0);
  if (worker_num_ == 0) {
    SCRIBE_LOG_ERROR("worker_num_ is empty");
    return -1;
  }
  uint32_t queue_size= ini_config.GetUInt(ScribeDefine::kMaxQueueSize, 0);

  if (queue_size == 0) {
    SCRIBE_LOG_ERROR("queue_size is empty");
    return -1;
  }
  // Log queue size
  log_interval_ = ini_config.GetUInt(ScribeDefine::kLogInterval, 1);
  int ret = CreateWorkers();
  if (OK != ret) {
    SCRIBE_LOG_ERROR("Create Workers failed,ret=%d", ret); 
    return ret;
  }
  scribe_message_queue_ptr_.reset(new ScribeMessageQueue(queue_size));
  ret = InitWorkers();
  if (OK != ret) {
    SCRIBE_LOG_ERROR("Init Workers failed,ret=%d", ret); 
    return ret;
  }
  return 0;
}

ResultCode::type ScribeLogHandler::Log(const vector<LogEntry>& log_entries) {
  if (0 == log_entries.size()) {
    return ResultCode::OK;
  }
  long current_time = time(NULL);
  if ((current_time - last_log_time_) > log_interval_) {
    cout<<"current_time:"<<current_time<<" log_interval_:"<<log_interval_ <<endl;
    SCRIBE_LOG_NOTICE("Get Queue Size=%u", GetQueueSize());
    last_log_time_ = current_time;
  }
  size_t i = 0;
  ScribeMessage* scribe_msg_ptr;
  std::string content;
  SCRIBE_LOG_DEBUG("logentries size=%lu", log_entries.size());
  for (i = 0; i < log_entries.size(); ++i) {
    SCRIBE_LOG_DEBUG("receive log cat:%s,message：%s", 
        log_entries[i].category.c_str(),
        log_entries[i].message.c_str());
    scribe_msg_ptr = LogEntryHandle(log_entries[i]);

    if (NULL != scribe_msg_ptr) {

      while (OK != scribe_message_queue_ptr_->Push(scribe_msg_ptr)) {

        SCRIBE_LOG_ERROR("Push queue is error,size=%u", 
            scribe_message_queue_ptr_->Size());
        usleep(10);
        continue;

      }
      // ret = scribe_message_queue_ptr_->Push(scribe_msg_ptr);

    }
    else {
      SCRIBE_LOG_ERROR("LogEntryHandle is failed");
    }
  }
  return ResultCode::OK;
}

int ScribeLogHandler::CreateWorkers() {
  
  ScribeWorkerPtr scribe_worker;
  int ret = 0;
  for (uint32_t i = 0; i < worker_num_; ++i) {
    ret = CreateWorker(scribe_worker);
    if (0 != ret) {
      continue;
    }
    workers_.push_back(scribe_worker);
  }
  
  return OK;
  
}

int ScribeLogHandler::InitWorkers() {
  size_t i = 0;
  for (i = 0; i < workers_.size(); ++i) {
    (workers_[i])->SetScribeMessageQueuePtr(scribe_message_queue_ptr_);
  }
  return 0;
}

uint32_t ScribeLogHandler::GetQueueSize() {
  return scribe_message_queue_ptr_->Size();
}

int ScribeLogHandler::Start() {
  size_t i = 0;
  for (i = 0; i < workers_.size(); ++i) {
    (workers_[i])->Start();
  }
  return 0;
}

int ScribeLogHandler::Stop() {
  size_t i = 0;
  for (i = 0; i < workers_.size(); ++i) {
    (workers_[i])->SetRuningStat(false);
  }
  scribe_message_queue_ptr_->Stop();
  for (i = 0; i < workers_.size(); ++i) {
    (workers_[i])->Stop();
  }
 
  return 0;
}


SCRIBESERVER_END_NAMESPACE_SCRIBE_SERVER
