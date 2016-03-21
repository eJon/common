#ifndef MONITOR_SERVER_FAKE_TCP_CONTEXT_H
#define MONITOR_SERVER_FAKE_TCP_CONTEXT_H

#include <monitor/common.h>
#include <monitor/common/log.h>
#include <sharelib/server/tcp_context.h>
MONITOR_BS;
class FakeTcpContext : public sharelib::CTcpContext
{
public:
    FakeTcpContext(std::string typeIn) {type =typeIn;}
    ~FakeTcpContext(){}
public:
    /*override*/void HandlePkg(std::string& pkg);
    /*override*/void Reset(){
        idx = 0;
    }

public:
    std::string type;
//type="cumulation"
public:
    static uint32_t cumulationCount;
//type="snapshot"
public:
    static uint32_t snapshotCount;
//type = ""
public:
    static uint32_t idx;
};

MONITOR_ES;

#endif
