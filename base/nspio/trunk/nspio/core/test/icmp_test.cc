// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/test/icmp_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <uuid/uuid.h>
#include <gtest/gtest.h>
#include <nspio/errno.h>
#include "os/memalloc.h"
#include "proto/icmp.h"


using namespace nspio;


static int test_icmp_deliver_error() {
    struct spiort rt[3] = {};
    struct nspiomsg msg = {};
    struct nspiomsg *icmp = NULL;
    struct deliver_status_icmp *deicmp = NULL;
    
    msg.hdr.ttl = 3;
    msg.hdr.size = 3 * RTLEN;
    msg.data = (char *)&rt;
    msg.route = &rt[2];
    uuid_generate(rt[0].u.env.uuid);
    rt[0].u.env.ip[0] = 192;
    rt[0].u.env.ip[1] = 168;
    rt[0].u.env.ip[2] = 0;
    rt[0].u.env.ip[3] = 1;
    rt[0].u.env.port = 1002;

    uuid_generate(rt[1].u.env.uuid);
    rt[1].u.env.ip[0] = 192;
    rt[1].u.env.ip[1] = 168;
    rt[1].u.env.ip[2] = 0;
    rt[1].u.env.ip[3] = 2;
    rt[1].u.env.port = 1002;
    
    uuid_generate(rt[2].u.env.uuid);
    rt[2].u.env.ip[0] = 192;
    rt[2].u.env.ip[1] = 168;
    rt[2].u.env.ip[2] = 0;
    rt[2].u.env.ip[3] = 3;
    rt[2].u.env.port = 1002;

    icmp = make_deliver_status_icmp(&msg, SPIO_ENODISPATCHER);
    deicmp = (struct deliver_status_icmp *)icmp->data;
    EXPECT_EQ(deicmp->hdr.ttl, 3);
    EXPECT_EQ(deicmp->status, SPIO_ENODISPATCHER);
    EXPECT_EQ(icmp->hdr.size, sizeof(*deicmp) + sizeof(rt) * 2);
    EXPECT_TRUE(memcmp(icmp->data + sizeof(*deicmp), &rt, sizeof(rt)) == 0);
    mem_free(icmp, MSGPKGLEN(icmp));
    return 0;
}

















TEST(icmp, deliver_error) {
    test_icmp_deliver_error();
}
