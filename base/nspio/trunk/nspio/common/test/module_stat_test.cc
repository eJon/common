// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/test/module_stat_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <gtest/gtest.h>
#include <nspio/errno.h>
#include "log.h"
#include "module_stats.h"

using namespace nspio;

static int64_t _s(5);
static int64_t _m(20);
static int64_t _h(50);
static int64_t _d(100);

enum MODULE_STAT_ITEM {
    KEY1 = 1,
    KEY2,
    KEY3,
    KEY4,
    KEYRANGE,
};

class my_module_trigger : public module_stat_trigger {
public:
    my_module_trigger() :
	module_stat_trigger(KEYRANGE),
	_s_trigger_cnt(0), _m_trigger_cnt(0), _h_trigger_cnt(0), _d_trigger_cnt(0)
    {
    }
    ~my_module_trigger() {

    }
    int trigger_s_threshold(module_stat *self, int key, int64_t threshold, int64_t val) {
	EXPECT_GT(val, threshold);
	_s_trigger_cnt++;
	return 0;
    }
    int trigger_m_threshold(module_stat *self, int key, int64_t threshold, int64_t val) {
	EXPECT_GT(val, threshold);
	_m_trigger_cnt++;
	return 0;
    }
    int trigger_h_threshold(module_stat *self, int key, int64_t threshold, int64_t val) {
	EXPECT_GT(val, threshold);
	_h_trigger_cnt++;
	return 0;
    }
    int trigger_d_threshold(module_stat *self, int key, int64_t threshold, int64_t val) {
	EXPECT_GT(val, threshold);
	_d_trigger_cnt++;
	return 0;
    }

    int _s_trigger_cnt;
    int _m_trigger_cnt;
    int _h_trigger_cnt;
    int _d_trigger_cnt;
};


static int test_module_stat_non_trigger() {
    my_module_trigger monitor;
    module_stat tstat(KEYRANGE, &monitor);
    int64_t start_tt = rt_mstime(), cur_tt = 0;
    int i = 0, cnt = 1000;

    tstat.change_threshold_interval(_s, _m, _h, _d);
    tstat.batch_unset_threshold();
    while (({cur_tt = rt_mstime(); cur_tt;}) - start_tt < _d * 2) {
	for (i = 0; i < cnt; i++) {
	    tstat.incrkey(KEY1, 1);
	    tstat.incrkey(KEY2, 1);
	    tstat.incrkey(KEY3, 1);
	    tstat.incrkey(KEY4, 1);
	}
	tstat.update_timestamp(cur_tt);
    }
    EXPECT_TRUE(KEYRANGE == 5);
    EXPECT_EQ(monitor._s_trigger_cnt, 0);
    EXPECT_EQ(monitor._m_trigger_cnt, 0);
    EXPECT_EQ(monitor._h_trigger_cnt, 0);
    EXPECT_EQ(monitor._d_trigger_cnt, 0);
    return 0;
}


