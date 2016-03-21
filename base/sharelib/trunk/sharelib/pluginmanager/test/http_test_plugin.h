#ifndef SHARELIB_HTTP_TEST_PLUGIN_H_
#define SHARELIB_HTTP_TEST_PLUGIN_H_

#include <string>
#include <map>
#include <sharelib/pluginmanager/handler_iplugin.h>

namespace sharelib{
  class HttpRequestTestPlugin: public HandlerPlugin{
    public:
      HttpRequestTestPlugin();
      ~HttpRequestTestPlugin();
    public:
      virtual int Init(const STR_MAP& config_map);
      virtual int Destroy();
      virtual int Handle(const STR_MAP& query_map, std::string & result);

  };
}

#endif// end
