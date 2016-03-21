// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/test/loadbalance_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <gtest/gtest.h>
#include "dispatcher.h"
#include "lb_random.h"
#include "lb_fair.h"
#include "lb_rrbin.h"
#include "lb_iphash.h"

using namespace nspio;

static void lb_iphash_test() {
    lb_iphash ipp;
    Role *r = NULL;
    string rid;
    struct spiort rt;
    module_stat *rstat = NULL;
    struct nspiomsg req = {};
    vector<Role *> all_roles;
    vector<Role *>::iterator it;
    int roles_num = 100, i, test_cnt = 1000, nothit = 0;

    req.route = &rt;
    req.hdr.ttl = 1;
    srand(time(NULL));
    for (i = 0; i < roles_num; i++) {
	route_genid(rid);
	r = new Dispatcher("mockapp", rid);
	ipp.add(r);
	SETROLE_W(r);
	all_roles.push_back(r);
    }
    for (it = all_roles.begin(); it != all_roles.end(); ++it) {
	r = *it;
	ipp.del(r);
    }
    for (it = all_roles.begin(); it != all_roles.end(); ++it) {
	r = *it;
	ipp.add(r);
    }
    for (i = 0; i < test_cnt; i++) {
	route_genid(rid);
	route_setid(&rt, rid);
	route_setip(&rt, "127.0.0.1");
	r = ipp.loadbalance_send(&req);
	rstat = r->Stat();
	rstat->incrkey(SEND_PACKAGES);
    }

    for (it = all_roles.begin(); it != all_roles.end(); ++it) {
	r = *it;
	rstat = r->Stat();
	if (rstat->getkey(SEND_PACKAGES) == 0)
	    nothit++;
    }
    for (it = all_roles.begin(); it != all_roles.end(); ++it)
	delete *it;
    EXPECT_TRUE(nothit == 0);
    return;
}


TEST(load_balance, ip_hash) {
    lb_iphash_test();
}


static void lb_rrbin_test() {
    lb_rrbin rrp;
    Role *r;
    string rid;
    vector<Role *> all_roles;
    vector<Role *>::iterator it;
    int roles_num = 50, i, test_cnt = 100000;

    srand(time(NULL));
    for (i = 0; i < roles_num; i++) {
	route_genid(rid);
	r = new Dispatcher("mockapp", rid);
	all_roles.push_back(r);
	SETROLE_W(r);
	rrp.add(r);
    }
    for (it = all_roles.begin(); it != all_roles.end(); ++it) {
	r = *it;
	rrp.del(r);
    }
    for (it = all_roles.begin(); it != all_roles.end(); ++it) {
	r = *it;
	rrp.add(r);
    }
    for (i = 0; i < test_cnt; i++) {
	r = rrp.loadbalance_send(NULL);
	EXPECT_TRUE(r == all_roles.at(i % roles_num));
    }
    for (it = all_roles.begin(); it != all_roles.end(); ++it)
	delete *it;
    return;
}



TEST(load_balance, round_robin) {
    lb_rrbin_test();
}



static void lb_random_test() {
    lb_random lbp;
    Role *r;
    string rid;
    vector<Role *> all_roles;
    vector<Role *>::iterator it;
    int roles_num = 50, i, test_cnt = 100000;

    srand(time(NULL));
    for (i = 0; i < roles_num; i++) {
	route_genid(rid);
	r = new Dispatcher("mockapp", rid);
	all_roles.push_back(r);
	SETROLE_W(r);
	lbp.add(r);
    }
    for (it = all_roles.begin(); it != all_roles.end(); ++it) {
	r = *it;
	lbp.del(r);
    }
    for (it = all_roles.begin(); it != all_roles.end(); ++it) {
	r = *it;
	lbp.add(r);
    }
    for (i = 0; i < test_cnt; i++)
	r = lbp.loadbalance_send(NULL);

    for (it = all_roles.begin(); it != all_roles.end(); ++it) {
	r = *it;
	delete r;
    }
    return;
}


TEST(load_balance, random) {
    lb_random_test();
}



static void lb_fair_test() {
    lb_fair lbp;
    Role *r;
    string rid;
    vector<Role *> all_roles;
    vector<Role *>::iterator it;
    int roles_num = 50, i, test_cnt = 100000;

    srand(time(NULL));
    for (i = 0; i < roles_num; i++) {
	route_genid(rid);
	r = new Dispatcher("mockapp", rid);
	all_roles.push_back(r);
	SETROLE_W(r);
	lbp.add(r);
    }
    for (it = all_roles.begin(); it != all_roles.end(); ++it) {
	r = *it;
	lbp.del(r);
    }
    for (it = all_roles.begin(); it != all_roles.end(); ++it) {
	r = *it;
	lbp.add(r);
    }
    for (i = 0; i < test_cnt; i++)
	r = lbp.loadbalance_send(NULL);
    for (it = all_roles.begin(); it != all_roles.end(); ++it) {
	r = *it;
	delete r;
    }
    return;
}


TEST(load_balance, lb_fair) {
    lb_fair_test();
}
