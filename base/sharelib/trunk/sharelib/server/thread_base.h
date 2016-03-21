#ifndef _SHARELIB_THREADBASE_H
#define _SHARELIB_THREADBASE_H
#include <sharelib/common.h>
#include <pthread.h>
SHARELIB_BS;
class ThreadBase {
public:
    virtual ~ThreadBase() {}
    int Run();
    int Stop();
private:
    static void *ThreadProc(void *arg);
    virtual void *InternalLoop() = 0;
protected:
    pthread_t pid;
    bool onRunning;
};
SHARELIB_ES;
#endif
