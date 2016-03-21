#include <sharelib/util/condition.h>
#include <sys/time.h>
#include <errno.h>
#include <assert.h>
SHARELIB_BS;
Condition::Condition() {
    int ret = pthread_mutex_init(&_lock, NULL);
    assert(ret == 0);
    ret = pthread_cond_init(&_cond, NULL);
    assert(ret == 0);

}

Condition::~Condition() {
    int ret = pthread_cond_destroy(&_cond);
    assert(ret == 0);
    ret = pthread_mutex_destroy(&_lock);
    assert(ret == 0);
}

ret_t Condition::Lock() {
    return (pthread_mutex_lock(&_lock) == 0 ? r_succeed : r_failed);
}

ret_t Condition::Unlock() {
    return (pthread_mutex_unlock(&_lock) == 0 ? r_succeed : r_failed);
}

ret_t Condition::Wait() {
    return (pthread_cond_wait(&_cond, &_lock) == 0 ? r_succeed : r_failed);
}

ret_t Condition::Timedwait(uint32_t ms) {
    if( unlikely(ms == 0) ) {
        return Wait();
    }
    struct timespec tim;
    struct timeval  tv;
    gettimeofday(&tv , 0);
    tim.tv_sec = ms / 1000 + tv.tv_sec + (ms % 1000 + tv.tv_usec / 1000) / 1000;
    tim.tv_nsec = (ms % 1000 * 1000 + tv.tv_usec) % 1000000 * 1000;
    int ret = pthread_cond_timedwait(&_cond, &_lock, &tim);
    if( ret == ETIMEDOUT ) {
        return r_eagain;
    }
    return (ret == 0 ? r_succeed : r_failed);
}

ret_t Condition::Signal() {
    return (pthread_cond_signal(&_cond) ? r_succeed : r_failed);
}

ret_t Condition::Broadcast() {
    return (pthread_cond_broadcast(&_cond) ? r_succeed : r_failed);
}

SHARELIB_ES;
