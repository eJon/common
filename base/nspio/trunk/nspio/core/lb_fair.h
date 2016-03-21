// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/lb_fair.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_LBFAIR_
#define _H_LBFAIR_

#include "load_balance.h"

NSPIO_DECLARATION_START


class lb_fair : public LBAdapter {
 public:
    lb_fair();
    ~lb_fair();

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




#endif  // _H_LBFAIR_
