// copyright:
//            (C) SINA Inc.
//
//           file: nspio/service/nspiod/app_svr.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <map>
#include "log.h"
#include "os/memalloc.h"
#include "appctx.h"

using namespace std;
using namespace nspio;


typedef int (*am_walkfn) (AppCtx *app, void *data);

class app_maphead {
public:
    ~app_maphead();

    int insert(AppCtx *app);
    int remove(AppCtx *app);
    bool exist(const string &appname);
    int walk(am_walkfn walkfn, void *data);
    int walkone(const string &appname, am_walkfn walkfn, void *data);

private:
    map<string, struct list_head *> _head;
};


app_maphead::~app_maphead() {
    AppCtx *app = NULL;
    map<string, struct list_head *>::iterator it;
    struct list_head *head = NULL;
    struct list_link *pos = NULL, *next = NULL;
    
    for (it = _head.begin(); it != _head.end(); ++it) {
	head = it->second;
	list_for_each_list_link_safe(pos, next, head) {
	    app = list_app(pos);
	    app->detach_from_apps_head();
	}
	mem_free(head, sizeof(*head));
    }
}

int app_maphead::insert(AppCtx *app) {
    struct list_head *head = NULL;
    map<string, struct list_head *>::iterator it;

    if ((it = _head.find(app->Id())) != _head.end()) {
	head = it->second;
    } else if ((head = (struct list_head *)mem_zalloc(sizeof(*head))) != NULL) {
	INIT_LIST_HEAD(head);
	_head.insert(make_pair(app->Id(), head));
    }
    if (head)
	app->attach_to_apps_head(head);
    return 0;
}

int app_maphead::remove(AppCtx *app) {
    struct list_head *head = NULL;
    map<string, struct list_head *>::iterator it;

    app->detach_from_apps_head();
    if ((it = _head.find(app->Id())) != _head.end()) {
	head = it->second;
	if (list_empty(head)) {
	    _head.erase(it);
	    mem_free(head, sizeof(*head));
	}
    }
    return 0;
}


bool app_maphead::exist(const string &appname) {
    map<string, struct list_head *>::iterator it;

    if ((it = _head.find(appname)) != _head.end())
	return true;
    return false;
}


int app_maphead::walkone(const string &appname, am_walkfn walkfn, void *data) {
    map<string, struct list_head *>::iterator it;
    struct list_head *head = NULL;
    struct list_link *pos = NULL, *next = NULL;
    
    if ((it = _head.find(appname)) == _head.end())
	return -1;
    head = it->second;
    list_for_each_list_link_safe(pos, next, head) {
	walkfn(list_app(pos), data);
    }
    return 0;
}


int app_maphead::walk(am_walkfn walkfn, void *data) {
    map<string, struct list_head *>::iterator it;

    for (it = _head.begin(); it != _head.end(); ++it)
	walkone(it->first, walkfn, data);
    return 0;
}



static app_maphead maphead;
extern Rgm *rgm;
extern SpioConfig spio_conf;
extern TaskPool iothreadpool;

static int __startup_app(CtxConf *conf) {
    int ret = 0;
    AppCtx *app = NULL;
    TaskPool *tp = spio_conf.iothreadpool_workers > 0 ? &iothreadpool : NULL;

    if (!(app = new (std::nothrow) AppCtx())) {
	NSPIOLOG_ERROR("create appgroup failed of oom");
	return -1;
    }
    if ((ret = app->InitFromConf(conf)) < 0) {
	NSPIOLOG_ERROR("appid %s init failed", conf->appid.c_str());
	delete app;
	return -1;
    }
    maphead.insert(app);
    app->SetDebugMode(1);
    app->EnableRegistry(rgm);
    app->StartThread(tp);
    return 0;
}


static int __update_app(AppCtx *app, void *data) {
    CtxConf *conf = (CtxConf *)data;
    return app->Update(conf);
}

static int startone_app(const string &fconf) {
    int core = 0;
    CtxConf conf, clone_conf;

    if (conf.Init(fconf.c_str()) < 0) {
	NSPIOLOG_ERROR("init from conf %s", fconf.c_str());
	return -1;
    }
    if (maphead.exist(conf.appid)) {
	maphead.walkone(conf.appid, __update_app, &conf);
	return 0;
    }
    core = conf.virtual_node;
    while (core--) {
	conf.clone(&clone_conf);
	__startup_app(&clone_conf);
    }
    return 0;
}

int nspiod_start_apps(vector<string> &apps) {
    vector<string>::iterator it;

    for (it = apps.begin(); it != apps.end(); ++it)
	startone_app(*it);
    return 0;
}

static int __stop_app(AppCtx *app, void *data) {
    app->StopThread();
    app->detach_from_apps_head();
    delete app;
    return 0;
}


int nspiod_stop_apps() {
    maphead.walk(__stop_app, NULL);
    return 0;
}

