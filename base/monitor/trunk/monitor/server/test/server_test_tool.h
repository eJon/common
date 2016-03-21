#ifndef MONITOR_SERVER_SERVER_TEST_TOOL_H
#define MONITOR_SERVER_SERVER_TEST_TOOL_H

#include <monitor/common.h>
#include <monitor/server/app_conf.h>
#include <monitor/common/monitor_info.h>
MONITOR_BS;

class ServerTestTool
{
public:
    ServerTestTool();
    ~ServerTestTool();
public:
    static AppConfPtr GenerateAppconf(std::string writeDir);
    static MonitorInfoPtr GenerateMonitorInfo(std::string appid, std::string content);
    static MonitorInfoPtr GenerateMonitorInfo(std::string appid, std::string dataid, std::string content);
private:
};

MONITOR_ES;

#endif //MONITOR_SERVER_TEST_TOOL_H
