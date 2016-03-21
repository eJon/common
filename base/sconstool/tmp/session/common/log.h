#ifndef SESSION_LOG_H
#define SESSION_LOG_H
#include <sharelib/util/log.h>

enum TRACE_FLAG{ 
  TRACE_FLAG_OFF,
  TRACE_FLAG_ON
};

static const std::string loggerName = "session";

#define SESSION_LOG_DEBUG(format, args...)    \
    LOG(LOG_LEVEL_DEBUG, sharelib::Log4cppWrapper::GetLog(loggerName),format, ##args)

#define SESSION_LOG_INFO(format, args...)    \
    LOG(LOG_LEVEL_INFO, sharelib::Log4cppWrapper::GetLog(loggerName),  format, ##args)

#define SESSION_LOG_NOTICE(format, args...)    \
    LOG(LOG_LEVEL_NOTICE, sharelib::Log4cppWrapper::GetLog(loggerName), format, ##args)

#define SESSION_LOG_WARN(format, args...)     \
    LOG(LOG_LEVEL_WARN, sharelib::Log4cppWrapper::GetLog(loggerName), format, ##args)

#define SESSION_LOG_ERROR(format, args...)    \
    LOG(LOG_LEVEL_ERROR, sharelib::Log4cppWrapper::GetLog(loggerName), format, ##args)

#define SESSION_LOG_TRACE(trace_flag,format,...)  \
    if (trace_flag == TRACE_FLAG_ON) {  \
      SESSION_LOG_NOTICE(format, ##__VA_ARGS__);  \
    } \
    else {  \
      SESSION_LOG_DEBUG(format, ##__VA_ARGS__); \
    }

#define SESSION_LOG_TRACEON(format,...)  \
    SESSION_LOG_TRACE(TRACE_FLAG_ON, format, ##__VA_ARGS__)    


#endif
