// copyright:
//            (C) SINA Inc.
//
//           file: nspio/service/nspiod/rgm_svr.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "log.h"
#include "regmgr/regmgr.h"


using namespace std;
using namespace nspio;

Rgm *rgm = NULL;

int nspiod_start_regmgr(SpioConfig *spio_conf) {
    set<string>::iterator it;

    if (!(rgm = NewRegisterManager(spio_conf))) {
	NSPIOLOG_ERROR("start regmgr failed of oom");
	return -1;
    }
    for (it = spio_conf->regmgr_listen_addrs.begin();
	 it != spio_conf->regmgr_listen_addrs.end(); ++it) {
	rgm->Listen(*it);
    }
    return rgm->StartThread();
}


void nspiod_stop_regmgr() {
    if (rgm) {
	rgm->StopThread();
	delete rgm;
    }
}

