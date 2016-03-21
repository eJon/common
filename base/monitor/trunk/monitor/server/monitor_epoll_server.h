#ifndef MONITOR_SERVER_MONITOR_EPOLL_SERVER_H
#define MONITOR_SERVER_MONITOR_EPOLL_SERVER_H

#include <monitor/common.h>
#include <monitor/server/monitor_task_handler.h>
#include <sharelib/server/epoll_server.h>
#include <monitor/server/monitor_server_context.h>
#include <monitor/server/app_conf_manager.h>

MONITOR_BS;

class MonitorEpollServer : public sharelib::CEpollServer
{
public:
    MonitorEpollServer();
    ~MonitorEpollServer();
public:

    void SetTaskHandler(MonitorTaskHandlerPtr taskHandleIn){
        taskHandle = taskHandleIn;
    }
    
public:
    sharelib::CTcpContext* CreateTcpContext(){
        MonitorServerContext* context = new MonitorServerContext();
        context->SetTaskHandler(taskHandle);
        return context;
    }
private:
    MonitorTaskHandlerPtr taskHandle;
};

MONITOR_ES;

#endif //MONITOR_MONITOR_EPOLL_SERVER_H
