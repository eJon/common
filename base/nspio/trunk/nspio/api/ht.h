// copyright:
//            (C) SINA Inc.
//
//           file: nspio/api/ht.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_HQ_
#define _H_HQ_

#include <nspio/async_api.h>
#include "sync/tmutex.h"
#include "base/list.h"
#include "base/skrb.h"


NSPIO_DECLARATION_START

typedef struct resp_callback_head {
    tmutex_t lock;
    struct hlist_head head;
    skrb_t timer_rbtree;
} nspio_respcallback_head_t;

typedef struct resp_callback {
    int64_t hid, ctime;
    void *callback;
    int rblinked;
    skrb_node_t timer_node;
    struct hlist_node cb_link;
} nspio_respcallback_t;
 


class HandlerTable {
 public:
    HandlerTable();
    ~HandlerTable();

    int Init(int size);
    int64_t GetHid();
    int cleanup_tw_callbacks(struct hlist_head *to_head);
    int InsertHandler(int64_t hid, void *callback, int to_msec = 0);
    int FindHandler(int64_t hid, void **callback);

 private:
    int inited;
    int slots, slot_cleanup_idx;
    int64_t seqid;
    tmutex_t lock;
    nspio_respcallback_head_t *hts;

    int __cleanup_tw_callbacks(nspio_respcallback_head_t *head, struct hlist_head *to_head);
};











}
#endif   // _H_HQ_
