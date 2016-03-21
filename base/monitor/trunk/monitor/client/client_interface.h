#ifndef MONITOR_CLIENT_CLIENT_INTERFACE_H
#define MONITOR_CLIENT_CLIENT_INTERFACE_H

#include <monitor/common.h>
#include <string>
MONITOR_BS;

class ClientInterface
{
public:
    ClientInterface(){}
    virtual ~ClientInterface(){}
public:
    virtual int Init(std::string& confFile) = 0;
    virtual void SendSnapshot(std::string& info, 
                              std::string dataid) = 0;
    virtual void Add(std::string& key, int64_t value) = 0;
    
private:
};

MONITOR_ES;

#endif //MONITOR_CLIENT_INTERFACE_H
