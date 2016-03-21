#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "thread_sem.h"

thread_sem_t *
thread_sem_create(int shared, size_t value) {
    thread_sem_t *thread_sem = NULL;
    thread_sem = (thread_sem_t *)malloc(sizeof(thread_sem_t));
    memset(thread_sem, 0, sizeof(thread_sem_t));
    thread_sem->shared = shared;
    thread_sem->init_value = value;
    thread_sem->state = thread_sem->s_undefine;

    if(0 != sem_init(&thread_sem->cond, thread_sem->shared, thread_sem->init_value)) {
        free(thread_sem);
        return NULL;
    }

    thread_sem->state = thread_sem->s_init;
    return thread_sem;
}

int
thread_sem_destroy(thread_sem_t *thread_sem) {
    if(NULL == thread_sem || thread_sem->s_init != thread_sem->state) {
        return -1;
    }

    int ret = sem_destroy(&thread_sem->cond);

    if(0 == ret) {
        thread_sem->state = thread_sem->s_undefine;
    }

    free(thread_sem);
    return ret;
}

int
thread_sem_signal(thread_sem_t *thread_sem) {
    if(NULL == thread_sem || thread_sem->s_init != thread_sem->state) {
        return -1;
    }

    return sem_post(&thread_sem->cond);
}

int
thread_sem_wait(thread_sem_t *thread_sem) {
    if(NULL == thread_sem || thread_sem->s_init != thread_sem->state) {
        return -1;
    }

    while(0 != sem_wait(&thread_sem->cond) && EINTR == errno);

    return 0;
}
