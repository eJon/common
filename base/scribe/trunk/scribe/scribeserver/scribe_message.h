#ifndef SCRIBESERVER_SCRIBE_MESSAGE_H_
#define SCRIBESERVER_SCRIBE_MESSAGE_H_
#include "scribe/common/common.h"

SCRIBESERVER_BEGIN_NAMESPACE_SCRIBE_SERVER;

class ScribeMessage {
  public:
    ScribeMessage() {};
    virtual ~ScribeMessage() {};
  public:
    virtual void ToString(std::string& msg_str) = 0;

};

SCRIBESERVER_END_NAMESPACE_SCRIBE_SERVER;
#endif // end SCRIBESERVER_SCRIBE_MESSAGE_H_
