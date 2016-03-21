// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/test/rgm_mh_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <gtest/gtest.h>
#include <list>
#include "proto/route.h"
#include "regmgr/rgmh_tb.h"

using namespace std;
using namespace nspio;

class my_r {
public:
    my_r(int rtype) :
	_type(rtype)
    {
    }

    int Type() {
	return _type;
    }
private:
    int _type;
};

class my_rolemgr : public RgmHandler {
public:
    my_rolemgr(const string &app);
    ~my_rolemgr();

    inline const char *cappid() {
	return appid.c_str();
    }
    int Register(struct spioreg *header, Conn *conn);
    bool reg_keep_session(const struct spioreg *reghdr);
    int stats(struct rgmh_stats *stat);


private:
    string appid;
    struct rgmh_stats romstat;
    set<string> newroles, active_roles, timewait_roles;
};

my_rolemgr::my_rolemgr(const string &app) {
    appid = app;
    memset(&romstat, 0, sizeof(romstat));
}

my_rolemgr::~my_rolemgr() {
    newroles.clear();
    active_roles.clear();
    timewait_roles.clear();
}

int my_rolemgr::stats(struct rgmh_stats *info) {
    *info = romstat;
    return 0;
}

bool my_rolemgr::reg_keep_session(const struct spioreg *reghdr) {
    char uid[UUID_STRLEN] = {};
    set<string>::iterator it;

    uuid_unparse(reghdr->rid, uid);
    if ((it = timewait_roles.find(uid)) != timewait_roles.end())
	return true;
    return false;
}

int my_rolemgr::Register(struct spioreg *header, Conn *conn) {
    string roleid;
    set<string>::iterator it;
    my_r r(header->rtype);

    newroles.insert(header->appname);
    INCR_NR_COUNTER(&r, &romstat);
    switch (rand() % 3) {
    case 0:
	if ((it = active_roles.begin()) != active_roles.end()) {
	    roleid = *it;
	    active_roles.erase(it);
	    timewait_roles.insert(roleid);
	    DECR_AT_COUNTER(&r, &romstat);
	    INCR_TW_COUNTER(&r, &romstat);
	}
	break;
    case 1:
	if ((it = newroles.begin()) != newroles.end()) {
	    roleid = *it;
	    newroles.erase(it);
	    active_roles.insert(roleid);
	    DECR_NR_COUNTER(&r, &romstat);
	    INCR_AT_COUNTER(&r, &romstat);
	}
	break;
    case 2:
	if ((it = timewait_roles.begin()) != timewait_roles.end()) {
	    roleid = *it;
	    timewait_roles.erase(it);
	    newroles.insert(roleid);
	    DECR_TW_COUNTER(&r, &romstat);
	    INCR_NR_COUNTER(&r, &romstat);
	}
	break;
    }
    return 0;
}


static int receiver_cnt = 8;
static int dispatcher_cnt = 8;
static int max_cpu_core = 4;
static int cnt = receiver_cnt + dispatcher_cnt;
static string appname = "testapp";

static int __check(RgmHandler *rgmh, void *data) {
    int r_cnt = 0;
    struct rgmh_stats romstat = {};

    rgmh->stats(&romstat);
    r_cnt = DISPATCHER_COUNTER(&romstat);
    EXPECT_EQ(dispatcher_cnt/max_cpu_core, r_cnt);
    r_cnt = RECEIVER_COUNTER(&romstat);
    EXPECT_EQ(receiver_cnt/max_cpu_core, r_cnt);
    return 0;
}

static int __detach_and_free(RgmHandler *rgmh, void *data) {
    my_rolemgr *mgr = (my_rolemgr *)rgmh;
    mgr->detach_from_rgm_head();
    delete mgr;
    return 0;
}

static int rgmh_tb_test() {
    int i = 0;
    struct spioreg hdr = {};
    rgmh_tb maphead;
    RgmHandler *rgmh = NULL;
    
    for (i = 0; i < max_cpu_core; i++) {
	rgmh = new my_rolemgr(appname);
	maphead.insert(rgmh);
    }
    EXPECT_TRUE(maphead.exist(appname) == true);
    EXPECT_TRUE(maphead.exist("mockapp") == false);

    sprintf(hdr.appname, "%s", appname.c_str());
    for (i = 0; i < dispatcher_cnt; i++) {
	hdr.rtype = ROLE_DISPATCHER;
	uuid_generate(hdr.rid);
	if ((rgmh = maphead.balance_find(&hdr)) != NULL) 
	    rgmh->Register(&hdr, NULL);
    }
    for (i = 0; i < receiver_cnt; i++) {
	hdr.rtype = ROLE_RECEIVER;
	uuid_generate(hdr.rid);
	if ((rgmh = maphead.balance_find(&hdr)) != NULL)
	    rgmh->Register(&hdr, NULL);
    }
    maphead.walkone(appname, __check, NULL);
    maphead.walkone(appname, __detach_and_free, NULL);
    return 0;
}


TEST(regmgr, rgmh_tb) {
    rgmh_tb_test();
}
