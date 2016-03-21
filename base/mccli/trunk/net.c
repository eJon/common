#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include "mccli.h"

void __mcSetError(mcContext *c, const char *fmt, ...);

static void __mcSetErrorFromErrno(mcContext *c, int type, const char *prefix)
{
	char buf[128];
	size_t len = 0;

	if (prefix != NULL)
		len = snprintf(buf,sizeof(buf), "%s: ", prefix);// len not include null

	char* p = strerror_r(errno, buf + len, sizeof(buf) - len);
	strncpy(buf+len, p, sizeof(buf) - len);
	buf[127] = '\0';
	__mcSetError(c, buf);
}

static int mcCreateSocket(mcContext *c, int type)
{
	int s, on = 1;
	if ((s = socket(type, SOCK_STREAM, 0)) == -1) {
		__mcSetErrorFromErrno(c, MCCLI_ERR_IO, NULL);
		return MCCLI_ERR;
	}
	if (type == AF_INET) {
		if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
			__mcSetErrorFromErrno(c, MCCLI_ERR_IO, NULL);
			close(s);
			return MCCLI_ERR;
		}
	}
	return s;
}

static int mcSetBlocking(mcContext *c, int fd, int blocking)
{
	int flags;

	/* Set the socket nonblocking.
	* Note that fcntl(2) for F_GETFL and F_SETFL can't be
	* interrupted by a signal. */
	if ((flags = fcntl(fd, F_GETFL)) == -1) {
		__mcSetErrorFromErrno(c, MCCLI_ERR_IO, "fcntl(F_GETFL)");
		close(fd);
		return MCCLI_ERR;
	}

	if (blocking)
		flags &= ~O_NONBLOCK;
	else
		flags |= O_NONBLOCK;

	if (fcntl(fd, F_SETFL, flags) == -1) {
		__mcSetErrorFromErrno(c, MCCLI_ERR_IO, "fcntl(F_SETFL)");
		close(fd);
		return MCCLI_ERR;
	}
	return MCCLI_OK;
}

static int mcSetTcpNoDelay(mcContext *c, int fd)
{
	int yes = 1;
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) == -1) {
		__mcSetErrorFromErrno(c, MCCLI_ERR_IO, "setsockopt(TCP_NODELAY)");
		close(fd);
		return MCCLI_ERR;
	}
	return MCCLI_OK;
}

static int mcContextWaitReady(mcContext *c, int fd, const struct timeval *timeout)
{
	struct timeval to;
	struct timeval *toptr = NULL;
	fd_set wfd;
	int err;
	socklen_t errlen;

	/* Only use timeout when not NULL. */
	if (timeout != NULL) {
		to = *timeout;
		toptr = &to;
	}

	if (errno == EINPROGRESS) {
		FD_ZERO(&wfd);
		FD_SET(fd, &wfd);

		if (select(FD_SETSIZE, NULL, &wfd, NULL, toptr) == -1) {
			__mcSetErrorFromErrno(c, MCCLI_ERR_IO, "select(2)");
			close(fd);
			return MCCLI_ERR;
		}

		if (!FD_ISSET(fd, &wfd)) {
			errno = ETIMEDOUT;
			__mcSetErrorFromErrno(c, MCCLI_ERR_IO, NULL);
			close(fd);
			return MCCLI_ERR;
		}

		err = 0;
		errlen = sizeof(err);
		if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &errlen) == -1) {
			__mcSetErrorFromErrno(c, MCCLI_ERR_IO, "getsockopt(SO_ERROR)");
			close(fd);
			return MCCLI_ERR;
		}

		if (err) {
			errno = err;
			__mcSetErrorFromErrno(c, MCCLI_ERR_IO, NULL);
			close(fd);
			return MCCLI_ERR;
		}

		return MCCLI_OK;
	}

	__mcSetErrorFromErrno(c, MCCLI_ERR_IO, NULL);
	close(fd);
	return MCCLI_ERR;
}

int mcContextSetTimeout(mcContext *c, struct timeval tv)
{
	if (setsockopt(c->fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
		__mcSetErrorFromErrno(c, MCCLI_ERR_IO, "setsockopt(SO_RCVTIMEO)");
		return MCCLI_ERR;
	}
	if (setsockopt(c->fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == -1) {
		__mcSetErrorFromErrno(c, MCCLI_ERR_IO, "setsockopt(SO_SNDTIMEO)");
		return MCCLI_ERR;
	}
	return MCCLI_OK;
}

int mcContextConnectTcp(mcContext *c, const char *addr, int port, struct timeval *timeout) 
{
	int s;
	int blocking = (c->flags & MCCLI_BLOCK);
	struct sockaddr_in sa;

	if ((s = mcCreateSocket(c, AF_INET)) < 0)
		return MCCLI_ERR;
	if (mcSetBlocking(c, s, 0) != MCCLI_OK)
		return MCCLI_ERR;

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	if (inet_aton(addr, &sa.sin_addr) == 0) {
		struct hostent *he;

		he = gethostbyname(addr);
		if (he == NULL) {
			char buf[128];
			snprintf(buf,sizeof(buf), "Can't resolve: %s", addr);
			__mcSetError(c, buf);
			close(s);
			return MCCLI_ERR;
		}
		memcpy(&sa.sin_addr, he->h_addr, sizeof(struct in_addr));
	}

	if (connect(s, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
		if (errno == EINPROGRESS && !blocking) {
		/* This is ok. */
		} else {
			if (mcContextWaitReady(c, s, timeout) != MCCLI_OK)
				return MCCLI_ERR;
		}
	}

	/* Reset socket to be blocking after connect(2). */
	if (blocking && mcSetBlocking(c, s, 1) != MCCLI_OK)
		return MCCLI_ERR;

	if (mcSetTcpNoDelay(c, s) != MCCLI_OK)
		return MCCLI_ERR;

	c->fd = s;
	c->flags |= MCCLI_CONNECTED;
	c->err = 0;
	return MCCLI_OK;
}

int mcContextConnectUnix(mcContext *c, const char *path, struct timeval *timeout)
{
	int s;
	int blocking = (c->flags & MCCLI_BLOCK);
	struct sockaddr_un sa;

	if ((s = mcCreateSocket(c, AF_LOCAL)) < 0)
		return MCCLI_ERR;
	if (mcSetBlocking(c, s, 0) != MCCLI_OK)
		return MCCLI_ERR;

	sa.sun_family = AF_LOCAL;
	strncpy(sa.sun_path, path, sizeof(sa.sun_path) - 1);
	if (connect(s, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
		if (errno == EINPROGRESS && !blocking) {
			/* This is ok. */
		} else {
			if (mcContextWaitReady(c, s, timeout) != MCCLI_OK)
			return MCCLI_ERR;
		}
	}

	/* Reset socket to be blocking after connect(2). */
	if (blocking && mcSetBlocking(c, s, 1) != MCCLI_OK)
		return MCCLI_ERR;

	c->fd = s;
	c->flags |= MCCLI_CONNECTED;
	return MCCLI_OK;
}

