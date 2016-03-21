#include <monitor/server/kv_file_writer.h>
#include <monitor/server/kv_parser.h>
#include <sharelib/util/time_utility.h>
#include <sharelib/util/file_util.h>
#include <monitor/server/kv_parser.h>
using namespace std;
using namespace sharelib;
MONITOR_BS;



MonitorInfoPtr KvFileWriter::NormalizeInfo(MonitorInfoPtr info, AppConfPtr appconf){
    if(appconf->patterns.size() == 0 || info->content == "")
        return info;

    map<string,int64_t> maps;
    bool ret= KvParser::Deserialize(maps, info->content);
    if(!ret) return info;
    std::vector<SingleWritePattern>& patterns = appconf->patterns;
    for(uint32_t i =0;i < patterns.size();i++){
        KvParser::ExpandFields(maps, patterns[i]);
    }
    std::string content = KvParser::Serialize(maps);
    MonitorInfoPtr monitorInfo(new MonitorInfo);
    monitorInfo->appid = info->appid;
    monitorInfo->dataid = info->dataid;
    monitorInfo->ip = info->ip;
    monitorInfo->threadid = info->threadid;
    monitorInfo->time = info->time;
    monitorInfo->content = content;
    return monitorInfo;
}
MONITOR_ES;

