#ifndef MONITOR_CLIENT_MONITOR_TRANSPORTER_H
#define MONITOR_CLIENT_MONITOR_TRANSPORTER_H

#include <monitor/common.h>
#include <monitor/common/log.h>
#include <sharelib/server/tcp_client.h>
#include <sharelib/util/thread.h>
#include <sharelib/util/lock.h>
#include <monitor/client/client_conf.h>
#include <signal.h>
#include <vector>
MONITOR_BS;

class TcpClientWrapper{
public:
    TcpClientWrapper();
    ~TcpClientWrapper();
public:
    int SendInfo(std::string info);
    int Init(std::string& confPath);
    ClientConfPtr GetClientConf(){
        return clientConf;
    }
private:
    void DestroyClients(std::vector<sharelib::TcpClient*>& clients){
        for(uint32_t i =0;i < clients.size();i++){
            delete clients[i];
        }
        clients.clear();
    }

    bool InitClients(ClientConfPtr clientConf, std::vector<sharelib::TcpClient*>& clients);
    
private:
    std::vector<sharelib::TcpClient*> clients;
    ClientConfPtr clientConf;

};
typedef std::tr1::shared_ptr<TcpClientWrapper> TcpClientWrapperPtr;

class MonitorTransporter : public sharelib::Thread
{
public:
    MonitorTransporter(bool ignoreError = true);
    ~MonitorTransporter();
public:
    /*override*/ret_t Run();
public:
    int Init(std::string& confPath);
    int SendInfo(std::string info);
public:
    ClientConfPtr GetClientConf();
    uint32_t GetReloadCount(){return reloadCount;}
    
    void SetReloadSpan(int64_t span){
        reloadSpanMs = span;
    }
private:
    void IgnoreError(){
        signal(SIGPIPE,SIG_IGN);
    }
private:
    std::string confpath;
    int64_t lastModify;
    bool started;
    mutable uint32_t reloadCount;
    int64_t reloadSpanMs;
private:
    
    sharelib::RWLock lock;
    TcpClientWrapperPtr clientWrapper;
private:
    
};

typedef std::tr1::shared_ptr<MonitorTransporter> MonitorTransporterPtr;
MONITOR_ES;

#endif //MONITOR_MONITOR_TRANSPORTER_H
