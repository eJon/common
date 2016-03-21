#ifndef MONITOR_SERVER_MONITOR_TASK_HANDLER_H
#define MONITOR_SERVER_MONITOR_TASK_HANDLER_H
#include <monitor/server/app_conf_manager.h>
#include <monitor/common.h>
#include <sharelib/task/task_handler.h>
#include <monitor/server/monitor_task_worker.h>
MONITOR_BS;

class MonitorTaskHandler : public sharelib::TaskHandler
{
public:
    MonitorTaskHandler();
    ~MonitorTaskHandler();
public:
    /*override*/sharelib::TaskMessage* HandleMessage(sharelib::TaskMessage* task_msg){
        return task_msg;
    }
    /*override*/int CreateWorker(sharelib::TaskWorkerPtr& task_worker_ptr){
        MonitorTaskWorker* worker = new MonitorTaskWorker();
        worker->SetAppConfManager(appconfManager);
        task_worker_ptr.reset(worker);
        return 0;
    }
public:
    void SetAppConfManager(AppConfManagerPtr appconfManagerIn){
        appconfManager = appconfManagerIn;
    }
    AppConfManagerPtr GetAppConfManager(){return appconfManager;}
private:
    AppConfManagerPtr appconfManager;
};

typedef std::tr1::shared_ptr<MonitorTaskHandler> MonitorTaskHandlerPtr;
MONITOR_ES;

#endif //MONITOR_MONITOR_TASK_HANDLER_H
