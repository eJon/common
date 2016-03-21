#ifndef TOOLS_SCRIBE_CLIENT_UNITTEST_H_
#define TOOLS_SCRIBE_CLIENT_UNITTEST_H_
#include "scribe/scribeclient/scribe_processor.h"
#include <tr1/memory>
namespace scribeclient {
  typedef std::tr1::shared_ptr<ScribeProcessor> ScribeProcessorPtr;

  class ScribeClient {

    public:
      ScribeClient() {
          warnCount = 2000;}
      ~ScribeClient() {}
      int Init(const char* config_file);
      int SendLog(const std::string& log_str);

      int SendFileLog(int64_t speedPerSecond);
  public:
      static int sendErrorCount;
      static int sendCount;
      static int lastTime;
    private:
      int warnCount;
      std::string host_;
      int port_;
      std::string log_path_;
      std::string category_;
      std::string data_file_;

      ScribeProcessorPtr scribeprocessor_ptr;
  };
  typedef std::tr1::shared_ptr<ScribeClient> ScribeClientPtr;
}
#endif //end TOOLS_SCRIBE_CLIENT_UNITTEST_H_
