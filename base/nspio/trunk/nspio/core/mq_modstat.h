// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/mq_modstat.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_MQ_STAT_MODULE_
#define _H_MQ_STAT_MODULE_

#include "module_stats.h"

NSPIO_DECLARATION_START


enum MQ_MODULE_STATITEM {
    SIZE = 1,
    PASSED,
    TIMEOUTED,
    RESI_MSEC,
    MQ_MODULE_STATITEM_KEYRANGE,
};

int init_mq_module_stat_trigger(module_stat *astat, const string &tl);

class __mq_module_stat_trigger : public module_stat_trigger {
public:
    __mq_module_stat_trigger();
    ~__mq_module_stat_trigger();

    int setup(const string &appid, const string &id);
    int trigger_s_threshold(module_stat *self, int key, int64_t threshold, int64_t val);
    int trigger_m_threshold(module_stat *self, int key, int64_t threshold, int64_t val);
    int trigger_h_threshold(module_stat *self, int key, int64_t threshold, int64_t val);
    int trigger_d_threshold(module_stat *self, int key, int64_t threshold, int64_t val);

 private:
    string _appid, _rid;
    int64_t _s_trigger_cnt;
    int64_t _m_trigger_cnt;
    int64_t _h_trigger_cnt;
    int64_t _d_trigger_cnt;
};



}
#endif  // _H_APPCTX_STAT_MODULE_
