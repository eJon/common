// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/net/unixlistener.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <nspio/errno.h>
#include "net/ip.h"
#include "net/unix.h"
#include "os/memalloc.h"


NSPIO_DECLARATION_START

static mem_stat_t listenunix_mem_stats = {};


UnixListener *listenUnix(string net, string laddr, int backlog) {
    UnixListener *unixlistener = new (std::nothrow) UnixListener();
    
    if (!unixlistener) {
	errno = ENOMEM;
	return NULL;
    }
    unixlistener->localaddr = laddr;
    unixlistener->backlog = backlog;

    if (resolve_local_path(&unixlistener->addr,
			   &unixlistener->addrlen, laddr.c_str())) {
	delete unixlistener;
	return NULL;
    }
    
    unixlistener->sockfd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (unixlistener->sockfd == -1) {
	delete unixlistener;
	return NULL;
    }
    
    if (-1 == bind(unixlistener->sockfd, (struct sockaddr *) &unixlistener->addr,
		   unixlistener->addrlen) ||
	-1 == listen(unixlistener->sockfd, backlog)) {
	close(unixlistener->sockfd);
	delete unixlistener;
	return NULL;
    }

    return unixlistener;
}


UnixListener *ListenUnix(string net, string laddr, int backlog) {
    UnixListener *listen = NULL;
    
    if (laddr == "" || (net != "unix" && net != "unixpacket")) {
	errno = EINVAL;
	return NULL;
    }
    listen = listenUnix(net, laddr, backlog);
    return listen;
}





UnixListener::UnixListener() :
    sockfd(-1), backlog(0), addrlen(0)
{
    memset(&addr, 0, sizeof(addr));
    listenunix_mem_stats.alloc++;
    listenunix_mem_stats.alloc_size += sizeof(UnixListener);
}

UnixListener::~UnixListener() {
    Close();
    listenunix_mem_stats.alloc--;
    listenunix_mem_stats.alloc_size -= sizeof(UnixListener);
}

UnixConn *UnixListener::Accept() {
    UnixConn *unixconn = NULL;
    
    if (sockfd == -1) {
	errno = SPIO_EINTERN;
	return NULL;
    }
    if (!(unixconn = new (std::nothrow) UnixConn())) {
	errno = ENOMEM;
	return NULL;
    }

    // Importance!
    memset(&unixconn->laddr, 0, sizeof(unixconn->laddr));
    unixconn->laddrlen = sizeof(unixconn->laddr);

    memset(&unixconn->raddr, 0, sizeof(unixconn->raddr));
    unixconn->raddrlen = sizeof(unixconn->raddr);
    
    // Accept one incoming connection
    unixconn->openmode = O_PASSIVE;
    unixconn->sockfd = accept(sockfd, (struct sockaddr *) &unixconn->raddr, &unixconn->raddrlen);

    if (unixconn->sockfd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || 
        errno == EINTR || errno == ECONNABORTED)) {
	errno = EAGAIN;
	delete unixconn;
        return NULL;
    } else if (unixconn->sockfd == -1) {
	// fixed for other errno, like EMFILE, ENFILE, ENOBUFS, ENOMEM.
	delete unixconn;
	return NULL;
    }

    if (-1 == getsockname(unixconn->sockfd, (struct sockaddr *)&unixconn->laddr,
			  &unixconn->laddrlen)) {
	delete unixconn;
	return NULL;
    }
    unixconn->localaddr = localaddr;
    return unixconn;
}

int UnixListener::Close() {
    
    if (sockfd != -1) {
	close(sockfd);
	unlink(localaddr.c_str());
    }
    sockfd = -1;

    return 0;
}

int UnixListener::SetNonBlock(bool nonblock) {
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



}
