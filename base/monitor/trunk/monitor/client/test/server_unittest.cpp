#include <gtest/gtest.h>
#include <sharelib/server/tcp_client.h>
#include <sharelib/server/epoll_server.h>
#include <monitor/client/test/fake_epoll_server.h>
#include <monitor/client/monitor_transporter.h>
#include <monitor/test/test.h>
#include <monitor/common/log.h>
#include <iostream>
#include <sstream>
using namespace std;
using namespace monitor;
using namespace sharelib;
TEST(client, BasicFunction)
{
    FakeTcpContext::idx = 0;
    string confPath = string(TEST_DATA_PATH) + "client.conf";
    string address = "localhost";
    int port = 19999;
    FakeEpollServer* server = new FakeEpollServer();
    EXPECT_TRUE( 0 == server->Init(port, ""));
    server->Run();
    
    ClientConfPtr clientConf(new ClientConf);
    IpPort ipport;
    ipport.ip = address;
    ipport.port = port;
    clientConf->ips.push_back(ipport);
    clientConf->span = 1;
    ClientConf::WriteToFile(clientConf, confPath);
    
    MonitorTransporterPtr transporter(new MonitorTransporter);
    EXPECT_TRUE(0 ==transporter->Init(confPath));
    
    
    uint32_t sendcount = 100;
    stringstream ss;
    
    for(uint32_t i =0 ;i < sendcount ;i++){
        ss << i;
        string info = ss.str();
        transporter->SendInfo(info);
    }
    while(FakeTcpContext::idx != sendcount){
        usleep(10000);
        MONITOR_LOG_DEBUG("waiting get info %d", FakeTcpContext::idx);
    }
    transporter.reset();
    
    server->Shutdown();
    server->Stop();
    delete server;
    system((string("rm -rf ") + confPath ).c_str());
}


