// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/test/utils_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <gtest/gtest.h>
#include <nspio/errno.h>
#include "net/ip.h"

using namespace nspio;




static int test_ip_parse() {
    string ipaddr;
    uint16_t port;
    uint8_t ip[4] = {};

    ipaddr_stoa("192.168.0.1:20", ip, &port);
    EXPECT_EQ(ip[0], 192);
    EXPECT_EQ(ip[1], 168);
    EXPECT_EQ(ip[2], 0);
    EXPECT_EQ(ip[3], 1);
    EXPECT_EQ(port, 20);


    ipaddr_stoa("0.0.-1.0", ip, &port);
    EXPECT_EQ(ip[0], 0);
    EXPECT_EQ(ip[1], 0);
    EXPECT_EQ(ip[2], 255);
    EXPECT_EQ(ip[3], 0);

    ipaddr_stoa("0.0.-1.", ip, &port);
    EXPECT_EQ(ip[0], 0);
    EXPECT_EQ(ip[1], 0);
    EXPECT_EQ(ip[2], 255);

    ipaddr_stoa("0.0.-1", ip, &port);
    EXPECT_EQ(ip[0], 0);
    EXPECT_EQ(ip[1], 0);
    EXPECT_EQ(ip[2], 255);
    
    ipaddr_stoa("192.154.", ip, &port);
    EXPECT_EQ(ip[0], 192);
    EXPECT_EQ(ip[1], 154);

#define testing_ipaddr_revert_parse(_ip) do {		\
	ipaddr_stoa(_ip, ip, &port);	\
	ipaddr_atos(ip, port, ipaddr);	\
	EXPECT_TRUE(_ip == ipaddr);			\
    } while (0)

    testing_ipaddr_revert_parse("0.168.0.1:20");
    testing_ipaddr_revert_parse("129.0.0.1:20");
    testing_ipaddr_revert_parse("129.1.0.1:20");
    testing_ipaddr_revert_parse("0.0.0.0:0");
    testing_ipaddr_revert_parse("129.168.0.1:0");

    return 0;
}


TEST(api, util) {
    test_ip_parse();
}
