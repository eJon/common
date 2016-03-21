#ifndef SCRIBESERVER_SCRIBE_SERVER_H_
#define SCRIBESERVER_SCRIBE_SERVER_H_
#include <sharelib/util/thread_base.h>

#include <arpa/inet.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/fb303/FacebookService.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/concurrency/PosixThreadFactory.h>

#include "scribe/common/common.h"
#include "scribe/common/scribe.h"
#include "scribe/scribeserver/scribe_message_queue.h"
#include "scribe/scribeserver/scribe_log_handler.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using namespace apache::thrift::concurrency;

using namespace  ::scribe::thrift;

SCRIBESERVER_BEGIN_NAMESPACE_SCRIBE_SERVER;

//using namespace apache::thrift::protocol;
class ScribeServer : public CThreadBase {
  
  public:
    ScribeServer();
    virtual ~ScribeServer();

  public:
    int Init(const char* config_file);

    int Stop();

    void SetScribeLogHandler(ScribeLogHandlerPtr scribe_log_handler) {
      scribe_log_handler_ = scribe_log_handler;
    }
  private:
    virtual void* InternalProcess();

    int CreateNonBlockServer();
  private:

    uint32_t thread_num_;
    uint32_t max_connection_;
    std::string config_file_path_;
    // Save worker
    std::vector<ScribeWorker*> worker_ptr;
    boost::shared_ptr<TProcessor> scribe_processor_;
    boost::shared_ptr<TProtocolFactory> protocol_factory_;
    boost::shared_ptr<ThreadManager> thread_manager_;
    boost::shared_ptr<PosixThreadFactory>  thread_factory_;
    boost::shared_ptr<TNonblockingServer> nonblock_server_;
    ScribeLogHandlerPtr scribe_log_handler_;   
    uint32_t port_;
    bool go_on_;
};

typedef std::tr1::shared_ptr<ScribeServer> ScribeServerPtr;

SCRIBESERVER_END_NAMESPACE_SCRIBE_SERVER;

#endif // SCRIBESERVER_SCRIBE_SERVER_H_
