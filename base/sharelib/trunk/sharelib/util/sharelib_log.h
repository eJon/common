#ifndef SHARELIB_SHARELIB_LOG_H
#define SHARELIB_SHARELIB_LOG_H
#include <sharelib/util/log.h>


static const std::string sharelib_loggerName = "sharelib";


#define SHARELIB_LOG_DEBUG(format, args...)    \
    LOG(LOG_LEVEL_DEBUG, sharelib::Log4cppWrapper::GetLog(sharelib_loggerName),format, ##args)

#define SHARELIB_LOG_INFO(format, args...)    \
    LOG(LOG_LEVEL_INFO, sharelib::Log4cppWrapper::GetLog(sharelib_loggerName),  format, ##args)

#define SHARELIB_LOG_NOTICE(format, args...)    \
    LOG(LOG_LEVEL_NOTICE, sharelib::Log4cppWrapper::GetLog(sharelib_loggerName), format, ##args)

#define SHARELIB_LOG_WARN(format, args...)     \
    LOG(LOG_LEVEL_WARN, sharelib::Log4cppWrapper::GetLog(sharelib_loggerName), format, ##args)

#define SHARELIB_LOG_ERROR(format, args...)    \
    LOG(LOG_LEVEL_ERROR, sharelib::Log4cppWrapper::GetLog(sharelib_loggerName), format, ##args)

/*
 *second use method
 */
#define SHARELIB_LOG_DEFINE()                                           \
    logger = sharelib::Log4cppWrapper::GetLog(sharelib_loggerName);

#define SHARELIB_LOG_DECLEAR()                \
    static sharelib::ThreadSafeLog* logger;

#define SHARELIB_LOG_SETUP(classtype) \
    sharelib::ThreadSafeLog* classtype::logger;
#define SHARELIB_DEBUG(format, args...)    \
    LOG(LOG_LEVEL_DEBUG, logger, format, ##args)


#define SHARELIB_INFO(format, args...)    \
    LOG(LOG_LEVEL_INFO, logger,  format, ##args)

#define SHARELIB_NOTICE(format, args...)    \
    LOG(LOG_LEVEL_NOTICE, logger, format, ##args)

#define SHARELIB_WARN(format, args...)     \
    LOG(LOG_LEVEL_WARN, logger, format, ##args)


#define SHARELIB_ERROR(format, args...)    \
    LOG(LOG_LEVEL_ERROR, logger, format, ##args)

#endif
