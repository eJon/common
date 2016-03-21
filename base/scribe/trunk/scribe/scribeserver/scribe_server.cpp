#include <scribe/scribeserver/scribe_server.h>
#include <scribe/scribeserver/scribe_define.h>
#include <sharelib/util/ini_config.h>
#include <iostream>
#include "scribe/common/scribe_debug_log.h"
using namespace  sharelib;
using namespace  std;

SCRIBESERVER_BEGIN_NAMESPACE_SCRIBE_SERVER

ScribeServer::ScribeServer():go_on_(true) {
}

ScribeServer::~ScribeServer() {
}

int ScribeServer::Init(const char* config_file) {
  CINIConfig ini_config(config_file);
  ini_config.SetAreaName(ScribeDefine::kScribeServerStr.c_str());
  thread_num_ = ini_config.GetUInt(ScribeDefine::kThreadNum, 0);

  cout<<"thread_num_ is "<<thread_num_<<endl;
  max_connection_= ini_config.GetUInt(ScribeDefine::kScribeMaxconnection);
  port_ = ini_config.GetUInt(ScribeDefine::kScribePort);
  int ret = scribe_log_handler_->Init(ini_config);
  if (OK != ret) {
    SCRIBE_LOG_ERROR("scribe_log_handler_ int is failed,ret=%d", ret);
    return ret;
  }
  CreateNonBlockServer();
  scribe_log_handler_->Start();
  Run();
  return 0;
}

int ScribeServer::CreateNonBlockServer() {
  scribe_processor_.reset(new scribeProcessor(scribe_log_handler_));
  protocol_factory_.reset(new TBinaryProtocolFactory(0, 0, false, false));
  thread_manager_ = ThreadManager::newSimpleThreadManager(thread_num_);
  thread_factory_.reset(new PosixThreadFactory());
  thread_manager_->threadFactory(thread_factory_);
  thread_manager_->start();
  nonblock_server_.reset(new (std::nothrow)TNonblockingServer(
                         scribe_processor_, protocol_factory_,
                         port_, thread_manager_));
  nonblock_server_->setMaxConnections(max_connection_);
  nonblock_server_->setOverloadAction(T_OVERLOAD_CLOSE_ON_ACCEPT);

  return OK;
}

int ScribeServer::Stop() {
  scribe_log_handler_->Stop();
  thread_manager_->stop();
  nonblock_server_->stop();
  return 0;
}

void* ScribeServer::InternalProcess() {
  while (onRunning) {
      try {
        if (go_on_) {
          cout<<"ScribeServer::InternalProcess nonblock_server_ serve start"<<endl;
          
          nonblock_server_->serve();

          cout<<"ScribeServer::InternalProcess nonblock_server_ serve over"<<endl;
          go_on_ = false;
        }
    }catch (const std::exception& e){
      cout<<"ScribeServer::InternalProcess exception"<<endl;
      nonblock_server_->stop();
      CreateNonBlockServer();
      go_on_ = true;
    }
    sleep(1); 
    //SCRIBE_LOG_NOTICE("Get Queue Size=%u", scribe_log_handler_->GetQueueSize());
  }
  return NULL;
}

SCRIBESERVER_END_NAMESPACE_SCRIBE_SERVER
