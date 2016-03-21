// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/test/regmgr_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include "log.h"
#include "sync_api.h"
#include "runner/thread.h"
#include "role.h"
#include "regmgr/regmgr.h"
#include "appctx.h"
#include "receiver.h"
#include "dispatcher.h"
#include "sectionreader.h"

using namespace nspio;

static string appname = "testapp";

static int send_data_by_chunk(Conn *conn, void *data, int size, int chunksize) {
    int nbytes = 0, ret, sendcnt;

    while (nbytes < size) {
	sendcnt = (size - nbytes) >= chunksize ? chunksize : size - nbytes;
	ret = conn->Write((char *)data + nbytes, sendcnt);
	if (ret < 0)
	    return -1;
	nbytes += ret;
	usleep(100);
    }
    return 0;
}


static RoleManager *rom;
static Rgm *rgm;

static int regmgr_server(void *arg_) {
    struct rgmh_stats romstat = {};

    if (0 != rgm->InsertHandler(rom))
	return -1;

    rgm->Start();
    rom->stats(&romstat);
    EXPECT_EQ(50, romstat.nr_receivers + romstat.nr_dispatchers);
    return 0;
}


static int regmgr_client(void *arg_) {
    struct rgmh_stats romstat = {};
    TCPConn *conn[50];
    struct spioreg header[50] = {};
    SectionReadWriter sr[50];
    int i;
    int recvdone = 0;

    for (i = 0; i < 50; i++) {
	sr[i].InitReader(REGHDRLEN);
	sr[i].InitWriter(REGHDRLEN);	
	conn[i] = DialTCP("tcp", "", "127.0.0.1:19998");
	if (!conn[i])
	    continue;
	recvdone++;
	conn[i]->SetSockOpt(SO_NONBLOCK, 1);
	if (rand() % 3 == 0)
	    header[i].rtype = ROLE_RECEIVER;
	else
	    header[i].rtype = ROLE_DISPATCHER;
	header[i].version = 4;
	header[i].timeout = 100;
	strcpy((char *)&header[i].appname, appname.c_str());
	uuid_generate(header[i].rid);
	send_data_by_chunk(conn[i], &header[i], REGHDRLEN, 10);
    }

    for (i = 0; i < 50; i++) {
	if (!conn[i])
	    continue;
	delete conn[i];
    }
    do {
	rom->stats(&romstat);
    } while (romstat.nr_receivers + romstat.nr_dispatchers != 50);

    return 0;
}

static int regmgr_test() {
    Thread thread[2];

    rgm = NewRegisterManager(NULL);
    rom = new RoleManager();
    rom->Init(appname, NULL);
    
    if (0 != rgm->Listen("*:19998"))
	return -1;
    
    thread[0].Start(regmgr_server, rgm);
    thread[1].Start(regmgr_client, NULL);
    thread[1].Stop();
    rgm->Stop();
    thread[0].Stop();

    delete rgm;
    delete rom;
    return 0;
}


TEST(regmgr, registe) {
    EXPECT_EQ(0, regmgr_test());
}


static Rgm *rgm2 = NULL;


static int regmgr_server2(void *arg_) {
    rgm2->Start();
    return 0;
}


static int regmgr_client2(void *arg_) {
    int i = 0, cnt = 100;
    __comsumer sr;

    if (sr.Connect(appname, "127.0.0.1:20005") < 0)
	NSPIOLOG_ERROR("connect error");

    for (i = 0; i < cnt; i++) {
	sr.Close();
	EXPECT_TRUE(0 == sr.Connect(appname, "127.0.0.1:20005"));
    }
    return 0;
}


static int regmgr_test2() {
    Thread thread[2];

    rgm2 = NewRegisterManager(NULL);
    if (0 != rgm2->Listen("*:20005"))
	return -1;
    
    thread[0].Start(regmgr_server2, NULL);
    thread[1].Start(regmgr_client2, NULL);
    thread[1].Stop();
    rgm2->Stop();
    thread[0].Stop();

    delete rgm2;
    return 0;
}

TEST(regmgr, reregiste) {
    EXPECT_EQ(0, regmgr_test2());
}



static int counter_test() {
    struct rgmh_stats romstat = {};
    Receiver r("testapp", "r");
    Dispatcher d("testapp", "d");

    INCR_AT_COUNTER(&r, &romstat);
    INCR_NR_COUNTER(&r, &romstat);
    INCR_TW_COUNTER(&r, &romstat);

    INCR_AT_COUNTER(&r, &romstat);
    INCR_NR_COUNTER(&r, &romstat);
    INCR_TW_COUNTER(&r, &romstat);

    INCR_AT_COUNTER(&d, &romstat);
    INCR_NR_COUNTER(&d, &romstat);
    INCR_TW_COUNTER(&d, &romstat);

    INCR_AT_COUNTER(&d, &romstat);
    INCR_NR_COUNTER(&d, &romstat);
    INCR_TW_COUNTER(&d, &romstat);

    
    DECR_AT_COUNTER(&r, &romstat);
    DECR_NR_COUNTER(&r, &romstat);
    DECR_TW_COUNTER(&r, &romstat);
    DECR_AT_COUNTER(&d, &romstat);
    DECR_NR_COUNTER(&d, &romstat);
    DECR_TW_COUNTER(&d, &romstat);

    EXPECT_EQ(3, RECEIVER_COUNTER(&romstat));
    EXPECT_EQ(3, DISPATCHER_COUNTER(&romstat));
    return 0;
}


TEST(regmgr, counter) {
    counter_test();
}
