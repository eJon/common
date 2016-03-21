// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/role_modstat.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include <iostream>
#include "log.h"
#include "role_modstat.h"

using namespace std;

NSPIO_DECLARATION_START

const char *role_stat_module_item[ROLE_MODULE_STATITEM_KEYRANGE] = {
    "",
    "RRTT",
    "DRTT",
    "POLLIN",
    "POLLOUT",
    "POLLERR",
    "RECONNECT",
    "RECV_BYTES",
    "SEND_BYTES",
    "RECV_PACKAGES",
    "SEND_PACKAGES",
    "RECV_ICMPS",
    "SEND_ICMPS",
    "RECV_ERRORS",
    "SEND_ERRORS",
    "CHECKSUM_ERRORS",
};

__role_module_stat_trigger::__role_module_stat_trigger() :
    module_stat_trigger(ROLE_MODULE_STATITEM_KEYRANGE),
    _s_trigger_cnt(0), _m_trigger_cnt(0), _h_trigger_cnt(0), _d_trigger_cnt(0)
{

}


__role_module_stat_trigger::~__role_module_stat_trigger() {

}

int __role_module_stat_trigger::setup(const string &appid, const string &id, const string &ip) {
    _appid = appid;
    _rid = id;
    _rip = ip;
    return 0;
}

#define __check_key_range(__key)					\
    do {								\
	if (__key <= 0 || __key > ROLE_MODULE_STATITEM_KEYRANGE) {	\
	    NSPIOLOG_ERROR("module_stat invalid key %d", __key);	\
	    errno = EINVAL;						\
	    return -1;							\
	}								\
    } while (0)


#define __generic_role_module_stat_output(t)				\
    do {								\
	stringstream ss;						\
	string sval;							\
									\
	__check_key_range(key);						\
	ss << "appctx:";						\
	ss << _appid << " role:" << _rid << " ip:" << _rip;		\
	if (key == RECV_BYTES || key == SEND_BYTES) {			\
	    sval = bytes_to_string(val);				\
	    ss << " [" << role_stat_module_item[key] << ":" << sval << "]"; \
	} else if ((key == RRTT || key == DRTT)				\
		   && self->getkey_##t(RECV_PACKAGES) > 0) {		\
	    val = val / self->getkey_##t(RECV_PACKAGES);		\
	    ss << " [" << role_stat_module_item[key] << ":" << val << "ms]"; \
	} else								\
	    ss << " [" << role_stat_module_item[key] << ":" << val << "]"; \
	NSPIOLOG_NOTICE("%s", ss.str().c_str());			\
	return 0;							\
    } while (0)


int __role_module_stat_trigger::trigger_s_threshold(module_stat *self,
						    int key, int64_t threshold, int64_t val) {
    __generic_role_module_stat_output(s);
    return 0;
}

int __role_module_stat_trigger::trigger_m_threshold(module_stat *self,
						    int key, int64_t threshold, int64_t val) {
    __generic_role_module_stat_output(m);
    return 0;
}


int __role_module_stat_trigger::trigger_h_threshold(module_stat *self,
						    int key, int64_t threshold, int64_t val) {
    __generic_role_module_stat_output(h);
    return 0;
}

int __role_module_stat_trigger::trigger_d_threshold(module_stat *self,
						    int key, int64_t threshold, int64_t val) {
    __generic_role_module_stat_output(d);
    return 0;
}


int init_role_module_stat_trigger(module_stat *rstat, const string &tl) {
    return generic_init_module_stat_trigger(rstat, ROLE_MODULE_STATITEM_KEYRANGE,
					    role_stat_module_item, tl);
}






}
