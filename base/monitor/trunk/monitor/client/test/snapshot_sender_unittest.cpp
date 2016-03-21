#include <gtest/gtest.h>
#include <sharelib/server/tcp_client.h>
#include <sharelib/util/time_utility.h>
#include <sharelib/server/epoll_server.h>
#include <monitor/client/test/fake_epoll_server.h>
#include <monitor/client/monitor_transporter.h>
#include <monitor/client/snapshot_sender.h>
#include <monitor/test/test.h>
#include <monitor/client/test/client_data_tool.h>
#include <monitor/common/log.h>
#include <iostream>
#include <sstream>
using namespace std;
using namespace monitor;
using namespace sharelib;
TEST(snapshotsender, BasicFunction)
{
    FakeTcpContext::snapshotCount = 0;
    string confPath = string(TEST_DATA_PATH) + "snapshot_client.conf";
    string address = "localhost";
    int port = 20001;
    
    
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
    
    MonitorTransporterPtr transporter(new MonitorTransporter);
    EXPECT_TRUE(0 ==transporter->Init(confPath));
 

    SnapshotSenderPtr snapSender(new SnapshotSender());
    snapSender->SetTransporter(transporter);
    snapSender->Init();
    
    
    uint32_t sendcount = 100;
    string info = "info";
    string appid = "appid";
    string dataid = "dataid";
    string ip = "ip";
    for(uint32_t i =0 ;i < sendcount ;i++){
        pthread_t threadid =1 + i;
        string  time = TimeUtility::CurrentTimeInSecondsReadable();
        snapSender->Send(info, appid, dataid, ip, threadid, time);
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


