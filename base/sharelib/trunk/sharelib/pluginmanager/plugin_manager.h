#ifndef SHARELIB_PLUGINMANANGER_PLUGIN_MANAGER_H_
#define SHARELIB_PLUGINMANANGER_PLUGIN_MANAGER_H_
#include <dlfcn.h>
#include <map>
#include <string>
#include <tr1/memory>
#include "plugin_manager.conf.pb.h"
#include "iplugin.h"
#include <tr1/memory>
namespace sharelib {
enum ErrorCode{
    RET_OK = 0,
    ERROR_FILE_NULL=5000,
    ERROR_FILE_OPEN,
    ERROR_PARSE_CONFIG,
    ERROR_INIT_PLUGIN,
    ERROR_LOAD_PLUGIN,
    ERROR_DLSYM,
    ERROR_CREATE_INSTANCE,
    ERROR_DLOPEN
};

#define CREATE_INSTANCE_FUNC "create_instance"

typedef std::map<std::string, PluginConf> PluginConfMap;

struct PluginInfo {
    PluginConf plugin_conf;
    IPlugin* plugin_ptr;
    STR_MAP conf_map;
    void* so_handler;
    PluginInfo() {
      so_handler = NULL;
      plugin_ptr = NULL;
    }
    ~PluginInfo() {
      if (NULL != plugin_ptr) {
        delete plugin_ptr;
        plugin_ptr = NULL;
      }
      if (NULL != so_handler) {
        dlclose(so_handler);
        so_handler = NULL;
      }
    }
};
typedef std::tr1::shared_ptr<PluginInfo> PluginInfoPtr;
typedef std::map<std::string, PluginInfoPtr> PluginInfoPtrMap;

class PluginManager {

public:

    int Init(const char* config_home);

    IPlugin* GetPlugin(const std::string &plugin_name);

    int LoadPlugin(PluginInfoPtr& plugin_info);

    int LoadPlugins();

    int ParseStr2Map(const std::string& content, STR_MAP& content_map);

    //int InitPlugins();

    int ReadFileContent(const char* config_file, std::string &content);
    const PluginInfoPtrMap& GetPlugins(){ return plugins_info_map_;}
public:
    PluginManager();
    virtual ~PluginManager();
public:
    const static std::string kPluginManagerConfFile;
#ifdef _PLUGIN_MANAGER_TEST_
private:
#else
public:
#endif
    //void Destroy();
#ifdef _PLUGIN_MANAGER_TEST_
private:
#else
public:
#endif
    PluginManagerConf config_obj_;

    PluginInfoPtrMap plugins_info_map_;
    std::string plugin_mananger_home_path_;
    const static std::string kCreatePluginFunc;
      
};
typedef std::tr1::shared_ptr<PluginManager> PluginManagerPtr;

}



#endif // end SHARELIB_PLUGINMANANGER_PLUGIN_MANAGER_H_

