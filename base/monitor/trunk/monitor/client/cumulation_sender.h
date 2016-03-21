#ifndef MONITOR_CLIENT_CUMULATION_SENDER_H
#define MONITOR_CLIENT_CUMULATION_SENDER_H

#include <monitor/common.h>
#include <monitor/client/client_common.h>
#include <sharelib/util/thread.h>
#include <monitor/client/monitor_transporter.h>
MONITOR_BS;



class CumulationSender : public sharelib::Thread
{
public:
    CumulationSender();
    ~CumulationSender();
public:
    int Init(uint32_t cumulationmaxsizeIn =10000 );
public:
    void SendInfo(std::string& key, int64_t value, CumulationType type = add);
    void SetTransporter(MonitorTransporterPtr transporterIn){
        transporter = transporterIn;
    }
    void SetAppid(std::string appidIn){appid = appidIn;}
    void SetIp(std::string ipIn){ip = ipIn;}
    /*override*/ret_t Run();
private:
    bool started;
    MonitorTransporterPtr transporter;
private:
    uint32_t cumulationmaxsize;
    KvCurrentQueuePtr queue;
    std::string dataid ;
    std::string ip;
    std::string appid;

    KeyValueManagerPtr kvManager;
private:
    int64_t sendLastTime;
};

typedef std::tr1::shared_ptr<CumulationSender> CumulationSenderPtr;
MONITOR_ES;

#endif //MONITOR_CUMULATION_SENDER_H
