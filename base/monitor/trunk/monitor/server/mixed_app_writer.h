#ifndef MONITOR_SERVER_MIXED_APP_WRITER_H
#define MONITOR_SERVER_MIXED_APP_WRITER_H

#include <monitor/common.h>
#include <monitor/server/app_writer.h>
#include <monitor/server/file_writer.h>
#include <monitor/server/app_conf.h>
#include <monitor/server/server_common.h>
MONITOR_BS;

class MixedAppWriter :public AppWriter
{
public:
    MixedAppWriter();
    ~MixedAppWriter();
public:
    /*override*/int Init(AppConfPtr appConf, std::string appnameIn);
    
    /*override*/int Write(MonitorInfoPtr& monitorInfo);
    /*override*/std::string GetAppname(){return appname;}

private:
    FileWriterPtr rawWriter;
    FileWriterPtr snapShotWriter;
    FileWriterPtr aggregateWriter;
    FileWriterPtr ipWriter; 
private:
};

MONITOR_ES;

#endif //MONITOR_MIXED_APP_WRITER_H
