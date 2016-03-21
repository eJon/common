#include <monitor/server/bin/monitor_server_conf.h>
#include <monitor/common/log.h>
#include <sharelib/util/file_util.h>
using namespace std;
using namespace sharelib;
MONITOR_BS;

MonitorServerConf::MonitorServerConf(){
}

MonitorServerConf::~MonitorServerConf() { 
}

MonitorServerConfPtr MonitorServerConf::ReadFromFile(std::string file)
{
    MonitorServerConf* conf = NULL;
    if(!FileUtil::IsFileExist(file)){
        MONITOR_LOG_ERROR("file %s not exist ", file.c_str());
        return MonitorServerConfPtr();
    }
    string content = FileUtil::ReadLocalFile(file);
    try{
        FromJsonString(conf, content);
    }catch(exception& e){
        if(conf != NULL){
            delete conf;
            conf = NULL;
            return MonitorServerConfPtr();
        }
    }
    
    return MonitorServerConfPtr(conf);
    
}


bool MonitorServerConf::WriteToFile(MonitorServerConfPtr conf, std::string file){
    string str = ToJsonString(conf.get());
    if(!FileUtil::WriteLocalFile(file, str)){
        MONITOR_LOG_ERROR("write file %s fail ", file.c_str());
        return false;
    }
    return true;
}
MONITOR_ES;

