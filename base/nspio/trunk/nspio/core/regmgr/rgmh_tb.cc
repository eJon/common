// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/regmgr/rgmh_tb.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <map>
#include <iostream>
#include "log.h"
#include "os/memalloc.h"
#include "rgmh_tb.h"

using namespace std;

NSPIO_DECLARATION_START


rgmh_tb::rgmh_tb() {
    pmutex_init(&lock);
}
    

rgmh_tb::~rgmh_tb() {
    RgmHandler *rgmh = NULL;
    map<string, struct list_head *>::iterator it;
    struct list_head *head = NULL;
    struct list_link *pos = NULL, *next = NULL;

    pmutex_destroy(&lock);
    for (it = _head.begin(); it != _head.end(); ++it) {
	head = it->second;
	list_for_each_list_link_safe(pos, next, head) {
	    rgmh = list_rgmh(pos);
	    rgmh->detach_from_rgm_head();
	}
	mem_free(head, sizeof(*head));
    }
}

int rgmh_tb::insert(RgmHandler *rgmh) {
    string appname;
    struct list_head *head = NULL;
    map<string, struct list_head *>::iterator it;

    appname = rgmh->cappid();
    pmutex_lock(&lock);
    if ((it = _head.find(appname)) != _head.end()) {
	head = it->second;
    } else if ((head = (struct list_head *)mem_zalloc(sizeof(*head))) != NULL) {
	INIT_LIST_HEAD(head);
	_head.insert(make_pair(appname, head));
    }
    if (head) {
	_head_cnt[appname]++;
	rgmh->attach_to_rgm_head(head);
    }
    pmutex_unlock(&lock);
    return 0;
}

int rgmh_tb::remove(RgmHandler *rgmh) {
    int ret = 0;
    string appname;
    struct list_head *head = NULL;
    map<string, struct list_head *>::iterator it;

    appname = rgmh->cappid();
    pmutex_lock(&lock);
    ret = rgmh->detach_from_rgm_head();
    if ((it = _head.find(appname)) != _head.end()) {
	head = it->second;
	if (list_empty(head)) {
	    _head.erase(it);
	    mem_free(head, sizeof(*head));
	} else if (ret == 0)
	    _head_cnt[appname]--;
    }
    pmutex_unlock(&lock);
    return 0;
}


bool rgmh_tb::exist(const string &appname) {
    bool isexist = false;
    map<string, struct list_head *>::iterator it;

    pmutex_lock(&lock);
    if ((it = _head.find(appname)) != _head.end())
	isexist = true;
    pmutex_unlock(&lock);
    return isexist;
}

enum {
    WALK_STOP = 0x01,
};
    

int rgmh_tb::_walkone(const string &appname, rgm_walkfn walkfn, void *data) {
    int ret = 0;
    map<string, struct list_head *>::iterator it;
    struct list_head *head = NULL;
    struct list_link *pos = NULL, *next = NULL;

    if ((it = _head.find(appname)) == _head.end())
	return -1;
    head = it->second;
    list_for_each_list_link_safe(pos, next, head) {
	if ((ret = walkfn(list_rgmh(pos), data)) == WALK_STOP)
	    break;
    }
    return ret;
}
    
int rgmh_tb::walkone(const string &appname, rgm_walkfn walkfn, void *data) {
    int ret = 0;
    pmutex_lock(&lock);
    ret = _walkone(appname, walkfn, data);
    pmutex_unlock(&lock);
    return ret;
}


int rgmh_tb::walk(rgm_walkfn walkfn, void *data) {
    map<string, struct list_head *>::iterator it;

    pmutex_lock(&lock);
    for (it = _head.begin(); it != _head.end(); ++it) {
	if (_walkone(it->first, walkfn, data) == WALK_STOP)
	    break;
    }
    pmutex_unlock(&lock);
    return 0;
}


struct __rgmh_kscheck_data {
    struct spioreg *rgh;
    RgmHandler *rgmh;
};
    
static int __keepsession_check(RgmHandler *rgmh, void *data) {
    struct __rgmh_kscheck_data *env = (struct __rgmh_kscheck_data *)data;

    if (!env || !env->rgh)
	return WALK_STOP;
    if (rgmh->reg_keep_session(env->rgh) == true) {
	env->rgmh = rgmh;
	return WALK_STOP;
    }
    return 0;
}


struct __rgmh_pickmin_data {
    int receivers;
    int dispatchers;
    struct spioreg *rgh;
    RgmHandler *rgmh;
};

static int __pick_minbalance_rgmh(RgmHandler *rgmh, void *data) {
    struct rgmh_stats rgmh_info = {};
    struct spioreg *rgh = NULL;
    struct __rgmh_pickmin_data *env = (struct __rgmh_pickmin_data *)data;
    
    if (!env || !env->rgh)
	return WALK_STOP;
    rgmh->stats(&rgmh_info);
    rgh = env->rgh;
    if (IS_RECEIVER(rgh->rtype)) {
	if (env->receivers < 0 || env->receivers > RECEIVER_COUNTER(&rgmh_info)) {
	    env->rgmh = rgmh;
	    env->receivers = RECEIVER_COUNTER(&rgmh_info);
	}
    } else if (IS_DISPATCHER(rgh->rtype)) {
	if (env->dispatchers < 0 || env->dispatchers > DISPATCHER_COUNTER(&rgmh_info)) {
	    env->rgmh = rgmh;
	    env->dispatchers = DISPATCHER_COUNTER(&rgmh_info);
	}
    }
    return 0;
}

    
RgmHandler *rgmh_tb::balance_find(const struct spioreg *rgh) {
    string appname;
    struct __rgmh_kscheck_data ksc_env = {};
    struct __rgmh_pickmin_data pic_env = {};

    appname = rgh->appname;
    if (exist(appname) == false) {
	NSPIOLOG_ERROR("invalid register appname: %s", rgh->appname);
	return NULL;
    }
    ksc_env.rgh = (struct spioreg *)rgh;
    walkone(appname, __keepsession_check, &ksc_env);
    if (ksc_env.rgmh)
	return ksc_env.rgmh;

    pic_env.rgh = (struct spioreg *)rgh;
    pic_env.receivers = pic_env.dispatchers = -1;
    walkone(appname, __pick_minbalance_rgmh, &pic_env);
    if (pic_env.rgmh)
	return pic_env.rgmh;

    return NULL;
}

    

}
