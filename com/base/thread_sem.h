#ifndef _THREAD_SEM_H_
#define _THREAD_SEM_H_
#include <semaphore.h>

struct thread_sem_t {
    sem_t cond;
    int shared;
    int init_value;
    enum sem_state {
        s_undefine,
        s_init
    };
    sem_state        state;
};

thread_sem_t *thread_sem_create(int shared = 0, size_t value = 0);
int thread_sem_destroy(thread_sem_t *thread_sem);
int thread_sem_signal(thread_sem_t *thread_sem);
int thread_sem_wait(thread_sem_t *thread_sem);

#endif
