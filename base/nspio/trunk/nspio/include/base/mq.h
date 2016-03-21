// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/base/mq.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_BASEMQ_
#define _H_BASEMQ_

#include "base/list.h"

NSPIO_DECLARATION_START


#define DEFINE_MQ(__class, __type, __node)				\
    class __class {							\
    public:								\
	__class() {							\
	    ref = 0; size = 0; cap = 0;					\
	    INIT_LIST_HEAD(&massages_queue);				\
	}								\
	~__class() {}							\
									\
	int Init(int qcap) {						\
	    ref = 0;							\
	    cap = qcap;							\
	    return 0;							\
	}								\
	inline int Ref() {						\
	    ref++;							\
	    return ref;							\
	}								\
	inline int unRef() {						\
	    ref--;							\
	    return ref;							\
	}								\
	inline uint32_t Size() {					\
	    return size;						\
	}								\
	inline uint32_t Cap() {						\
	    return cap;							\
	}								\
	inline int Push(__type *elem) {					\
	    if (Size() >= cap) {					\
		return -1;						\
	    }								\
	    size++;							\
	    list_add_tail(&elem->__node, &massages_queue);		\
	    return 0;							\
	}								\
	inline __type *Pop() {						\
	    __type *elem = NULL;					\
	    if (!list_empty(&massages_queue)) {				\
		elem = list_first(&massages_queue, __type, __node);	\
		list_del(&elem->__node);				\
		size--;							\
	    }								\
	    return elem;						\
	}								\
									\
    private:								\
	int ref;							\
	uint32_t size;							\
	uint32_t cap;							\
	struct list_head massages_queue;				\
    }

}
#endif
