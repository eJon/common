#include <dlfcn.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <google/protobuf/text_format.h>
#include "plugin_manager.h"
#include <sharelib/util/sharelib_log.h>  
#include <sharelib/util/file_util.h>

using namespace std;
using namespace std::tr1;
namespace sharelib {

  typedef IPlugin *(*CreatePluginFunc)();

  const string PluginManager::kCreatePluginFunc = "create_instance";

  const string PluginManager::kPluginManagerConfFile = "./plugin_manager.conf";

  int PluginManager::ReadFileContent(const char* config_file, string &content)
  {
    if (config_file == NULL) {
      return ERROR_FILE_NULL;
    }
    ifstream fin(config_file);
    if (!fin.is_open()) { 
      return ERROR_FILE_OPEN;
    }
    ostringstream tmp;
    tmp << fin.rdbuf();
    content = tmp.str();
    fin.close();
    return RET_OK;
  }

  int PluginManager::Init(const char* config_home)
  {
    string config_str;
    if (NULL == config_home) {
      return ERROR_FILE_NULL;
    }
    plugin_mananger_home_path_ = string(config_home);
    string path = plugin_mananger_home_path_ + kPluginManagerConfFile;
    int ret = ReadFileContent(path.c_str(), config_str);
    if (RET_OK != ret) {
      return ret;
    }
    bool parse_ret = google::protobuf::TextFormat::ParseFromString(config_str, &config_obj_);
    if (!parse_ret) {
      return ERROR_PARSE_CONFIG;
    }
    cout<<"init plugin"<<endl;
    if (RET_OK != (ret = LoadPlugins())) {
      return ret;
    }
    return 0;
  }


  int PluginManager::LoadPlugin(shared_ptr<PluginInfo> &plugin_info)
  {  
    //front
    string so_path = plugin_info->plugin_conf.so_home_path()
      + '/' + plugin_info->plugin_conf.so_name();
    so_path = FileUtil::GenerateAbsolutePath(plugin_mananger_home_path_,so_path);
    void *so_handler = dlopen(so_path.c_str(), RTLD_LAZY);
    if (so_handler == NULL) {
      printf("dlopen so path=%s,error=%s",
          so_path.c_str(), dlerror());   
      return ERROR_DLOPEN;
    }
    CreatePluginFunc handler = NULL;
    handler = (CreatePluginFunc)dlsym(so_handler, CREATE_INSTANCE_FUNC);
    if (NULL == handler) {
      dlclose(so_handler);
      printf("create plugin error,so path=%s,error=%s",
          so_path.c_str(), dlerror());   
      return ERROR_DLSYM;
    }

    IPlugin* plugin = (*handler)();

    if (plugin == NULL) {
      dlclose(so_handler);
      printf("create plugin error,so path=%s,error=%s",
          so_path.c_str(), dlerror());   
      return ERROR_CREATE_INSTANCE;
    }
    int ret = plugin->Init(plugin_info->conf_map);
    if (0 != ret) {
      return ret;
    }
    plugin_info->plugin_ptr = plugin;
    plugin_info->so_handler = so_handler;
    return 0;
  }
  // Key = value
#define EQ_STR "="
  int PluginManager::ParseStr2Map(const std::string& content, STR_MAP& content_map)
  {
    if (content.size() == 0) {
      return 0;
    }
    string key,value;
    size_t pos = content.find(EQ_STR);
    if (pos != string::npos) {
      key = content.substr(0, pos);
      value = content.substr(pos+1);
      content_map[key] = value;
    }
    return 0;
  }
  /*
  int PluginManager::InitPlugins() {
    int ret = RET_OK;
    cout<<"InitPlugins"<<endl;
    PluginInfoMap::iterator iter = plugins_info_map_.begin();
    while (iter != plugins_info_map_.end()) {
      IPlugin* plugin = iter->second.plugin_ptr;
      if (NULL != plugin) {
        if (*iter->second.stat_ptr) {
          cout<<"this have been inited"<<iter->first<<endl;
          iter++;
          continue;
        }
        cout<<"init plugin,key="<<iter->first<<endl;
        ret = plugin->Init(iter->second.conf_map);
        if (RET_OK != ret) {
          return ret;
        }
        *iter->second.stat_ptr = kInitedType;
      }
      else {
        return ERROR_INIT_PLUGIN;
      }
      iter++;
    }
    return RET_OK;
  }
  */

  int PluginManager::LoadPlugins()
  {
    int i = 0, j = 0, ret = RET_OK;
    for (i = 0; i < config_obj_.plugin_conf_list_size(); ++i)
    {
      PluginInfoPtr plugin_info_ptr(new PluginInfo());
      plugin_info_ptr->plugin_conf = config_obj_.plugin_conf_list(i);
      plugin_info_ptr->conf_map[CONF_PATH] = plugin_info_ptr->plugin_conf.conf_path();
      plugin_info_ptr->conf_map[PLUGIN_MANAGER_HOME_PATH] = plugin_mananger_home_path_;
      cout<<"plugin_mananger_home_path_:"<<PLUGIN_MANAGER_HOME_PATH<<":"<< plugin_mananger_home_path_<<endl;
      for (j = 0; j < plugin_info_ptr->plugin_conf.key_val_list_size(); ++j) {
        string key_val = plugin_info_ptr->plugin_conf.key_val_list(j);
        ParseStr2Map(key_val, plugin_info_ptr->conf_map);
      }
      ret = LoadPlugin(plugin_info_ptr);
      if (RET_OK != ret) {
        cout<<"LoadPlugin is error"<<endl;
        return ERROR_LOAD_PLUGIN;
      }
      for (j = 0; j < plugin_info_ptr->plugin_conf.name_size(); ++j){
        cout<<"key:"<<plugin_info_ptr->plugin_conf.name(j)<<endl;
        plugins_info_map_.insert(make_pair<string,PluginInfoPtr>(plugin_info_ptr->plugin_conf.name(j), 
              plugin_info_ptr));
      }
    

    }
    return 0;
  }

 IPlugin *PluginManager::GetPlugin(const string &queryName) {
    PluginInfoPtrMap::iterator it = plugins_info_map_.find(queryName);
    if (it == plugins_info_map_.end()) {
      return NULL;
    }
    return it->second->plugin_ptr;
  }

  PluginManager::PluginManager()
  {}

  PluginManager::~PluginManager() {
  }
  
}

