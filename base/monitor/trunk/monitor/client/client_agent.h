#ifndef MONITOR_CLIENT_CLIENT_AGENT_H
#define MONITOR_CLIENT_CLIENT_AGENT_H

#include <monitor/common.h>
#include <monitor/client/client_interface.h>
#include <tr1/memory> 
MONITOR_BS;

class ClientAgent
{
public:
    ClientAgent();
    ~ClientAgent();
public:
    int Init(std::string& confFile);
    void SendSnapshot(std::string& info,
                      std::string dataid="default");
    void Add(std::string& key, int64_t value);
    
private:
    ClientInterface* interface;
};

typedef std::tr1::shared_ptr<ClientAgent> ClientAgentPtr;
MONITOR_ES;

#endif //MONITOR_CLIENT_AGENT_H
