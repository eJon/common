// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/lb_iphash.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_LB_IPHASH_
#define _H_LB_IPHASH_

#include "load_balance.h"

NSPIO_DECLARATION_START

class lb_iphash : public LBAdapter {
 public:
    lb_iphash();
    ~lb_iphash();

    int add(Role *r);
    int del(Role *);
    int size();
    int balance();
    Role *loadbalance_recv();
    Role *loadbalance_send(struct nspiomsg *msg);

 private:
    int idx, numbers, cap;
    Role **backend_servers;
};




}




#endif  // _H_LB_IPHASH_
