// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/log.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


/**
 * thread safe logger
 */
#ifndef _NSPIOLOG_h
#define _NSPIOLOG_h

#include <iostream>
#include <sys/syscall.h>
#include <sharelib/util/log4cpp_wrapper.h>
#include "os/time.h"
#include "decr.h"


NSPIO_DECLARATION_START

using namespace std;
using namespace sharelib;

static inline pid_t __tid()
{
    return syscall(SYS_gettid);
}

enum SHARELIB_LOG_LEVEL{
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_NOTICE,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
};

extern ThreadSafeLog *nspio_logger;

#define NSPIOLOG_CONFIG(filename) do {					\
	try {								\
	    sharelib::Log4cppWrapper::Init(filename);			\
	} catch(std::exception &e) {					\
	    std::cerr << "Error!!! Failed to configure logger"		\
		      << e.what() << std::endl;				\
	    exit(-1);							\
	}								\
	nspio_logger = sharelib::Log4cppWrapper::GetLog("nspio");	\
    }while(0)



#define NSPIOLOG_SHUTDOWN() sharelib::Log4cppWrapper::Shutdown()
#define NSPIOLOG_SHUTDOWN_BEFORE() sharelib::Log4cppWrapper::ShutdownBeforeLogr()


#define __LOG_DEBUG(logger, fmt, ...) do {			\
	(logger)->Debug("tid:%d %s:%d %s(), "#fmt, __tid(),	\
			basename(__FILE__),			\
			__LINE__, __func__, ##__VA_ARGS__);	\
    } while (0) 

#define __LOG_INFO(logger, fmt, ...) do {			\
	(logger)->Info("tid:%d %s:%d %s(), "#fmt, __tid(),	\
		       basename(__FILE__),			\
		       __LINE__, __func__, ##__VA_ARGS__);	\
    } while (0) 

#define __LOG_NOTICE(logger, fmt, ...) do {			\
	(logger)->Notice("tid:%d %s:%d %s(), "#fmt, __tid(),	\
			 basename(__FILE__),			\
			 __LINE__, __func__, ##__VA_ARGS__);	\
    } while (0) 


#define __LOG_WARN(logger, fmt, ...) do {			\
	(logger)->Warn("tid:%d %s:%d %s(), "#fmt, __tid(),	\
		       basename(__FILE__),			\
		       __LINE__, __func__, ##__VA_ARGS__);	\
    } while (0) 


#define __LOG_ERROR(logger, fmt, ...) do {			\
	(logger)->Error("tid:%d %s:%d %s(), "#fmt, __tid(),	\
			basename(__FILE__),			\
			__LINE__, __func__, ##__VA_ARGS__);	\
    } while (0) 


#define NSPIOLOG_DEBUG(format, args...) __LOG_DEBUG(nspio_logger, format, ##args)
#define NSPIOLOG_INFO(format, args...) __LOG_INFO(nspio_logger, format, ##args)
#define NSPIOLOG_NOTICE(format, args...) __LOG_NOTICE(nspio_logger, format, ##args)
#define NSPIOLOG_WARN(format, args...) __LOG_WARN(nspio_logger, format, ##args)
#define NSPIOLOG_ERROR(format, args...) __LOG_ERROR(nspio_logger, format, ##args)





}
#endif

