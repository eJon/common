// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/role_modstat.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_ROLE_STAT_MODULE_
#define _H_ROLE_STAT_MODULE_

#include "module_stats.h"

NSPIO_DECLARATION_START


enum ROLE_MODULE_STATITEM {
    RRTT = 1,
    DRTT,
    POLLIN,
    POLLOUT,
    POLLERR,
    RECONNECT,
    RECV_BYTES,
    SEND_BYTES,
    RECV_PACKAGES,
    SEND_PACKAGES,
    RECV_ICMPS,
    SEND_ICMPS,
    RECV_ERRORS,
    SEND_ERRORS,
    CHECKSUM_ERRORS,
    ROLE_MODULE_STATITEM_KEYRANGE,
};


int init_role_module_stat_trigger(module_stat *astat, const string &tl);

class __role_module_stat_trigger : public module_stat_trigger {
public:
    __role_module_stat_trigger();
    ~__role_module_stat_trigger();

    int setup(const string &appid, const string &id, const string &ip);
    int trigger_s_threshold(module_stat *self, int key, int64_t threshold, int64_t val);
    int trigger_m_threshold(module_stat *self, int key, int64_t threshold, int64_t val);
    int trigger_h_threshold(module_stat *self, int key, int64_t threshold, int64_t val);
    int trigger_d_threshold(module_stat *self, int key, int64_t threshold, int64_t val);

 private:
    string _appid, _rid, _rip;
    int64_t _s_trigger_cnt;
    int64_t _m_trigger_cnt;
    int64_t _h_trigger_cnt;
    int64_t _d_trigger_cnt;
};



}
#endif  // _H_APPCTX_STAT_MODULE_
