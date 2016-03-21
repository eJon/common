#ifndef MONITOR_SERVER_APP_CONF_MANAGER_H
#define MONITOR_SERVER_APP_CONF_MANAGER_H

#include <monitor/common.h>
#include <monitor/common/monitor_info.h>
#include <monitor/server/app_conf_container.h>
#include <sharelib/util/thread.h>
#include <sharelib/util/lock.h>
MONITOR_BS;

class AppConfManager : public sharelib::Thread
{
public:
    AppConfManager();
    ~AppConfManager();
public:
    int Init(std::string path);
    /*override*/ret_t Run();
    
    
    void SetReloadSpan(int64_t span){
        reloadSpanMs = span;
    }
    int Write(MonitorInfoPtr& monitorInfo);
public:
    AppConfContainerPtr  GetAppConfContainer();
    uint32_t GetReloadCount(){return reloadCount;}
private:
    std::string confdir;
    bool started;
private:
    AppConfContainerPtr container;
    WriterManagerPtr writers;

    sharelib::RWLock lock;
    int64_t lastModify;
    int64_t reloadSpanMs;
    
    volatile uint32_t reloadCount;
};

typedef std::tr1::shared_ptr<AppConfManager> AppConfManagerPtr;
MONITOR_ES;

#endif //MONITOR_APP_CONF_MANAGER_H