static int test_module_stat_trigger() {
    my_module_trigger monitor;
    module_stat tstat(KEYRANGE, &monitor);
    int64_t start_tt = rt_mstime(), cur_tt = 0;
    int key = 0, i = 0, cnt = 1000;

    tstat.change_threshold_interval(_s, _m, _h, _d);
    for (key = 1; key <= KEYRANGE; key++) {
	tstat.set_s_threshold(key, 1);
	tstat.set_m_threshold(key, 1);
	tstat.set_h_threshold(key, 1);
	tstat.set_d_threshold(key, 1);
    }
    
    while (({cur_tt = rt_mstime(); cur_tt;}) - start_tt < _d * 2) {
	for (i = 0; i < cnt; i++) {
	    tstat.incrkey(KEY1, 1);
	    tstat.incrkey(KEY2, 1);
	    tstat.incrkey(KEY3, 1);
	    tstat.incrkey(KEY4, 1);
	}
	tstat.update_timestamp(cur_tt);
    }
    EXPECT_TRUE(KEYRANGE == 5);
    EXPECT_GT(monitor._s_trigger_cnt, monitor._m_trigger_cnt);
    EXPECT_GT(monitor._m_trigger_cnt, monitor._h_trigger_cnt);
    EXPECT_GT(monitor._h_trigger_cnt, monitor._d_trigger_cnt);
    NSPIOLOG_INFO("module stat _s trigger %d", monitor._s_trigger_cnt);
    NSPIOLOG_INFO("module stat _m trigger %d", monitor._m_trigger_cnt);
    NSPIOLOG_INFO("module stat _h trigger %d", monitor._h_trigger_cnt);
    NSPIOLOG_INFO("module stat _d trigger %d", monitor._d_trigger_cnt);

    EXPECT_LE(tstat.getkey_s(KEY1, MIN), tstat.getkey_s(KEY1, AVG));
    EXPECT_GE(tstat.getkey_s(KEY1, MAX), tstat.getkey_s(KEY1, AVG));
    EXPECT_LE(tstat.getkey_s(KEY2, MIN), tstat.getkey_s(KEY2, AVG));
    EXPECT_GE(tstat.getkey_s(KEY2, MAX), tstat.getkey_s(KEY2, AVG));
    EXPECT_LE(tstat.getkey_s(KEY3, MIN), tstat.getkey_s(KEY3, AVG));
    EXPECT_GE(tstat.getkey_s(KEY3, MAX), tstat.getkey_s(KEY3, AVG));
    EXPECT_LE(tstat.getkey_s(KEY4, MIN), tstat.getkey_s(KEY4, AVG));
    EXPECT_GE(tstat.getkey_s(KEY4, MAX), tstat.getkey_s(KEY4, AVG));

    EXPECT_LE(tstat.getkey_m(KEY1, MIN), tstat.getkey_m(KEY1, AVG));
    EXPECT_GE(tstat.getkey_m(KEY1, MAX), tstat.getkey_m(KEY1, AVG));
    EXPECT_LE(tstat.getkey_m(KEY2, MIN), tstat.getkey_m(KEY2, AVG));
    EXPECT_GE(tstat.getkey_m(KEY2, MAX), tstat.getkey_m(KEY2, AVG));
    EXPECT_LE(tstat.getkey_m(KEY3, MIN), tstat.getkey_m(KEY3, AVG));
    EXPECT_GE(tstat.getkey_m(KEY3, MAX), tstat.getkey_m(KEY3, AVG));
    EXPECT_LE(tstat.getkey_m(KEY4, MIN), tstat.getkey_m(KEY4, AVG));
    EXPECT_GE(tstat.getkey_m(KEY4, MAX), tstat.getkey_m(KEY4, AVG));

    EXPECT_LE(tstat.getkey_h(KEY1, MIN), tstat.getkey_h(KEY1, AVG));
    EXPECT_GE(tstat.getkey_h(KEY1, MAX), tstat.getkey_h(KEY1, AVG));
    EXPECT_LE(tstat.getkey_h(KEY2, MIN), tstat.getkey_h(KEY2, AVG));
    EXPECT_GE(tstat.getkey_h(KEY2, MAX), tstat.getkey_h(KEY2, AVG));
    EXPECT_LE(tstat.getkey_h(KEY3, MIN), tstat.getkey_h(KEY3, AVG));
    EXPECT_GE(tstat.getkey_h(KEY3, MAX), tstat.getkey_h(KEY3, AVG));
    EXPECT_LE(tstat.getkey_h(KEY4, MIN), tstat.getkey_h(KEY4, AVG));
    EXPECT_GE(tstat.getkey_h(KEY4, MAX), tstat.getkey_h(KEY4, AVG));

    EXPECT_LE(tstat.getkey_d(KEY1, MIN), tstat.getkey_d(KEY1, AVG));
    EXPECT_GE(tstat.getkey_d(KEY1, MAX), tstat.getkey_d(KEY1, AVG));
    EXPECT_LE(tstat.getkey_d(KEY2, MIN), tstat.getkey_d(KEY2, AVG));
    EXPECT_GE(tstat.getkey_d(KEY2, MAX), tstat.getkey_d(KEY2, AVG));
    EXPECT_LE(tstat.getkey_d(KEY3, MIN), tstat.getkey_d(KEY3, AVG));
    EXPECT_GE(tstat.getkey_d(KEY3, MAX), tstat.getkey_d(KEY3, AVG));
    EXPECT_LE(tstat.getkey_d(KEY4, MIN), tstat.getkey_d(KEY4, AVG));
    EXPECT_GE(tstat.getkey_d(KEY4, MAX), tstat.getkey_d(KEY4, AVG));

    
    return 0;
}



static int gtip(const string &str, const string &item, int &tr, int &v) {
    return generic_trigger_item_parse(str, item, tr, v);
}


static int test_trigger_item_parse() {
    int tr = 0;
    int v = 0;
    EXPECT_TRUE(gtip(";;RECONNECT:m:1;SEND_BYTES:m:1;", "RECONNECT", tr, v) == 0);
    EXPECT_EQ(tr, SL_M);
    EXPECT_EQ(v, 1);

    EXPECT_TRUE(gtip(";;RECONNECT:m:1000000000000;SEND_BYTES:m:1;", "RECONNECT", tr, v) == 0);
    EXPECT_EQ(tr, SL_M);
    EXPECT_EQ(v, 1);

    EXPECT_TRUE(gtip("RECONNECT:d:120;SEND_BYTES:m:1;", "RECONNECT", tr, v) == 0);
    EXPECT_EQ(tr, SL_D);
    EXPECT_EQ(v, 120);

    EXPECT_TRUE(gtip("RECONNECT:s:120", "RECONNECT", tr, v) == 0);
    EXPECT_EQ(tr, SL_S);
    EXPECT_EQ(v, 120);

    EXPECT_TRUE(gtip("RECONNECT:h:", "RECONNECT", tr, v) == -1);
    EXPECT_TRUE(gtip("RECONNECT:a:9", "RECONNECT", tr, v) == -1);
    EXPECT_TRUE(gtip("ECONNECT:m:1", "RECONNECT", tr, v) == -1);
    EXPECT_TRUE(gtip("RECONNECT:h", "RECONNECT", tr, v) == -1);
    EXPECT_TRUE(gtip("RECONNECT:12", "RECONNECT", tr, v) == -1);
    EXPECT_TRUE(gtip("RECONNECTh", "RECONNECT", tr, v) == -1);
    EXPECT_TRUE(gtip("RECONNECTh:m:12", "RECONNECT", tr, v) == -1);
    return 0;
}


TEST(module_stat, trigger) {
    test_module_stat_trigger();
}

TEST(module_stat, non_trigger) {
    test_module_stat_non_trigger();
}


TEST(module_stat, trigger_parse) {
    test_trigger_item_parse();
}
