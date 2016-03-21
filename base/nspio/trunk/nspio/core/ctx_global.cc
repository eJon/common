// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/ctx_global.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "ctx_global.h"


using namespace std;

NSPIO_DECLARATION_START

ctx_stat_recorder *csr = NULL;

int set_ctx_global_stat_recorder(ctx_stat_recorder *__csr) {
    csr = __csr;
    return 0;
}

}
