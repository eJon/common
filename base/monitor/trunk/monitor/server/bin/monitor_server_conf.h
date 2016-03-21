#ifndef MONITOR_BIN_MONITOR_SERVER_CONF_H
#define MONITOR_BIN_MONITOR_SERVER_CONF_H

#include <monitor/common.h>
#include <sharelib/json/jsonizable.h>
MONITOR_BS;

class MonitorServerConf;
typedef std::tr1::shared_ptr<MonitorServerConf>  MonitorServerConfPtr;
class MonitorServerConf:public sharelib::Jsonizable
{
public:
    MonitorServerConf();
    ~MonitorServerConf();
public:

    void Jsonize(sharelib::Jsonizable::JsonWrapper& json){
        json.Jsonize("server_port", port);
        json.Jsonize("writer_threads", threadNum);
        json.Jsonize("queue_size", queueSize);
        json.Jsonize("app_confdir", appConfDir);
        
    }

public:
    static MonitorServerConfPtr ReadFromFile(std::string file);
    static bool WriteToFile(MonitorServerConfPtr conf, std::string file);
public:
    int port;
    uint32_t queueSize;
    uint32_t threadNum;
    std::string appConfDir;
    
private:
    
};

    

    
MONITOR_ES;

#endif //MONITOR_MONITOR_SERVER_CONF_H
