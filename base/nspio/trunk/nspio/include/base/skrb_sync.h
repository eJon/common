// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/base/skrb_sync.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_SKRB_SYNC_
#define _H_SKRB_SYNC_

#include <stdint.h>
#include <inttypes.h>
#include "base/skrb.h"
#include "sync/pmutex.h"
#include "sync/pspinlock.h"


using namespace nspio;

NSPIO_DECLARATION_START

#define skrb_mutex_empty(tree, mutex) ({	\
	    bool __empty(false);		\
	    pmutex_lock(mutex);			\
	    __empty = skrb_empty(tree);		\
	    pmutex_unlock(mutex);		\
	    __empty;				\
	})

#define skrb_mutex_min(tree, mutex) ({		\
	    skrb_node_t *__node = NULL;		\
	    pmutex_lock(mutex);			\
	    __node = skrb_min(tree);		\
	    pmutex_unlock(mutex);		\
	    __node;				\
	})

#define skrb_mutex_max(tree, mutex) ({		\
	    skrb_node_t *__node = NULL;		\
	    pmutex_lock(mutex);			\
	    __node = skrb_max(tree);		\
	    pmutex_unlock(mutex);		\
	    __node;				\
	})

#define skrb_mutex_insert(tree, node, mutex) do {	\
	pmutex_lock(mutex);				\
	skrb_insert(tree, node);			\
	pmutex_unlock(mutex);				\
    } while (0)

#define skrb_mutex_delete(tree, node, mutex) do {	\
	pmutex_lock(mutex);				\
	skrb_delete(tree, node);			\
	pmutex_unlock(mutex);				\
    } while (0)


#define skrb_spin_empty(tree, spin) ({		\
	    bool __empty(false);		\
	    pspin_lock(spin);			\
	    __empty = skrb_empty(tree);		\
	    pspin_unlock(spin);			\
	    __empty;				\
	})

#define skrb_spin_min(tree, spin) ({		\
	    skrb_node_t *__node = NULL;		\
	    pspin_lock(spin);			\
	    __node = skrb_min(tree);		\
	    pspin_unlock(spin);			\
	    __node;				\
	})

#define skrb_spin_max(tree, spin) ({		\
	    skrb_node_t *__node = NULL;		\
	    pspin_lock(spin);			\
	    __node = skrb_max(tree);		\
	    pspin_unlock(spin);			\
	    __node;				\
	})

#define skrb_spin_insert(tree, node, spin) do {	\
	pspin_lock(spin);			\
	skrb_insert(tree, node);		\
	pspin_unlock(spin);			\
    } while (0)

#define skrb_spin_delete(tree, node, spin) do {	\
	pspin_lock(spin);			\
	skrb_delete(tree, node);		\
	pspin_unlock(spin);			\
    } while (0)

}
#endif /* _H_SKRB_ */
