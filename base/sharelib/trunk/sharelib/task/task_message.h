#ifndef SHARELIB_TASK_TASK_MESSAGE_H_
#define SHARELIB_TASK_TASK_MESSAGE_H_
#include "sharelib/common.h"

SHARELIB_BS
class TaskMessage {
  public:
    TaskMessage() {};
    virtual ~TaskMessage() {};
  public:
    virtual void ToString(std::string& msg_str) = 0;

};
SHARELIB_ES
#endif // end SHARELIB_UTIL_TASK_MESSAGE_H_
