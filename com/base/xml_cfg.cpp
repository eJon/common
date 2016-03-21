#include "xml_cfg.h"
#include <queue>
#include <sstream>

static string
trim_string(const string& str) {
    string whitespace = " \t";
    size_t start      = str.find_first_not_of(whitespace);
    size_t end        = str.find_last_not_of(whitespace);

    if(start != string::npos) {
        return str.substr(start, end - start + 1);
    } else {
        return "";
    }
}

int
xml_cfg_t::parse_config(const string& filename) {
    queue<string> config_strings;

    string line;
    ifstream config_file;

    config_file.open(filename.c_str());
    if(!config_file.good()) {
        return -1;
    }

    while(getline(config_file, line)) {
        config_strings.push(line);
    }

    config_file.close();
    return parse_xml(config_strings, this);
}

int
xml_cfg_t::get_xmls(list<xml_cfg_t* > &buckets) {
    xml_cfg_map_t::iterator iter = bucket_cfgs_.begin();
    while(bucket_cfgs_.end() != iter) {
        buckets.push_back(iter->second);
        iter++;
    }
    return 0;
}

int
xml_cfg_t::get_unsigned(string key, size_t &value, size_t default_value) {
    string str;
    if(0 != get_string(key, str)) {
        value = default_value;
        return 0;
    }
    char *valid = NULL;
    int value_temp = strtol(str.c_str(), &valid, 10);
    if(!(*str.c_str() != '\0' && NULL != valid && *valid == '\0')) {
        return -1;
    }
    if(value_temp < 0) {
        return -1;
    }
    value = value_temp;
    return 0;
}

int
xml_cfg_t::get_string(string key, string &value) {
    string_map_t::iterator iter = values_.find(key);
    if(values_.end() == iter) {
        return -1;
    }
    value = iter->second;
    return 0;
}

int
xml_cfg_t::parse_xml(queue<string>& config_strings, xml_cfg_t *bucket) {
    string line;
    int store_index = 0; // used to give things named "store" different names

    while(!config_strings.empty()) {
        line = config_strings.front();
        config_strings.pop();

        line = trim_string(line);

        // remove comment
        size_t comment = line.find_first_of('#');
        if(comment != string::npos) {
            line.erase(comment);
        }

        int length = line.size();
        if(0 >= length) {
            continue;
        }
        if(line[0] == '<') {
            if(length > 1 && line[1] == '/') {
                return 0;
            }

            string::size_type pos = line.find('>');
            if(pos == string::npos) {
                continue;
            }
            string store_name = line.substr(1, pos - 1);

            xml_cfg_t *new_bucket = new(std::nothrow)xml_cfg_t;
            if(NULL == new_bucket) {
                return -1;
            }
            if(0 == parse_xml(config_strings, new_bucket)) {
                if(0 == store_name.compare("bucket") || 0 == store_name.compare("configure")) {
                    // This is a special case for the top-level stores. They share
                    // the same name, so we append an index to put them in the map
                    ostringstream oss;
                    oss << store_index;
                    store_name += oss.str();
                    ++store_index;
                }
                if(bucket->bucket_cfgs_.find(store_name) != bucket->bucket_cfgs_.end()) {
                    return -1;
                }
                bucket->bucket_cfgs_[store_name] = new_bucket;
            }
        } else {
            string::size_type eq = line.find('=');
            if(string::npos != eq) {
                string arg = line.substr(0, eq);
                string val = line.substr(eq + 1, string::npos);

                arg = trim_string(arg);
                val = trim_string(val);

                if(bucket->values_.find(arg) != bucket->values_.end()) {
                    return -1;
                }
                bucket->values_[arg] = val;
            }
        }
    }
    return 0;
}

int
xml_cfg_t::free_xml_cfg(xml_cfg_map_t &bucket_cfgs) {
    if(true == bucket_cfgs.empty()) {
        return 0;
    }
    for(xml_cfg_map_t::iterator iter = bucket_cfgs.begin(); iter != bucket_cfgs.end();) {
        free_xml_cfg(iter->second->bucket_cfgs_);
        delete iter->second;
        bucket_cfgs.erase(iter++);
    }
    return 0;
}

xml_cfg_t::~xml_cfg_t() {
    free_xml_cfg(bucket_cfgs_);
}
