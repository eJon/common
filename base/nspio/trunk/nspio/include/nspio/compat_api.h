// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/nspio/compat_api.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_NSPIOCOMPAT_
#define _H_NSPIOCOMPAT_

#include <iostream>
#include <nspio/api.h>


using namespace std;

namespace nspio {

#define MODE_RR 1
#define MODE_ROUTE 2

class CSpioApi {
 public:
    CSpioApi();
    ~CSpioApi();

    int init(const string &groupname);
    int join_client(const string &grouphost);
    int join_server(const string &grouphost);
    int rejoin();
    int recv(string &msg, int timeout = 0);
    int send(const string &msg, int timeout = 0);
    void terminate();

#if defined(__NSPIO_UT__)
    int fd();
#endif

    Comsumer *get_comsumer();
    Producer *get_producer();
    
 private:
    bool m_joined;
    bool m_server;
    int m_msgid;
    int m_to_msec, m_to_cnt;
    string m_groupname, m_grouphost;
    string cur_rt;
    Comsumer *cr;
    Producer *pr;
};

}
















#endif   // _H_SPIOCOMPAT_
