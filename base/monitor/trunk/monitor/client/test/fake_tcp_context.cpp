#include <iostream>
#include <assert.h>
#include <gtest/gtest.h>
#include <monitor/client/test/fake_tcp_context.h>
#include <sstream>
using namespace std;
MONITOR_BS;

uint32_t FakeTcpContext::idx = 0;
uint32_t FakeTcpContext::snapshotCount = 0;
uint32_t FakeTcpContext::cumulationCount = 0;
void FakeTcpContext::HandlePkg(std::string& pkg)
{
    MONITOR_LOG_DEBUG("tcp server recive pkg length %d", pkg.length());
    stringstream ss;
    ss << pkg;
    if(type == ""){
        //uint32_t now;
        //ss >> now;
        //EXPECT_TRUE(idx == now);
        idx ++;
    }else if(type == "snapshot"){
        MONITOR_LOG_DEBUG( "snapshot recv %d %s" ,snapshotCount , ss.str().c_str() );
        snapshotCount++;
    }else if(type == "cumulation"){
        MONITOR_LOG_DEBUG( "cumulation recv %d %s" ,cumulationCount , ss.str().c_str() );
        cumulationCount++;
    }
    
}
MONITOR_ES;

