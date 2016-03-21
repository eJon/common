// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/net/tcpconn.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <stdio.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <nspio/errno.h>
#include "net/ip.h"
#include "net/tcp.h"
#include "os/memalloc.h"

NSPIO_DECLARATION_START

static mem_stat_t tcpconn_mem_stats = {};

enum {
    NETCORE_PAGESIZE = 4096,
};

// Ignore laddr argument
static int dialTCP(TCPConn *tcpconn, string net, string laddr, string raddr) {

    struct sockaddr_in *sa_in;
    // enough for ipv4 address: ip[16] + port[5]
    char addrbuf[32] = {};     

    tcpconn->openmode = O_ACTIVE;
    tcpconn->network = net;
    tcpconn->remoteaddr = raddr;

    memset(&tcpconn->laddr, 0, sizeof(tcpconn->laddr));
    tcpconn->laddrlen = sizeof(tcpconn->laddr);

    memset(&tcpconn->raddr, 0, sizeof(tcpconn->raddr));
    tcpconn->raddrlen = sizeof(tcpconn->raddr);
    
    if (0 != resolve_ip_hostname(&tcpconn->raddr,
				 &tcpconn->raddrlen, raddr.c_str()))
	return -1;
    if ((tcpconn->sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	return -1;

    if (-1 == connect(tcpconn->sockfd, (struct sockaddr *)&tcpconn->raddr,
		      tcpconn->raddrlen)) {
	close(tcpconn->sockfd);
	return -1;
    }

    // get socket local address
    if (-1 == getsockname(tcpconn->sockfd, (struct sockaddr *)&tcpconn->laddr,
			  &tcpconn->laddrlen)) {
	close(tcpconn->sockfd);
	return -1;
    }
    sa_in = (struct sockaddr_in *)&tcpconn->laddr;
    inet_ntop(AF_INET, (char *)&sa_in->sin_addr, addrbuf, sizeof(addrbuf));
    snprintf(addrbuf + strlen(addrbuf),
	     sizeof(addrbuf) - strlen(addrbuf), ":%d", ntohs(sa_in->sin_port));
    tcpconn->localaddr = addrbuf;

    return 0;
}


// DialTCP connects to the remote address raddr on the network net,
// which must be "tcp", "tcp4", or "tcp6".  If laddr is not nil, it is
// used as the local address for the connection.

TCPConn *DialTCP(string net, string laddr, string raddr) {
    TCPConn *tcpconn = NULL;

    if (raddr == "" || (net != "tcp" && net != "tcp4" && net != "tcp6")) {
	errno = EINVAL;
	return NULL;
    }

    tcpconn = new(std::nothrow) TCPConn();
    if (!tcpconn) {
	errno = ENOMEM;
	return NULL;
    }

    if (0 == dialTCP(tcpconn, net, laddr, raddr))
	return tcpconn;
    delete tcpconn;
    return NULL;
}


TCPConn::TCPConn() :
    __errno(0), sockfd(-1), openmode(0), laddrlen(0), raddrlen(0),
    recvbuf(NULL), sendbuf(NULL),
    recvbuf_cap(0), recvbuf_len(0), recvbuf_idx(0),
    sendbuf_cap(0), sendbuf_len(0), sendbuf_idx(0)
{
    memset(&laddr, 0, sizeof(laddr));
    memset(&raddr, 0, sizeof(raddr));
    tcpconn_mem_stats.alloc++;
    tcpconn_mem_stats.alloc_size += sizeof(TCPConn);
}

TCPConn::~TCPConn() {
    if (sockfd != -1) {
	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);
    }
    if (recvbuf)
	mem_free(recvbuf, recvbuf_cap);
    if (sendbuf)
	mem_free(sendbuf, sendbuf_cap);
    tcpconn_mem_stats.alloc--;
    tcpconn_mem_stats.alloc_size -= sizeof(TCPConn);
}


void TCPConn::reset_recvbuf() {
    recvbuf_len = recvbuf_idx = 0;
}

void TCPConn::reset_sendbuf() {
    sendbuf_len = sendbuf_idx = 0;
}


int TCPConn::Flush() {
    int64_t nbytes = 0;

    if (sockfd < 0 || !sendbuf) {
	errno = SPIO_EINTERN;
	return -1;
    }

    while (sendbuf_len - sendbuf_idx > 0) {
	nbytes = raw_write(sendbuf + sendbuf_idx,
			   sendbuf_len - sendbuf_idx);
	if (nbytes < 0)
	    return -1;
	sendbuf_idx += nbytes;
    }
    reset_sendbuf();
    return 0;
}

int64_t TCPConn::raw_read(char *buf, int64_t len) {
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
    //  Signalise peer failure.
    if (nbytes == 0) {
	__errno = errno;
	errno = EPIPE;
	return -1;
    }

    return nbytes;
}


int64_t TCPConn::Read(char *buf, int64_t len) {
    int64_t nbytes = 0;

    if (!buf || len <= 0) {
	errno = EINVAL;
	return -1;
    }
    if (sockfd < 0) {
	errno = EBADF;
	return -1;
    }
    if (!recvbuf)
	return raw_read(buf, len);
    if (recvbuf_idx == recvbuf_len) {
	reset_recvbuf();
	if ((nbytes = raw_read(recvbuf, recvbuf_cap)) <= 0)
	    return nbytes;
	recvbuf_len = nbytes;
    }
    if (len > (recvbuf_len - recvbuf_idx))
	len = recvbuf_len - recvbuf_idx;
    memcpy(buf, recvbuf + recvbuf_idx, len);
    recvbuf_idx += len;
    return len;
}

int64_t TCPConn::ReadCache(char *buf, int64_t len) {

    if (!buf || len <= 0) {
	errno = EINVAL;
	return -1;
    }
    if (sockfd < 0) {
	errno = EBADF;
	return -1;
    }
    if (!recvbuf) {
	return 0;
    }
    if (len > (recvbuf_len - recvbuf_idx))
	len = recvbuf_len - recvbuf_idx;
    memcpy(buf, recvbuf + recvbuf_idx, len);
    recvbuf_idx += len;
    return len;
}


int64_t TCPConn::raw_write(const char *buf, int64_t len) {
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
	// Signalise peer failure.
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

int64_t TCPConn::Write(const char *buf, int64_t len) {
    int64_t nbytes = 0;

    if (!buf || len <= 0) {
	errno = EINVAL;
	return -1;
    }
    if (sockfd < 0) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (!sendbuf)
	return raw_write(buf, len);
    if (sendbuf_len == sendbuf_cap) {
	nbytes = raw_write(sendbuf + sendbuf_idx,
			   sendbuf_len - sendbuf_idx);
	if (nbytes <= 0)
	    return nbytes;
	sendbuf_idx += nbytes;
	if (sendbuf_idx != sendbuf_len)  // please testing here. thx
	    return 0;
	reset_sendbuf();
    }
    if (len > sendbuf_cap - sendbuf_len)
	len = sendbuf_cap - sendbuf_len;
    memcpy(sendbuf + sendbuf_len, buf, len);
    sendbuf_len += len;
    return len;
}



int TCPConn::Close() {
    if (sockfd != -1) {
	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);
    }
    sockfd = -1;
    reset_sendbuf();
    reset_recvbuf();
    return 0;
}

int TCPConn::Reconnect() {
    int ret = 0;

    if (sockfd >= 0) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (remoteaddr == "" || (network != "tcp" &&
			     network != "tcp4" && network != "tcp6"))
	return -1;
    ret = dialTCP(this, network, localaddr, remoteaddr);

    return ret;
}


int TCPConn::LocalAddr(string &laddr) {

    laddr = localaddr;

    return 0;
}

int TCPConn::RemoteAddr(string &raddr) {
    raddr = remoteaddr;
    return 0;
}


int TCPConn::SetSockOpt(int op, ...) {
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
	    if (setsockopt (sockfd, SOL_SOCKET,
			    SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
		return -1;
	    }
	    if (setsockopt (sockfd, SOL_SOCKET,
			    SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
		return -1;
	    }
	    break;
	}
    case SO_WRITECACHE:
	{
	    int bufcap = 0;
	    if (sendbuf && sendbuf_cap)
		break;
	    va_start(ap, op);
	    bufcap = va_arg(ap, int);
	    va_end(ap);
	    if (bufcap < 0) {
		errno = EINVAL;
		return -1;
	    }
	    if (bufcap == 0)
		bufcap = NETCORE_PAGESIZE;
	    if (!(sendbuf = (char *)mem_zalloc(bufcap))) {
		errno = ENOMEM;
		return -1;
	    }
	    sendbuf_cap = bufcap;
	    break;
	}
    case SO_READCACHE:
	{
	    int bufcap = 0;
	    if (recvbuf && recvbuf_cap)
		break;
	    va_start(ap, op);
	    bufcap = va_arg(ap, int);
	    va_end(ap);
	    if (bufcap < 0) {
		errno = EINVAL;
		return -1;
	    }
	    if (bufcap == 0)
		bufcap = NETCORE_PAGESIZE;
	    if (!(recvbuf = (char *)mem_zalloc(bufcap))) {
		errno = ENOMEM;
		return -1;
	    }
	    recvbuf_cap = bufcap;	
	    break;
	}
    case SO_NONBLOCK:
	{
	    int flags, nonblock;
	    va_start(ap, op);
	    nonblock = va_arg(ap, int);
	    va_end(ap);

	    if ((flags = fcntl(sockfd, F_GETFL, 0)) == -1)
		flags = 0;
	    if (nonblock)
		flags |= O_NONBLOCK;
	    else
		flags &= ~O_NONBLOCK;
	    if (fcntl(sockfd, F_SETFL, flags) < 0)
		return -1;
	    break;
	}
    case SO_NODELAY:
	{
	    int flags, nodelay;
	    va_start(ap, op);
	    nodelay = va_arg(ap, int);
	    va_end(ap);
	    flags = nodelay ? 1 : 0;
	    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flags, sizeof(flags)) < 0)
		return -1;
	    break;
	}
    case SO_QUICKACK:
	{
	    if (setsockopt(sockfd, IPPROTO_TCP, TCP_QUICKACK, (int[]){1}, sizeof(int)) < 0)
		return -1;
	    break;
	}
    }

    return 0;
}




}
