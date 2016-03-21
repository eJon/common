// copyright:
//            (C) SINA Inc.
//
//           file: nspio/service/simplenspio/simplenspio.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#define __NSPIO_UT__
#include <inttypes.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <nspio/errno.h>
#include "os.h"
#include "net.h"
#include "log.h"
#include "CRC.h"
#include "osthread.h"
#include "epoller.h"
#include "apiintern.h"


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

typedef struct my_package {
    int64_t sendstamp;
    uint16_t checksum;
    char __padding[1024];
} my_pkg_t;

static int tcp_server(void *arg_) {
    TCPListener *listener = NULL;
    TCPConn *cli = NULL;
    my_pkg_t req = {};
    int64_t nbytes = sizeof(my_pkg_t), cur, idx = 0;
    EpollEvent ee;
    Epoller *poller = NULL;
    int64_t rtt = 0, cnt = 0;
    struct list_head io_head, to_head;

    INIT_LIST_HEAD(&io_head);
    INIT_LIST_HEAD(&to_head);

    poller = EpollCreate(1024, 100);
    listener = ListenTCP("tcp", "*:1530", 1000);

    if (!(cli = listener->Accept())) {
	printf("fuck\n");
	return -1;
    }

    cli->SetSockOpt(SO_NONBLOCK, 1);
    cli->SetSockOpt(SO_NODELAY, 1);
    ee.SetEvent(cli->Fd(), EPOLLIN, NULL);
    poller->CtlAdd(&ee);

    while (1) {
	if (poller->Wait(&io_head, &to_head, 0) < 0 || list_empty(&io_head)) {
	    detach_for_each_poll_link(&to_head);
	    continue;
	}
	if (ee.happened & EPOLLIN) {
	    while (1) {
		cur = cli->Read(((char *)&req) + idx, nbytes - idx);
		if (cur > 0) {
		    idx += cur;
		    if (idx == nbytes) {
			idx = 0;
			rtt += rt_mstime() - req.sendstamp;
			if (crc16(req.__padding, 1024) != req.checksum)
			    printf("fuck check\n");
			memset(&req, 0, nbytes);
			cnt++;
			if (cnt % 1000 == 0 && cnt) {
			    printf("cur avg rtt: %ld\n", rtt/cnt);
			}
		    }
		} else if (cur == 0){
		    break;
		}
	    }
	}
	detach_for_each_poll_link(&io_head);
	detach_for_each_poll_link(&to_head);
    }
    return 0;
}


static int tcp_client(void *arg_) {
    TCPConn *cli = NULL;
    my_pkg_t req = {};
    int i, len, ret;
    int g_time = 1000000;
    int g_freq = 1000;
    int g_pkgs = 300;
    int64_t nbytes = sizeof(my_pkg_t), idx = 0;
    int64_t start_tt, end_tt, intern_tt;

    start_tt = rt_mstime();
    end_tt = start_tt + g_time;
    intern_tt = 1000000 / g_freq;
    if (!(cli = DialTCP("tcp", "", "127.0.0.1:1530"))) {
	printf("fuck\n");
	return -1;
    }
    cli->SetSockOpt(SO_NONBLOCK, 1);
    cli->SetSockOpt(SO_QUICKACK, 1);

    int sockfd = cli->Fd();
    int sock_buf_size = 60388608;
    setsockopt( sockfd, SOL_SOCKET, SO_SNDBUF,
		(char *)&sock_buf_size, sizeof(sock_buf_size) );
    setsockopt( sockfd, SOL_SOCKET, SO_RCVBUF,
		(char *)&sock_buf_size, sizeof(sock_buf_size) );

    randstr(req.__padding, 1024);
    req.checksum = crc16(req.__padding, 1024);
    while (rt_mstime() <= end_tt) {
	for (i = 0; i < g_pkgs; i++) {
	    len = rand() % 512;
	    req.sendstamp = rt_mstime();
	    idx = 0;
	    while (idx < nbytes) {
		ret = cli->Write((char *)&req + idx, nbytes - idx);
		if (ret < 0)
		    return -1;
		else if (ret == 0)
		    printf("fuck again\n");
		idx += ret;
	    }
	}
	rt_usleep(intern_tt);
    }
    return 0;
}


int main(int argc, char **argv) {
    OSThread t[2];

    t[0].Start(tcp_server, NULL);
    sleep(2);
    t[1].Start(tcp_client, NULL);

    sleep(1000);
    return 0;
}
