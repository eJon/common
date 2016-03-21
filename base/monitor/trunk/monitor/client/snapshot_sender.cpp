#include <monitor/client/snapshot_sender.h>
using namespace std;
using namespace sharelib;
MONITOR_BS;

int64_t SnapshotSender::pkgCount = 0;
SnapshotSender::SnapshotSender(){
    started = false;
}

void SnapshotSender::Init(uint32_t maxsize){
    maxQueueSize = maxsize;
    queue.reset(new CurrentQueue<MonitorInfoPtr>(maxQueueSize));
    
    this->Start();
    while(started != true){
        MONITOR_LOG_ERROR("waiting monitor transporter start");
        usleep(100000);
    }
}
SnapshotSender::~SnapshotSender() { 
    queue->Stop();
    if(started){
        this->Terminate();
        this->Join();
    }
    MONITOR_LOG_DEBUG("destruct snapshotsender");
}

void SnapshotSender::Send(std::string& content, std::string& appid, 
                          std::string& dataid, std::string& ip, 
                          pthread_t threadid, std::string time){
    MonitorInfoPtr info(new MonitorInfo());
    info->content = content;
    info->appid = appid;
    info->dataid = dataid;
    info->ip = ip;
    info->threadid = threadid;
    info->time = time;
    if(0 != queue->Push(info)){
        MONITOR_LOG_ERROR("snapshot send info error %s", info->content.c_str());
    }
}

ret_t SnapshotSender::Run(){
    started  =true;
    MonitorInfoPtr info;
    std::string sendinfo;
    bool pop;
    while(_stat != thr_terminated){
        pop = queue->TryPop(info);
        if(pop == false){
            usleep(10000);
            continue;
        }else{
            if(info == NULL)
                MONITOR_LOG_ERROR("system error, wait and pop, but null");
        }
        sendinfo = ToJsonStringCompact(info.get());
        MONITOR_LOG_DEBUG("send info %d %s", pkgCount, sendinfo.c_str());
        transporter->SendInfo(sendinfo);
        pkgCount ++;
    }
    return r_succeed;
}

MONITOR_ES;

