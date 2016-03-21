// copyright:
//            (C) SINA Inc.
//
//           file: nspio/api/ht.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include "os/time.h"
#include "ht.h"
#include "os/memalloc.h"
#include "ed.h"

NSPIO_DECLARATION_START



HandlerTable::HandlerTable() :
    inited(0), slots(0), slot_cleanup_idx(0), seqid(0), hts(NULL)
{
    tmutex_init(&lock);
}

HandlerTable::~HandlerTable() {
    int i = 0;
    nspio_respcallback_t *pos = NULL, *next = NULL;
    struct hlist_head *head = NULL;

    if (!inited)
	return;
    tmutex_destroy(&lock);
    for (i = 0; i < slots; i++) {
	tmutex_destroy(&hts[i].lock);
	head = &hts[i].head;
	hlist_for_each_entry_safe(pos, next, head, nspio_respcallback_t, cb_link) {
	    hlist_del(&pos->cb_link);
	    mem_free(pos, sizeof(*pos));
	}
    }
    mem_free(hts, slots * sizeof(*hts));
}


int HandlerTable::Init(int size) {
    int i;
    nspio_respcallback_head_t *ht = NULL;

    if (inited) {
	errno = SPIO_EDUPOP;
	return -1;
    }
    if (size <= 0) {
	errno = EINVAL;
	return -1;
    }
    if ((hts = (nspio_respcallback_head_t *)
	 mem_zalloc(sizeof(*hts) * size)) == NULL) {
	errno = ENOMEM;
	return -1;
    }
    for (i = 0; i < size; i++) {
	ht = &hts[i];
	tmutex_init(&ht->lock);
	INIT_HLIST_HEAD(&ht->head);
	skrb_init(&ht->timer_rbtree);
    }
    inited = 1;
    slots = size;
    return 0;
}


int64_t HandlerTable::GetHid() {
    int64_t hid = -1;

    if (!inited) {
	errno = SPIO_EINTERN;
	return -1;
    }
    tmutex_lock(&lock);
    hid = seqid;
    seqid++;
    tmutex_unlock(&lock);
    return hid;
}



int HandlerTable::InsertHandler(int64_t hid, void *callback, int to_msec) {
    nspio_respcallback_t *rcp = NULL;
    nspio_respcallback_head_t *ht = NULL;

    if (!inited) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (!(rcp = (nspio_respcallback_t *)mem_zalloc(sizeof(*rcp)))) {
	errno = ENOMEM;
	return -1;
    }
    rcp->hid = hid;
    rcp->callback = callback;
    INIT_HLIST_NODE(&rcp->cb_link);
    ht = &hts[hid % slots];
    tmutex_lock(&ht->lock);
    hlist_add_head(&rcp->cb_link, &ht->head);
    rcp->ctime = rt_mstime();
    if (to_msec) {
	rcp->rblinked = 1;
	rcp->timer_node.key = rcp->ctime + to_msec;
	rcp->timer_node.data = rcp;
	skrb_insert(&ht->timer_rbtree, &rcp->timer_node);
    }
    tmutex_unlock(&ht->lock);
    return 0;
}


int HandlerTable::__cleanup_tw_callbacks(nspio_respcallback_head_t *ht, struct hlist_head *to_head) {
    int64_t cur_mstime = 0;
    skrb_node_t *node = NULL;
    nspio_respcallback_t *rcp = NULL;

    cur_mstime = rt_mstime();
    while (!skrb_empty(&ht->timer_rbtree)) {
	node = skrb_min(&ht->timer_rbtree);
	if (node->key > cur_mstime)
	    break;
	rcp = (nspio_respcallback_t *)node->data;
	hlist_del(&rcp->cb_link);
	skrb_delete(&ht->timer_rbtree, &rcp->timer_node);
	hlist_add_head(&rcp->cb_link, to_head);
    }
    return 0;
}


int HandlerTable::cleanup_tw_callbacks(struct hlist_head *to_head) {
    int i = 0, tc = 1;
    nspio_respcallback_head_t *ht = NULL;

    if (!inited) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (tc <= 0)
	tc = 1;
    for (i = 0; i < tc; i++) {
	if (slot_cleanup_idx >= slots)
	    slot_cleanup_idx = 0;
	ht = &hts[slot_cleanup_idx];
	tmutex_lock(&ht->lock);
	__cleanup_tw_callbacks(ht, to_head);
	tmutex_unlock(&ht->lock);
	slot_cleanup_idx++;
    }
    return 0;
}



int HandlerTable::FindHandler(int64_t hid, void **callback) {
    int ret = -1;
    nspio_respcallback_head_t *ht = NULL;
    nspio_respcallback_t *pos = NULL, *next = NULL;

    ht = &hts[hid % slots];
    tmutex_lock(&ht->lock);
    hlist_for_each_entry_safe(pos, next, &ht->head, nspio_respcallback_t, cb_link) {
	if (pos->hid != hid)
	    continue;
	hlist_del(&pos->cb_link);
	if (pos->rblinked)
	    skrb_delete(&ht->timer_rbtree, &pos->timer_node);
	*callback = pos->callback;
	mem_free(pos, sizeof(*pos));
	ret = 0;
	break;
    }
    tmutex_unlock(&ht->lock);
    return ret;
}












}
