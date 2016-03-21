#include <mem_db/mem_db.h>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;
using namespace mem_db;

int main (int argc, char **argv) {
  mem_db::MemDB mem_db;
  mem_db.Init ("./mem_db.conf");

  std::string key;
  std::string value;
  std::string stored_value;



  int ret = 0;
  std::vector<HostInfo> hosts;
  ret = mem_db.ListHosts(hosts);
  if (ret != 0) {
    std::cout << "ListHosts error" << std::endl;
  } else {
    std::vector<HostInfo>::iterator it;
    std::cout << "ListHosts Info: " << std::endl;
    for (it = hosts.begin(); it != hosts.end(); ++it) {
      std::cout << "    Host: " << it->host_name
                << "   ip: "<< it->ip_addr
                <<"    port: "<< it->port <<std::endl;
    }
  }

  std::cout << ""<< std::endl;
  std::cout << "-----------------------------------------------------" << std::endl;
  key = "basickey";
  value = "basic_value";
  ret = mem_db.SetGeneralValue (key, value);
  if (ret != 0) {
    std::cout << "SetGeneralValue: insert error" << std::endl;
  } else {
    std::cout << "SetGeneralValue: insert ok" << std::endl;
  }
  ret = mem_db.SetGeneralValue (key + key, value);
  if (ret != 0) {
    std::cout << "SetGeneralValue: insert error" << std::endl;
  } else {
    std::cout << "SetGeneralValue: insert ok" << std::endl;
  }

  sleep(5);
  std::cout << ""<< std::endl;
  std::cout << "-----------------------------------------------------" << std::endl;
  ret = mem_db.GetGeneralValue (key, stored_value);
  if (ret != 0) {
    std::cout << "GetGeneralValue: get error" << std::endl;
  } else {
    std::cout << "GetGeneralValue: get ok value: " << stored_value << std::endl;
  }

  std::cout << ""<< std::endl;
  std::cout << "-----------------------------------------------------" << std::endl;
  std::vector<std::string> keys;
  std::vector<std::string> values;

  keys.push_back(key);
  keys.push_back(key + key);
  ret = mem_db.MultiGetGeneralValue (keys, values);
  if (ret != 0) {
    std::cout << "MultiGetGeneralValue: get error" << std::endl;
  } else {
    std::cout << "MultiGetGeneralValue: get ok " << std::endl;
    for (int i = 0; i < keys.size(); ++i) {
      std::cout << "key: " << keys[i] << "   values: " << values[i] << std::endl;
    }
  }

  std::cout << ""<< std::endl;
  std::cout << "-----------------------------------------------------" << std::endl;
  ret = mem_db.DeleteGeneralValue(key);
  if (ret != 0) {
    std::cout << "DeleteGeneralValue: delete error" << std::endl;
  } else {
    std::cout << "DeleteGeneralValue: delete ok key: " << key << std::endl;
  }

  std::cout << ""<< std::endl;
  std::cout << "-----------------------------------------------------" << std::endl;
  stored_value = "";
  ret = mem_db.GetGeneralValue (key, stored_value);
  if (ret != 0) {
    std::cout << "GetGeneralValue: get error" << std::endl;
  } else {
    std::cout << "GetGeneralValue: get ok key: " << key << "  value: " << stored_value << std::endl;
  }

  mem_db.Free ();

  return 0;
}
