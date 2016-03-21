#ifndef SCRIBE_SCRIBE_LOG_H
#define SCRIBE_SCRIBE_LOG_H
#include <sharelib/util/log.h>

static const std::string scribe_loggerName = "scribe";

#define SCRIBE_LOG_DEBUG(format, args...)    \
    LOG(LOG_LEVEL_DEBUG, sharelib::Log4cppWrapper::GetLog(scribe_loggerName),format, ##args)

#define SCRIBE_LOG_INFO(format, args...)    \
    LOG(LOG_LEVEL_INFO, sharelib::Log4cppWrapper::GetLog(scribe_loggerName),  format, ##args)

#define SCRIBE_LOG_NOTICE(format, args...)    \
    LOG(LOG_LEVEL_NOTICE, sharelib::Log4cppWrapper::GetLog(scribe_loggerName), format, ##args)

#define SCRIBE_LOG_WARN(format, args...)     \
    LOG(LOG_LEVEL_WARN, sharelib::Log4cppWrapper::GetLog(scribe_loggerName), format, ##args)

#define SCRIBE_LOG_ERROR(format, args...)    \
    LOG(LOG_LEVEL_ERROR, sharelib::Log4cppWrapper::GetLog(scribe_loggerName), format, ##args)

/*
 *second use method
 */
#define SCRIBE_LOG_DEFINE()                                           \
    logger = sharelib::Log4cppWrapper::GetLog(scribe_loggerName);

#define SCRIBE_LOG_DECLEAR()                \
    static sharelib::ThreadSafeLog* logger;

#define SCRIBE_LOG_SETUP(classtype) \
    sharelib::ThreadSafeLog* classtype::logger;
#define SCRIBE_DEBUG(format, args...)    \
    LOG(LOG_LEVEL_DEBUG, logger, format, ##args)


#define SCRIBE_INFO(format, args...)    \
    LOG(LOG_LEVEL_INFO, logger,  format, ##args)

#define SCRIBE_NOTICE(format, args...)    \
    LOG(LOG_LEVEL_NOTICE, logger, format, ##args)

#define SCRIBE_WARN(format, args...)     \
    LOG(LOG_LEVEL_WARN, logger, format, ##args)


#define SCRIBE_ERROR(format, args...)    \
    LOG(LOG_LEVEL_ERROR, logger, format, ##args)

#endif
