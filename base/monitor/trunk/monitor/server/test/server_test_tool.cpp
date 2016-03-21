#include <monitor/server/test/server_test_tool.h>
#include <monitor/server/app_conf.h>
using namespace std;
MONITOR_BS;

ServerTestTool::ServerTestTool(){
}

ServerTestTool::~ServerTestTool() { 
}

MonitorInfoPtr ServerTestTool::GenerateMonitorInfo(std::string appid, std::string content){
    MonitorInfoPtr info(new MonitorInfo());
    info->appid = appid;
    info->dataid="dataid";
    info->ip="localhost";
    info->time = "time";
    info->content = content;
    return info;
}

MonitorInfoPtr ServerTestTool::GenerateMonitorInfo(std::string appid, std::string dataid, std::string content)
{
    MonitorInfoPtr info(new MonitorInfo());
    info->appid = appid;
    info->dataid=dataid;
    info->ip="localhost";
    info->time = "time";
    info->content = content;
    return info;
}
AppConfPtr ServerTestTool::GenerateAppconf(std::string writeDir){
    AppConfPtr appConf(new AppConf);
    appConf->maxFileSize = 100000;
    appConf->maxFileCount = 10;
    appConf->writePath = writeDir;
    
    SingleWritePattern pattern1;
    pattern1.sourceFields.push_back("a_1");
    pattern1.sourceFields.push_back("a_2");
    pattern1.sourceFields.push_back("a_3");
    pattern1.destField = "b_1";
    pattern1.calculation = "+";
    appConf->patterns.push_back(pattern1);
    
    appConf->maxFileTime = 86400;
    appConf->aggrSpanTime = 300;
    return appConf;
}
MONITOR_ES;

