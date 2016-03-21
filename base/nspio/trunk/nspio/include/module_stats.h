// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/module_stats.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_MODULESTAT_
#define _H_MODULESTAT_

#include <iostream>
#include <sstream>
#include "decr.h"

using namespace std;

NSPIO_DECLARATION_START


class module_stat;
class module_stat_trigger {
 public:
    module_stat_trigger(int range);
    virtual ~module_stat_trigger();
    friend class module_stat;

    virtual int trigger_s_threshold(module_stat *self, int key, int64_t threshold, int64_t val) = 0;
    virtual int trigger_m_threshold(module_stat *self, int key, int64_t threshold, int64_t val) = 0;
    virtual int trigger_h_threshold(module_stat *self, int key, int64_t threshold, int64_t val) = 0;
    virtual int trigger_d_threshold(module_stat *self, int key, int64_t threshold, int64_t val) = 0;
    int64_t getlast_s(int key);
    int64_t getlast_m(int key);
    int64_t getlast_h(int key);
    int64_t getlast_d(int key);
    
 private:
    int key_range;
    int64_t *last_s, *last_m, *last_h, *last_d;
};

enum STAT_TYPE {
    NOW = 0,
    MIN = 1,
    MAX = 2,
    AVG = 3,
};

enum STAT_LEVEL {
    SL_S = 0,
    SL_M,
    SL_H,
    SL_D,
};


class module_stat {
 public:
    module_stat(int key_range, module_stat_trigger *handler);
    ~module_stat();
    int reset();
    int set_timestamp(int64_t cur_mstime);
    int batch_set_threshold(int64_t threshold);
    int batch_unset_threshold();
    int set_s_threshold(int key, int64_t threshold);
    int set_m_threshold(int key, int64_t threshold);
    int set_h_threshold(int key, int64_t threshold);
    int set_d_threshold(int key, int64_t threshold);
    int setkey(int key, int64_t val);
    int incrkey(int key, int64_t val = 1);
    int64_t getkey(int key);
    int64_t getkey_s(int key, int op = NOW);
    int64_t getkey_m(int key, int op = NOW);
    int64_t getkey_h(int key, int op = NOW);
    int64_t getkey_d(int key, int op = NOW);
    int update_timestamp(int64_t cur_ms_time);
    int change_threshold_interval(int64_t _s, int64_t _m, int64_t _h, int64_t _d);
    
 private:
    int range;
    int64_t _S, _M, _H, _D;
    module_stat_trigger *h;

    int64_t *keys_a_now, *keys_a_min, *keys_a_max, *keys_a_avg;
    int64_t *keys_s_now, *keys_s_min, *keys_s_max, *keys_s_avg;
    int64_t *keys_m_now, *keys_m_min, *keys_m_max, *keys_m_avg;
    int64_t *keys_h_now, *keys_h_min, *keys_h_max, *keys_h_avg;
    int64_t *keys_d_now, *keys_d_min, *keys_d_max, *keys_d_avg;
    int64_t *keys_s_threshold;
    int64_t *keys_m_threshold;
    int64_t *keys_h_threshold;
    int64_t *keys_d_threshold;
    int64_t *keys_s_timestamp;
    int64_t *keys_m_timestamp;
    int64_t *keys_h_timestamp;
    int64_t *keys_d_timestamp;
    int64_t *keys_s_trigger_cnt;
    int64_t *keys_m_trigger_cnt;
    int64_t *keys_h_trigger_cnt;
    int64_t *keys_d_trigger_cnt;
    
    int trigger_one_key_stat(int key, int64_t cur_ms_time);
    int update_one_key_stat(int key, int64_t cur_ms_time);
};

int generic_trigger_item_parse(const string &str, const string &item, int &tr, int &v);
int generic_set_trigger_threshold(module_stat *stat, int key, int tr, int val);
int generic_init_module_stat_trigger(module_stat *self, int range,
				     const char **items, const string &tl);


static inline string bytes_to_bytes(int64_t bytes) {
    stringstream ss;
    ss << bytes << "b";
    return ss.str();
}

static inline string bytes_to_kbstring(int64_t bytes) {
    stringstream ss;
    ss << (bytes + 1024 - 1) / 1024 << "kb";
    return ss.str();
}

static inline string bytes_to_mbstring(int64_t bytes) {
    stringstream ss;
    ss << (bytes + 1048576 - 1) / 1048576 << "mb";
    return ss.str();
}

static inline string bytes_to_gbstring(int64_t bytes) {
    stringstream ss;
    ss << (bytes + 1073741824 - 1) / 1073741824 << "gb";
    return ss.str();
}

static inline string bytes_to_string(int64_t bytes) {
    if (bytes < 1024)
	return bytes_to_bytes(bytes);
    else if (bytes < 1048576)
	return bytes_to_kbstring(bytes);
    else if (bytes < 1073741824)
	return bytes_to_mbstring(bytes);
    return bytes_to_gbstring(bytes);
}



}
#endif  // _H_ROLESTAT_
