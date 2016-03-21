#include <monitor/client/test/fake_epoll_server.h>
using namespace std;
MONITOR_BS;

FakeEpollServer::FakeEpollServer(std::string type){
    server = type;
}

FakeEpollServer::~FakeEpollServer() { 
}

MONITOR_ES;

