// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/test/rolemgr_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <gtest/gtest.h>
#include <string>
#include "net/tcp.h"
#include "log.h"
#include "appctx.h"

using namespace std;
using namespace nspio;


static int rolemanager(void *arg_) {
    RoleManager rom;
    Conn *conn;
    Role *r;
    CtxConf cfg;
    struct spioreg header = {};
    int cnt = 50, i = 0;

    rom.Init("testapp", &cfg);
    strcpy(header.appname, "testapp");
    for (i = 0; i < cnt; i++) {
	conn = new TCPConn();
	if (rand() % 3 == 0)
	    header.rtype = ROLE_RECEIVER;
	else
	    header.rtype = ROLE_DISPATCHER;
	uuid_generate(header.rid);
	rom.Register(&header, conn);
    }

    for (i = 0; i < cnt; i++) {
	if (NULL != (r = rom.PopNewer())) {
	    r->unBind(&conn);
	    rom.TimeWait(r);
	    delete conn;
	} else
	    NSPIOLOG_ERROR("invalid poprole %d", i);
	break;
    }
    return 0;
}


TEST(role, manager) {
    rolemanager(NULL);
}
