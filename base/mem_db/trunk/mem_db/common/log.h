#ifndef MEM_DB_LOG_H
#define MEM_DB_LOG_H
#include <sharelib/util/log.h>

enum TRACE_FLAG {
  TRACE_FLAG_OFF,
  TRACE_FLAG_ON
};

static const std::string loggerName = "mem_db";

#define MEM_DB_LOG_DEBUG(format, args...)    \
  LOG(LOG_LEVEL_DEBUG, sharelib::Log4cppWrapper::GetLog(loggerName),format, ##args)

#define MEM_DB_LOG_INFO(format, args...)    \
  LOG(LOG_LEVEL_INFO, sharelib::Log4cppWrapper::GetLog(loggerName),  format, ##args)

#define MEM_DB_LOG_NOTICE(format, args...)    \
  LOG(LOG_LEVEL_NOTICE, sharelib::Log4cppWrapper::GetLog(loggerName), format, ##args)

#define MEM_DB_LOG_WARN(format, args...)     \
  LOG(LOG_LEVEL_WARN, sharelib::Log4cppWrapper::GetLog(loggerName), format, ##args)

#define MEM_DB_LOG_ERROR(format, args...)    \
  LOG(LOG_LEVEL_ERROR, sharelib::Log4cppWrapper::GetLog(loggerName), format, ##args)

#define MEM_DB_LOG_TRACE(trace_flag,format,...)  \
  if (trace_flag == TRACE_FLAG_ON) {  \
    MEM_DB_LOG_NOTICE(format, ##__VA_ARGS__);  \
  } \
  else {  \
    MEM_DB_LOG_DEBUG(format, ##__VA_ARGS__); \
  }

#define MEM_DB_LOG_TRACEON(format,...)  \
  MEM_DB_LOG_TRACE(TRACE_FLAG_ON, format, ##__VA_ARGS__)


#endif
