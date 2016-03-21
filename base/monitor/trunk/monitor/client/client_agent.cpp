#include <monitor/client/client_agent.h>
#include <monitor/client/monitor_client.h>
using namespace std;
MONITOR_BS;

ClientAgent::ClientAgent(){
    interface = NULL;
}

ClientAgent::~ClientAgent() { 
    MONITOR_DELETE_AND_SET_NULL(interface);
}

int ClientAgent::Init(std::string& confFile){
    interface = new MonitorClient;
    return interface->Init(confFile);
}

void ClientAgent::SendSnapshot(std::string& info,
                               std::string dataid){
    interface->SendSnapshot(info,dataid);
}
void ClientAgent::Add(std::string& key, int64_t value){
    interface->Add(key,value);
}

MONITOR_ES;

