//by Andrew(pengdong@staff.sina.com.cn)
//first at 2013-12-24
//used for all ms and plugins

#ifndef __MS_LOG_H__
#define __MS_LOG_H__

#include <string>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <sharelib/util/log.h>


using namespace std;
using namespace sharelib;
namespace ms {
    namespace log {

	static const int TRACE_FLAG_OFF = 0;
	static int LOG_INIT(string log_cfg_file) {
	    static int global_inited = 0;

	    if(!global_inited) {
		LOG_CONFIG(log_cfg_file);
		global_inited++;
	    }

	    return 0;
	}

#define LOGROOT "ms"

#define MSNEW_FORMAT(module, format)			\
	string fmt(format), md(module);			\
	fmt = "[" + md + "]" + " %s:%d %s() " + fmt;	\

#define DEBUG(module,format,...) do {					\
	    MSNEW_FORMAT(module, format)				\
		Log4cppWrapper::GetLog(module)->Debug(fmt.c_str(), basename(__FILE__), __LINE__, __func__, ##__VA_ARGS__); \
	}while(0)

#define WARN(module,format,...) do {					\
	    MSNEW_FORMAT(module, format)				\
		Log4cppWrapper::GetLog(module)->Warn(fmt.c_str(), basename(__FILE__), __LINE__, __func__, ##__VA_ARGS__); \
	}while(0)

#define INFO(module,format,...) do {					\
	    MSNEW_FORMAT(module, format)				\
		Log4cppWrapper::GetLog(module)->Info(fmt.c_str(), basename(__FILE__), __LINE__, __func__, ##__VA_ARGS__); \
	}while(0)

#define NOTICE(module,format,...) do {					\
	    MSNEW_FORMAT(module, format)				\
		Log4cppWrapper::GetLog(module)->Notice(fmt.c_str(), basename(__FILE__), __LINE__, __func__, ##__VA_ARGS__); \
	}while(0)

#define ERROR(module,format,...) do {					\
	    MSNEW_FORMAT(module, format)				\
		Log4cppWrapper::GetLog(module)->Error(fmt.c_str(), basename(__FILE__), __LINE__, __func__, ##__VA_ARGS__); \
	}while(0)

#define FATAL(module,format,...) do {                                   \
	MSNEW_FORMAT(module, format)                                \
	Log4cppWrapper::GetLog(module)->Fatal(fmt.c_str(), basename(__FILE__), __LINE__, __func__, ##__VA_ARGS__); \
}while(0)



    }//namespace log
}//namespace ms

using namespace ms::log;
#endif

