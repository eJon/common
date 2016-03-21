#include <monitor/server/writer_manager.h>
#include <monitor/server/single_app_writer.h>
#include <monitor/server/mixed_app_writer.h>
#include <monitor/common/log.h>
using namespace std;
MONITOR_BS;

WriterManager::WriterManager(){
}

WriterManager::~WriterManager() { 
}

AppWriter* WriterManager::GenerateAppWriter(){
        return new MixedAppWriter();
}
AppWriterPtr WriterManager::GetWriter(std::string appname){
    AppWriterMapIt it = writerMap.find(appname);
    if(it == writerMap.end()){
        return AppWriterPtr();
    }
    return it->second;
    
}

void WriterManager::AddWriter(std::string appname, AppWriterPtr writer){
    writerMap[appname] = writer;
}
void WriterManager::AddWriter(std::string appname, AppConfPtr appConf){
    AppWriter* writer = GenerateAppWriter();
    if(writer->Init(appConf, appname) != 0){
        MONITOR_LOG_ERROR("init single app writer error %s", appname.c_str());
        delete writer;
        return;
    }
    writerMap[appname] = AppWriterPtr(writer);
    return ;
    
}
MONITOR_ES;

