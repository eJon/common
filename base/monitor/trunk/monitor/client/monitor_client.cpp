#include <monitor/client/monitor_client.h>
#include <sharelib/util/time_utility.h>
#include <sharelib/util/ip_utility.h>
#include <pthread.h>
using namespace std;
using namespace sharelib;
MONITOR_BS;

MonitorClient::MonitorClient(){
}

MonitorClient::~MonitorClient() { 
}


void MonitorClient::Add(std::string& key, int64_t value){
    cumulationSender->SendInfo(key, value, add);
}

int MonitorClient::Init(std::string& confFile){
    transport.reset(new MonitorTransporter);
    if(transport->Init(confFile) != 0)
    {
        MONITOR_LOG_ERROR("transport init error %s" , confFile.c_str());
        return -1;
    }
    string localeth = transport->GetClientConf()->eth;
    ip = IpUtility::GetLocalIp(localeth);
    appid = transport->GetClientConf()->appid;
    MONITOR_LOG_INFO("eth %s ip %s appid %s" , localeth.c_str(), ip.c_str(), appid.c_str());
    uint32_t cumulationmaxsize = 10000 ;
    cumulationSender.reset(new CumulationSender());
    cumulationSender->SetTransporter(transport);
    cumulationSender->Init(cumulationmaxsize);
    cumulationSender->SetAppid(appid);
    cumulationSender->SetIp(ip);
    uint32_t snapshotmaxsize = 1000 ;
    snapshotSender.reset(new SnapshotSender());
    snapshotSender->SetTransporter(transport);
    snapshotSender->Init(snapshotmaxsize);

    
    return 0;
}



void MonitorClient::SendSnapshot(std::string& info, std::string dataid)
{
    snapshotSender->Send(info, appid, dataid, ip, 
                         pthread_self(), TimeUtility::CurrentTimeInSecondsReadable());
}
MONITOR_ES;

