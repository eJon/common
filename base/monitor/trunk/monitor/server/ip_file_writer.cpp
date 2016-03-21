#include <monitor/server/ip_file_writer.h>
using namespace std;
MONITOR_BS;

IpFileWriter::IpFileWriter(){
}

IpFileWriter::~IpFileWriter() { 
    ipWriters.clear();
}

int IpFileWriter::Init(AppConfPtr appconfIn, std::string fileName)
{
    appconf = appconfIn;
    writedir = fileName;
    return 0;
}
int IpFileWriter::Write(MonitorInfoPtr monitorInfo)
{
    FileWriterPtr fileWriter;
    boost::mutex::scoped_lock lock(mutex_);
    MapIt it = ipWriters.find(monitorInfo->ip);
    if(it == ipWriters.end()){
        std::string file = writedir + "/" + monitorInfo->ip;
        fileWriter.reset(new KvFileWriter);
        if(fileWriter->Init(appconf, file) != 0){
            return -1;
        }
        ipWriters[monitorInfo->ip] = fileWriter;
    }else{
        fileWriter = it->second;
    }
    fileWriter->Write(monitorInfo);
    return 0;
}
MONITOR_ES;

