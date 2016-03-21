// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/proto/route.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_ROUTEID_
#define _H_ROUTEID_

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <uuid/uuid.h>
#include "proto/proto.h"
#include "net/ip.h"

using namespace std;

NSPIO_DECLARATION_START


#define UUID_LEN ((int)(sizeof(uuid_t)))
#define UUID_STRLEN ((int)(sizeof(uuid_t) * 2 + 1 + 4))


static inline void route_genid(string &rid) {
    uuid_t uu;
    char __uu[UUID_STRLEN] = {};
    uuid_generate(uu);
    uuid_unparse(uu, __uu);
    rid.assign(__uu);
}

static inline string route_cid(const string &rid) {
    char out[UUID_STRLEN] = {};
    uuid_unparse((const unsigned char *)rid.data(), out);
    return out;
}


static inline void route_getid(const struct spiort *rt, string &rid) {
    string __uu;
    __uu.assign((const char *)rt->u.env.uuid, sizeof(uuid_t));
    rid = route_cid(__uu);
}

static inline void route_getip(const struct spiort *rt, string &rip) {
    stringstream ss;
    ss << rt->u.env.ip[0] << "." << rt->u.env.ip[1] << ".";
    ss << rt->u.env.ip[2] << "." << rt->u.env.ip[3] << ":";
    ss << rt->u.env.port;
    rip = ss.str();
}

static inline void route_setid(struct spiort *rt, const string &rid) {
    uuid_parse(rid.data(), rt->u.env.uuid);
}

static inline void route_setip(struct spiort *rt, const string &rip) {
    ipaddr_stoa(rip.data(), rt->u.env.ip, &rt->u.env.port);
}

static inline void rgh_getid(const struct spioreg *rgh, string &rid) {
    string __uu;
    __uu.assign((const char *)rgh->rid, sizeof(uuid_t));
    rid = route_cid(__uu);
}

static inline void rgh_setid(struct spioreg *rgh, const string &rid) {
    uuid_parse(rid.data(), rgh->rid);
}


}


#endif   // _H_ROUTEID_
