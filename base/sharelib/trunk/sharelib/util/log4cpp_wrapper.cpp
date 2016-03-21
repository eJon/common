/**
 * 线程安全的日志，改自"Log.cpp"
 */
#include <log4cpp/PropertyConfigurator.hh>
#include <iostream>
#ifndef WIN32
#include <unistd.h>
#else
#include <process.h>
#endif
#include <stdio.h>
#include <sstream>
#include <sharelib/util/log4cpp_wrapper.h>

using namespace log4cpp;
using namespace std;
SHARELIB_US;

Log4cppWrapper::ThreadSafeLogMap Log4cppWrapper::categoryMap;
bool Log4cppWrapper::Init(std::string configFile)
{
    try
    {
        PropertyConfigurator::configure(configFile);
    }
    catch(ConfigureFailure& f)
    {
        cout << "Log4Cpp Configure Problem !" << f.what() << endl;
        return false;
    }
    return  InitAfterLogr();
}

bool Log4cppWrapper::InitAfterLogr()
{
    
    vector<Category*>* cateVec = Category::getCurrentCategories();
    string name;
    ThreadSafeLog* log;
    for(uint32_t i =0 ;i < cateVec->size();i++){
        log = new ThreadSafeLog();
        log->SetCategory((*cateVec)[i]);
        name = (*cateVec)[i]->getName();
        cout << "cate name is" << name << endl;
        if(name == "") 
        {
            categoryMap["rootCategory"] = log;
        }else{
            categoryMap[name] = log;
        }
    }
    delete cateVec;
    return true;
}

void Log4cppWrapper::ShutdownBeforeLogr(){
    for (ThreadSafeLogMapIter it = categoryMap.begin();
         it != categoryMap.end();it++){
        delete it->second;
    }        
}
void Log4cppWrapper::Shutdown()
{
    log4cpp::Category::shutdown(); 
    ShutdownBeforeLogr();
}






///////////////////////////////////////////////////////////////

void ThreadSafeLog::Warn(const char* format, ...)
{
    if (m_category->isPriorityEnabled(Priority::WARN))
    {
        threading::ScopedLock lock(*m_mutex);
        va_list ap;
        va_start(ap,format);
        m_category->logva(Priority::WARN,format,ap);
        va_end(ap);
    }
}

void ThreadSafeLog::Error(const char* format, ...)
{
    if (m_category->isPriorityEnabled(Priority::ERROR))
    {
        threading::ScopedLock lock(*m_mutex);
        va_list ap;
        va_start(ap,format);
        m_category->logva(Priority::ERROR,format,ap);
        va_end(ap);
    }
}
void ThreadSafeLog::Info(const char* format, ...)
{
    if (m_category->isPriorityEnabled(Priority::INFO))
    {
        threading::ScopedLock lock(*m_mutex);
        va_list ap;
        va_start(ap,format);
        m_category->logva(Priority::INFO,format,ap);
        va_end(ap);
    }
}


void ThreadSafeLog::Notice(const char* format, ...)
{
    if (m_category->isPriorityEnabled(Priority::NOTICE))
    {
        threading::ScopedLock lock(*m_mutex);
        va_list ap;
        va_start(ap,format);
        m_category->logva(Priority::NOTICE,format,ap);
        va_end(ap);
    }
}


void ThreadSafeLog::Debug(const char* format, ...)
{
    if (m_category->isPriorityEnabled(Priority::DEBUG))
    {
        threading::ScopedLock lock(*m_mutex);
        va_list ap;
        va_start(ap,format);
        m_category->logva(Priority::DEBUG,format,ap);
        va_end(ap);
    }
}


void ThreadSafeLog::Crit(const char* format, ...)
{
    if (m_category->isPriorityEnabled(Priority::CRIT))
    {
        threading::ScopedLock lock(*m_mutex);
        va_list ap;
        va_start(ap,format);
        m_category->logva(Priority::CRIT,format,ap);
        va_end(ap);
    }
}


void ThreadSafeLog::Fatal(const char* format, ...)
{
    if (m_category->isPriorityEnabled(Priority::FATAL))
    {
        threading::ScopedLock lock(*m_mutex);
        va_list ap;
        va_start(ap,format);
        m_category->logva(Priority::FATAL,format,ap);
        va_end(ap);
    }
}

