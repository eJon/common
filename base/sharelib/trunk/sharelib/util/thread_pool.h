#ifndef SHARELIB_THREAD_POOL_H
#define SHARELIB_THREAD_POOL_H

#include <sharelib/common.h>

#include <sharelib/util/thread.h>

SHARELIB_BS;

class ThreadPool {
public:
    ThreadPool(uint32_t capability = 20, uint32_t stack_size = 0);
    ~ThreadPool();
public:
    ret_t NewThread(Thread ** thrp, RunnablePtr runner);
    void Join();
    void Terminate();
private:
    friend class Thread;
    ret_t FreeThread(Thread * thr);
    ret_t TerminateThread(Thread * thr);
    
/*
    void interrupt();
*/
    
public:
    uint32_t GetFreeCount() const { return _freecnt; }
    uint32_t GetBusyCount() const { return _busycnt; }
    uint32_t GetTerminatedCount() const { return _termcnt; }
    uint32_t Size() const { return _size; }
    uint32_t Capability() const { return _capability; }
    bool IsTerminated() const { return _terminated; }
private:
    Thread * _freelist;
    uint32_t _freecnt;
    Thread * _busylist;
    uint32_t _busycnt;
    Thread * _termlist;
    uint32_t _termcnt;
    uint32_t _size;
    uint32_t _capability;
    uint32_t _stack_size;
    uint32_t _stack_guard_size;
    bool _terminated;
    Conditions _conds;
private:
    uint32_t _maxThreadId;
private:

};

typedef std::tr1::shared_ptr<ThreadPool> ThreadPoolPtr;

SHARELIB_ES;


#endif // SHARELIB_THREAD_POOL_H
