// copyright:
//            (C) SINA Inc.
//
//           file: nspio/api/ed.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <string.h>
#include <sstream>
#include "os/time.h"
#include "proto/icmp.h"
#include "ed.h"


NSPIO_DECLARATION_START


deicmp_ed::deicmp_ed() :
    msg(NULL)
{

}


deicmp_ed::~deicmp_ed() {

}

int deicmp_ed::parsefrom(struct appmsg *__msg) {
    msg = __msg;
    return 0;
}

string deicmp_ed::Str() {
    stringstream ss;
    int i = 0;
    struct spiort *rt = NULL;
    struct deliver_status_icmp *deicmp = NULL;

    deicmp = (struct deliver_status_icmp *)msg->s.data;
    ss << "{seqid:" << deicmp->hdr.seqid << " ";
    ss << "ttl:" << (int)deicmp->hdr.ttl << " ";
    ss << "errno:" << deicmp->status << " ";
    ss << "msg_time:" << rt_mstime() - deicmp->hdr.timestamp << "ms ";
    ss << "icmp_time:" << rt_mstime() - msg->hdr.timestamp << "ms";
    for (i = 0; i < deicmp->hdr.ttl; i++) {
	rt = &deicmp->rts[i];
	ss << " [go:" << rt->u.env.go << "ms";
	ss << " to " << (int)rt->u.env.ip[0] << "." << (int)rt->u.env.ip[1];
	ss << "." << (int)rt->u.env.ip[2] << "." << (int)rt->u.env.ip[3];
	ss << ":" << (int)rt->u.env.port << " cost:" << (int)rt->u.env.gocost << "ms";
	ss << " stay:" << (int)rt->u.env.gostay << "ms]";
    }
    ss << "}";
    return ss.str();
}



int simple_timeout_ed::parsefrom(int64_t _hid, int64_t _stime) {
    hid = _hid;
    stime = _stime;
    return 0;
}

string simple_timeout_ed::Str() {
    stringstream ss;

    ss << "{seqid:" << hid << " is timeout of " << rt_mstime() - stime << "ms}";
    return ss.str();
}





}
