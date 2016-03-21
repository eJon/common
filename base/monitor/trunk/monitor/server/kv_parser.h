#ifndef MONITOR_SERVER_KV_PARSER_H
#define MONITOR_SERVER_KV_PARSER_H

#include <monitor/common.h>
#include <monitor/server/server_common.h>
#include <monitor/server/app_conf.h>
#include <map>

MONITOR_BS;

class KvParser
{
public:
    KvParser();
    ~KvParser();
public:
    static bool Deserialize(std::map<std::string,int64_t>& maps, std::string& content, bool add =false);
    static void ExpandFields(std::map<std::string,int64_t>& maps, SingleWritePattern& pattern);
    static std::string Serialize(std::map<std::string,int64_t>& maps);
    
private:
};

MONITOR_ES;

#endif //MONITOR_KV_PARSER_H
