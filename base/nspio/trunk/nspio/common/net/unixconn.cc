// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/net/unixconn.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <sys/socket.h>
#include <fcntl.h>
#include <stdarg.h>
#include <nspio/errno.h>
#include "net/ip.h"
#include "net/unix.h"
#include "os/memalloc.h"


NSPIO_DECLARATION_START

static mem_stat_t unixconn_mem_stats = {};



static int dialUnix(UnixConn *unixconn, string net, string laddr, string raddr) {

    struct sockaddr *sa = (struct sockaddr *) &unixconn->raddr;
    
    unixconn->openmode = O_ACTIVE;
    unixconn->network = net;
    unixconn->localaddr = laddr;
    unixconn->remoteaddr = raddr;

    memset(&unixconn->laddr, 0, sizeof(unixconn->laddr));
    unixconn->laddrlen = sizeof(unixconn->laddr);
    
    memset(&unixconn->raddr, 0, sizeof(unixconn->raddr));
    unixconn->raddrlen = sizeof(unixconn->raddr);

    if (0 != resolve_local_path(&unixconn->raddr, &unixconn->raddrlen, raddr.c_str()))
	return -1;
    
    if (-1 == (unixconn->sockfd = socket(AF_UNIX, SOCK_STREAM, 0)))
	return -1;

    if (-1 == connect(unixconn->sockfd, sa, unixconn->raddrlen)) {
	close(unixconn->sockfd);
	return -1;
    }

    if (-1 == getsockname(unixconn->sockfd, (struct sockaddr *)&unixconn->laddr,
			  &unixconn->laddrlen)) {
	close(unixconn->sockfd);
	return -1;
    }

    return 0;
}


UnixConn *DialUnix(string net, string laddr, string raddr) {
    UnixConn *unixconn;
    
    if (net != "unix" || raddr == "") {
	errno = EINVAL;
	return NULL;
    }
    unixconn = new (std::nothrow) UnixConn();
    if (!unixconn) {
	errno = ENOMEM;
	return NULL;
    }
    if (0 == dialUnix(unixconn, net, laddr, raddr))
	return unixconn;

    delete unixconn;
    return NULL;
}


UnixConn::UnixConn() :
    __errno(0), sockfd(-1), openmode(0), laddrlen(0), raddrlen(0),
    recvbuf(NULL), sendbuf(NULL),
    recvbuf_cap(0), recvbuf_len(0), recvbuf_idx(0),
    sendbuf_cap(0), sendbuf_len(0), sendbuf_idx(0)
{
    memset(&laddr, 0, sizeof(laddr));
    memset(&raddr, 0, sizeof(raddr));
    unixconn_mem_stats.alloc++;
    unixconn_mem_stats.alloc_size += sizeof(UnixConn);
}

UnixConn::~UnixConn() {
    if (sockfd != -1)
	close(sockfd);
    if (recvbuf)
	mem_free(recvbuf, recvbuf_cap);
    if (sendbuf)
	mem_free(sendbuf, recvbuf_cap);
    unixconn_mem_stats.alloc--;
    unixconn_mem_stats.alloc_size -= sizeof(UnixConn);
}


int64_t UnixConn::raw_read(char *buf, int64_t len) {

    int64_t nbytes;
    nbytes = recv(sockfd, buf, len, 0);

    //  Several errors are OK. When speculative read is being done we may not
    //  be able to read a single byte to the socket. Also, SIGSTOP issued
    //  by a debugging tool can result in EINTR error.
    if (nbytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
	__errno = errno;
	errno = EAGAIN;
        return -1;
    }
    // Signalise peer failure. Orderly shutdown by the other peer.
    if (nbytes == 0) {
	__errno = errno;
	errno = EPIPE;
	return -1;
    }
    /*
    if (nbytes == -1 && (errno == ECONNRESET || errno == ECONNREFUSED ||
        errno == ETIMEDOUT || errno == EHOSTUNREACH)) {
	errp = strerror_r(errno, errmsg, ESTRLEN);
        log_error("connect lost, errno %d., errmsg: %s", errno, errp);
        return -1;
    }
    */
    return nbytes;
}


int64_t UnixConn::Read(char *buf, int64_t len) {

    if (!buf || len <= 0) {
	errno = EINVAL;
	return -1;
    }
    if (sockfd < 0) {
	errno = SPIO_EINTERN;
	return -1;
    }
    return raw_read(buf, len);
}


int64_t UnixConn::raw_write(const char *buf, int64_t len) {
    int64_t nbytes;
    nbytes = send(sockfd, buf, len, 0);

    //  Several errors are OK. When speculative write is being done we may not
    //  be able to write a single byte to the socket. Also, SIGSTOP issued
    //  by a debugging tool can result in EINTR error.
    if (nbytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
	__errno = errno;
	errno = EAGAIN;
        return -1;
    } else if (nbytes == -1) {
	//  Signalise peer failure.
	__errno = errno;
	errno = EPIPE;
	return -1;
    }

    /*
    if (nbytes == -1 && (errno == ECONNRESET || errno == EPIPE)) {
	errp = strerror_r(errno, errmsg, ESTRLEN);
        log_error("connect lost, errno %d, errmsg: %s", errno, errp);
        return -1;
    }
    */

    return nbytes;
}


int64_t UnixConn::Write(const char *buf, int64_t len) {
    if (!buf || len <= 0) {
	errno = EINVAL;
	return -1;
    }
    if (sockfd < 0) {
	errno = SPIO_EINTERN;
	return -1;
    }
    return raw_write(buf, len);
}

int UnixConn::Close() {
    if (sockfd != -1)
	close(sockfd);
    sockfd = -1;
    return 0;
}

int UnixConn::Reconnect() {
    int ret = 0;

    if (sockfd >= 0) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (network != "unix" || remoteaddr == "") {
	return -1;
    }

    ret = dialUnix(this, network, localaddr, remoteaddr);
    return ret;
}


int UnixConn::LocalAddr(string &laddr) {
    laddr = localaddr;

    return 0;
}

int UnixConn::RemoteAddr(string &raddr) {
    raddr = remoteaddr;

    return 0;
}


int UnixConn::SetSockOpt(int op, ...) {
    va_list ap;

    if (sockfd < 0) {
	errno = SPIO_EINTERN;
	return -1;
    }
    
    switch (op) {
    case SO_TIMEOUT:
	{
	    int to_msec;
	    struct timeval timeout;
	    va_start(ap, op);
	    to_msec = va_arg(ap, int);
	    va_end(ap);

	    if (to_msec <= 0) {
		errno = EINVAL;
		return -1;
	    }
	    timeout.tv_sec = to_msec / 1000;
	    timeout.tv_usec = (to_msec - timeout.tv_sec * 1000) * 1000;
	    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
		return -1;
	    }
	    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
		return -1;
	    }
	    break;
	}
    case SO_NONBLOCK:
	{
	    int flags = 0, nonblock = 0;
	    va_start(ap, op);
	    nonblock = va_arg(ap, int);
	    va_end(ap);
	    flags = fcntl(sockfd, F_GETFL, 0);
	    if (flags == -1)
		flags = 0;
	    if (nonblock)
		flags |= O_NONBLOCK;
	    else
		flags &= ~O_NONBLOCK;
	    if ((fcntl(sockfd, F_SETFL, flags)))
		return -1;
	    break;
	}
    case SO_NODELAY:
	break;
    }

    return 0;
}



}
