
#ifndef SCRIBESERVER_FAKE_SCRIBE_LOG_HANDLER_H_
#define SCRIBESERVER_FAKE_SCRIBE_LOG_HANDLER_H_

#include "scribe/scribeserver/scribe_log_handler.h"
#include <string>
#include <iostream>
#include <scribe/test/test.h>

SCRIBESERVER_BEGIN_NAMESPACE_SCRIBE_SERVER;

using namespace  scribe::thrift;
using namespace std;

class FakeScribeMessage :public ScribeMessage{

  public:
    FakeScribeMessage() {}
    ~FakeScribeMessage() {}
  public:
    void ToString(std::string& msg_str) {
      msg_str = "FakeScribeMessage";
      std::cout<<msg_str<<std::endl;
    }
    
};

class FakeScribeWorker: public ScribeWorker {

  public:
    FakeScribeWorker(int idin) {id = idin;}
    ~FakeScribeWorker() { 
      std::cout<<"~FakeScribeWorker"<<std::endl; 
    }
    int Handle(ScribeMessage* scribe_message_ptr) {
        SCRIBE_LOG_ERROR("FakeScribeWorker working id %d", id);
      if (NULL != scribe_message_ptr) {
          delete scribe_message_ptr;
      }
      int loog_size = 100;
      for (int i = 0; i < loog_size; ++i) {
       // cout<<"get thread id:"<<pid<<" i"<<i<<endl;
       // sleep(1);
      }
    
      return 0;
    }
public:
    int id;
};


class FakeScribeLogHandler: public ScribeLogHandler {
  public:
    virtual ScribeMessage* LogEntryHandle(const LogEntry& log_entry) {
      std::cout<<"category:"<<log_entry.category<<"\t message:"
               <<log_entry.message<<std::endl;
      
      int loog_size = 100;
      return new FakeScribeMessage();
      for (int i = 0; i< loog_size; ++i) {
        usleep(100);
      }
    }
    int CreateWorker(ScribeWorkerPtr& scribe_worker) {
        scribe_worker.reset(new FakeScribeWorker(id++));
        return 0;
    }
public:
    static int id;
};

class ScribeClientForUnitTest:public CThreadBase {
  
  private :
    virtual void* InternalProcess() {
      std::string cmd = string("sh " + string(TEST_DATA_PATH) +"/conf/scribe_client_tool.sh");
      sleep(2);
      cout<<"ScribeClientForUnitTest cmd:"<<cmd<<endl;
      system(cmd.c_str());
    }
};

typedef std::tr1::shared_ptr<FakeScribeLogHandler> FakeScribeLogHandlerPtr;

SCRIBESERVER_END_NAMESPACE_SCRIBE_SERVER;

#endif //end SCRIBESERVER_FAKE_SCRIBE_LOG_HANDLER_H_


