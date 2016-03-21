#ifndef MONITOR_CLIENT_MONITOR_CLIENT_H
#define MONITOR_CLIENT_MONITOR_CLIENT_H

#include <monitor/common.h>
#include <monitor/common/log.h>
#include <monitor/client/cumulation_sender.h>
#include <monitor/client/snapshot_sender.h>
#include <string>
#include <monitor/client/client_interface.h>
MONITOR_BS;
/**
   two needs:
   1.send complete info
   2.increase/send/reset int type value every n secs
 **/


class MonitorClient : public ClientInterface
{
public:
    MonitorClient();
    ~MonitorClient();
public:
    /*override*/int Init(std::string& confFile);
public:
    /*override*/void SendSnapshot(std::string& info, 
                                  std::string dataid="default");
    /*override*/void Add(std::string& key, int64_t value);
public:
    
    
private:
    CumulationSenderPtr cumulationSender;
    SnapshotSenderPtr snapshotSender;
private:
    MonitorTransporterPtr transport;
    std::string appid;
    std::string ip;
};

typedef std::tr1::shared_ptr<MonitorClient> MonitorClientPtr;
MONITOR_ES;

#endif //MONITOR_MONITOR_CLIENT_H
