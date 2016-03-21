#include <monitor/server/monitor_server_context.h>
#include <monitor/server/server_task_message.h>
using namespace std;
MONITOR_BS;

MonitorServerContext::MonitorServerContext(){
}

MonitorServerContext::~MonitorServerContext() { 
}

void MonitorServerContext::HandlePkg(std::string& pkg)
{
    ServerTaskMessage* message= new ServerTaskMessage(pkg);
    taskHandle->Handle(message);
}

void MonitorServerContext::Reset(){
}

    
MONITOR_ES;

