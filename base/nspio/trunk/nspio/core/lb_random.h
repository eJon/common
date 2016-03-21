// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/lb_random.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_LB_RANDOM_
#define _H_LB_RANDOM_


#include "load_balance.h"


NSPIO_DECLARATION_START

class lb_random : public LBAdapter {
 public:
    lb_random();
    ~lb_random();

    int add(Role *r);
    int del(Role *r);
    int size();
    int balance();
    Role *loadbalance_recv();
    Role *loadbalance_send(struct nspiomsg *msg);

 private:
    int numbers, cap;
    Role **backend_servers;

    Role *__loadbalance_send(struct nspiomsg *msg);
};



}












#endif   // _H_LB_RANDOM_
