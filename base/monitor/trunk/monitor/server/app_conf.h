#ifndef MONITOR_SERVER_APP_CONF_H
#define MONITOR_SERVER_APP_CONF_H

#include <monitor/common.h>
#include <vector>
#include <sharelib/json/jsonizable.h>
MONITOR_BS;

class SingleWritePattern : public sharelib::Jsonizable{
    void Jsonize(sharelib::Jsonizable::JsonWrapper& json){
        json.Jsonize("source", sourceFields);
        json.Jsonize("dest", destField);
        json.Jsonize("calculation", calculation);
    }
public:
    std::vector<std::string> sourceFields;
    std::string calculation; // only + - * /
    std::string destField;
};

class AppConf;
typedef std::tr1::shared_ptr<AppConf> AppConfPtr;
class AppConf : public sharelib::Jsonizable
{
public:
    AppConf();
    ~AppConf();
public:
    void Jsonize(sharelib::Jsonizable::JsonWrapper& json){
        json.Jsonize("maxFileSize", maxFileSize);
        json.Jsonize("maxFileCount", maxFileCount);
        json.Jsonize("maxFileTime", maxFileTime);
        json.Jsonize("aggrSpanTime", aggrSpanTime);
        json.Jsonize("dir", writePath);
        json.Jsonize("writePatternSet",patterns);        
    }

    static AppConfPtr ReadFromFile(std::string& file);
    static bool WriteToFile(AppConfPtr conf, std::string file);
public:
    uint32_t maxFileSize;
    uint32_t maxFileCount;
    std::string writePath;
    int64_t maxFileTime;
    int64_t aggrSpanTime;
    std::vector<SingleWritePattern> patterns;
};


MONITOR_ES;

#endif //MONITOR_APP_CONF_H
