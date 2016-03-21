
#include "ngx_adhandler_conf.h"
#include "ngx_handler.h"
#include <iostream>

using namespace std;
using namespace sharelib;

namespace ngx_handler {

  enum {
    RET_OK = 0,
    ERROR_INIT_NEW_FAILED=30000,
    ERROR_PLUGIN_MANAGER_CONFIG_FILE
  };

  Handler::Handler():plugin_manager_(NULL) {
  
  }

  Handler::~Handler() {
    delete plugin_manager_;
  }
  // Init process,every worker process use this function to complete init.
  int Handler::InitProcess() {
    int ret = RET_OK;
    plugin_manager_ = new PluginManager();
    if (NULL == plugin_manager_) {
      return ERROR_INIT_NEW_FAILED;
    }
    STR_MAP::const_iterator iter = config_map_.find(PLUGIN_MANAGER_HOME_PATH);
    if (iter != config_map_.end()) {
      cout<<"key:"<<PLUGIN_MANAGER_HOME_PATH<<" value:"<< iter->second.c_str()<<endl;
      if (RET_OK != (ret = plugin_manager_->Init(iter->second.c_str()))) {
        return ret;
      }
    }
    else {
      return ERROR_PLUGIN_MANAGER_CONFIG_FILE;
    }
    return RET_OK;
  }
  int Handler::Init(const STR_MAP& config_map)
  {
    config_map_ = config_map;
    return RET_OK;
  }
  void Handler::Destroy()
  {

  }
  int Handler::Handle(const STR_MAP& query_map, string& result)
  {
    int ret = RET_OK;
    STR_MAP::const_iterator iter = query_map.find(HTTP_REQUEST_PLUGINNAME);
    
    if (iter != query_map.end()) {
      HandlerPlugin* plugin = static_cast<HandlerPlugin*>
        (plugin_manager_->GetPlugin(iter->second));
      if (NULL == plugin) {
        return -1;
      }
      ret = plugin->Handle(query_map, result);
      if (RET_OK != ret) {
        return ret;
      }
    }
    return RET_OK;
  }
}
