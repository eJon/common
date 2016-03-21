#include <sharelib/server/test/fake_tcp_context.h>
#include <iostream>
#include <assert.h>
#include <sharelib/util/sharelib_log.h>
#include <gtest/gtest.h>
using namespace std;
SHARELIB_BS;


void FakeTcpContext::HandlePkg(std::string& pkg)
{
    //usleep(100000);
    SHARELIB_LOG_DEBUG("tcp server recive pkg length %d", pkg.length());
    EXPECT_TRUE(idx == pkg.length());
    //assert(0 == CTcpContext::SendMessage(sock_, pkg.c_str(), pkg.length()));
    idx ++;
    
}
SHARELIB_ES;

