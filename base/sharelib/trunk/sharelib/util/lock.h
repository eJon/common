#ifndef SHARELIB_LOCK_H_
#define SHARELIB_LOCK_H_
#include <pthread.h>
#include <sharelib/common.h>
SHARELIB_BS

class RWLock
{
public:
    RWLock() {Init();}
    ~RWLock() {Destroy();}

    int Init() { return pthread_rwlock_init(&lock, NULL);}
    int Destroy() { return pthread_rwlock_destroy(&lock);}
    int WLock() { return pthread_rwlock_wrlock(&lock);}
    int RLock() { return pthread_rwlock_rdlock(&lock);}
    int TryWLock()
    {
        if (!pthread_rwlock_trywrlock(&lock))
            return 0;
        else
            return -1;
    }

    int TryRLock()
    {
        if (!pthread_rwlock_tryrdlock(&lock))
            return 0;
        else
            return -1;
    }
    int UnLock() { return pthread_rwlock_unlock(&lock);}

private:
    pthread_rwlock_t lock;
};

class MutexLock
{
public:
    MutexLock() {Init();}
    ~MutexLock() {Destroy();}

    int Init() { return pthread_mutex_init(&lock, NULL);}
    int Destroy() { return pthread_mutex_destroy(&lock);}
    int Lock() { return pthread_mutex_lock(&lock);}
    int UnLock() { return pthread_mutex_unlock(&lock);}
private:
    pthread_mutex_t lock;
};


SHARELIB_ES
#endif //SHARELIB_LOCK_H_

