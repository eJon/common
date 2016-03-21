#ifndef SHARELIB_THREAD_H
#define SHARELIB_THREAD_H

#include <sharelib/common.h>
#include <sharelib/util/runnable.h>
#include <sharelib/util/conditions.h>

SHARELIB_BS;

class ThreadPool;

enum _thread_stat_t {
    thr_unknown,
    thr_inited,
    thr_started,
    thr_running,
    thr_waiting,
    thr_terminated
};
typedef enum _thread_stat_t thread_stat_t;

class Thread : public Runnable {
public:
    Thread(uint32_t threadId = 0, uint32_t stack_size = 0, uint32_t stack_guard_size = 0,
           ThreadPool * p = NULL);
    ~Thread();
public:
    uint32_t GetThreadId();
    ret_t Start();
    ret_t Dispatch(RunnablePtr runner);
    //void interrupt();
    void Terminate();
    void Join();
    ret_t SetPriority(int priority, int policy);
    thread_stat_t GetStat() const { return _stat; }
    ret_t GetLastReturnValue() const { return _lastret; }
public:
    virtual ret_t Run();
protected:
    pthread_t _tid;
    uint32_t _stack_size;
    uint32_t _stack_guard_size;
    ret_t _lastret;
    volatile thread_stat_t _stat;
    RunnablePtr _runner;
    Conditions _conds;
public:
    ThreadPool * pool;
    Thread * next;
    Thread * prev;
private:
    uint32_t _threadId;
};

typedef std::tr1::shared_ptr<Thread> ThreadPtr;

SHARELIB_ES;


#endif //SHARELIB_THREAD_H
