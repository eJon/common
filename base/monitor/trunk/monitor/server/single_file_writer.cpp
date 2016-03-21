#include <monitor/server/single_file_writer.h>
#include <sharelib/util/time_utility.h>
#include <sharelib/util/file_util.h>
using namespace std;
using namespace sharelib;
MONITOR_BS;



int SingleFileWriter::Init(AppConfPtr appconfIn, std::string fileNameIn){
    startTime = TimeUtility::CurrentTimeInSeconds();
    appconf = appconfIn;
    fileName = fileNameIn;
    of = new ofstream(fileName.c_str(), std::ios::app |std::ios::out);
    return 0;
}

void SingleFileWriter::WriteFunc(MonitorInfoPtr monitorInfo){
    int64_t now = TimeUtility::CurrentTimeInSeconds();
    if(now > startTime + appconf->maxFileTime){
        of->close();
        delete of;
        
        for(uint32_t i =appconf->maxFileCount - 2; appconf->maxFileCount > 2&& i > 0;i--){
            stringstream ss;
            ss << fileName << "." << i;
            if(FileUtil::IsFileExist(fileName)){
                stringstream ssn;
                ssn <<fileName << "." << i+1;
                FileUtil::Rename(ss.str(), ssn.str());
            }
        }
        FileUtil::Rename(fileName, fileName+".1");
        
        of = new ofstream(fileName.c_str(), std::ios::app |std::ios::out);
        startTime = now;
    }
    
    if(monitorInfo == NULL) return;
    std::string str = ToJsonStringCompact(monitorInfo.get());
    of->write( (str + "\n").c_str(), str.length() + 1);
    of->flush();
}


int SingleFileWriter::Write(MonitorInfoPtr monitorInfo){
    boost::mutex::scoped_lock lock(mutex_);  
    WriteFunc(NormalizeInfo(monitorInfo,appconf));
    return 0;
}

MonitorInfoPtr SingleFileWriter::NormalizeInfo(MonitorInfoPtr info, AppConfPtr appconf){
    return info;
}
MONITOR_ES;

