// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/test/sectionreader_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <gtest/gtest.h>
#include <stdlib.h>
#include <nspio/errno.h>
#include "net/tcp.h"
#include "runner/thread.h"
#include "log.h"
#include "sectionreader.h"


using namespace nspio;

static string tcp_sock = "*:18999";

static int sr_client(void *arg_) {
    TCPConn *cli;
    char buf[1024];
    int ret;
    SectionReadWriter sr;

    if (!(cli = DialTCP("tcp", "", tcp_sock)))
	return -1;
    cli->SetSockOpt(SO_NONBLOCK, 1);
    sr.InitReader(1024);
    sr.InitWriter(1024);
    while (1) {
	if ((ret = sr.ReadSection(cli, buf)) < 0 && errno == EAGAIN)
	    continue;
	else if (ret < 0)
	    return -1;
	break;
    }
    EXPECT_EQ(0, sr.WriteSection(cli, buf));
    delete cli;
    return 0;
}


static int randstr(char *buf, int len) {
    int i, idx;
    char token[] = "qwertyuioplkjhgfdsazxcvbnm1234567890";
    for (i = 0; i < len; i++) {
	idx = rand() % strlen(token);
	buf[i] = token[idx];
    }
    return 0;
}


static int sr_server(void *arg_) {
    TCPListener *listener;
    TCPConn *cli;
    char sendbuf[1024], recvbuf[1024];
    Thread cli_thread;
    int cli_ret = 0, ret;
    int64_t nbytes = 0, idx = 0;
    SectionReadWriter sr;

    randstr(sendbuf, 1024);
    randstr(recvbuf, 1024);
    EXPECT_NE(0, memcmp(sendbuf, recvbuf, 1024));

    
    if (!(listener = ListenTCP("tcp", tcp_sock, 100)))
	return -1;
    cli_thread.Start(sr_client, NULL);

    if (!(cli = listener->Accept()))
	return -1;
    cli->SetSockOpt(SO_NONBLOCK, 1);
    cli->SetSockOpt(SO_WRITECACHE, 1);
    sr.InitReader(1024);


    idx = nbytes = 0;
    while (idx < (int)sizeof(sendbuf)) {
	nbytes = cli->Write(sendbuf + idx, sizeof(sendbuf) - idx);
	EXPECT_TRUE(nbytes >= 0);
	idx += nbytes;
    }
    while (cli->Flush() < 0 && errno == EAGAIN) {}
    EXPECT_TRUE(nbytes == 1);
    EXPECT_TRUE(idx != nbytes);
    while (1) {
	if ((ret = sr.ReadSection(cli, recvbuf)) < 0 && errno == EAGAIN)
	    continue;
	else if (ret < 0)
	    return -1;
	break;
    }
    EXPECT_EQ(0, memcmp(sendbuf, recvbuf, 1024));
    delete cli;
    listener->Close();
    delete listener;

    cli_ret = cli_thread.Stop();
    EXPECT_EQ(0, cli_ret);

    return 0;
}
    

TEST(sectionReadWriter, readwrite) {
    Thread svr_thread;
    int svr_ret = 0;

    svr_thread.Start(sr_server, NULL);
    svr_ret = svr_thread.Stop();
    EXPECT_EQ(0, svr_ret);
}
