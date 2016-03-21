#ifndef _THREAD_RW_LOCK_H_
#define _THREAD_RW_LOCK_H_
#include <pthread.h>

struct thread_rw_lock_t 
{
    enum lock_state{
        s_undefine,
        s_init
    };
    lock_state        state;
    pthread_rwlock_t  rwlock;
};

thread_rw_lock_t *thread_rw_lock_create();
int thread_rw_lock_destroy(thread_rw_lock_t *lock);
int thread_rw_lock_rdlock(thread_rw_lock_t *lock);
int thread_rw_lock_tryrdlock(thread_rw_lock_t *lock);
int thread_rw_lock_wrlock(thread_rw_lock_t *lock);
int thread_rw_lock_trywrlock(thread_rw_lock_t *lock);
int thread_rw_lock_unlock(thread_rw_lock_t *lock);
#endif
