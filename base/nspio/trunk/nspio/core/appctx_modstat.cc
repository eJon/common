// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/appctx_modstat.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include <iostream>
#include "log.h"
#include "ctx_global.h"
#include "appctx_modstat.h"

using namespace std;

NSPIO_DECLARATION_START

const char *app_stat_module_item[APP_MODULE_STATITEM_KEYRANGE] = {
    "",
    "RPOLLIN",
    "RPOLLOUT",
    "DPOLLIN",
    "DPOLLOUT",
    "RRCVPKG",
    "RSNDPKG",
    "DRCVPKG",
    "DSNDPKG",
    "POLLTIMEOUT"
};

__appctx_module_stat_trigger::__appctx_module_stat_trigger() :
    module_stat_trigger(APP_MODULE_STATITEM_KEYRANGE),
    _s_trigger_cnt(0), _m_trigger_cnt(0), _h_trigger_cnt(0), _d_trigger_cnt(0)
{

}


__appctx_module_stat_trigger::~__appctx_module_stat_trigger() {

}

int __appctx_module_stat_trigger::setup(const string &appid) {
    id = appid;
    return 0;
}

#define __check_key_range(__key)					\
    do {								\
	if (__key <= 0 || __key > APP_MODULE_STATITEM_KEYRANGE) {	\
	    NSPIOLOG_ERROR("module_stat invalid key %d", __key);	\
	    errno = EINVAL;						\
	    return -1;							\
	}								\
    } while (0)


#define __generic_app_module_stat_output()				\
    do {								\
	stringstream ss;						\
									\
	__check_key_range(key);						\
	ss << "appctx:" << id;						\
	ss << "[" << app_stat_module_item[key] << ":" << val << "]";	\
	NSPIOLOG_NOTICE("%s", ss.str().c_str());			\
    } while (0)

int __appctx_module_stat_trigger::trigger_s_threshold(module_stat *self,
						      int key, int64_t threshold, int64_t val) {
    __generic_app_module_stat_output();
    return 0;
}

int __appctx_module_stat_trigger::trigger_m_threshold(module_stat *self,
						      int key, int64_t threshold, int64_t val) {
    __generic_app_module_stat_output();
    return 0;
}


int __appctx_module_stat_trigger::trigger_h_threshold(module_stat *self,
						      int key, int64_t threshold, int64_t val) {
    __generic_app_module_stat_output();
    return 0;
}

int __appctx_module_stat_trigger::trigger_d_threshold(module_stat *self,
						      int key, int64_t threshold, int64_t val) {
    __generic_app_module_stat_output();
    return 0;
}


int init_appctx_module_stat_trigger(module_stat *astat, const string &tl) {
    return generic_init_module_stat_trigger(astat, APP_MODULE_STATITEM_KEYRANGE,
					    app_stat_module_item, tl);
}





}
