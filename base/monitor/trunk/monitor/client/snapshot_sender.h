#ifndef MONITOR_CLIENT_SNAPSHORT_SENDER_H
#define MONITOR_CLIENT_SNAPSHORT_SENDER_H

#include <monitor/common.h>
#include <monitor/client/monitor_transporter.h>
#include <monitor/common/monitor_info.h>
#include <sharelib/task/concurrent_queue.h>
#include <monitor/client/client_common.h>
#include <sharelib/util/thread.h>
MONITOR_BS;

typedef std::tr1::shared_ptr<CurrentQueue<MonitorInfoPtr> > InfoCurrentQueuePtr;

class SnapshotSender : public sharelib::Thread
{
public:
    SnapshotSender();
    ~SnapshotSender();
public:
    void Init(uint32_t maxsize = 1000);
    /*override*/ret_t Run();
public:
    void Send(std::string& info, std::string& appid, 
              std::string& dataid, std::string& ip, 
              pthread_t threadid, std::string time);
    void SetTransporter(MonitorTransporterPtr transporterIn){
        transporter = transporterIn;
    }
public:
    static int64_t pkgCount ;    
private:
    bool started;
    uint32_t maxQueueSize;
    InfoCurrentQueuePtr queue;
    MonitorTransporterPtr transporter;


};

typedef std::tr1::shared_ptr<SnapshotSender> SnapshotSenderPtr;
MONITOR_ES;

#endif //MONITOR_SNAPSHORT_SENDER_H
