// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/rolemgr.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_ROLEMANAGER_H
#define _H_ROLEMANAGER_H


#include "sync/pmutex.h"
#include "role.h"
#include "regmgr/regmgr.h"

NSPIO_DECLARATION_START

typedef int (*rwalkfn) (Role *r, void *data);

class RoleManager : public RgmHandler {
 public:
    RoleManager();
    ~RoleManager();

    inline const char *cappid() {
	return appid.c_str();
    }
    void Init(const string &_appid, CtxConf *conf) {
	appid = _appid;
	cfg = conf;
    }
    int stats(struct rgmh_stats *info);
    Role *PopNewer();
    Role *Find(const string &id);
    int Walk(rwalkfn walkfn, void *data);
    int WalkReceivers(rwalkfn walkfn, void *data);
    int WalkDispatchers(rwalkfn walkfn, void *data);
    int TimeWait(Role *r);
    bool reg_keep_session(const struct spioreg *rgh);
    int Register(struct spioreg *rgh, Conn *conn);
    int OutputRoleStatus(FILE *fp);
    
 private:
    CtxConf *cfg;
    string appid;
    pmutex lock;
    inline int biglock();
    inline int unbiglock();

    struct rgmh_stats romstat;
    map<string, Role *> active_roles;
    struct list_head receivers, dispatchers;

    int __walk_receivers(rwalkfn walkfn, void *data);
    int __walk_dispatchers(rwalkfn walkfn, void *data);
    
    struct list_head newroles;
    inline Role *pop_new_role();
    inline void push_new_role(Role *r);

    inline Role *find_exist_role(const string rid, uint32_t rtyp);
    
    struct list_head tw_receivers;
    struct list_head tw_dispatchers;
    skrb_t tw_tree;
    inline void clean_time_wait_roles();
    inline Role *__find_tw_r(const string rid, struct list_head *head);
    inline Role *find_time_wait_role(const string rid, uint32_t rtyp);
    inline void insert_time_wait_role(Role *r);
};




}


#endif    // _H_ROLEMANAGER_H
