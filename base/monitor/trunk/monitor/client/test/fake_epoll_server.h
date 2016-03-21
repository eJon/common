#ifndef MONITOR_SERVER_FAKE_EPOLL_SERVER_H
#define MONITOR_SERVER_FAKE_EPOLL_SERVER_H

#include <monitor/common.h>
#include <sharelib/server/epoll_server.h>
#include <monitor/client/test/fake_tcp_context.h>
MONITOR_BS;

class FakeEpollServer : public sharelib::CEpollServer
{
public:
    FakeEpollServer(std::string type ="");
    ~FakeEpollServer();
public:
    sharelib::CTcpContext* CreateTcpContext(){
        FakeTcpContext* fake = new monitor::FakeTcpContext(server);
        return fake;
    }
private:
    std::string server;
};

MONITOR_ES;

#endif 
