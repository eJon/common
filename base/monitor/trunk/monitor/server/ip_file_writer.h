#ifndef MONITOR_SERVER_IP_FILE_WRITER_H
#define MONITOR_SERVER_IP_FILE_WRITER_H

#include <monitor/common.h>
#include <monitor/server/file_writer.h>
#include <monitor/server/kv_file_writer.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

MONITOR_BS;

class IpFileWriter : public KvFileWriter
{
public:
    typedef std::map<std::string, FileWriterPtr>::iterator MapIt;
public:
    IpFileWriter();
    ~IpFileWriter();
public:
    int Init(AppConfPtr appconf, std::string fileName);
    int Write(MonitorInfoPtr monitorInfo);
private:
    std::map<std::string, FileWriterPtr> ipWriters;
    mutable boost::mutex mutex_;
    AppConfPtr appconf;
    std::string writedir;
};

MONITOR_ES;

#endif //MONITOR_IP_FILE_WRITER_H
