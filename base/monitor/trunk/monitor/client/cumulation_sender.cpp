#include <monitor/client/cumulation_sender.h>
#include <sharelib/util/time_utility.h>
using namespace std;
using namespace sharelib;
MONITOR_BS;

CumulationSender::CumulationSender(){
    started = false;
    kvManager.reset(new KeyValueManager);
    dataid = "cumulation";
}

CumulationSender::~CumulationSender() { 
    queue->Stop();
    if(started){
        Terminate();
        Join();
    }
}


void CumulationSender::SendInfo(std::string& key, int64_t value, CumulationType type){
    KeyValuePtr keyvalue(new KeyValue(key, value, type));
    if(0!= queue->Push(keyvalue)){
        MONITOR_LOG_ERROR("cumulation send info error %s", key.c_str());
    }
}

ret_t CumulationSender::Run(){
    started =true;
    KeyValuePtr kv;
    int size;
    int64_t sendSpan;
    while(_stat != thr_terminated){
        size = queue->GetCurrentQueueSize();
        if(size != 0)
        {            
            for(int i =0 ;i < size ;i++){
                queue->WaitAndPop(kv);
                if(kv == NULL){
                    MONITOR_LOG_ERROR("system error, cumulatioin sender wait and pop,but get null");
                    continue;
                }
                kvManager->Caculate(kv);
            }
        }else{
            usleep(10);
        }
        sendSpan = transporter->GetClientConf()->span;
        if(TimeUtility::CurrentTimeInSeconds() >= sendLastTime + sendSpan){
            MonitorInfo monitorInfo;
            monitorInfo.appid  =appid;
            monitorInfo.dataid = dataid;
            monitorInfo.ip = ip;
            monitorInfo.threadid = pthread_self();
            monitorInfo.time = TimeUtility::CurrentTimeInSecondsReadable();
            monitorInfo.content = kvManager->ToTextString();
            transporter->SendInfo(ToJsonStringCompact(monitorInfo));
            kvManager->Reset();
            sendLastTime = TimeUtility::CurrentTimeInSeconds();
        }
        
    }
    return r_succeed;
}
int CumulationSender::Init(uint32_t cumulationmaxsizeIn ){
    sendLastTime = TimeUtility::CurrentTimeInSeconds();
    cumulationmaxsize = cumulationmaxsizeIn;
    queue.reset(new CurrentQueue<KeyValuePtr>(cumulationmaxsize));
    this->Start();
    while(started != true){
        MONITOR_LOG_ERROR("waiting monitor transporter start");
        usleep(10000);
    }
    return 0;
}
MONITOR_ES;

