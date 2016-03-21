// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/appctx_icmp.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "log.h"
#include "os/memalloc.h"
#include "proto/icmp.h"
#include "appctx.h"




NSPIO_DECLARATION_START

static int __send_role_status_icmp(Role *r, void *data) {
    int *dispatchers = (int *)data;
    struct nspiomsg *icmp_msg = NULL;

    if (r->Type() != ROLE_PIORECEIVER)
	return -1;
    if (!(icmp_msg = make_role_status_icmp(*dispatchers))) {
	NSPIOLOG_ERROR("%s make role status icmp with errno %d", r->cid(), errno);
	return -1;
    }
    if (r->PushSNDICMP(icmp_msg) < 0) {
	NSPIOLOG_ERROR("%s push role status icmp with errno %d", r->cid(), errno);
	mem_free(icmp_msg, MSGPKGLEN(icmp_msg));	
	return -1;
    }
    r->send_icmp();
    NSPIOLOG_INFO("%s push role status icmp", r->cid());
    return 0;
}


int AppCtx::broadcast_role_status_icmp() {
    int dispatchers = ratm.count_canw_dispatchers();
    rom.WalkReceivers(__send_role_status_icmp, &dispatchers);
    return 0;
}



}
