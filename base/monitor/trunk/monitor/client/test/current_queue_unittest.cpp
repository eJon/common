#include <gtest/gtest.h>
#include <sharelib/server/tcp_client.h>
#include <sharelib/server/epoll_server.h>
#include <sharelib/util/time_utility.h>
#include <sharelib/util/thread.h>
#include <monitor/client/client_common.h>
#include <monitor/test/test.h>
#include <monitor/common/log.h>
#include <iostream>
#include <sstream>
using namespace std;
using namespace monitor;
using namespace sharelib;

class ThreadSet : public Thread
{
public:
    ThreadSet(InfoCurrentQueuePtr queueIn){
        queue = queueIn;
    }
    ret_t Run(){
        while(_stat != thr_terminated){
            {
                string appid = "appid";
                string dataid = "dataid";
                string ip = "ip";
                string content= "content";
            
                pthread_t threadid =1 ;
                string  time = TimeUtility::CurrentTimeInSecondsReadable();
        
                MonitorInfoPtr info(new MonitorInfo());
                info->content = content;
                info->appid = appid;
                info->dataid = dataid;
                info->ip = ip;
                info->threadid = threadid;
                info->time = time;
                if(0 != queue->Push(info)){
                    MONITOR_LOG_ERROR("set info error %s", info->content.c_str());
                }
                MONITOR_LOG_DEBUG("set info %s", info->content.c_str());
            }
            /*
             {
                MonitorInfoPtr info;
                std::string sendinfo;
                bool pop;
                queue->WaitAndPop(info);
                //if(!pop) continue;
                sendinfo = ToJsonStringCompact(info.get());
                MONITOR_LOG_DEBUG("get info  %s", sendinfo.c_str());
            
                }*/
        }
        
        return r_succeed;
    }

    InfoCurrentQueuePtr queue;
};

class ThreadGet : public Thread
{
public:
    ThreadGet(InfoCurrentQueuePtr queueIn){
        queue = queueIn;
    }
    
    ret_t Run(){
        while(_stat != thr_terminated){
            MonitorInfoPtr info;
            std::string sendinfo;
            queue->WaitAndPop(info);
            //if(!pop) continue;
            sendinfo = ToJsonStringCompact(info.get());
            MONITOR_LOG_DEBUG("get info  %s", sendinfo.c_str());
        }
        return r_succeed;
    }
    InfoCurrentQueuePtr queue;
};
TEST(CurrentQueue, BasicFunction)
{
    InfoCurrentQueuePtr queue;
    queue.reset(new CurrentQueue<MonitorInfoPtr>(10000));
    ThreadSet set(queue);
    set.Start();
    
    ThreadGet get(queue);
    get.Start();

    sleep(3);
    queue->Stop();

    
    set.Terminate();
    set.Join();

    
    get.Terminate();
    get.Join();
    
}


