// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/load_balance.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "load_balance.h"
#include "lb_rrbin.h"
#include "lb_iphash.h"
#include "lb_random.h"
#include "lb_fair.h"

NSPIO_DECLARATION_START


LBAdapter *MakeLBAdapter(int lbalgo, ...) {
    LBAdapter *lbp = NULL;

    switch (lbalgo) {
    case LB_RRBIN:
	lbp = new (std::nothrow) lb_rrbin();
	return lbp;
    case LB_RANDOM:
	lbp = new (std::nothrow) lb_random();
	return lbp;
    case LB_IPHASH:
	lbp = new (std::nothrow) lb_iphash();
	return lbp;
    case LB_FAIR:
	lbp = new (std::nothrow) lb_fair();
	return lbp;
    }
    errno = EINVAL;
    return NULL;
}

















}
