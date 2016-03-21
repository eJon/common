// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/load_balance.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_LOAD_BALANCE_
#define _H_LOAD_BALANCE_

#include "role.h"
#include "proto/proto.h"


NSPIO_DECLARATION_START


enum BALANCE_ALGO {
    LB_RRBIN = 0,
    LB_RANDOM,
    LB_IPHASH,
    LB_FAIR,
    BALANCE_ALGO_KEYRANGE,
};

class LBAdapter {
 public:
    virtual ~LBAdapter() {}
    virtual int size() = 0;
    virtual int balance() = 0;
    virtual int add(Role *r) = 0;
    virtual int del(Role *r) = 0;
    virtual Role *loadbalance_recv() = 0;
    virtual Role *loadbalance_send(struct nspiomsg *msg) = 0;
};


LBAdapter *MakeLBAdapter(int lbalgo, ...);

}


#endif // _H_LOAD_BALANCE_
