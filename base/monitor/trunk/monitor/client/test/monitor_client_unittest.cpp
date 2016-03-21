#include <gtest/gtest.h>
#include <sharelib/server/tcp_client.h>
#include <sharelib/util/time_utility.h>
#include <sharelib/server/epoll_server.h>
#include <monitor/client/test/fake_epoll_server.h>
#include <monitor/client/monitor_client.h>
#include <monitor/test/test.h>
#include <monitor/client/test/client_data_tool.h>
#include <monitor/common/log.h>
#include <iostream>
#include <sstream>
using namespace std;
using namespace monitor;
using namespace sharelib;
TEST(MonitorClient, cumulation){
    FakeTcpContext::cumulationCount = 0;
    string confPath = string(TEST_DATA_PATH) + "monitore_client2.conf";
    string address = "localhost";
    int port = 40001;
    
    
    FakeEpollServer* server = new FakeEpollServer("cumulation");
    EXPECT_TRUE( 0 == server->Init(port, ""));
    server->Run();
    
    ClientConfPtr clientConf(new ClientConf);
    IpPort ipport;
    ipport.ip = address;
    ipport.port = port;
    clientConf->ips.push_back(ipport);
    clientConf->span = 3;
    clientConf->appid = "appid";
    clientConf->eth="eth0";
    ClientConf::WriteToFile(clientConf, confPath);
    
    MonitorClientPtr monitorclient(new MonitorClient);
    EXPECT_TRUE(0 ==monitorclient->Init(confPath));
 

        
    uint32_t sendcount = 10;
    for(uint32_t i =0 ;i < sendcount ;i++){
        string key = "key";
        int64_t value = i;
        monitorclient->Add(key, value);
        MONITOR_LOG_DEBUG("send %d recv %d", i+1, FakeTcpContext::snapshotCount);
    }
    for(uint32_t i =0 ;i < sendcount ;i++){
        string key = "key1";
        int64_t value = i;
        monitorclient->Add(key, value);
        MONITOR_LOG_DEBUG("send %d recv %d", i+1, FakeTcpContext::snapshotCount);
    }
    sleep(3);
    for(uint32_t i =1 ;i < sendcount+1 ;i++){
        string key = "key";
        int64_t value = i;
        monitorclient->Add(key, value);
        MONITOR_LOG_DEBUG("send %d recv %d", i+1, FakeTcpContext::snapshotCount);
    }

    for(uint32_t i =1 ;i < sendcount+1 ;i++){
        string key = "key1";
        int64_t value = i;
        monitorclient->Add(key, value);
        MONITOR_LOG_DEBUG("send %d recv %d", i+1, FakeTcpContext::snapshotCount);
    }
    
    while(FakeTcpContext::cumulationCount < 2){
        usleep(10000);
        MONITOR_LOG_DEBUG("cumulation waiting get info %d", FakeTcpContext::cumulationCount);
    }
    
    server->Shutdown();
    server->Stop();
    delete server;
    system((string("rm -rf ") + confPath ).c_str());
    
}
TEST(MonitorClient, snapshot)
{
    FakeTcpContext::snapshotCount = 0;
    string confPath = string(TEST_DATA_PATH) + "monitore_client1.conf";
    string address = "localhost";
    int port = 30001;
    
    
    FakeEpollServer* server = new FakeEpollServer("snapshot");
    EXPECT_TRUE( 0 == server->Init(port, ""));
    server->Run();
    
    ClientConfPtr clientConf(new ClientConf);
    IpPort ipport;
    ipport.ip = address;
    ipport.port = port;
    clientConf->ips.push_back(ipport);
    clientConf->span = 1;
    ClientConf::WriteToFile(clientConf, confPath);
    
    MonitorClientPtr monitorclient(new MonitorClient);
    EXPECT_TRUE(0 ==monitorclient->Init(confPath));
 

        
    uint32_t sendcount = 100;    
    for(uint32_t i =0 ;i < sendcount ;i++){
        string info = "info";
        string dataid = "i";
        monitorclient->SendSnapshot(info);
        MONITOR_LOG_DEBUG("send %d recv %d", i+1, FakeTcpContext::snapshotCount);
    }
    
    while(FakeTcpContext::snapshotCount < sendcount){
        usleep(10000);
        MONITOR_LOG_DEBUG("snapshot waiting get info %d", FakeTcpContext::snapshotCount);
    }
    MONITOR_LOG_DEBUG("has get info %d", FakeTcpContext::snapshotCount);
    
    server->Shutdown();
    server->Stop();
    delete server;
    system((string("rm -rf ") + confPath ).c_str());
    
}


