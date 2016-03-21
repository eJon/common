// copyright:
//            (C) SINA Inc.
//
//           file: nspio/service/benchmark/benchmark_modstat.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_BENCHMARK_MODSTAT_H_
#define _H_BENCHMARK_MODSTAT_H_

#include "module_stats.h"

NSPIO_DECLARATION_START

enum {
    BC_RTT = 1,
    BC_RECVPKG,
    BC_SENDPKG,
    BC_DELIVER_ERROR,
    BC_CHECKSUM_ERROR,
    BC_RECONNECT,
    BENCHMARK_MODULE_STAT_KEYRANGE,
};


class benchmark_module_stat_trigger : public module_stat_trigger {
 public:
    benchmark_module_stat_trigger();
    
    int trigger_s_threshold(module_stat *self, int key, int64_t threshold, int64_t val);
    int trigger_m_threshold(module_stat *self, int key, int64_t threshold, int64_t val);
    int trigger_h_threshold(module_stat *self, int key, int64_t threshold, int64_t val);
    int trigger_d_threshold(module_stat *self, int key, int64_t threshold, int64_t val);
};







}
#endif   // _H_BENCHMARK_MODSTAT_H_
