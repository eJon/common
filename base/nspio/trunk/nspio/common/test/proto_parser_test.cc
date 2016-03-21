// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/test/proto_parser_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#define __NSPIO_UT__
#include <inttypes.h>
#include <gtest/gtest.h>
#include <time.h>
#include <unistd.h>
#include <nspio/errno.h>
#include "log.h"
#include "proto/proto.h"
#include "net/tcp.h"
#include "os/epoll.h"
#include "runner/thread.h"

using namespace nspio;


#define buflen 1024
static char buffer[buflen] = {};
static  int cnt = 10;

static int randstr(char *buf, int len) {
    int i, idx;
    char token[] = "qwertyuioplkjhgfdsazxcvbnm1234567890";
    for (i = 0; i < len; i++) {
	idx = rand() % strlen(token);
	buf[i] = token[idx];
    }
    return 0;
}

// test api for proto_parser
//    int _recv_massage(Conn *conn, struct nspiomsg **header, int reserve);
//    int _send_massage(Conn *conn, struct nspiomsg *header);
//    int _send_massage_async(Conn *conn, struct nspiomsg *header);

static volatile int pp_server_started = 0;

static int pp_server(void *arg_) {
    TCPListener *listen = NULL;
    TCPConn *conn;
    struct nspiomsg *hdr = NULL;
    proto_parser pp;
    int i, sendcnt = 0, recvcnt = 0;

    listen = ListenTCP("tcp", "*:20011", 100);
    listen->SetReuseAddr(true);
    EXPECT_TRUE(listen != NULL);
    pp_server_started = 1;
    conn = listen->Accept();
    conn->SetSockOpt(SO_NONBLOCK, 1);
    conn->SetSockOpt(SO_WRITECACHE, 10);
    conn->SetSockOpt(SO_READCACHE, 10);
    pp._set_recv_max(buflen / 2);
    for (i = 0; i < cnt; i++) {
	while (pp._recv_massage(conn, &hdr, 0) != 0)
	    usleep(10);
	recvcnt++;
	if (rand() % 2 == 0) {
	    EXPECT_TRUE(pp._send_massage(conn, hdr) == 0);
	} else {
	    while (pp._send_massage_async(conn, hdr) != 0)
		usleep(10);
	}
	sendcnt++;
	free(hdr);
	while (conn->Flush() < 0 && errno == EAGAIN) {}
    }
    delete conn;
    delete listen;
    return 0;
}



static int pp_client(void *arg_) {
    TCPConn *conn = NULL;
    struct nspiomsg req = {}, *resp = NULL;
    proto_parser pp;
    int i, sendcnt = 0, recvcnt = 0;

    while (!pp_server_started)
	usleep(10);
    if (!(conn = DialTCP("tcp", "", "127.0.0.1:20011"))) {
	NSPIOLOG_ERROR("DialTCP with errno %d", errno);
	return -1;
    }
    conn->SetSockOpt(SO_NONBLOCK, 1);
    conn->SetSockOpt(SO_WRITECACHE, 10);
    conn->SetSockOpt(SO_READCACHE, 10);
    req.data = buffer;
    for (i = 0; i < cnt; i++) {
	req.hdr.size = rand() % buflen;
	req.hdr.datacheck = crc16(buffer, req.hdr.size);
	package_makechecksum(&req.hdr, NULL, 0);
	if (rand() % 2 == 0) {
	    EXPECT_TRUE(pp._send_massage(conn, &req) == 0);
	} else {
	    while (pp._send_massage_async(conn, &req) != 0)
		usleep(10);
	}
	sendcnt++;
	while (conn->Flush() < 0 && errno == EAGAIN) {}
	if (req.hdr.size < buflen / 2) {
	    while (pp._recv_massage(conn, &resp, 0) != 0)
		usleep(10);
	    EXPECT_TRUE(resp->hdr.datacheck == crc16(resp->data, resp->hdr.size));
	    free(resp);
	    recvcnt++;
	} else
	    i--;
    }
    conn->SetSockOpt(SO_NONBLOCK, 0);
    conn->SetSockOpt(SO_TIMEOUT, 10);
    EXPECT_TRUE(-1 == pp._send_massage(conn, &req) && errno != EAGAIN);
    EXPECT_TRUE(-1 == pp._send_massage_async(conn, &req) && errno != EAGAIN);
    EXPECT_TRUE(-1 == pp._recv_massage(conn, &resp, 0));
    delete conn;
    return 0;
}


