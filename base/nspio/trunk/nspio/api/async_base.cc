// copyright:
//            (C) SINA Inc.
//
//           file: nspio/api/async_base.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "async_api.h"


NSPIO_DECLARATION_START

__async_base::__async_base() :
    inited(0), stats(ASAPI_MODULE_STATITEM_KEYRANGE, NULL), astp(NULL)
{
    INIT_LIST_LINK(&astp_link);
}

__async_base::~__async_base() {
}

EpollEvent *__async_base::epollevent() {
    return &ee;
}

int __async_base::try_disable_event(int ev) {
    int ret = 0;
    bool __update(false);

    if (!astp)
	return -1;
    if (ee.events & ev) {
	ee.events &= ~ev;
	__update = true;
    }
    if (__update && (ret = astp->update_eventpoll(this)) < 0)
	{}
    return ret;
}

int __async_base::try_enable_event(int ev) {
    int ret = 0;
    bool __update(false);

    if (!astp)
	return -1;
    if (!(ee.events & ev)) {
	ee.events |= ev;
	__update = true;
    }
    if (__update && (ret = astp->update_eventpoll(this)) < 0)
	{}
    return ret;
}

int __async_base::attach_to_transport_head(struct list_head *head) {
    if (!astp_link.linked) {
	astp_link.linked = 1;
	list_add(&astp_link.node, head);
	return 0;
    }
    return -1;
}

int __async_base::detach_from_transport_head() {
    if (astp_link.linked) {
	list_del(&astp_link.node);
	astp_link.linked = 0;
	return 0;
    }
    return -1;
}



}
