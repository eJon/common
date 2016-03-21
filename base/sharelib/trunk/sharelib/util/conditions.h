#ifndef SHARELIB_CONDITIONS_H
#define SHARELIB_CONDITIONS_H

#include <sharelib/common.h>
#include <pthread.h>
#include <stdint.h>
SHARELIB_BS;

class Conditions {
public:
    Conditions(uint32_t n);
    ~Conditions();
private:
    COPY_CONSTRUCTOR(Conditions);
public:
    ret_t lock();
    ret_t unlock();
    ret_t wait(uint32_t idx);
    ret_t timedwait(uint32_t idx, uint32_t ms);
    ret_t signal(uint32_t idx);
    ret_t broadcast(uint32_t idx);
private:
    uint32_t _unCount;
    pthread_cond_t * _conds;
    pthread_mutex_t _lock;
};

SHARELIB_ES;

#endif //SHARELIB_CONDITIONS_H
