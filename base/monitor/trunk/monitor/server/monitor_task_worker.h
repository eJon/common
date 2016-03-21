#ifndef MONITOR_SERVER_MONITOR_TASK_WORKER_H
#define MONITOR_SERVER_MONITOR_TASK_WORKER_H

#include <monitor/common.h>
#include <sharelib/task/task_worker.h>
#include <monitor/server/app_conf_manager.h>
MONITOR_BS;

class MonitorTaskWorker : public sharelib::TaskWorker
{
public:
    MonitorTaskWorker();
    virtual ~MonitorTaskWorker();
public:
    /*override*/int HandleOne(sharelib::TaskMessage* task_message_ptr);
public:
    void SetAppConfManager(AppConfManagerPtr appconfManagerIn){
        appconfManager = appconfManagerIn;
    }
private:
    AppConfManagerPtr appconfManager;
};

MONITOR_ES;

#endif //MONITOR_MONITOR_TASK_WORKER_H
