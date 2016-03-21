#ifndef _BUCKET_CFG_H_
#define _BUCKET_CFG_H_
#include <stdlib.h>
#include <map>
#include <string>
#include <list>
#include <fstream>
#include <queue>
using namespace std;

class xml_cfg_t {
    typedef std::map<std::string, std::string> string_map_t;
    typedef std::map<std::string, xml_cfg_t*> xml_cfg_map_t;
  public:
    xml_cfg_t() {}
    virtual ~xml_cfg_t();
    int get_unsigned(string key, size_t &value, size_t default_value);
    int get_string(string key, string &value);
    int get_xmls(list<xml_cfg_t *> &buckets);
    int parse_config(const string& filename);
  private:
    int free_xml_cfg(xml_cfg_map_t &bucket_cfgs);
    static int parse_xml(queue<string>& config_strings, xml_cfg_t *bucket);
  private:
    string_map_t     values_;
    xml_cfg_map_t bucket_cfgs_;
};

#endif
