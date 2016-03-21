#ifndef SHARELIB_PLUGINMANANGER_PLUGIN_MANAGER_WRAPPER_H_
#define SHARELIB_PLUGINMANANGER_PLUGIN_MANAGER_WRAPPER_H_

#include <map>
#include <string>
#include <tr1/memory>
#include "sharelib/pluginmanager/plugin_manager.h"
#include <sharelib/util/file_util.h>
#include <sharelib/util/thread.h>
namespace sharelib {

class PluginManagerChecker: public Thread{
    
public:
    PluginManagerChecker(){}
    int Init(const std::string& configHomeIn){
        configHome = configHomeIn;
        PluginManagerPtr manager(new PluginManager());
        if(0 == manager->Init(configHome.c_str())){
            pluginManager = manager;
        }else{
            return -1;
        }
        lastModifyTime = GetConfModifyTime();
        
        return 0;
    }
    ~PluginManagerChecker(){}
    PluginManagerPtr GetPluginManager(){
        return pluginManager;
    }
    
    bool IsStarted(){ return started;}
private:
    time_t GetConfModifyTime(){
        struct stat s;
        FileUtil::GetFileStatus(configHome + "/" + PluginManager::kPluginManagerConfFile, &s);
        return s.st_mtime;
    }
public:
    /*override*/ret_t Run(){
        started =true;
        time_t modify;
        while(_stat != thr_terminated){
            usleep(1000);
            modify = GetConfModifyTime();
            if(modify != lastModifyTime){
                PluginManagerPtr manager(new PluginManager());
                if(0 == manager->Init(configHome.c_str())){
                    //success
                    pluginManager = manager;
                }else{
                    //log error
                }
                lastModifyTime = modify;
            }
        }
        return r_succeed;
    }
private:
    bool started;
    std::string configHome;
    time_t lastModifyTime;
    PluginManagerPtr pluginManager;
};

class PluginManagerWrapper{
public:
    PluginManagerWrapper(): checker(NULL){}
    ~PluginManagerWrapper(){
        if(checker != NULL){
            checker->Terminate();
            checker->Join();
            delete checker;
            checker =  NULL;
        }
    }
public:
    int Init(const std::string& config_home){
        checker =new PluginManagerChecker();
        int ret = checker->Init(config_home);
        if (0 !=ret){
            delete checker;
            return ret;
        }
        checker->Start();
        while(!checker->IsStarted()){
            usleep(1000);
        }
        return 0;
    }
    PluginManagerPtr GetPluginManager(){
        return checker->GetPluginManager();
    }
private:
    PluginManagerChecker* checker;
};
typedef std::tr1::shared_ptr<PluginManagerWrapper> PluginManagerWrapperPtr;

}

#endif // end SHARELIB_PLUGINMANANGER_PLUGIN_MANAGER_WRAPPER_H_

