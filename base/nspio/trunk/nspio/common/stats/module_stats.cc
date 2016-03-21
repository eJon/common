// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/stats/module_stats.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include <stdio.h>
#include <string.h>
#include "os/time.h"
#include "os/memalloc.h"
#include "module_stats.h"



NSPIO_DECLARATION_START

static int chunk_num = 32;

module_stat::module_stat(int key_range, module_stat_trigger *handler) :
    range(key_range), _S(1000), _M(60000), _H(3600000), _D(86400000), h(handler)
{
    int64_t *__start = NULL;
    uint64_t chunk_size = range * sizeof(int64_t);
    uint64_t alloc_size = chunk_size * chunk_num;

    do {
	__start = (int64_t *)mem_zalloc(alloc_size);
    } while (!__start);

#define __init_module_stat(typ, group)				\
    {								\
	keys_##typ##_now = __start + (group * 4 + 0) * range;	\
	keys_##typ##_min = __start + (group * 4 + 1) * range;	\
	keys_##typ##_max = __start + (group * 4 + 2) * range;	\
	keys_##typ##_avg = __start + (group * 4 + 3) * range;	\
    }

    __init_module_stat(a, 0);
    __init_module_stat(s, 1);
    __init_module_stat(m, 2);
    __init_module_stat(h, 3);
    __init_module_stat(d, 4);

#undef __init_module_stat
    
    keys_s_threshold = __start + 20 * range;
    keys_m_threshold = __start + 21 * range;
    keys_h_threshold = __start + 22 * range;
    keys_d_threshold = __start + 23 * range;
    keys_s_timestamp = __start + 24 * range;
    keys_m_timestamp = __start + 25 * range;
    keys_h_timestamp = __start + 26 * range;
    keys_d_timestamp = __start + 27 * range;

    keys_s_trigger_cnt = __start + 28 * range;
    keys_m_trigger_cnt = __start + 29 * range;
    keys_h_trigger_cnt = __start + 30 * range;
    keys_d_trigger_cnt = __start + 31 * range;

    batch_set_threshold(1);
}

module_stat::~module_stat() {
    uint64_t alloc_size = range * sizeof(int64_t) * chunk_num;
    mem_free(keys_a_now, alloc_size);
}


int module_stat::reset() {
    uint64_t alloc_size = range * sizeof(int64_t) * chunk_num;
    memset(keys_a_now, 0, alloc_size);
    return 0;
}

int module_stat::set_timestamp(int64_t cur_mstime) {
    int idx = 0;

    for (idx = 0; idx < range - 1; idx++) {
	if (keys_s_timestamp[idx] == 0) {
	    keys_s_timestamp[idx] = keys_m_timestamp[idx] = keys_h_timestamp[idx] =
		keys_d_timestamp[idx] = cur_mstime;
	}
    }
    return 0;
}


int module_stat::batch_unset_threshold() {
    int key = 1;

    for (key = 1; key <= range; key++) {
	set_s_threshold(key, 0);
	set_m_threshold(key, 0);
	set_h_threshold(key, 0);
	set_d_threshold(key, 0);
    }
    return 0;
}

int module_stat::batch_set_threshold(int64_t threshold) {
    int key = 1;

    for (key = 1; key <= range; key++) {
	set_s_threshold(key, threshold);
	set_m_threshold(key, threshold);
	set_h_threshold(key, threshold);
	set_d_threshold(key, threshold);
    }
    return 0;
}



#define __set_module_threshold(t, __key, __threshold)	\
    do {						\
	if (__key <= 0 || __key > range) {		\
	    errno = EINVAL;				\
	    return -1;					\
	}						\
	keys_##t##_threshold[__key - 1] = __threshold;	\
    } while (0)
    

int module_stat::set_s_threshold(int key, int64_t threshold) {
    __set_module_threshold(s, key, threshold);
    return 0;
}

int module_stat::set_m_threshold(int key, int64_t threshold) {
    __set_module_threshold(m, key, threshold);
    return 0;
}

int module_stat::set_h_threshold(int key, int64_t threshold) {
    __set_module_threshold(h, key, threshold);
    return 0;
}

int module_stat::set_d_threshold(int key, int64_t threshold) {
    __set_module_threshold(d, key, threshold);
    return 0;
}

int module_stat::setkey(int key, int64_t val) {
    int idx = key - 1;

    if (key <= 0 || key > range) {
	errno = EINVAL;
	return -1;
    }
    if (keys_s_timestamp[idx] == 0) {
	keys_s_timestamp[idx] = keys_m_timestamp[idx] = keys_h_timestamp[idx] =
	    keys_d_timestamp[idx] = rt_mstime();
    }
    keys_a_now[idx] = val;
    keys_s_now[idx] = val;
    keys_m_now[idx] = val;
    keys_h_now[idx] = val;
    keys_d_now[idx] = val;
    return 0;
}



