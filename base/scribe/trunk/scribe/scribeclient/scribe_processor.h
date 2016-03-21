#ifndef NGINX_TAOBAO_ZHITONGCHE_SCRIBE_PROCESSOR_H_
#define NGINX_TAOBAO_ZHITONGCHE_SCRIBE_PROCESSOR_H_
 #include <string>
 #include <arpa/inet.h>
 #include <vector>
 // scribe headers                                                                                      
 #include "scribe/common/scribe.h" // this header is in the gen-cpp directory                                         
 #include <thrift/protocol/TBinaryProtocol.h>                                                                  
 #include <thrift/transport/TSocket.h>                                                                         
 #include <thrift/transport/TTransportUtils.h>                                                             
 #include "thread_rw_lock.h"

 using namespace boost;                                                                                 
 using namespace apache::thrift;                                                                        
 using namespace apache::thrift::protocol;                                                              
 using namespace apache::thrift::transport;                                                             
 using namespace scribe::thrift; 
enum scribe_ret_t{
    scribe_log_success = 0,
    scribe_transport_close = 1,
    scribe_ommit_log = 2,
    scribe_init_error = 3
};

 class ScribeProcessor {
 public:
     //pair
     ScribeProcessor(const std::string host, const int port, const std::string category, const std::string log_repersist_file);
     int log(const std::string &msg);

     //pair
     ScribeProcessor(const std::string host, const int port, const std::string log_repersist_file);
     int log(const std::string &msg, const std::string& category);
 public:
     ~ScribeProcessor();
     int init();
 private:
     int logFunc(const std::string &msg, const std::string& category) ;
     int serialize_messages();
   private:
      std::string host_; 
      int port_;
      std::string category_;
      std::string log_repersist_file_;
      shared_ptr<TTransport> transport_;
      shared_ptr<scribeClient> sbclient_;
      std::vector<LogEntry> messages_;
 };
#endif //NGINX_TAOBAO_ZHITONGCHE_SCRIBE_PROCESSOR_H_
