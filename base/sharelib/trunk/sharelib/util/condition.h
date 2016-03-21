#ifndef SHARELIB_UTIL_CONDITION_H
#define SHARELIB_UTIL_CONDITION_H
#include <tr1/memory>
#include <sharelib/common.h>

SHARELIB_BS;
class Condition
{
public:
    Condition();
    ~Condition();
private:
    COPY_CONSTRUCTOR(Condition);
public:
    ret_t Lock();
    ret_t Unlock();
    ret_t Wait();
    ret_t Timedwait(uint32_t ms);
    ret_t Signal();
    ret_t Broadcast();
private:
    pthread_mutex_t _lock;
    pthread_cond_t _cond;
private:
};

typedef std::tr1::shared_ptr<Condition> ConditionPtr;

SHARELIB_ES;
#endif //SHARELIB_CONDITION_H
