#ifndef MONITOR_SERVER_MONITOR_SERVER_HANDLE_H
#define MONITOR_SERVER_MONITOR_SERVER_HANDLE_H

#include <monitor/common.h>
#include <sharelib/server/tcp_context.h>
#include <monitor/common/monitor_info.h>
#include <monitor/server/monitor_task_handler.h>
MONITOR_BS;

class MonitorServerContext : public sharelib::CTcpContext
{
public:
    MonitorServerContext();
    ~MonitorServerContext();
public:
    void SetTaskHandler(MonitorTaskHandlerPtr taskHandleIn){
        taskHandle = taskHandleIn;
    }
public:
    /*override*/void HandlePkg(std::string& pkg);
    /*override*/void Reset();
    
private:
    MonitorTaskHandlerPtr taskHandle;
};

MONITOR_ES;

#endif //MONITOR_MONITOR_SERVER_HANDLE_H
