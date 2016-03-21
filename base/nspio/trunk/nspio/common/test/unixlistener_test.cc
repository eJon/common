// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/test/unixlistener_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <gtest/gtest.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <nspio/errno.h>
#include "net/unix.h"
#include "log.h"
#include "runner/thread.h"

using namespace nspio;


static string unix_sock = "/tmp/unix_listener.sock";


static int randstr(char *buf, int len) {
    int i, idx;
    char token[] = "qwertyuioplkjhgfdsazxcvbnm1234567890";
    for (i = 0; i < len; i++) {
	idx = rand() % strlen(token);
	buf[i] = token[idx];
    }
    return 0;
}



static int unix_client(void *arg_) {
    UnixConn *cli;
    char buf[1024];
    int64_t nbytes;

    if (!(cli = DialUnix("unix", "", unix_sock))) {
	NSPIOLOG_ERROR("DialUnix with errno %d", errno);
	return -1;
    }
    randstr(buf, 1024);
    
    EXPECT_EQ(500, nbytes = cli->Write(buf, 500));
    EXPECT_EQ(nbytes, cli->Read(buf, nbytes));
    cli->Close();

    cli->Reconnect();
    EXPECT_EQ(500, nbytes = cli->Write(buf, 500));
    EXPECT_EQ(nbytes, cli->Read(buf, nbytes));
    cli->Close();

    delete cli;
    return 0;
}

static int unix_server(void *arg_) {
    UnixListener *listener;
    UnixConn *cli;
    char buf[1024];
    int64_t nbytes;
    Thread cli_thread;
    int cli_ret = 0;

    randstr(buf, 1024);
    if (!(listener = ListenUnix("unix", unix_sock, 100))) {
	NSPIOLOG_ERROR("ListenUnix with errno %d", errno);
	return -1;
    }

    cli_thread.Start(unix_client, NULL);

    // client first connect
    if (!(cli = listener->Accept())) {
	NSPIOLOG_ERROR("Accept with errno %d", errno);
	return -1;
    }
    nbytes = cli->Read(buf, 1024);
    EXPECT_EQ(nbytes, cli->Write(buf, nbytes));
    cli->Close();
    delete cli;

    // client reconnect
    if (!(cli = listener->Accept())) {
	NSPIOLOG_ERROR("Accept with errno %d", errno);
	return -1;
    }
    nbytes = cli->Read(buf, 1024);
    EXPECT_EQ(nbytes, cli->Write(buf, nbytes));
    cli->Close();
    delete cli;

    listener->Close();
    delete listener;

    cli_ret = cli_thread.Stop();
    EXPECT_EQ(0, cli_ret);

    return 0;
}



TEST(unixconn, dialunix) {
    Thread svr_thread;
    int svr_ret = 0;

    svr_thread.Start(unix_server, NULL);
    svr_ret = svr_thread.Stop();
    EXPECT_EQ(0, svr_ret);
}
