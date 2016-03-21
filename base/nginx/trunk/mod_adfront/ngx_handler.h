// copyright:
//            (C) SINA Inc.
//
//      file: handler.h
//      desc: declaration for Hander--the mainly work process
//    author: kefeng
//     email: xidianzkf@gmail.com
//      date: 2013-04-18
//
//    change: 

#ifndef ADFRONT_HANDLER_MANAGER_HANDLER_H_
#define ADFRONT_HANDLER_MANAGER_HANDLER_H_

#include <string>
#include <vector>
#include <map>

#include <sharelib/pluginmanager/http_request_define.h>
#include <sharelib/pluginmanager/plugin_manager.h>
#include <sharelib/pluginmanager/handler_iplugin.h>

namespace ngx_handler{
  typedef std::map<std::string, std::string> STR_MAP;
  class Handler {
    public:
      Handler();
      ~Handler();
      // Init handler config;
      int Init(const STR_MAP& config_map);
      // Init work process.
      int InitProcess();
      // do initialization of Hander
      //int Init(const char* config_file);
      // release the resouces
      void Destroy();
      // handle one request
      int Handle(const STR_MAP& query_map, std::string& result);

    private:
      sharelib::PluginManager* plugin_manager_;
      STR_MAP config_map_;
  };
}
#endif  // ADFRONT_HANDLER_MANAGER_HANDLER_H_
