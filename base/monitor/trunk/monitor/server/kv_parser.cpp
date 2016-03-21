#include <monitor/server/kv_parser.h>
#include <monitor/server/app_conf.h>
#include <monitor/common.h>
#include <monitor/common/log.h>
#include <sharelib/util/file_util.h>

using namespace std;
using namespace sharelib;
MONITOR_BS;

KvParser::KvParser(){
}

KvParser::~KvParser() { 
}


bool KvParser::Deserialize(std::map<std::string,int64_t>& maps, std::string& content, bool add){
    string tokensplit = "|";
    std::vector<std::string> tokens = StringUtil::Split(content, tokensplit);
    if(tokens.size() == 0) return false;
    string kvsplit = ":";
    int64_t value;
    for(uint32_t i =0 ;i < tokens.size();i++){
        std::vector<std::string> kvs = StringUtil::Split(tokens[i], kvsplit);
        if(!StringUtil::StrToInt64(kvs[1].c_str(), value)){
            continue;
        }
        if(!add){
            maps[kvs[0]] = value;
        }else{
            maps[kvs[0]] += value;
        }
    }
    return true;
}
void KvParser::ExpandFields(std::map<std::string,int64_t>& maps, SingleWritePattern& pattern){
    std::map<std::string,int64_t>::iterator it;
    if(pattern.calculation == "+"){
        int64_t result = 0;
        for(uint32_t i =0;i < pattern.sourceFields.size();i++){
            it = maps.find(pattern.sourceFields[i]);
            if(it == maps.end()) continue;
            result += it->second;
        }
        maps[pattern.destField] = result;
    }else if(pattern.calculation == "-"){
        if(pattern.sourceFields.size() != 2){
            MONITOR_LOG_ERROR("cal - ,source not 2");
            return;
        }
        
        it = maps.find(pattern.sourceFields[0]);
        if(it == maps.end()) return;
        int64_t s1 = it->second;
        it = maps.find(pattern.sourceFields[1]);
        if(it == maps.end()) return;
        int64_t s2 = it->second;
        int64_t result  = s1 - s2;
        maps[pattern.destField] = result;
    }else if (pattern.calculation == "*"){
        if(pattern.sourceFields.size() != 2){
            MONITOR_LOG_ERROR("cal * ,source not 2");
            return;
        }
        
        it = maps.find(pattern.sourceFields[0]);
        if(it == maps.end()) return;
        int64_t s1 = it->second;
        it = maps.find(pattern.sourceFields[1]);
        if(it == maps.end()) return;
        int64_t s2 = it->second;
        int64_t result  = s1*s2;
        maps[pattern.destField] = result;
    }else if(pattern.calculation == "/"){
        if(pattern.sourceFields.size() != 2){
            MONITOR_LOG_ERROR("cal / ,source not 2");
            return;
        }
        
        it = maps.find(pattern.sourceFields[0]);
        if(it == maps.end()) return;
        int64_t s1 = it->second;
        it = maps.find(pattern.sourceFields[1]);
        if(it == maps.end()) return;
        int64_t s2 = it->second;
        if(s2 == 0) return;
        int64_t result  = s1/s2;
        maps[pattern.destField] = result;
    }else{
        MONITOR_LOG_ERROR("expand error, caculation + - * /");
    }
}
std::string KvParser::Serialize(std::map<std::string,int64_t>& kvs)
{
    std::map<std::string,int64_t>::iterator it;
    if(kvs.size() == 0) return "";
    std::stringstream ss;
    ss << "|" ;

    for(it = kvs.begin();it != kvs.end(); it++){
        ss << it->first << ":" << it->second;
        ss << "|";
    }
    return ss.str();
}
MONITOR_ES;

