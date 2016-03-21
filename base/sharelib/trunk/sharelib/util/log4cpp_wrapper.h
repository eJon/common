/**
 * 线程安全的日志，改自Log.h
 */
#ifndef SHARELIB_LOG4CPP_WRAPPER_HPP
#define SHARELIB_LOG4CPP_WRAPPER_HPP

#include <string>
#include <log4cpp/Category.hh>
#include <log4cpp/CategoryStream.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/threading/PThreads.hh>
#include <sharelib/common.h>
#include <stdio.h>
SHARELIB_BS
class ThreadSafeLog;
class Log4cppWrapper{
public:
    typedef std::map<std::string, ThreadSafeLog*> ThreadSafeLogMap;
    typedef std::map<std::string, ThreadSafeLog*>::const_iterator ThreadSafeLogMapIter;
public:
    static bool Init(std::string configFile);
    static bool InitAfterLogr();
    static inline ThreadSafeLog* GetLog(const std::string& cateName){
        
        ThreadSafeLogMapIter it = categoryMap.find(cateName);
        if(it == categoryMap.end()){
            return categoryMap["rootCategory"];
        }else{
            return it->second;
        }   
    }
    static inline ThreadSafeLog* GetLog(const char* cateName){
        std::string catename(cateName);
        return GetLog(catename);
    }
    
    static inline ThreadSafeLog* GetRootLog(){
        ThreadSafeLogMapIter it = categoryMap.find("rootCategory");
        if(it == categoryMap.end()){
            return NULL;
        }
        return it->second;   
    }
    static void Shutdown();
    static void ShutdownBeforeLogr();
private:
    
    static ThreadSafeLogMap categoryMap;
};

class ThreadSafeLog
{
public:
    ThreadSafeLog(){
        m_category = NULL;
        m_mutex = new log4cpp::threading::Mutex();
    }
    ~ThreadSafeLog(){
        delete m_mutex;
    }
public:
    void SetCategory(log4cpp::Category* category){
        assert(category != NULL);
        m_category = category;
        m_modName = category->getName();
    }
public:
    void Debug(const char* format, ...);
    void Info(const char* format, ...)  ;
    void Notice(const char* format, ...);
    void Warn(const char* format, ...) ;
    void Error(const char* format, ...) ;
    void Crit(const char* format, ...) ;
    void Fatal(const char* format, ...) ;
private:
private:
    log4cpp::Category* m_category;
    log4cpp::threading::Mutex* m_mutex;
    std::string m_modName;

};		

SHARELIB_ES
#endif
