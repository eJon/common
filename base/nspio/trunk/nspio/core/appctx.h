// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/appctx.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_APPCONTEX_
#define _H_APPCONTEX_


#include "sync/tmutex.h"
#include "runner/thread.h"
#include "load_balance.h"
#include "config.h"
#include "mqp.h"
#include "regmgr/regmgr.h"
#include "rolemgr.h"
#include "role_attr_evh.h"
#include "appctx_modstat.h"
#include "ctx_global.h"
#include "base/lock_list.h"
#include "runner/task_pool.h"

using namespace std;

NSPIO_DECLARATION_START

#define list_app(link) ((AppCtx *)link->self) 
#define list_first_app(head)						\
    ({struct list_link *__pos =						\
	    list_first(head, struct list_link, node);  list_app(__pos);})

class AppCtx {
 public:
    AppCtx();
    ~AppCtx();

    string Id();
    int SetDebugMode(int mode);
    int Init(const string &id);
    int InitFromConf(CtxConf *app_conf);
    int Update(CtxConf *app_conf);
    int EnableRegistry(Rgm *_rgm);
    int Running();
    int Stop();
    int Start();
    int StopThread();
    int StartThread(TaskPool *tp = NULL);

    int attach_to_apps_head(struct list_head *head);
    int detach_from_apps_head();

    int __generic_role_recv(Role *r, struct lock_list *ll);
    int __generic_role_send(Role *r, struct lock_list *ll);

 private:
    bool stopping;
    uint32_t attr;
    int64_t now_time;
    int64_t first_no_dispatchers;
    int debugmode;
    int inited;
    string appid;
    Thread thread;
    tmutex_t conf_lock;
    CtxConf conf, nconf;
    LBAdapter *lbp;
    MQueuePool mqp;
    RoleManager rom;
    rattr_ev_monitor ratm;
    Epoller *poller;
    Rgm *rgm;
    struct list_link apps_link;
    TaskPool *iothreadpool;
    
    inline const char *cid() {
	return appid.c_str();
    }
    __appctx_module_stat_trigger __appctx_sm;
    module_stat astat;

    struct list_head err_roles;
    int wait_massage(struct list_head *rin, struct list_head *rout,
		     struct list_head *din, struct list_head *dout);
    int process_massage();

    int __deliver_forward(struct list_head *head, struct list_head *el_head);
    int __deliver_backward(struct list_head *head);
    int deliver_forward(struct list_head *head);
    int recv_massage(struct list_head *rhead, struct list_head *dhead);

    int sendto_massage(struct list_head *rhead, struct list_head *dhead);

    
    int process_conf_update();
    int process_removed_app();
    int process_connectto_app();
    bool is_time_to_close_all_receivers();
    int update_all_roles_status();
    int process_roles_register();
    int process_roles_error();
    int init_r_module_stat_trigger_level(Role *r);
    int process_single_error(Role *r);
    int broadcast_role_status_icmp();

    int process_console_show(int fd);
    int record_appctx_status_from_module_stat();
};







}







#endif  // _H_APPGROUP_
