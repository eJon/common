// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/receiver.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_RECEIVER_H_
#define _H_RECEIVER_H_

#include <iostream>
#include "role.h"
#include "proto/proto.h"

using namespace std;


NSPIO_DECLARATION_START

class Receiver : public Role {
 public:
    Receiver(const string &_appid, const string &roleid);
    Receiver(int rt, const string &_appid, const string &roleid);
    ~Receiver();

    void Reset() {
	pp._reset_send();
	pp._reset_recv();
    }
    int Recv(struct nspiomsg **req);
    int Send(struct nspiomsg *resp);
    int BatchSend(int max_send, struct list_head *to_head = NULL);
    int send_icmp();
    
 private:
    proto_parser pp;

    int process_normal_massage(struct nspiomsg *msg);
    int process_self_icmp(struct nspiomsg *icmp_msg);
    int process_role_status_icmp(struct nspiomsg *msg);
};







}







#endif  // _H_RECEIVER_H_
