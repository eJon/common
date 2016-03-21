#include "thread_proc.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static void *
dummy_work(void *args) {
    thread_info_t *thd = (thread_info_t *)args;

    if(NULL == thd) {
        return NULL;
    }

    void *ret = thd->func(thd->args, thd->args_data);
    thread_sem_signal(thd->thread_sem);
    return ret;
}

thread_t *
thread_create() {
    thread_t *thd = NULL;
    thd = (thread_t *)malloc(sizeof(thread_t));
    memset(thd, 0, sizeof(thread_t));
    thd->thread_sem = thread_sem_create();

    if(NULL == thd->thread_sem) {
        free(thd);
        return NULL;
    }

    pthread_attr_init(&thd->thd_attr);
    pthread_attr_setdetachstate(&thd->thd_attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&thd->thd_attr, 0);
    return thd;
}

int
thread_start(thread_t *thd) {
    if(NULL == thd || NULL == thd->thread_info) {
        return -1;
    }

    for(int count = 0; count < thd->thread_num; count++) {
        pthread_create(&thd->thread_info[count].thd, NULL, dummy_work, &thd->thread_info[count]);
        //pthread_create(&thd->thread_info[count].thd, &thd->thd_attr, dummy_work, &thd->thread_info[count]);
        thd->thread_running++;
    }

    return 0;
}

int
thread_destroy(thread_t *thd) {
    if(NULL == thd) {
        return -1;
    }

    int ret = -1;

    for(int count = 0; count < thd->thread_num; count++) {
        if(NULL != thd->thread_info[count].args_data) {
            free(thd->thread_info[count].args_data);
            thd->thread_info[count].args_data = NULL;
        }
    }

    if(NULL != thd->thread_info) {
        free(thd->thread_info);
        thd->thread_info = NULL;
    }

    if(NULL != thd->thread_sem) {
        thread_sem_destroy(thd->thread_sem);
    }

    pthread_attr_destroy(&thd->thd_attr);
    free(thd);
    return ret;
}

int
thread_join(thread_t *thd) {
    if(NULL == thd || NULL == thd->thread_sem) {
        return -1;
    }

    void *value = NULL;

    while(0 < thd->thread_running) {
        pthread_join(thd->thread_info[thd->thread_running - 1].thd, &value);

        if(0 == thread_sem_wait(thd->thread_sem)) {
            thd->thread_running--;
        }
    }

    return 0;
}

int
thread_add(thread_t *thd, thread_func func, int args, ...) {
    if(NULL == thd || 0 > args || NULL == func) {
        return -1;
    }

    void *temp = realloc(thd->thread_info, sizeof(thread_info_t) * (thd->thread_num + 1));

    if(NULL == temp) {
        thread_destroy(thd);
        return -1;
    }

    thd->thread_info = (thread_info_t *)temp;
    thd->thread_info[thd->thread_num].thread_sem = thd->thread_sem;
    thd->thread_info[thd->thread_num].func = func;
    thd->thread_info[thd->thread_num].args = args;
    thd->thread_info[thd->thread_num].args_data = (void **)malloc(sizeof(void *) * thd->thread_info[thd->thread_num].args);
    memset(thd->thread_info[thd->thread_num].args_data, 0, sizeof(void *) * thd->thread_info[thd->thread_num].args);
    va_list ap;
    va_start(ap, args);

    for(int i = 0; i < thd->thread_info[thd->thread_num].args; i++) {
        * (((char **)thd->thread_info[thd->thread_num].args_data) + i) = (char *)va_arg(ap, void *);
    }

    va_end(ap);
    thd->thread_num++;
    return 0;
}
