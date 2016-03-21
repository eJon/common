// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/ctx_global.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_CTX_GLOBAL_H_
#define _H_CTX_GLOBAL_H_


#include <string>
#include <sstream>
#include <iostream>
#include "decr.h"

using namespace std;

NSPIO_DECLARATION_START


class ctx_stat_recorder {
 public:
    virtual ~ctx_stat_recorder() {}
    virtual int add(string &key, int64_t val) = 0;
};

int set_ctx_global_stat_recorder(ctx_stat_recorder *__csr);


}
#endif   // _H_CTX_GLOBAL_H_
