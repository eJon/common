// copyright:
//            (C) SINA Inc.
//
//           file: nspio/api/sync_api.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_NSPIO_RAWAPI_
#define _H_NSPIO_RAWAPI_

#include <nspio/api.h>
#include "net/tcp.h"
#include "proto/proto.h"
#include "module_stats.h"

NSPIO_DECLARATION_START

#define SPIOTEST

typedef struct _msghdr {
    int64_t timestamp;
    uint16_t checksum;
    union {
	int64_t hid;
    } u;
} Msghdr;
#define HDRLEN sizeof(struct _msghdr)

enum {
    API_RECV_NORMALMSG = 1,
    API_RECV_ICMPMSG,
    API_RECV_PACKAGES,
    API_SEND_PACKAGES,
    API_MODULE_STATITEM_KEYRANGE,
};


class __comsumer : public Comsumer {
 public:
    __comsumer();
    ~__comsumer();

    int Fd();
    SPIOTEST int SetOption(int op, ...);
    int GetOption(int op, ...);
    int FlushCache();
    int CacheSize();
    module_stat *Stat() {
	return &api_stats;
    }
    SPIOTEST int Connect(const string &appname, const string &remoteaddr);
    SPIOTEST int Close();

    SPIOTEST int Recv(string &msg, string &rt);
    SPIOTEST int Send(const string &msg, const string &rt);
    SPIOTEST int Send(const char *data, uint32_t len, const string &rt);
    
    SPIOTEST int Recv(Msghdr *hdr, string &msg, string &rt);
    SPIOTEST int Send(const Msghdr *hdr, const string &msg, const string &rt);

    SPIOTEST int Recv(struct appmsg *raw);
    SPIOTEST int Send(const struct appmsg *raw);
    
 private:
    Conn *internconn;
    proto_parser pp;
    module_stat api_stats;
    int timeout_msec, options;
    int64_t st_timestamp;
    string _roleid, _appname, _apphost;
};



class __producer : public Producer {
 public:
    __producer();
    ~__producer();

    int Fd();
    SPIOTEST int SetOption(int op, ...);
    int GetOption(int op, ...);
    int FlushCache();
    int CacheSize();
    module_stat *Stat() {
	return &api_stats;
    }

    SPIOTEST int Connect(const string &appname, const string &remoteaddr);
    SPIOTEST int Close();

    SPIOTEST int Send(const string &msg);
    SPIOTEST int Recv(string &msg);
    SPIOTEST int Send(const char *data, uint32_t len);
    
    SPIOTEST int Send(const Msghdr *hdr, const string &msg);
    SPIOTEST int Recv(Msghdr *hdr, string &msg);

    SPIOTEST int Send(const struct appmsg *raw);
    SPIOTEST int Recv(struct appmsg *raw);


 private:
    Conn *internconn;
    proto_parser pp;
    module_stat api_stats;
    int crsid, timeout_msec, options;
    int64_t st_timestamp;
    string _roleid, _appname, _apphost;

    int get_seqid() {
	crsid++;
	return crsid;
    }
};


}
 

#endif
