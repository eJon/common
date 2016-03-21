#ifndef MONITOR_SERVER_SERVER_TASK_MESSAGE_H
#define MONITOR_SERVER_SERVER_TASK_MESSAGE_H

#include <monitor/common.h>
#include <monitor/common/log.h>
#include <sharelib/task/task_message.h>
#include <monitor/common/monitor_info.h>
#include <string>

MONITOR_BS;

class ServerTaskMessage : public sharelib::TaskMessage
{
public:
    ServerTaskMessage(std::string& str): content(str){}
    ~ServerTaskMessage(){}
public:
    /*override*/void ToString(std::string& msg_str){   
        msg_str =  content;
    }
    MonitorInfoPtr ToMonitorInfo(){
        MonitorInfo* info = NULL;
        try{
            sharelib::FromJsonString(info, content);
        }catch(std::exception& e){
            MONITOR_LOG_ERROR("error info %s",e.what());
            return MonitorInfoPtr();
        }
        return MonitorInfoPtr(info);
    }
public:
    std::string content;
    
};

MONITOR_ES;

#endif //MONITOR_SERVER_TASK_MESSAGE_H
