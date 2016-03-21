#include <gtest/gtest.h>
#include <sharelib/server/tcp_client.h>
#include <sharelib/server/epoll_server.h>
#include <sharelib/server/test/fake_epoll_server.h>
#include <sharelib/test/test.h>
#include <sharelib/util/log.h>
#include <iostream>
using namespace std;
using namespace sharelib;

TEST(Server, readwrite)
{
    char* address = "localhost";
    int port = 19999;
    FakeEpollServer* server = new FakeEpollServer();
    EXPECT_TRUE( 0 == server->Init(port, ""));
    server->Run();
    TcpClient client;
    client.Conn(address, port);
    
    
    string send = "gogogo";
    
    
    uint32_t sendcount = 100;
    for(uint32_t i =0 ;i < sendcount ;i++){
        stringstream ss;
        ss << 1;
        for(uint32_t j =1 ;j <= i ;j++){
            ss << 1;
        }
        client.SendData(ss.str().c_str(), ss.str().length());
        /*
        EXPECT_TRUE(0 == client.Receive());
        size_t size ;
        
        const char* re = client.GetReceive(size);
        SHARELIB_LOG_ERROR("client receive size %d", client.GetReceiveBytes());
        EXPECT_TRUE(string(re, size) == ss.str());
        */
    }

    server->Shutdown();
    server->Stop();
    delete server;

}

