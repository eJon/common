// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/lb_rrbin.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_LB_RRBIN_
#define _H_LB_RRBIN_


#include "load_balance.h"


NSPIO_DECLARATION_START

class lb_rrbin : public LBAdapter {
 public:
    lb_rrbin();
    ~lb_rrbin();

    int add(Role *r);
    int del(Role *r);
    int size();
    int balance();
    Role *loadbalance_recv();
    Role *loadbalance_send(struct nspiomsg *msg);
    
 private:
    int idx, numbers, cap;
    Role **backend_servers;

    Role *__loadbalance_send(struct nspiomsg *msg);
};


}













#endif   // _H_LB_RRBIN_
