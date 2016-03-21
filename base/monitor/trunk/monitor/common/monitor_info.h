#ifndef MONITOR_COMMON_MONITOR_INFO_H
#define MONITOR_COMMON_MONITOR_INFO_H

#include <monitor/common.h>
#include <sharelib/json/jsonizable.h>
MONITOR_BS;

class MonitorInfo : public sharelib::Jsonizable
{
public:
    MonitorInfo();
    ~MonitorInfo();
public:
    void Jsonize(sharelib::Jsonizable::JsonWrapper& json){
        json.Jsonize("appid", appid);
        json.Jsonize("dataid", dataid);
        json.Jsonize("ip", ip);
        json.Jsonize("threadid", threadid);
        json.Jsonize("time", time);
        json.Jsonize("content", content);
    }
public:
    std::string appid;
    std::string dataid;
    std::string ip;
    int64_t threadid;
    std::string time;
    std::string content;
private:
};
typedef std::tr1::shared_ptr<MonitorInfo > MonitorInfoPtr;
MONITOR_ES;

#endif //MONITOR_MONITOR_INFO_H
