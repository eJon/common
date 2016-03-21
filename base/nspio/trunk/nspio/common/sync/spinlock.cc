// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/sync/spinlock.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include "sync/pspinlock.h"


NSPIO_DECLARATION_START

int pspin_init(pspin_t *spin) {
    pthread_spinlock_t *lock = (pthread_spinlock_t *)spin;
    return pthread_spin_init(lock, PTHREAD_PROCESS_SHARED);
}


int pspin_lock(pspin_t *spin) {
    int ret = 0;
    pthread_spinlock_t *lock = (pthread_spinlock_t *)spin;

    ret = pthread_spin_lock(lock);

    return ret;
}

int pspin_unlock(pspin_t *spin) {
    int ret = 0;
    pthread_spinlock_t *lock = (pthread_spinlock_t *)spin;
    ret = pthread_spin_unlock(lock);
    return ret;
}


int pspin_destroy(pspin_t *spin) {
    int ret = 0;
    pthread_spinlock_t *lock = (pthread_spinlock_t *)spin;    
    ret = pthread_spin_destroy(lock);
    return ret;
}


}
