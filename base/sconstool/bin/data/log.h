#ifndef EXAMPLE_LOG_H
#define EXAMPLE_LOG_H
#include <sharelib/util/log.h>

enum TRACE_FLAG{ 
  TRACE_FLAG_OFF,
  TRACE_FLAG_ON
};

static const std::string loggerName = "example";

#define EXAMPLE_LOG_DEBUG(format, args...)    \
    LOG(LOG_LEVEL_DEBUG, sharelib::Log4cppWrapper::GetLog(loggerName),format, ##args)

#define EXAMPLE_LOG_INFO(format, args...)    \
    LOG(LOG_LEVEL_INFO, sharelib::Log4cppWrapper::GetLog(loggerName),  format, ##args)

#define EXAMPLE_LOG_NOTICE(format, args...)    \
    LOG(LOG_LEVEL_NOTICE, sharelib::Log4cppWrapper::GetLog(loggerName), format, ##args)

#define EXAMPLE_LOG_WARN(format, args...)     \
    LOG(LOG_LEVEL_WARN, sharelib::Log4cppWrapper::GetLog(loggerName), format, ##args)

#define EXAMPLE_LOG_ERROR(format, args...)    \
    LOG(LOG_LEVEL_ERROR, sharelib::Log4cppWrapper::GetLog(loggerName), format, ##args)

#define EXAMPLE_LOG_TRACE(trace_flag,format,...)  \
    if (trace_flag == TRACE_FLAG_ON) {  \
      EXAMPLE_LOG_NOTICE(format, ##__VA_ARGS__);  \
    } \
    else {  \
      EXAMPLE_LOG_DEBUG(format, ##__VA_ARGS__); \
    }

#define EXAMPLE_LOG_TRACEON(format,...)  \
    EXAMPLE_LOG_TRACE(TRACE_FLAG_ON, format, ##__VA_ARGS__)    


#endif
