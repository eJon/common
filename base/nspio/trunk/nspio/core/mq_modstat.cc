// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/mq_modstat.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include <iostream>
#include "log.h"
#include "mq_modstat.h"

using namespace std;

NSPIO_DECLARATION_START

const char *mq_stat_module_item[MQ_MODULE_STATITEM_KEYRANGE] = {
    "",
    "SIZE",
    "PASSED",
    "TIMEOUTED",
    "RESI_MSEC",
};

__mq_module_stat_trigger::__mq_module_stat_trigger() :
    module_stat_trigger(MQ_MODULE_STATITEM_KEYRANGE),
    _s_trigger_cnt(0), _m_trigger_cnt(0), _h_trigger_cnt(0), _d_trigger_cnt(0)
{

}


__mq_module_stat_trigger::~__mq_module_stat_trigger() {

}

int __mq_module_stat_trigger::setup(const string &appid, const string &id) {
    _appid = appid;
    _rid = id;
    return 0;
}


#define __check_key_range(__key)					\
    do {								\
	if (__key <= 0 || __key > MQ_MODULE_STATITEM_KEYRANGE) {	\
	    NSPIOLOG_ERROR("module_stat invalid key %d", __key);	\
	    errno = EINVAL;						\
	    return -1;							\
	}								\
    } while (0)


#define __generic_mq_module_stat_output(t)				\
    do {								\
	stringstream ss;						\
	int64_t n = 0;							\
									\
	__check_key_range(key);						\
	ss << "appctx:" << _appid << " mq:" << _rid;			\
	if (key == RESI_MSEC) {						\
	    if ((n = self->getkey_##t(PASSED) +				\
		 self->getkey_##t(TIMEOUTED)) > 0)			\
		val = val / n;						\
	}								\
	ss << " [" << mq_stat_module_item[key] << ":" << val << "]";	\
	NSPIOLOG_NOTICE("%s", ss.str().c_str());			\
    } while (0)


int __mq_module_stat_trigger::trigger_s_threshold(module_stat *self,
						    int key, int64_t threshold, int64_t val) {
    __generic_mq_module_stat_output(s);
    return 0;
}

int __mq_module_stat_trigger::trigger_m_threshold(module_stat *self,
						    int key, int64_t threshold, int64_t val) {
    __generic_mq_module_stat_output(m);
    return 0;
}


int __mq_module_stat_trigger::trigger_h_threshold(module_stat *self,
						    int key, int64_t threshold, int64_t val) {
    __generic_mq_module_stat_output(h);
    return 0;
}

int __mq_module_stat_trigger::trigger_d_threshold(module_stat *self,
						    int key, int64_t threshold, int64_t val) {
    __generic_mq_module_stat_output(d);
    return 0;
}



int init_mq_module_stat_trigger(module_stat *qstat, const string &tl) {
    return generic_init_module_stat_trigger(qstat, MQ_MODULE_STATITEM_KEYRANGE,
					    mq_stat_module_item, tl);
}






}
