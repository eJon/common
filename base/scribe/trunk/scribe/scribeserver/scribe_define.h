#ifndef SCRIBESERVER_SCRIBE_DEFINE_H_
#define SCRIBESERVER_SCRIBE_DEFINE_H_
#include "scribe/common/common.h"
SCRIBESERVER_BEGIN_NAMESPACE_SCRIBE_SERVER

enum RetErrorCode {
  OK = 0
};


class ScribeDefine {

  public:
    const static std::string kScribeServerStr;
    const static std::string kThreadNum;
    const static std::string kWorkerNum;
    const static std::string kScribeLogHandlerStr;
    const static std::string kScribePort;
    const static std::string kScribeMaxconnection;
    const static std::string kMaxQueueSize;
    const static std::string kLogInterval;

};


SCRIBESERVER_END_NAMESPACE_SCRIBE_SERVER

#endif //end SCRIBESERVER_SCRIBE_DEFINE_H_
