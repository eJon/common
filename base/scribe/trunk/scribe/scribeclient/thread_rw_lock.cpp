#include "thread_rw_lock.h"
#include <stdlib.h>
#include <string.h>

thread_rw_lock_t *
thread_rw_lock_create()
{
    thread_rw_lock_t *lock = NULL;
    lock = (thread_rw_lock_t *)malloc(sizeof(thread_rw_lock_t));
    memset(lock, 0, sizeof(thread_rw_lock_t));
    lock->state = lock->s_undefine;
    if (0 != pthread_rwlock_init(&lock->rwlock, NULL)){
        free(lock);
        return NULL;
    }
    lock->state = lock->s_init;
    return lock;
}

int 
thread_rw_lock_destroy(thread_rw_lock_t *lock)
{
    if (NULL == lock || lock->s_init != lock->state){
        return -1;
    }
    int ret = -1;
    ret = pthread_rwlock_destroy(&lock->rwlock);
    if (0 == ret){
        lock->state = lock->s_undefine;
    }
    free(lock);
    return ret;
}

int 
thread_rw_lock_rdlock(thread_rw_lock_t *lock)
{
    if (NULL == lock || lock->s_init != lock->state){
        return -1;
    }
    return pthread_rwlock_rdlock(&lock->rwlock);
}

int 
thread_rw_lock_tryrdlock(thread_rw_lock_t *lock)
{
    if (NULL == lock || lock->s_init != lock->state){
        return -1;
    }
    return pthread_rwlock_tryrdlock(&lock->rwlock);
}

int 
thread_rw_lock_wrlock(thread_rw_lock_t *lock)
{
    if (NULL == lock || lock->s_init != lock->state){
        return -1;
    }
    return pthread_rwlock_wrlock(&lock->rwlock);
}

int 
thread_rw_lock_trywrlock(thread_rw_lock_t *lock)
{
    if (NULL == lock || lock->s_init != lock->state){
        return -1;
    }
    return pthread_rwlock_trywrlock(&lock->rwlock);
}

int 
thread_rw_lock_unlock(thread_rw_lock_t *lock)
{
    if ( NULL == lock || lock->s_init != lock->state){
        return -1;
    }
    return pthread_rwlock_unlock(&lock->rwlock);
}
