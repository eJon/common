// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/net/unix.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_NET_UNIX_
#define _H_NET_UNIX_

#include "net/conn.h"

using namespace std;
NSPIO_DECLARATION_START


class UnixConn: public Conn {
 public:
    UnixConn();
    ~UnixConn();

    inline int Fd() {
	return sockfd;
    }
    inline int OpenMode() {
	return openmode;
    }
    int64_t Read(char *buf, int64_t len);
    int64_t ReadCache(char *buf, int64_t len) {
	return 0;
    }
    int64_t Write(const char *buf, int64_t len);
    int64_t CacheSize(int op) {
	return 0;
    }
    int Flush() {
	return 0;
    }

    int Close();
    int Reconnect();
    int LocalAddr(string &laddr);
    int RemoteAddr(string &raddr);
    int SetSockOpt(int op, ...);
    
    int __errno, sockfd;
    int openmode;
    sockaddr_storage laddr, raddr;
    socklen_t laddrlen, raddrlen;
    string network, localaddr, remoteaddr;

 private:
    char *recvbuf, *sendbuf;
    int recvbuf_cap, recvbuf_len, recvbuf_idx;
    int sendbuf_cap, sendbuf_len, sendbuf_idx;

    int64_t raw_read(char *buf, int64_t len);
    int64_t raw_write(const char *buf, int64_t len);

    // Disable copy construction of UnixConn
    UnixConn (const UnixConn&);
    const UnixConn &operator = (const UnixConn&);
};

UnixConn *DialUnix(string net, string laddr, string raddr);





class UnixListener: public Listener {
 public:
    UnixListener();
    ~UnixListener();

    int Fd() {
	return sockfd;
    }
    UnixConn *Accept();
    int Close();
    int Addr(string &addr) {
	addr = localaddr;
	return 0;
    }
    int SetNonBlock(bool nonblock);
    
    int sockfd;
    int backlog;
    string localaddr;
    struct sockaddr_storage addr;
    socklen_t addrlen;
    
 private:
    // Disable copy construction of TCPConn
    UnixListener (const UnixListener&);
    const UnixListener &operator = (const UnixListener&);
};

UnixListener *ListenUnix(string net, string laddr, int backlog);


}
#endif
