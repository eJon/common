// copyright:
//            (C) SINA Inc.
//
//           file: cpmweb/cpmweb_stats.cpp
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <iostream>
#include "log.h"
#include "ms_plugin_stats.h"

using namespace std;

stats_trigger::stats_trigger(const string &module, int _keyrange, const char **_keyitems) :
    module_stat_trigger(_keyrange), module_stat(_keyrange, this),
    id(module), keyrange(_keyrange), keyitems(_keyitems),
    _s_trigger_cnt(0), _m_trigger_cnt(0), _h_trigger_cnt(0), _d_trigger_cnt(0)
{
}

stats_trigger::~stats_trigger() {
}

void stats_trigger::emit() {
    update_timestamp(rt_mstime());
}
void stats_trigger::init(const string &trigger_level) {
    generic_init_module_stat_trigger(this, keyrange, keyitems, trigger_level);
}

void stats_trigger::init(const string &module, const string &trigger_level) {
    id = module;
    init(trigger_level);
}


#define __check_key_range(__key)					\
    do {								\
	if (__key <= 0 || __key > keyrange) {				\
	    ERROR(LOGROOT, "module_stat invalid key %d", __key);	\
	    errno = EINVAL;						\
	    return -1;							\
	}								\
    } while (0)


#define __generic_module_stat_output()					\
    do {								\
	stringstream ss;						\
	__check_key_range(key);						\
	ss << id;							\
	ss << "[" << keyitems[key] << ": now=" << now << " avg=" << avg << " min=" << min << " max=" << max << " ]"; \
	FATAL(LOGROOT, "%s", ss.str().c_str());				\
    } while (0)

int stats_trigger::s_warn(module_stat *self, int key, int64_t ts, int64_t now, int64_t avg, int64_t min, int64_t max) {
    __generic_module_stat_output();
    return 0;
}

int stats_trigger::m_warn(module_stat *self, int key, int64_t ts, int64_t now, int64_t avg, int64_t min, int64_t max) {
    __generic_module_stat_output();
    return 0;
}


int stats_trigger::h_warn(module_stat *self, int key, int64_t ts, int64_t now, int64_t avg, int64_t min, int64_t max) {
    __generic_module_stat_output();
    return 0;
}

int stats_trigger::d_warn(module_stat *self, int key, int64_t ts, int64_t now, int64_t avg, int64_t min, int64_t max) {
    __generic_module_stat_output();
    return 0;
}
