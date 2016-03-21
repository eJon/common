// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/dispatcher.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_DISPATCHER_H_
#define _H_DISPATCHER_H_

#include "role.h"
#include "proto/proto.h"

using namespace std;
NSPIO_DECLARATION_START

class MQueue;
class Dispatcher : public Role {
 public:
    Dispatcher(const string &_appid, const string &roleid);
    Dispatcher(int rt, const string &_appid, const string &roleid);
    ~Dispatcher();

    void Reset() {
	pp._reset_recv();
	pp._reset_send();
    }
    int Recv(struct nspiomsg **resp);
    int Send(struct nspiomsg *req);
    int BatchSend(int max_send, struct list_head *to_head = NULL);
    int send_icmp();
    
 private:
    proto_parser pp;

    int process_normal_massage(struct nspiomsg *msg);
    int process_self_icmp(struct nspiomsg *icmp_msg);
    int process_role_status_icmp(struct nspiomsg *msg);
};















}


#endif // _H_DISPATCHER_H_
