#ifndef MONITOR_SERVER_WRITER_MANAGER_H
#define MONITOR_SERVER_WRITER_MANAGER_H

#include <monitor/common.h>
#include <monitor/common/monitor_info.h>
#include <monitor/server/app_writer.h>
#include <tr1/unordered_map>
MONITOR_BS;

class WriterManager
{
public:
    typedef std::tr1::unordered_map<std::string, AppWriterPtr> AppWriterMap;
    typedef std::tr1::unordered_map<std::string, AppWriterPtr>::iterator AppWriterMapIt;
public:
    WriterManager();
    ~WriterManager();
public:
    AppWriterPtr GetWriter(std::string appname);
    //TODO add feature
    void AddWriter(std::string appname, AppConfPtr appConf);
    void AddWriter(std::string appname, AppWriterPtr writer);
public:
    static AppWriter* GenerateAppWriter();
        
    
private:
    AppWriterMap writerMap;
};

typedef std::tr1::shared_ptr<WriterManager> WriterManagerPtr;
MONITOR_ES;

#endif //MONITOR_WRITER_MANAGER_H
