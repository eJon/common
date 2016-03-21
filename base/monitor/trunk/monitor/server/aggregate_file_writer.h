#ifndef MONITOR_SERVER_AGGREGATE_FILE_WRITER_H
#define MONITOR_SERVER_AGGREGATE_FILE_WRITER_H

#include <monitor/common.h>
#include <monitor/common/log.h>
#include <monitor/server/server_common.h>
#include <monitor/server/kv_file_writer.h>

MONITOR_BS;
class MonitorInfoWithTime{
public:
    MonitorInfoWithTime(MonitorInfoPtr ptrIn, int64_t timeIn){
        info = ptrIn;
        timeSecs = timeIn;
    }
    MonitorInfoPtr info;
    int64_t timeSecs;
};

class AggregateFileWriter : public KvFileWriter
{
public:
    AggregateFileWriter(){
        aggrStart = 0;
    }
    ~AggregateFileWriter();
public:
    /*override*/int Write(MonitorInfoPtr monitorInfo);
    
    /*override*/MonitorInfoPtr NormalizeInfo(std::vector<MonitorInfoWithTime>& infos, AppConfPtr appConf);
private:
    std::string IpSet(std::vector<MonitorInfoWithTime>& infos);
    std::string ThreadSet(std::vector<MonitorInfoWithTime>& infos);
private:
    std::vector<MonitorInfoWithTime> infos;
    int64_t aggrStart;
};

MONITOR_ES;

#endif //MONITOR_AGGREGATE_FILE_WRITER_H
