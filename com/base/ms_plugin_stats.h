// copyright:
//            (C) SINA Inc.
//
//           file: cpmweb/cpmweb_stats.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_MS_STAT_MODULE_
#define _H_MS_STAT_MODULE_

#include <inttypes.h>
#include <mscom/base/module_stats.h>

class stats_trigger : public module_stat_trigger, public module_stat {
 public:
    stats_trigger(const string &module, int keyrange, const char **keyitems);
    ~stats_trigger();

    void init(const string &trigger_level);
    void init(const string &module, const string &trigger_level);
    void emit();
    int s_warn(module_stat *self, int key, int64_t ts, int64_t now, int64_t avg, int64_t min, int64_t max);
    int m_warn(module_stat *self, int key, int64_t ts, int64_t now, int64_t avg, int64_t min, int64_t max);
    int h_warn(module_stat *self, int key, int64_t ts, int64_t now, int64_t avg, int64_t min, int64_t max);
    int d_warn(module_stat *self, int key, int64_t ts, int64_t now, int64_t avg, int64_t min, int64_t max);

 private:
    string id;
    int keyrange;
    const char **keyitems;
    int64_t _s_trigger_cnt;
    int64_t _m_trigger_cnt;
    int64_t _h_trigger_cnt;
    int64_t _d_trigger_cnt;
};

#endif
