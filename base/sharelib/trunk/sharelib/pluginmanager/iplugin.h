#ifndef SHARELIB_PLUGIN_INTERFACE_H_
#define SHARELIB_PLUGIN_INTERFACE_H_

#include <string>
#include <map>

namespace sharelib{

#define PLUGIN_MANAGER_HOME_PATH "__plugin_manager_home_path__"
#define CONF_PATH "__pluging_config_file__"

  typedef std::map<std::string, std::string> STR_MAP;
 
  class IPlugin
  {
    public:
        virtual ~IPlugin(){};
    public:
        // All return 0 is success, other is error, these error number is defined by 
        // child plugin.
        virtual int Init(const STR_MAP& config_map) = 0;
        virtual int Destroy() = 0;
       // virtual int Handle(const STR_MAP& query_map, std::string & result) = 0;
  };
}
#endif //end SHARELIB_PLUGIN_INTERFACE_H_
