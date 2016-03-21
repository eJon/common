#include <monitor/server/aggregate_file_writer.h>
#include <monitor/server/kv_parser.h>
#include <sharelib/util/time_utility.h>
using namespace std;
using namespace sharelib;
MONITOR_BS;

AggregateFileWriter::~AggregateFileWriter(){
    if(infos.size() !=0 )
    {
        MonitorInfoPtr info = NormalizeInfo(infos, appconf);
        WriteFunc(info);
    }
}

int AggregateFileWriter::Write(MonitorInfoPtr monitorInfo){
    int64_t now = TimeUtility::CurrentTimeInSeconds();
    MonitorInfoWithTime st(monitorInfo, now);
    boost::mutex::scoped_lock lock(mutex_);
    if(aggrStart == 0){
        aggrStart = now;
    }
    if(now <= aggrStart + appconf->aggrSpanTime){
        infos.push_back(st);
    }else
    {
        if(infos.size() !=0 )
        {
            MonitorInfoPtr info = NormalizeInfo(infos, appconf);
            WriteFunc(info);
        }
        infos.clear();
        infos.push_back(st);
        aggrStart = now;
    }    
   
    return 0;
}

std::string AggregateFileWriter::ThreadSet(std::vector<MonitorInfoWithTime>& infos){
    stringstream ss;
    ss << "|";
    for(uint32_t i =0;i < infos.size() - 1;i++){
        ss << infos[i].info->threadid << "|";
    }
    ss << infos[infos.size() -1].info->threadid << "|";
    return ss.str();
}
std::string AggregateFileWriter::IpSet(std::vector<MonitorInfoWithTime>& infos){
    stringstream ss;
    ss << "|";
    for(uint32_t i =0;i < infos.size() - 1;i++){
        ss << infos[i].info->ip << "|";
    }
    ss << infos[infos.size() -1].info->ip << "|";
    return ss.str();
}
MonitorInfoPtr AggregateFileWriter::NormalizeInfo(
        std::vector<MonitorInfoWithTime>& infos, AppConfPtr appConf){
    //iterator all ,add ,pattern normalize
    typedef  std::map<string, int64_t> MAP;
    MAP map;
    uint32_t count = 0;
    bool ret;
    for(uint32_t i = 0;i < infos.size();i++){
        if(!(ret = KvParser::Deserialize(map, infos[i].info->content, true))){
            continue;
        }
        count++;
    }
    std::vector<SingleWritePattern>& patterns = appConf->patterns;
    for(uint32_t i =0;i < patterns.size();i++){
        KvParser::ExpandFields(map, patterns[i]);
    }
    map["groupCount"] = count;
    std::string content = KvParser::Serialize(map);
    MonitorInfoPtr monitorInfo(new MonitorInfo);
    monitorInfo->appid = infos[0].info->appid;
    monitorInfo->dataid = infos[0].info->dataid;
    monitorInfo->ip = IpSet(infos);
    monitorInfo->threadid = 0;
    monitorInfo->time = infos[0].info->time;
    monitorInfo->content = content;
    return monitorInfo;
}

MONITOR_ES;