static int test_proto_nspio_massage() {
    Thread t[2];

    randstr(buffer, buflen);
    t[0].Start(pp_server, NULL);
    t[1].Start(pp_client, NULL);
    t[0].Stop();
    t[1].Stop();
    return 0;
}


TEST(proto, nspio_massage) {
    test_proto_nspio_massage();
}




// test api for proto_parser
//    int _recv_massage(Conn *conn, struct spiohdr *hdr, int c, struct slice *s);
//    int _send_massage(Conn *conn, struct spiohdr *hdr, int c, struct slice *s);
//    int _send_massage_async(Conn *conn, struct spiohdr *hdr, int c, struct slice *s);

static volatile int pp_server_started2 = 0;

static int pp_server2(void *arg_) {
    proto_parser pp;
    TCPListener *listen = NULL;
    TCPConn *conn;
    struct slice s = {};
    struct spiohdr hdr = {};
    int i, sendcnt = 0, recvcnt = 0;

    listen = ListenTCP("tcp", "*:20012", 100);
    listen->SetReuseAddr(true);
    EXPECT_TRUE(listen != NULL);
    pp_server_started2 = 1;
    conn = listen->Accept();
    conn->SetSockOpt(SO_NONBLOCK, 1);
    conn->SetSockOpt(SO_WRITECACHE, 10);
    conn->SetSockOpt(SO_READCACHE, 10);
    for (i = 0; i < cnt; i++) {
	while (pp._recv_massage(conn, &hdr, &s) != 0)
	    usleep(10);
	recvcnt++;
	if (rand() % 2 == 0) {
	    EXPECT_TRUE(pp._send_massage(conn, &hdr, 1, &s) == 0);
	} else {
	    while (pp._send_massage_async(conn, &hdr, 1, &s) != 0)
		usleep(10);
	}
	sendcnt++;
	while (conn->Flush() < 0 && errno == EAGAIN) {}
	free(s.data);
    }
    delete conn;
    delete listen;
    return 0;
}



static int pp_client2(void *arg_) {
    proto_parser pp;
    TCPConn *conn = NULL;
    int i, sendcnt = 0, recvcnt = 0;
    struct slice s = {};
    struct spiohdr reqhdr = {}, resphdr = {};

    while (!pp_server_started2)
	usleep(10);
    if (!(conn = DialTCP("tcp", "", "127.0.0.1:20012"))) {
	NSPIOLOG_ERROR("DialTCP with errno %d", errno);
	return -1;
    }
    conn->SetSockOpt(SO_NONBLOCK, 1);
    conn->SetSockOpt(SO_WRITECACHE, 10);
    conn->SetSockOpt(SO_READCACHE, 10);
    for (i = 0; i < cnt; i++) {
	reqhdr.size = rand() % buflen;
	reqhdr.datacheck = crc16(buffer, reqhdr.size);
	package_makechecksum(&reqhdr, NULL, 0);
	s.len = reqhdr.size;
	s.data = buffer;
	if (rand() % 2 == 0) {
	    EXPECT_TRUE(pp._send_massage(conn, &reqhdr, 1, &s) == 0);
	} else {
	    while (pp._send_massage_async(conn, &reqhdr, 1, &s) != 0) {
		printf("fuck\n");
		usleep(10);
	    }
	}
	sendcnt++;
	while (conn->Flush() < 0 && errno == EAGAIN) {}
	while (pp._recv_massage(conn, &resphdr, &s) != 0)
	    usleep(10);
	EXPECT_TRUE(resphdr.datacheck == crc16(s.data, resphdr.size));
	free(s.data);
	recvcnt++;
    }
    conn->SetSockOpt(SO_NONBLOCK, 0);
    conn->SetSockOpt(SO_TIMEOUT, 10);
    s.len = reqhdr.size;
    s.data = buffer;
    EXPECT_TRUE(-1 == pp._send_massage(conn, &reqhdr, 1, &s) && errno != EAGAIN);
    EXPECT_TRUE(-1 == pp._send_massage_async(conn, &reqhdr, 1, &s) && errno != EAGAIN);
    EXPECT_TRUE(-1 == pp._recv_massage(conn, &resphdr, &s) && errno != EAGAIN);
    delete conn;
    return 0;
}


