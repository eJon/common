#ifndef MONITOR_SERVER_KV_FILE_WRITER_H
#define MONITOR_SERVER_KV_FILE_WRITER_H

#include <monitor/common.h>
#include <monitor/common/log.h>
#include <monitor/server/single_file_writer.h>
#include <monitor/common/monitor_info.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <fstream>

MONITOR_BS;
class KvFileWriter : public SingleFileWriter
{
public:
    KvFileWriter(){}
    virtual ~KvFileWriter(){}

public:
    virtual MonitorInfoPtr NormalizeInfo(MonitorInfoPtr info, AppConfPtr appconf);
protected:

};
    
typedef std::tr1::shared_ptr<KvFileWriter> KvFileWriterPtr;
MONITOR_ES;

#endif //MONITOR_SINGLE_KV_WRITER_H
