#include <monitor/client/monitor_transporter.h>
#include <sharelib/util/file_util.h>
#include <sharelib/server/tcp_client.h>
using namespace std;
using namespace sharelib;
MONITOR_BS;


TcpClientWrapper::TcpClientWrapper(){
    
}
TcpClientWrapper::~TcpClientWrapper(){
    DestroyClients(clients);
}


int TcpClientWrapper::SendInfo(std::string info){
    for(uint32_t i =0;i < clients.size(); i++){
        if(0 == clients[i]->SendData(info.c_str(), info.size()))
            continue;
        
        if(clients[i]->Reconn(clients[i]->GetAddress(), clients[i]->Getport()) == 0){
            if(clients[i]->SendData(info.c_str(), info.size()) != 0){
                MONITOR_LOG_ERROR("send monitor data to %s %d error, reconn right,but resend error",
                        clients[i]->GetAddress().c_str(), clients[i]->Getport());
            }
        }else{
            MONITOR_LOG_ERROR("send monitor data to %s %d error, reconn error",
                    clients[i]->GetAddress().c_str(), clients[i]->Getport());
        }
    }
    return 0;
}

bool TcpClientWrapper::InitClients(ClientConfPtr clientConf, std::vector<sharelib::TcpClient*>& clients)
{
    for(uint32_t i =0 ;i < clientConf->ips.size();i++){
        
        TcpClient* client = new TcpClient();
        if(0 != client->Conn((clientConf->ips)[i].ip, (clientConf->ips)[i].port) ){
            MONITOR_LOG_ERROR("can't connect %s %d", (clientConf->ips)[i].ip.c_str(), (clientConf->ips)[i].port);
            delete client;
            return false;
        }
        clients.push_back(client);
    }
    return true;
}
int TcpClientWrapper::Init(std::string& confPath){
    clientConf = ClientConf::ReadFromFile(confPath);
    if(clientConf == NULL){
        return -1;
    }

    bool ret = InitClients(clientConf, clients);
    if(!ret || clients.size() != clientConf->GetServerSize()){
        MONITOR_LOG_ERROR("init clients from |%s| error", clientConf->ToString().c_str());
        return -1;
    }
    
    return 0;
}

MonitorTransporter::MonitorTransporter(bool ignoreError ){
    reloadCount = 0;
    reloadSpanMs = 3000000;
    started = false;
    if(ignoreError){
        IgnoreError();
    }
}

MonitorTransporter::~MonitorTransporter(){
    if(started){
        Terminate();
        Join();
    }
}


ClientConfPtr MonitorTransporter::GetClientConf(){
    TcpClientWrapperPtr tmp;
    lock.RLock();
    tmp = clientWrapper;
    lock.UnLock();
    return tmp->GetClientConf();
    
}
int MonitorTransporter::SendInfo(std::string info){
    if(info == "") return -1;
    TcpClientWrapperPtr tmp;
    lock.RLock();
    tmp = clientWrapper;
    lock.UnLock();
    tmp->SendInfo(info);
    return 0;
}
int MonitorTransporter::Init(std::string& confPath){
    confpath = confPath;
    struct stat st;
    FileUtil::GetFileStatus(confPath, &st);
    lastModify = st.st_mtime;
    
    clientWrapper.reset(new TcpClientWrapper);
    if(0 != clientWrapper->Init(confpath)){
        MONITOR_LOG_ERROR("client wrapper init error");
        return -1;
    }
    this->Start();
    while(started != true){
        MONITOR_LOG_ERROR("waiting monitor transporter start");
        usleep(10000);
    }
    
    return 0;
}

ret_t MonitorTransporter::Run(){
    started  =true;
    while(_stat != thr_terminated){
        usleep(reloadSpanMs);
        struct stat st;
        if(0 != FileUtil::GetFileStatus(confpath, &st)) 
            continue;
        if(lastModify == st.st_mtime) continue;
        lastModify = st.st_mtime;
        TcpClientWrapperPtr wrapper(new TcpClientWrapper);
        if(0 != wrapper->Init(confpath)){
            MONITOR_LOG_ERROR("client reload wrapper init error, please modify");
            continue;
        }
        lock.WLock();
        clientWrapper = wrapper;
        lock.UnLock();
        reloadCount ++;
    }
    return r_succeed;
}
MONITOR_ES;

