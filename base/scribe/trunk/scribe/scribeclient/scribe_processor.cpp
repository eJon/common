#include <sstream>
#include <fstream>
#include <time.h>
#include <sys/file.h>
#include "scribe_processor.h"

#define MAX_QUEUE_SIZE 100

ScribeProcessor::ScribeProcessor(const std::string host, const int port, const std::string category, const std::string log_repersist_file) : host_(host), port_(port), category_(category), log_repersist_file_(log_repersist_file) {
}

ScribeProcessor::ScribeProcessor(const std::string host, const int port, const std::string log_repersist_file)
    :host_(host), port_(port),log_repersist_file_(log_repersist_file) 
      
{
}
ScribeProcessor::~ScribeProcessor() {
  if (true == transport_->isOpen()) {
    transport_->close();
  }

  
}

int ScribeProcessor::init() {
    try{
        shared_ptr<TTransport> socket1(new TSocket(host_, port_));
        shared_ptr<TTransport> transport1(new TFramedTransport(socket1));
        shared_ptr<TProtocol> protocol1(new TBinaryProtocol(transport1));
        // use boost shared pointer
        shared_ptr<scribeClient> sbclient1(new scribeClient(protocol1));
        transport_ = transport1;
        sbclient_ = sbclient1;

        transport_->open();
        if (true != transport_->isOpen()) {
            return scribe_init_error;
        }
    }catch(TException &tx){
        return scribe_init_error;    
    }

    return scribe_log_success;
}

int ScribeProcessor::log(const std::string &msg, const std::string& category){
    return logFunc(msg,category); 
}
int ScribeProcessor::log(const std::string &msg) {
    return logFunc(msg,category_); 
}
int ScribeProcessor::logFunc(const std::string &msg, const std::string& category) {

    
  LogEntry le;
  le.message = msg;
  le.category = category;
  messages_.push_back(le);
  
  if (false == transport_->isOpen()) {
      try {
          transport_->open();
      } catch (TException &tx) {
          serialize_messages();
          return scribe_transport_close;
      }
  }
  
  if(false == transport_->isOpen()) {
      serialize_messages();
      return scribe_transport_close;
  }
  
  scribe_ret_t ret = scribe_log_success;
  FILE *fp;
  if (access(log_repersist_file_.c_str(), F_OK) == 0
      && ( NULL != (fp = fopen(log_repersist_file_.c_str(), "r")))){
      
      // grab exclusive lock, fail if can't obtain.
      int rc = flock(fileno(fp), LOCK_EX | LOCK_NB);
      if (0 != rc) {
          fclose(fp);
          ret = scribe_log_success;
      }else{
          char buf[1024];
          while (fgets(buf, 1024, fp) != NULL) {
              le.message = std::string(buf);
              le.category = category;
              messages_.push_back(le);
              if (messages_.size() >= MAX_QUEUE_SIZE) {
                  try{
                      if (ResultCode::OK == sbclient_->Log(messages_)) {
                          messages_.clear();
                      }
                  }catch(TException &tx){
                      //todo ommit some logs
                      ret = scribe_ommit_log;
                      continue;
                  }
              }
          }
          unlink(log_repersist_file_.c_str());
          flock(fileno(fp), LOCK_UN);
          fclose(fp);
      }
  }
      
  
  try {
      if (0 == sbclient_->Log(messages_)) {
          messages_.clear();
      }
  } catch (TException &tx) {
      serialize_messages();
      ret = scribe_transport_close;
  }
      
      
  return ret;
}

int ScribeProcessor::serialize_messages() {
  if (messages_.size() > MAX_QUEUE_SIZE) {
    std::fstream file;
    file.open(log_repersist_file_.c_str(), std::fstream::out|std::fstream::app);
    if (true == file.is_open()) {
      for (unsigned int i = 0; i < messages_.size(); i++) {
        file << messages_[i].message.c_str() << std::endl;
      }
      messages_.clear();
    }
    file.close();
    file.clear();
  }
  return 0;
}
