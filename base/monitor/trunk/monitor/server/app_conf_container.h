#ifndef MONITOR_SERVER_APP_CONF_CONTAINER_H
#define MONITOR_SERVER_APP_CONF_CONTAINER_H

#include <monitor/common.h>
#include <monitor/server/app_conf.h>
#include <tr1/unordered_map>
#include <monitor/server/writer_manager.h>
MONITOR_BS;

struct AppConfInfo {
    AppConfPtr appconf;
    int64_t modifyTime;
};

class AppConfContainer;
typedef std::tr1::shared_ptr<AppConfContainer> AppConfContainerPtr;

class AppConfContainer
{
public:
    typedef std::tr1::unordered_map<std::string, AppConfInfo> AppConfMap;
    typedef std::tr1::unordered_map<std::string, AppConfInfo>::iterator AppConfMapIt;
public:
    AppConfContainer();
    ~AppConfContainer();
public:
    int Init(std::string path);
    WriterManagerPtr GenerateRenewWriters(WriterManagerPtr oldWriters, AppConfContainerPtr oldConfs);
    int64_t GetLastTime(std::string appname);
private:
    AppConfMap appmap;
    std::string confPath;
};


MONITOR_ES;

#endif //MONITOR_APP_CONF_CONTAINER_H
