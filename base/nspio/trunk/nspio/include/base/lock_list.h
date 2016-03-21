// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/base/lock_list.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _LOCK_LIST_H_
#define _LOCK_LIST_H_

#include "sync/pmutex.h"
#include "base/list.h"

NSPIO_DECLARATION_START

struct lock_list {
    pmutex_t lock;
    struct list_head head;
};

#define INIT_LOCKLIST_HEAD(lhead) do {		\
	pmutex_init(&(lhead)->lock);		\
	INIT_LIST_HEAD(&(lhead)->head);		\
    } while (0)


#define DESTROY_LOCKLIST_HEAD(lhead) do {	\
	pmutex_destroy(&(lhead)->lock);		\
    } while (0)


static inline void lock_list_splice(struct list_head *head, struct lock_list *ll) {
    pmutex_lock(&ll->lock);
    list_splice(head, &ll->head);
    pmutex_unlock(&ll->lock);
}


}
#endif   // _LOCK_LIST_H_
