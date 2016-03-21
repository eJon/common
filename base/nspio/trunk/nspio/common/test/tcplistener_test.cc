// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/test/tcplistener_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <gtest/gtest.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <nspio/errno.h>
#include "log.h"
#include "net/tcp.h"
#include "runner/thread.h"



using namespace nspio;

static int randstr(char *buf, int len) {
    int i, idx;
    char token[] = "qwertyuioplkjhgfdsazxcvbnm1234567890";
    for (i = 0; i < len; i++) {
	idx = rand() % strlen(token);
	buf[i] = token[idx];
    }
    return 0;
}

static int tcp_client(void *arg_) {
    TCPConn *cli;
    char buf[1024] = {};
    int64_t nbytes;

    if (!(cli = DialTCP("tcp", "", "127.0.0.1:18894"))) {
	NSPIOLOG_ERROR("DialTCP with errno %d", errno);
	return -1;
    }
    randstr(buf, 1024);
    EXPECT_EQ(500, nbytes = cli->Write(buf, 500));
    EXPECT_EQ(nbytes, cli->Read(buf, nbytes));
    cli->Close();

    if (0 != cli->Reconnect())
	return -1;
    EXPECT_EQ(500, nbytes = cli->Write(buf, 500));
    EXPECT_EQ(nbytes, cli->Read(buf, nbytes));

    cli->Close();
    delete cli;
    return 0;
}

static int tcp_server(void *arg_) {
    TCPListener *listener;
    TCPConn *cli;
    char buf[1024];
    int64_t nbytes;
    Thread cli_thread;
    int cli_ret = 0;
    
    if (!(listener = ListenTCP("tcp", "*:18894", 100))) {
	NSPIOLOG_ERROR("ListenTCP with errno %d", errno);
	return -1;
    }
    cli_thread.Start(tcp_client, NULL);

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



static int tcp_client2(void *arg_) {
    TCPConn *cli = NULL;
    char buf[1024] = {};
    int64_t nbytes;

    if (!(cli = DialTCP("tcp", "", "127.0.0.1:18894"))) {
	NSPIOLOG_ERROR("DialTCP with errno %d", errno);
	return -1;
    }
    randstr(buf, 1024);
    EXPECT_EQ(500, nbytes = cli->Write(buf, 500));
    while (cli->Read(buf, nbytes) == 0)
	usleep(1);

    delete cli;
    return 0;
}

static int tcp_server2(void *arg_) {
    TCPListener *listener;
    TCPConn *cli = NULL;
    char buf[1024];
    int64_t nbytes;
    Thread cli_thread;
    
    if (!(listener = ListenTCP("tcp", "*:18894", 100))) {
	NSPIOLOG_ERROR("ListenTCP with errno %d", errno);
	return -1;
    }

    cli_thread.Start(tcp_client2, NULL);

    // client first connect
    if (!(cli = listener->Accept())) {
	NSPIOLOG_ERROR("Accept with errno %d", errno);
	return -1;
    }
    EXPECT_EQ(500, nbytes = cli->Read(buf, 1024));
    cli->Write(buf, 500);
    cli_thread.Stop();

    delete cli;
    delete listener;
    return 0;
}



static int tcp_client3(void *arg_) {
    TCPConn *cli = NULL;
    char buf[1024] = {};
    char buf2[1024] = {};
    int fd, sock_buf_size = 1;
    int64_t nbytes, idx = 0, ret;
    
    if (!(cli = DialTCP("tcp", "", "127.0.0.1:18894"))) {
	NSPIOLOG_ERROR("DialTCP with errno %d", errno);
	return -1;
    }
    randstr(buf, 1024);
    fd = cli->Fd();
    cli->SetSockOpt(SO_NONBLOCK, 1);
    cli->SetSockOpt(SO_WRITECACHE, 5);
    cli->SetSockOpt(SO_READCACHE, 5);
    setsockopt( fd, SOL_SOCKET, SO_SNDBUF,
		(char *)&sock_buf_size, sizeof(sock_buf_size) );
    setsockopt( fd, SOL_SOCKET, SO_RCVBUF,
		(char *)&sock_buf_size, sizeof(sock_buf_size) );
    idx = 0;
    nbytes = 500;
    while (idx < nbytes) {
	if ((ret = cli->Write(buf + idx, nbytes - idx)) < 0 && errno != EAGAIN) {
	    NSPIOLOG_ERROR("%d", errno);
	    break;
	}
	if (ret > 0)
	    idx += ret;
    }
    while (cli->Flush() < 0 && errno == EAGAIN) {}
    idx = 0;
    nbytes = 500;
    while (idx < nbytes) {
	if ((ret = cli->Read(buf2 + idx, nbytes - idx)) < 0 && errno != EAGAIN) {
	    NSPIOLOG_ERROR("%d", errno);
	    break;
	}
	if (ret > 0)
	    idx += ret;
    }
    EXPECT_TRUE(memcmp(buf, buf2, 500) == 0);
    delete cli;
    return 0;
}

static int tcp_server3(void *arg_) {
    Thread cli_thread;
    TCPListener *listener;
    TCPConn *cli = NULL;
    char buf[1024];
    int fd, sock_buf_size = 1;
    int64_t nbytes, idx = 0, ret;
    
    if (!(listener = ListenTCP("tcp", "*:18894", 100))) {
	NSPIOLOG_ERROR("ListenTCP with errno %d", errno);
	return -1;
    }
    fd = listener->Fd();
    setsockopt( fd, SOL_SOCKET, SO_SNDBUF,
		(char *)&sock_buf_size, sizeof(sock_buf_size) );
    setsockopt( fd, SOL_SOCKET, SO_RCVBUF,
		(char *)&sock_buf_size, sizeof(sock_buf_size) );

    cli_thread.Start(tcp_client3, NULL);
    // client first connect
    if (!(cli = listener->Accept())) {
	NSPIOLOG_ERROR("Accept with errno %d", errno);
	return -1;
    }
    cli->SetSockOpt(SO_NONBLOCK, 1);
    cli->SetSockOpt(SO_WRITECACHE, 5);
    cli->SetSockOpt(SO_READCACHE, 5);
    
    idx = 0;
    nbytes = 500;
    while (idx < nbytes) {
	if ((ret = cli->Read(buf + idx, nbytes - idx)) < 0 && errno != EAGAIN)
	    break;
	if (ret > 0)
	    idx += ret;
    }
    idx = 0;
    nbytes = 500;
    while (idx < nbytes) {
	if ((ret = cli->Write(buf + idx, nbytes - idx)) < 0 && errno != EAGAIN)
	    break;
	if (ret > 0)
	    idx += ret;
    }
    while (cli->Flush() < 0 && errno == EAGAIN) {}
    cli_thread.Stop();
    delete cli;
    delete listener;
    return 0;
}





TEST(tcpconn, dialtcp) {
    Thread svr_thread;
    int svr_ret = 0;

    svr_thread.Start(tcp_server, NULL);
    svr_ret = svr_thread.Stop();
    EXPECT_EQ(0, svr_ret);

    svr_thread.Start(tcp_server2, NULL);
    svr_ret = svr_thread.Stop();
    EXPECT_EQ(0, svr_ret);

    svr_thread.Start(tcp_server3, NULL);
    svr_ret = svr_thread.Stop();
    EXPECT_EQ(0, svr_ret);

}
