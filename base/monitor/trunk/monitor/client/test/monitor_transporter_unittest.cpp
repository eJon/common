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
TEST(MonitorTransporter, reload)
{
    string confPath = string(TEST_DATA_PATH) + "client_monitor_transporter.conf";
    string address = "localhost";
    MonitorTransporterPtr transporter;
    {
        FakeTcpContext::idx = 0;
                
        int port = 20000;
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
    
        transporter.reset(new MonitorTransporter);
        EXPECT_TRUE(0 ==transporter->Init(confPath));
    
    
        uint32_t sendcount = 100;
        for(uint32_t i =0 ;i < sendcount ;i++){
            stringstream ss;
            ss << i;
            string info = ss.str();
            transporter->SendInfo(info);
        }
        while(FakeTcpContext::idx != sendcount){
            usleep(10000);
            MONITOR_LOG_ERROR("waiting get info %d", FakeTcpContext::idx);
        }
        
    
        server->Shutdown();
        server->Stop();
        delete server;
        system((string("rm -rf ") + confPath ).c_str());
    }
    {
        sleep(1);
        FakeTcpContext::idx = 0;
        transporter->SetReloadSpan(10);
        int port = 20001;
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
        
        while(transporter->GetReloadCount() !=1){
            MONITOR_LOG_INFO("waiting monitor reload %d",transporter->GetReloadCount());
            sleep(1);
        }
        

        uint32_t sendcount = 100;
        for(uint32_t i =0 ;i < sendcount ;i++){
            stringstream ss;
            ss << i;
            string info = ss.str();
            transporter->SendInfo(info);
        }
        while(FakeTcpContext::idx != sendcount){
            usleep(10000);
            MONITOR_LOG_ERROR("waiting get info %d", FakeTcpContext::idx);
        }
        
    
        server->Shutdown();
        server->Stop();
        delete server;
        system((string("rm -rf ") + confPath ).c_str());
    }
    //transporter.reset();
}


