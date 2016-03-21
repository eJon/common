#include <monitor/server/app_conf_container.h>
#include <sharelib/util/file_util.h>
#include <monitor/common/log.h>
using namespace std;
using namespace sharelib;
MONITOR_BS;

AppConfContainer::AppConfContainer(){
}

AppConfContainer::~AppConfContainer() { 
}

int64_t AppConfContainer::GetLastTime(std::string appname){
    AppConfMapIt it = appmap.find(appname);
    if(it == appmap.end())return 0;
    return (it->second).modifyTime;
}
int AppConfContainer::Init(std::string path){
    confPath = path;
    
    vector<std::string> files;
    FileUtil::ListLocalDir(confPath, files, 1);
    for(uint32_t i = 0;i < files.size();i++)
    {
        std::string absolute = confPath + "/" + files[i];
        AppConfPtr appConf = AppConf::ReadFromFile(absolute);
        if(appConf == NULL){
            MONITOR_LOG_ERROR("read error %s", absolute.c_str());
            return -1;
        }
        struct stat st;
        FileUtil::GetFileStatus(absolute, &st);
        AppConfInfo info;
        info.appconf = appConf;
        info.modifyTime= st.st_mtime;
        appmap[files[i]] = info;
    }
    return 0;
    
}

WriterManagerPtr AppConfContainer::GenerateRenewWriters(WriterManagerPtr oldWriters, AppConfContainerPtr oldConfs){
    WriterManagerPtr writeManager(new WriterManager());
    AppConfMapIt it;
    string appname;
    for(it = appmap.begin();it != appmap.end();it++){
        appname = it->first;
        AppWriterPtr oldWriter;
        if(oldConfs->GetLastTime(appname) != (it->second).modifyTime|| NULL ==  (oldWriter =oldWriters->GetWriter(appname))){
            writeManager->AddWriter(appname, (it->second).appconf);
        }else{
            writeManager->AddWriter(appname, oldWriter);
        }
    }
    return writeManager;
}
MONITOR_ES;

