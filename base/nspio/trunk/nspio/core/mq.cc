// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/mq.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "os/time.h"
#include "mq.h"
#include "proto/proto.h"
#include "mem_status.h"


NSPIO_DECLARATION_START
    
extern spio_mem_status_t spio_mem_stats;
static mem_stat_t *msgqueue_mem_stats = &spio_mem_stats.msgqueue;

msg_queue_monitor::msg_queue_monitor() {
    INIT_LIST_LINK(&link);
}

int msg_queue_monitor::__attach_to_mqmonitor_head(struct list_head *head) {
    if (!link.linked) {
	link.linked = 1;
	list_add(&link.node, head);
	return 0;
    }
    return -1;
}

int msg_queue_monitor::__detach_from_mqmonitor_head() {
    if (link.linked) {
	list_del(&link.node);
	link.linked = 0;
	return 0;
    }
    return -1;
}

MQueue::MQueue(const string &_appid, const string &_id) :
    qid(_id), appid(_appid), msg_to_msec(0), time_wait_msec(0), monitors_cnt(0),
    qstat(MQ_MODULE_STATITEM_KEYRANGE, &__mq_sm)
{
    INIT_LIST_HEAD(&monitors);
    INIT_LIST_LINK(&mqp_node);
    __mq_sm.setup(_appid, _id);
    memset(&tw_timeout_node, 0, sizeof(tw_timeout_node));
    msgqueue_mem_stats->alloc++;
    msgqueue_mem_stats->alloc_size += sizeof(MQueue);
}

MQueue::~MQueue() {
    struct nspiomsg *req = NULL;
    while ((req = Pop()) != NULL) {
	mem_free(req, MSGPKGLEN(req) + RTLEN);
    }
    msgqueue_mem_stats->alloc--;
    msgqueue_mem_stats->alloc_size -= sizeof(MQueue);
}

int MQueue::MonitorsNum() {
    return monitors_cnt;
}
    
int MQueue::AddMonitor(msg_queue_monitor *monitor) {
    if (monitor->__attach_to_mqmonitor_head(&monitors) == 0) {
	if (Size() == 0)
	    monitor->mq_empty_callback_func();
	else if (Size() > 0)
	    monitor->mq_nonempty_callback_func();
	monitors_cnt++;
    }
    return 0;
}
    
int MQueue::DelMonitor(msg_queue_monitor *monitor) {
    if (monitor->__detach_from_mqmonitor_head() == 0)
	monitors_cnt--;
    return 0;
}
    
int MQueue::Setup(int qcap, int tw_msec, int to_msec)
{
    msg_to_msec = to_msec;
    time_wait_msec = tw_msec;
    return Init(qcap);
}

static void __compute_resitime(struct nspiomsg *msg, module_stat *self) {
    int64_t rs_msec = rt_mstime() - msg->resitime;
    self->incrkey(RESI_MSEC, rs_msec > 0 ? rs_msec : 0);
}


int MQueue::PushMsg(struct nspiomsg *msg, struct list_head *el_head) {
    struct nspiomsg *tmp = NULL;
    struct list_link *pos = NULL;

    while (-1 == Push(msg)) {
	if ((tmp = Pop()) != NULL) {
	    __compute_resitime(tmp, &qstat);
	    if (!el_head)
		mem_free(tmp, MSGPKGLEN(tmp) + RTLEN);
	    else
		list_add_tail(&tmp->mq_node, el_head);
	}
    }
    msg->resitime = rt_mstime();
    qstat.setkey(SIZE, Size());
    if (Size() == 1) {
	list_for_each_list_link(pos, &monitors) {
	    msg_queue_monitor *monitor = list_monitor(pos);
	    monitor->mq_nonempty_callback_func();
	}
    }
    return 0;
}

int MQueue::BatchPushMsg(struct list_head *head, struct list_head *el_head) {
    struct nspiomsg *msg = NULL, *next = NULL;

    list_for_each_entry_safe(msg, next, head, struct nspiomsg, mq_node) {
	list_del(&msg->mq_node);
	PushMsg(msg, el_head);
    }
    return 0;
}


int MQueue::attach_to_mqpool_head(struct list_head *head) {
    if (!mqp_node.linked) {
	mqp_node.linked = 1;
	list_add(&mqp_node.node, head);
	return 0;
    }
    return -1;
}

int MQueue::detach_from_mqpool_head() {
    if (mqp_node.linked) {
	list_del(&mqp_node.node);
	mqp_node.linked = 0;
	return 0;
    }
    return -1;
}

bool MQueue::msg_is_timeout(struct nspiomsg *msg) {
    int64_t to_msec = rt_mstime() - msg->hdr.timestamp;
    
    if (msg_to_msec && msg->hdr.timestamp && to_msec > msg_to_msec) {
	qstat.incrkey(TIMEOUTED);
	return true;
    }
    qstat.incrkey(PASSED);
    return false;
}



struct nspiomsg *MQueue::PopMsg(struct list_head *to_head) {
    struct list_link *pos = NULL;
    struct nspiomsg *msg = NULL;

    while ((msg = Pop()) != NULL && msg_is_timeout(msg)) {
	__compute_resitime(msg, &qstat);
	if (!to_head)
	    mem_free(msg, MSGPKGLEN(msg) + RTLEN);
	else
	    list_add_tail(&msg->mq_node, to_head);
    }
    if (msg)
	__compute_resitime(msg, &qstat);    
    if (Size() == 0) {
	list_for_each_list_link(pos, &monitors) {
	    msg_queue_monitor *monitor = list_monitor(pos);
	    monitor->mq_empty_callback_func();
	}
    }
    qstat.setkey(SIZE, Size());
    return msg;
}



}
