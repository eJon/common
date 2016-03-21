#include <monitor/server/app_conf.h>
#include <monitor/common/log.h>
#include <sharelib/util/file_util.h>
using namespace std;
using namespace sharelib;
MONITOR_BS;

AppConf::AppConf(){
}

AppConf::~AppConf() { 
}

AppConfPtr AppConf::ReadFromFile(std::string& file){
    
    if(!FileUtil::IsFileExist(file)){
        MONITOR_LOG_ERROR("file %s not exist ", file.c_str());
        return AppConfPtr();
    }
    string content = FileUtil::ReadLocalFile(file);
    AppConf* conf = NULL;
    try{
        FromJsonString(conf, content);
    }catch(exception& e){
        if(conf != NULL){
            delete conf;
            conf = NULL;
            return AppConfPtr();
        }
    }
    
    return AppConfPtr(conf);
    
}
bool AppConf::WriteToFile(AppConfPtr conf, std::string file){
    string str = ToJsonString(conf.get());
    if(!FileUtil::WriteLocalFile(file, str)){
        MONITOR_LOG_ERROR("write file %s fail ", file.c_str());
        return false;
    }
    return true;
}
MONITOR_ES;

