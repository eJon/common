#include <sharelib/server/thread_base.h>

SHARELIB_BS;
int ThreadBase::Run() {
    onRunning = true;
    return pthread_create(&pid, NULL, ThreadBase::ThreadProc, (void *)this);
}

int ThreadBase::Stop() {
    void *res;
    onRunning = false;
    return pthread_join(pid, &res);
}


void *ThreadBase::ThreadProc(void *arg) {
    ThreadBase *me = (ThreadBase *)arg;
    return me->InternalLoop();
}
SHARELIB_ES;
