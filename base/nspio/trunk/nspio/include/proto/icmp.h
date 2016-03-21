// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/proto/icmp.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_SPIOICMP_
#define _H_SPIOICMP_

#include "proto/proto.h"

NSPIO_DECLARATION_START


struct role_status_icmp {
    int dispatchers;
};

struct deliver_status_icmp {
    int status;
    struct spiohdr hdr;
    struct spiort rts[0];
};

struct nspiomsg *make_role_status_icmp(int dispatchers);
struct appmsg *make_deliver_status_icmp(struct appmsg *msg, int status);
struct nspiomsg *make_deliver_status_icmp(struct nspiomsg *msg, int status);


}
#endif   // _H_SPIOICMP_
