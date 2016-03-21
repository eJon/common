#include <sharelib/util/conditions.h>
#include <sys/time.h>
#include <errno.h>
#include <assert.h>
SHARELIB_BS;

Conditions::Conditions(uint32_t n):_unCount(n), _conds(NULL) {
    assert(n > 0);
    _conds = (pthread_cond_t *)malloc(sizeof(pthread_cond_t) * n);
    assert(_conds != NULL);
    int r;
    for( uint32_t i = 0; i < n; i++ ) {
        r = pthread_cond_init(&_conds[i], NULL);
        if( r != 0 ) {
            for( uint32_t j = 0; j < i; j++ ) {
                pthread_cond_destroy(&_conds[j]);
            }
            free(_conds);
            _conds = NULL;
        }
        assert(r == 0);
    }
    r = pthread_mutex_init(&_lock, NULL);
    if( r != 0 ) {
        for( uint32_t j = 0; j < n; j++ ) {
            pthread_cond_destroy(&_conds[j]);
        }
        free(_conds);
        _conds = NULL;
    }
    assert(r == 0);
}

Conditions::~Conditions() {
    int r;
    r = pthread_mutex_destroy(&_lock);
    assert(r == 0);
    if( _conds != NULL ) {
        for( uint32_t i = 0; i < _unCount; i++ ) {
            int ret = pthread_cond_destroy(&_conds[i]);
            if( ret != 0 ) r = ret;
        }
        free(_conds);
        _conds = NULL;
        assert(r == 0);
    }
}

ret_t Conditions::lock() {
    return (pthread_mutex_lock(&_lock) ? r_succeed : r_failed);
}

ret_t Conditions::unlock() {
    return (pthread_mutex_unlock(&_lock) ? r_succeed : r_failed);
}

ret_t Conditions::wait(uint32_t idx) {
    if( unlikely(idx >= _unCount) ) return r_failed;
    return (pthread_cond_wait(&_conds[idx], &_lock) ? r_succeed : r_failed);
}

ret_t Conditions::timedwait(uint32_t idx, uint32_t ms) {
    if( unlikely(idx >= _unCount) ) return r_failed;
    if( ms == 0 ) {
        return wait(idx);
    }
    struct timespec tim;
    struct timeval  tv;
    gettimeofday(&tv , 0);
    tim.tv_sec = ms / 1000 + tv.tv_sec + (ms % 1000 + tv.tv_usec / 1000) / 1000;
    tim.tv_nsec = (ms % 1000 * 1000 + tv.tv_usec) % 1000000 * 1000;
    int ret = pthread_cond_timedwait(&_conds[idx], &_lock, &tim);
    if( ret == ETIMEDOUT ) {
        return r_eagain;
    }
    return (ret == 0 ? r_succeed : r_failed);
}

ret_t Conditions::signal(uint32_t idx) {
    if( unlikely(idx >= _unCount) ) return r_failed;
    return (pthread_cond_signal(&_conds[idx]) ? r_succeed : r_failed);
}

ret_t Conditions::broadcast(uint32_t idx) {
    if( unlikely(idx >= _unCount) ) return r_failed;
    return (pthread_cond_broadcast(&_conds[idx]) ? r_succeed : r_failed);
}

SHARELIB_ES;