int module_stat::incrkey(int key, int64_t val) {
    int idx = key - 1;

    if (key <= 0 || key > range) {
	errno = EINVAL;
	return -1;
    }

    if (keys_s_timestamp[idx] == 0) {
	keys_s_timestamp[idx] = keys_m_timestamp[idx] = keys_h_timestamp[idx] =
	    keys_d_timestamp[idx] = rt_mstime();
    }

    keys_a_now[idx] += val;
    keys_s_now[idx] += val;
    keys_m_now[idx] += val;
    keys_h_now[idx] += val;
    keys_d_now[idx] += val;
    return 0;
}

int64_t module_stat::getkey(int key) {
    if (key <= 0 || key > range) {
	errno = EINVAL;
	return -1;
    }
    return keys_a_now[key - 1];
}

#define __get_module_key(t, __key, __op)				\
    do {								\
	int __idx = __key - 1;						\
	int __all = 0;							\
	if (__key <= 0 || __key > range) {				\
	    errno = EINVAL;						\
	    return -1;							\
	}								\
	switch (__op) {							\
	case NOW: return keys_##t##_now[__idx];				\
	case MIN: return keys_##t##_min[__idx];				\
	case MAX: return keys_##t##_max[__idx];				\
	case AVG:							\
	    if (keys_##t##_trigger_cnt[__idx] > 0) {			\
		__all = keys_a_now[__idx] - keys_##t##_now[__idx];	\
		return __all / keys_##t##_trigger_cnt[__idx];		\
	    }								\
	    return keys_a_now[__idx];					\
	}								\
    } while (0)

int64_t module_stat::getkey_s(int key, int op) {
    __get_module_key(s, key, op);
    return 0;
}

int64_t module_stat::getkey_m(int key, int op) {
    __get_module_key(m, key, op);
    return 0;
}

int64_t module_stat::getkey_h(int key, int op) {
    __get_module_key(h, key, op);
    return 0;
}

int64_t module_stat::getkey_d(int key, int op) {
    __get_module_key(d, key, op);
    return 0;
}



#define __trigger_one_key_stat(t, interval, __key, __now_time)	\
    do {								\
	int idx = __key - 1;						\
	int64_t __timestamp = keys_##t##_timestamp[idx];		\
	if (__now_time - __timestamp > interval) {			\
	    if (h && keys_##t##_threshold[idx] &&			\
		keys_##t##_now[idx] > keys_##t##_threshold[idx])	\
		h->trigger_##t##_threshold(this, __key,			\
					   keys_##t##_threshold[idx],	\
					   keys_##t##_now[idx]);	\
	    h->last_##t[idx] = keys_##t##_now[idx];			\
	    if (!keys_##t##_min[idx] ||					\
		keys_##t##_now[idx] < keys_##t##_min[idx])		\
		keys_##t##_min[idx] = keys_##t##_now[idx];		\
	    if (!keys_##t##_max[idx] ||					\
		keys_##t##_now[idx] > keys_##t##_max[idx])		\
		keys_##t##_max[idx] = keys_##t##_now[idx];		\
	}								\
    } while (0)

#define __update_one_key_stat(t, interval, __key, __now_time) do {	\
	int idx = __key - 1;						\
	int64_t __timestamp = keys_##t##_timestamp[idx];		\
	if (__now_time - __timestamp > interval) {			\
	    keys_##t##_now[idx] = 0;					\
	    keys_##t##_trigger_cnt[idx]++;				\
	    keys_##t##_timestamp[idx] = __now_time;			\
	}								\
    } while (0)

int module_stat::trigger_one_key_stat(int key, int64_t cur_ms_time) {

    __trigger_one_key_stat(s, _S, key, cur_ms_time);
    __trigger_one_key_stat(m, _M, key, cur_ms_time);
    __trigger_one_key_stat(h, _H, key, cur_ms_time);
    __trigger_one_key_stat(d, _D, key, cur_ms_time);

    return 0;
}

int module_stat::update_one_key_stat(int key, int64_t cur_ms_time) {
    __update_one_key_stat(s, _S, key, cur_ms_time);
    __update_one_key_stat(m, _M, key, cur_ms_time);
    __update_one_key_stat(h, _H, key, cur_ms_time);
    __update_one_key_stat(d, _D, key, cur_ms_time);

    return 0;
}


int module_stat::update_timestamp(int64_t cur_ms_time) {
    int key = 1;

    for (key = 1; key < range; key++)
	trigger_one_key_stat(key, cur_ms_time);
    for (key = 1; key < range; key++)
	update_one_key_stat(key, cur_ms_time);
    return 0;
}


int module_stat::change_threshold_interval(int64_t _s, int64_t _m, int64_t _h, int64_t _d) {
    _S = _s;
    _M = _m;
    _H = _h;
    _D = _d;
    return 0;
}


}
