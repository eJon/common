// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/net/tcp.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_NET_TCP_
#define _H_NET_TCP_

#include "net/conn.h"

using namespace std;
NSPIO_DECLARATION_START


class TCPConn: public Conn {
 public:
    TCPConn();
    ~TCPConn();

    inline int Fd() {
	return sockfd;
    }
    inline int OpenMode() {
	return openmode;
    }
    int64_t Read(char *buf, int64_t len);
    int64_t ReadCache(char *buf, int64_t len);
    int64_t Write(const char *buf, int64_t len);
    inline int64_t CacheSize(int op) {
	switch (op) {
	case SO_READCACHE:
	    return recvbuf_len - recvbuf_idx;
	case SO_WRITECACHE:
	    return sendbuf_len - sendbuf_idx;
	}
	return -1;
    }
    int Flush();
    int Close();
    int Reconnect();
    int LocalAddr(string &laddr);
    int RemoteAddr(string &raddr);
    int SetSockOpt(int op, ...);
    
    // underlying socket infomation
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

    void reset_recvbuf();
    void reset_sendbuf();

    // Disable copy construction of TCPConn
    TCPConn (const TCPConn&);
    const TCPConn &operator = (const TCPConn&);
};

TCPConn *DialTCP(string net, string laddr, string raddr);






class TCPListener: public Listener {
 public:
    TCPListener();
    ~TCPListener();

    int Fd() {
	return sockfd;
    }
    TCPConn *Accept();
    int Close();
    int Addr(string &addr) {
	addr = localaddr;
	return 0;
    };
    int SetNonBlock(bool nonblock);
    int SetReuseAddr(bool reuse);

    int sockfd;
    int backlog;
    string localaddr;
    struct sockaddr_storage addr;
    socklen_t addrlen;

 private:

    // Disable copy construction of TCPConn
    TCPListener (const TCPListener&);
    const TCPListener &operator = (const TCPListener&);
};

TCPListener *ListenTCP(string net, string laddr, int backlog);



}
#endif
