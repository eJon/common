#ifndef _THREAD_PROC_H_
#define _THREAD_PROC_H_
#include <pthread.h>
#include "thread_sem.h"

typedef void * (*thread_func)(int args, void **args_data);

struct thread_info_t {
    thread_func     func;
    int             args;
    void            **args_data;
    pthread_t       thd;
    thread_sem_t    *thread_sem;
};

struct thread_t {
    thread_info_t   *thread_info;
    int             thread_num;
    int             thread_running;
    thread_sem_t    *thread_sem;
    pthread_attr_t  thd_attr;
};


thread_t *thread_create();
int thread_add(thread_t *thd, thread_func func, int args, ...);
int thread_start(thread_t *thd);
int thread_destroy(thread_t *thd);
int thread_join(thread_t *thd);


#endif
