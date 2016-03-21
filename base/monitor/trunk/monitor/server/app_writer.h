#ifndef MONITOR_SERVER_APP_WRITER_H
#define MONITOR_SERVER_APP_WRITER_H

#include <monitor/common.h>
#include <monitor/common/log.h>
#include <monitor/server/app_conf.h>
#include <monitor/common/monitor_info.h>
#include <sharelib/util/file_util.h>
MONITOR_BS;

class AppWriter
{
public:
    AppWriter(){}
    virtual ~AppWriter(){}
public:
    virtual int Init(AppConfPtr appConfIn, std::string appnameIn){
        appConf = appConfIn;
        appname = appnameIn;
        writeDir = appConf->writePath;
        bool mk = sharelib::FileUtil::MakeLocalDir(writeDir, true);
        if(!mk){
            MONITOR_LOG_ERROR("error mkdir dir %s", writeDir.c_str());
            return -1;
        }
        return 0;
    }
    virtual int Write(MonitorInfoPtr& monitorInfo) = 0;
    virtual std::string GetAppname() = 0;
protected:
    AppConfPtr appConf;
    std::string writeDir;
    std::string appname;
};

typedef std::tr1::shared_ptr<AppWriter> AppWriterPtr;
MONITOR_ES;

#endif //MONITOR_APP_WRITER_H
