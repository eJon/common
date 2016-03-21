#ifndef MONITOR_SERVER_FILE_WRITER_H
#define MONITOR_SERVER_FILE_WRITER_H

#include <monitor/common.h>
#include <monitor/common/monitor_info.h>
#include <monitor/server/app_conf.h>
MONITOR_BS;

class FileWriter
{
public:
    FileWriter(){}
    virtual ~FileWriter(){}
public:
    virtual int Init(AppConfPtr appconf, std::string fileName) = 0;
    virtual int Write(MonitorInfoPtr monitorInfo) = 0;
private:
};

typedef std::tr1::shared_ptr<FileWriter> FileWriterPtr;
MONITOR_ES;

#endif //MONITOR_FILE_WRITER_H
