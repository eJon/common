// copyright:
//            (C) SINA Inc.
//
//           file: nspio/service/benchmark/benchmark_modstat.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include <iostream>
#include "log.h"
#include "benchmark_modstat.h"

using namespace std;

NSPIO_DECLARATION_START

const char *benchmark_stat_module_item[BENCHMARK_MODULE_STAT_KEYRANGE] = {
    "",
    "BC_RTT",
    "BC_RECVPKG",
    "BC_SENDPKG",
    "BC_DELIVER_ERROR",
    "BC_CHECKSUM_ERROR",
    "BC_RECONNECT",
};


benchmark_module_stat_trigger::benchmark_module_stat_trigger() :
    module_stat_trigger(BENCHMARK_MODULE_STAT_KEYRANGE)
{

}


#define __check_key_range(__key)					\
    do {								\
	if (__key <= 0 || __key > BENCHMARK_MODULE_STAT_KEYRANGE) {	\
	    errno = EINVAL;						\
	    return -1;							\
	}								\
    } while (0)


#define __generic_benchmark_module_stat_output(t)			\
    do {								\
	stringstream ss;						\
									\
	__check_key_range(key);						\
	if (key == BC_RTT && self->getkey_##t(BC_RECVPKG) > 0)		\
	    val = val / self->getkey_##t(BC_RECVPKG);			\
	ss << " [" << benchmark_stat_module_item[key] << ":" << val << "]"; \
	fprintf(stdout, "%s\n", ss.str().c_str());			\
    } while (0)


int benchmark_module_stat_trigger::trigger_s_threshold(module_stat *self,
						    int key, int64_t threshold, int64_t val) {
    __generic_benchmark_module_stat_output(s);
    return 0;
}

int benchmark_module_stat_trigger::trigger_m_threshold(module_stat *self,
						    int key, int64_t threshold, int64_t val) {
    __generic_benchmark_module_stat_output(m);
    return 0;
}


int benchmark_module_stat_trigger::trigger_h_threshold(module_stat *self,
						    int key, int64_t threshold, int64_t val) {
    __generic_benchmark_module_stat_output(h);
    return 0;
}

int benchmark_module_stat_trigger::trigger_d_threshold(module_stat *self,
						    int key, int64_t threshold, int64_t val) {
    __generic_benchmark_module_stat_output(d);
    return 0;
}


}
