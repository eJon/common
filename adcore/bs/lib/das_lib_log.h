#ifndef DAS_LIB_DAS_LIB_LOG_H
#define	DAS_LIB_DAS_LIB_LOG_H

#include <com_log.h>

#define DL_LOG(_level_, _fmt_, args...)                                 \
    do {                                                                \
        com_writelog(                                                   \
            _level_, "[%s:%d|%s] " _fmt_,                               \
            __FILE__, __LINE__, __FUNCTION__, ##args);                  \
    } while (0);

#define DL_LOG_FATAL(_fmt_, args...) DL_LOG(COMLOG_FATAL, _fmt_, ##args)
#define DL_LOG_WARNING(_fmt_, args...) DL_LOG(COMLOG_WARNING, _fmt_, ##args)
#define DL_LOG_MON(_fmt_, args...) DL_LOG("DAS_MON", _fmt_, ##args)

#define DL_LOG_TRACE(_fmt_, args...)                \
    do {                                            \
        if (com_log_enabled(COMLOG_TRACE) == 1) {   \
            DL_LOG(COMLOG_TRACE, _fmt_, ##args)     \
        }                                           \
    } while(0)
    
#define DL_LOG_DEBUG(_fmt_, args...)                \
    do {                                            \
        if (com_log_enabled(COMLOG_DEBUG) == 1) {   \
            DL_LOG(COMLOG_DEBUG, _fmt_, ##args)     \
        }                                           \
    } while(0)
#endif

