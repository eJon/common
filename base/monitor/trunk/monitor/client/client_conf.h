#ifndef MONITOR_CLIENT_CLIENT_CONF_H
#define MONITOR_CLIENT_CLIENT_CONF_H

#include <monitor/common.h>
#include <sharelib/json/jsonizable.h>
MONITOR_BS;
class IpPort : public sharelib::Jsonizable{
public:
    void Jsonize(sharelib::Jsonizable::JsonWrapper& json){
        json.Jsonize("ip", ip);
        json.Jsonize("port", port);
    }
public:
    bool operator ==(const IpPort& other){
        return (ip == other.ip && port == other.port);
    }
    bool operator !=(const IpPort& other){
        return (!(*this == other));
    }
public:
    std::string ip;
    int port;
};
class ClientConf;
typedef std::tr1::shared_ptr<ClientConf> ClientConfPtr;
class ClientConf : public sharelib::Jsonizable{

public:
    void Jsonize(sharelib::Jsonizable::JsonWrapper& json){
        json.Jsonize("monitor_server", ips);
        json.Jsonize("app_id", appid);
        json.Jsonize("send_interval_secs", span);
        json.Jsonize("eth", eth);
    };
public:
    bool operator !=(const ClientConf& other){
        return (!(*this == other));
    }
    bool operator ==(const ClientConf& other){
        if(other.span != span) return false;
        if(ips.size() != other.ips.size()) return false;
        for(uint32_t i = 0;i < ips.size(); i++){
            if(ips[i] != other.ips[i])
                return false;
        }
        return true;
    }
    std::string ToString(){
        return sharelib::ToJsonString(this); 
    }
    uint32_t GetServerSize(){return ips.size();}
public:
    static ClientConfPtr ReadFromFile(std::string& file);
    static bool WriteToFile(ClientConfPtr conf, std::string& file);
public:
    std::vector<IpPort> ips;
    int64_t span;
    std::string appid;
    std::string eth;
};

MONITOR_ES;

#endif //MONITOR_CLIENT_CONF_H
