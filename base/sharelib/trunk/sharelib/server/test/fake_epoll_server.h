#ifndef SHARELIB_SERVER_FAKE_EPOLL_SERVER_H
#define SHARELIB_SERVER_FAKE_EPOLL_SERVER_H

#include <sharelib/common.h>
#include <sharelib/server/epoll_server.h>
#include <sharelib/server/test/fake_tcp_context.h>
#include <sharelib/server/test/fake_obj.h>
SHARELIB_BS;

class FakeEpollServer : public sharelib::CEpollServer
{
public:
    FakeEpollServer();
    ~FakeEpollServer();
public:
    sharelib::CTcpContext* CreateTcpContext(){
        FakeObj* obj = new FakeObj;
        sharelib::FakeTcpContext* fake = new sharelib::FakeTcpContext();
        fake->SetObj(obj);
        return fake;
    }
private:
};

SHARELIB_ES;

#endif //SHARELIB_FAKE_EPOLL_SERVER_H
