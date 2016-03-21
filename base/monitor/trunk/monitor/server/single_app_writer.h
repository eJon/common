#ifndef MONITOR_SERVER_SINGLE_WRITER_H
#define MONITOR_SERVER_SINGLE_WRITER_H

#include <monitor/common.h>
#include <monitor/common/monitor_info.h>
#include <monitor/server/app_conf.h>
#include <monitor/server/app_writer.h>
#include <monitor/server/single_file_writer.h>
#include <monitor/server/server_common.h>
#include <map>
MONITOR_BS;

class SingleAppWriter : public AppWriter
{

public:
    SingleAppWriter(){}
    ~SingleAppWriter(){}
    
public:
    /*override*/int Init(AppConfPtr appConf, std::string appnameIn);

    /*override*/int Write(MonitorInfoPtr& monitorInfo);
    /*override*/std::string GetAppname(){return appname;}
private:
    void Split(std::string in, std::string& front, std::string& end);
private:

private:
    SingleFileWriterPtr cumulation;
    SingleFileWriterPtr other;

};
typedef std::tr1::shared_ptr<SingleAppWriter> SingleAppWriterPtr;
MONITOR_ES;

#endif //MONITOR_SINGLE_WRITER_H