static int test_proto_appmsg() {
    Thread t[2];

    randstr(buffer, buflen);
    t[0].Start(pp_server2, NULL);
    t[1].Start(pp_client2, NULL);
    t[0].Stop();
    t[1].Stop();
    return 0;
}



TEST(proto, appmsg) {
    test_proto_appmsg();
}




// test api for proto_parser
//    int __recv_data();
//    int __send_data();

static volatile int pp_server_started3 = 0;

static int pp_server3(void *arg_) {
    proto_parser pp;
    TCPListener *listen = NULL;
    TCPConn *conn;
    char b0[buflen] = {};
    struct slice s = {};
    int i;

    listen = ListenTCP("tcp", "*:20013", 100);
    listen->SetReuseAddr(true);
    EXPECT_TRUE(listen != NULL);
    pp_server_started3 = 1;
    conn = listen->Accept();
    conn->SetSockOpt(SO_NONBLOCK, 1);
    conn->SetSockOpt(SO_WRITECACHE, 10);
    conn->SetSockOpt(SO_READCACHE, 10);
    for (i = 0; i < cnt; i++) {
	s.len = buflen;
	s.data = b0;
	while (pp.__recv_data(conn, 1, &s) != 0)
	    usleep(10);
	pp._reset_recv();
	while (pp.__send_data(conn, 1, &s) != 0)
	    usleep(10);
	while (conn->Flush() < 0 && errno == EAGAIN) {}
	pp._reset_send();
    }
    delete conn;
    delete listen;
    return 0;
}



static int pp_client3(void *arg_) {
    proto_parser pp;
    TCPConn *conn = NULL;
    int i;
    char b0[buflen] = {};
    char b1[buflen] = {};
    char b2[buflen] = {};
    struct slice s[3] = {}, s2[3] = {};

    while (!pp_server_started3)
	usleep(10);
    if (!(conn = DialTCP("tcp", "", "127.0.0.1:20013"))) {
	NSPIOLOG_ERROR("DialTCP with errno %d", errno);
	return -1;
    }
    conn->SetSockOpt(SO_NONBLOCK, 1);
    conn->SetSockOpt(SO_WRITECACHE, 10);
    conn->SetSockOpt(SO_READCACHE, 10);
    for (i = 0; i < cnt; i++) {
	s2[0].len = s[0].len = rand() % (buflen/2);
	s2[1].len = s[1].len = rand() % (buflen/2);
	s2[2].len = s[2].len = buflen - s[0].len - s[1].len;
	s[0].data = s[1].data = s[2].data = buffer;
	while (pp.__send_data(conn, 3, (struct slice *)&s) != 0)
	    usleep(10);
	while (conn->Flush() < 0 && errno == EAGAIN) {}
	pp._reset_send();

	s2[0].data = b0;
	s2[1].data = b1;
	s2[2].data = b2;
	while (pp.__recv_data(conn, 3, (struct slice *)&s2) != 0)
	    usleep(10);
	pp._reset_recv();
	EXPECT_TRUE(memcmp(s[0].data, s2[0].data, s2[0].len) == 0);
	EXPECT_TRUE(memcmp(s[1].data, s2[1].data, s2[1].len) == 0);
	EXPECT_TRUE(memcmp(s[2].data, s2[2].data, s2[2].len) == 0);
    }
    conn->SetSockOpt(SO_NONBLOCK, 0);
    conn->SetSockOpt(SO_TIMEOUT, 10);
    delete conn;
    return 0;
}


static int test_proto_rawapi() {
    Thread t[2];

    randstr(buffer, buflen);
    t[0].Start(pp_server3, NULL);
    t[1].Start(pp_client3, NULL);
    t[0].Stop();
    t[1].Stop();
    EXPECT_LE(RTLEN, SPIORT_WARNING_LEN);
    return 0;
}


TEST(proto, rawapi) {
    test_proto_rawapi();
}
