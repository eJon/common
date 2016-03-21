#include <sharelib/util/thread_base.h>

int CThreadBase::Run()
{
	onRunning=true;
	return pthread_create(&pid,NULL,CThreadBase::ThreadProc,(void*)this);
}

int CThreadBase::Stop()
{
    void *res;
	onRunning=false;
	return pthread_join(pid, &res);
}


void* CThreadBase::ThreadProc(void *arg)
{
	CThreadBase* me=(CThreadBase*)arg;
    me->pid = pthread_self();
	return me->InternalProcess();
}


int CThreadBase::Detach()
{
    return pthread_detach(pid);
}

