// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/net/conn.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_NET_CONN_
#define _H_NET_CONN_

#include <iostream>
#include <vector>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "decr.h"


using namespace std;
NSPIO_DECLARATION_START

enum OpenMode {
    O_ACTIVE = 0x01,
    O_PASSIVE = 0x02,
};

enum SockOpt {
    SO_TIMEOUT = 0x01,
    SO_WRITECACHE = 0x02,
    SO_READCACHE = 0x03,
    SO_NONBLOCK = 0x04,
    SO_NODELAY = 0x05,
    SO_QUICKACK = 0x06,
};


class Conn {
 public:
    virtual ~Conn() {

    }
    
    virtual int Fd() = 0;
    virtual int OpenMode() = 0;
    virtual int64_t Read(char *buf, int64_t len) = 0;
    virtual int64_t ReadCache(char *buf, int64_t len) = 0;
    virtual int64_t Write(const char *buf, int64_t len) = 0;
    virtual int64_t CacheSize(int op) = 0;
    virtual int Flush() = 0;
    virtual int Close() = 0;
    virtual int Reconnect() = 0;
    virtual int LocalAddr(string &addr) = 0;
    virtual int RemoteAddr(string &addr) = 0;
    virtual int SetSockOpt(int op, ...) = 0;
};



class Listener {
 public:
    virtual ~Listener() {

    }
    virtual int Fd() = 0;
    virtual Conn *Accept() = 0;
    virtual int Close() = 0;
    virtual int Addr(string &addr) = 0;
    virtual int SetNonBlock(bool nonblock) = 0;
};





}
#endif
