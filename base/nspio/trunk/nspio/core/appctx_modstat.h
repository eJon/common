// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/appctx_modstat.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_APPCTX_STAT_MODULE_
#define _H_APPCTX_STAT_MODULE_

#include "module_stats.h"

NSPIO_DECLARATION_START


enum APP_MODULE_STATITEM {
    RPOLLIN = 1,
    RPOLLOUT,
    DPOLLIN,
    DPOLLOUT,
    RRCVPKG,
    RSNDPKG,
    DRCVPKG,
    DSNDPKG,
    POLLTIMEOUT,
    APP_MODULE_STATITEM_KEYRANGE,
};



int init_appctx_module_stat_trigger(module_stat *astat, const string &tl);

class __appctx_module_stat_trigger : public module_stat_trigger {
public:
    __appctx_module_stat_trigger();
    ~__appctx_module_stat_trigger();

    int setup(const string &appid);
    int trigger_s_threshold(module_stat *self, int key, int64_t threshold, int64_t val);
    int trigger_m_threshold(module_stat *self, int key, int64_t threshold, int64_t val);
    int trigger_h_threshold(module_stat *self, int key, int64_t threshold, int64_t val);
    int trigger_d_threshold(module_stat *self, int key, int64_t threshold, int64_t val);

 private:
    string id;
    int64_t _s_trigger_cnt;
    int64_t _m_trigger_cnt;
    int64_t _h_trigger_cnt;
    int64_t _d_trigger_cnt;
};



}
#endif  // _H_APPCTX_STAT_MODULE_
