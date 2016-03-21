#define _PLUGIN_MANAGER_TEST_
#include <sharelib/pluginmanager/http_request_define.h>
#include <iostream>
#include "http_test_plugin.h"
using namespace std;
namespace sharelib {

  HttpRequestTestPlugin::HttpRequestTestPlugin()
  {

  }
  HttpRequestTestPlugin::~HttpRequestTestPlugin()
  {

  }
  int HttpRequestTestPlugin::Init(const STR_MAP& config_map) {
    STR_MAP::const_iterator iter = config_map.begin();
    while (iter != config_map.end()) {
      cout<<"key:"<<iter->first<<"\t value:"<<iter->second<<endl;
      iter++;
    }
    return 0;
  }
  int HttpRequestTestPlugin::Destroy() {
    return 0;
  }
  int HttpRequestTestPlugin::Handle(const STR_MAP& query_map, std::string & result)
  {
    STR_MAP::const_iterator iter = query_map.find(HTTP_REQUEST_PLUGINNAME);
    std::string key = std::string(HTTP_REQUEST_PLUGINNAME);
    if (iter != query_map.end()) {
      for (int i = 0; i < 100000; i++) {

      }
      result = "{ \"retcode\":0, \"creatives\": [ { \"adid\":\"12323\", \"adurl\":\"http:\\weibo.com\adRuld\", \"begintime\":13312344545, \"endtime\":13312354545, \"freq\":1, \"score\":13, \"sourceurl\":\"http:\\202.192.64.1/sdfrd\", \"tokenid\":\"tokenid_1111\", \"type\":\"image\", \"uid\":\"uidadsfasdf\" }, { \"adid\": \"1232\", \"adurl\":\"http:\\weibo.com\", \"begintime\":13311344545, \"endtime\":13313354545, \"freq\":1, \"score\":12, \"sourceurl\":\"http:\\202.192.64.1/sdfdsf\", \"tokenid\":\"tokenid_1asdf11\", \"type\":\"html5\", \"uid\":\"uid234234\" } ] }";
    
 //   result = "nginx handler delivery11111111111111111111111111111111111111111111111111111111";
    }
    else {
      result = "nginx handler delivery";
    }

    cout<<"================================result "<<endl;
    return 0;
  }

  extern "C" {
    IPlugin* create_instance() {
      return new (std::nothrow)HttpRequestTestPlugin;
    }
  }
}
