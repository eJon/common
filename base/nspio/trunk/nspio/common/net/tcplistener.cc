// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/net/tcplistener.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <stdio.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <nspio/errno.h>
#include "net/ip.h"
#include "net/tcp.h"
#include "os/memalloc.h"


NSPIO_DECLARATION_START

static mem_stat_t listentcp_mem_stats = {};



TCPListener *listenTCP(string net, string laddr, int backlog) {
    TCPListener *tcplistener = new (std::nothrow) TCPListener();

    if (!tcplistener) {
	errno = ENOMEM;
	return NULL;
    }

    tcplistener->localaddr = laddr;
    if (resolve_ip_interface(&tcplistener->addr, &tcplistener->addrlen, laddr.c_str())) {
	delete tcplistener;
	return NULL;
    }

    tcplistener->backlog = backlog;
    tcplistener->sockfd = socket(tcplistener->addr.ss_family, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == tcplistener->sockfd) {
	delete tcplistener;
	return NULL;
    }

    // default fast reuse addr
    tcplistener->SetReuseAddr(true);
    
    if (-1 == bind(tcplistener->sockfd, (struct sockaddr *) &tcplistener->addr,
		   tcplistener->addrlen)) {
	close(tcplistener->sockfd);
	delete tcplistener;
	return NULL;
    }
    
    if (-1 == listen(tcplistener->sockfd, backlog)) {
	close(tcplistener->sockfd);
	delete tcplistener;
	return NULL;
    }

    return tcplistener;
}


TCPListener *ListenTCP(string net, string laddr, int backlog) {
    TCPListener *listen = NULL;
    
    if (laddr == "" || (net != "tcp" && net != "tcp4" && net != "tcp6")) {
	errno = EINVAL;
	return NULL;
    }
    listen = listenTCP(net, laddr, backlog);
    return listen;
}

TCPListener::TCPListener() :
    sockfd(-1), backlog(0), addrlen(0)
{
    memset(&addr, 0, sizeof(addr));
    memset(&addrlen, 0, sizeof(addrlen));
    listentcp_mem_stats.alloc++;
    listentcp_mem_stats.alloc_size += sizeof(TCPListener);
}

TCPListener::~TCPListener() {
    if (sockfd != -1)
	close(sockfd);
    sockfd = -1;
    listentcp_mem_stats.alloc--;
    listentcp_mem_stats.alloc_size -= sizeof(TCPListener);
}

TCPConn *TCPListener::Accept() {

    char addrbuf[32] = {};   // enough for ipv4 address: ip[16] + port[5]
    TCPConn *tcpconn = NULL;
    struct sockaddr_in *sa_in = NULL;

    if (sockfd == -1) {
	errno = SPIO_EINTERN;
	return NULL;
    }
    if (!(tcpconn = new (std::nothrow) TCPConn())) {
	errno = ENOMEM;
	return NULL;
    }
    
    // Importance!
    memset(&tcpconn->laddr, 0, sizeof(tcpconn->laddr));
    tcpconn->laddrlen = sizeof(tcpconn->laddr);

    memset(&tcpconn->raddr, 0, sizeof(tcpconn->raddr));
    tcpconn->raddrlen = sizeof(tcpconn->raddr);

    
    // Accept one incoming connection and filled remote address
    tcpconn->openmode = O_PASSIVE;
    tcpconn->sockfd = accept(sockfd, (struct sockaddr *) &tcpconn->raddr, &tcpconn->raddrlen);
    
    if (tcpconn->sockfd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || 
        errno == EINTR || errno == ECONNABORTED)) {
	errno = EAGAIN;
	delete tcpconn;
        return NULL;
    } else if (tcpconn->sockfd == -1) {
	// fixed for other errno, like EMFILE, ENFILE, ENOBUFS, ENOMEM.
	delete tcpconn;
	return NULL;
    }

    sa_in = (struct sockaddr_in *)&tcpconn->raddr;
    inet_ntop(AF_INET, (char *)&sa_in->sin_addr, addrbuf, sizeof(addrbuf));
    snprintf(addrbuf + strlen(addrbuf),
	     sizeof(addrbuf) - strlen(addrbuf), ":%d", ntohs(sa_in->sin_port));
    tcpconn->remoteaddr = addrbuf;

    // get socket local address because the laddr maybe *:xxxx
    if (-1 == getsockname(tcpconn->sockfd, (struct sockaddr *)&tcpconn->laddr,
			  &tcpconn->laddrlen)) {
	delete tcpconn;
	return NULL;
    }
    sa_in = (struct sockaddr_in *)&tcpconn->laddr;
    inet_ntop(AF_INET, (char *)&sa_in->sin_addr, addrbuf, sizeof(addrbuf));
    snprintf(addrbuf + strlen(addrbuf),
	     sizeof(addrbuf) - strlen(addrbuf), ":%d", ntohs(sa_in->sin_port));
    tcpconn->localaddr = addrbuf;


    return tcpconn;
}


int TCPListener::Close() {
    if (sockfd != -1)
	close(sockfd);
    sockfd = -1;

    return 0;
}


int TCPListener::SetNonBlock(bool nonblock) {
    int flags = 0, ret = 0;

    if (sockfd < 0) {
	errno = SPIO_EINTERN;
	return -1;
    }

    flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
	flags = 0;
    if (nonblock)
	flags |= O_NONBLOCK;
    else
	flags &= ~O_NONBLOCK;
    ret = fcntl(sockfd, F_SETFL, flags);

    return ret;
}


int TCPListener::SetReuseAddr(bool reuse) {
    int flags = reuse ? 1 : 0;
    int ret = 0;

    if (sockfd < 0) {
	errno = SPIO_EINTERN;
	return -1;
    }
    ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(flags));

    return ret;
}



}
