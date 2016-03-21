// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/appctx_msg.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "log.h"
#include "os/memalloc.h"
#include "proto/icmp.h"
#include "appctx.h"


NSPIO_DECLARATION_START

#define make_dsicmp_for_each_appmsg(head, icmp_head, st) do {		\
	struct nspiomsg *_m = NULL, *_n = NULL, *_icmp = NULL;		\
	char _msgss[HEADER_BUFFERLEN] = {};				\
									\
	list_for_each_nspiomsg_safe(_m, _n, head) {			\
	    if (!IS_NORMALMSG(&_m->hdr))				\
		continue;						\
	    list_del(&_m->mq_node);					\
	    if ((_icmp = make_deliver_status_icmp(_m, st)))		\
		list_add_tail(&_icmp->mq_node, icmp_head);		\
	    mem_free(_m, MSGPKGLEN(_m) + RTLEN);			\
	    header_parse(&_icmp->hdr, _msgss);				\
	    NSPIOLOG_INFO("%s make dsicmp %s for errno %d", cid(),	\
			  _msgss, st);					\
	}								\
    } while (0)

#define drop_for_each_msg(head, err) do {				\
	struct nspiomsg *_msg = NULL, *_next = NULL;			\
	char _msgss[HEADER_BUFFERLEN] = {};				\
									\
	list_for_each_nspiomsg_safe(_msg, _next, head) {		\
	    list_del(&_msg->mq_node);					\
	    header_parse(&_msg->hdr, _msgss);				\
	    mem_free(_msg, MSGPKGLEN(_msg) + RTLEN);			\
	    NSPIOLOG_WARN("%s drop %s with errno %d", cid(), _msgss,	\
			  err);						\
	}								\
    } while (0)



int AppCtx::__deliver_forward(struct list_head *head, struct list_head *el_head) {
    MQueue *mq = NULL;
    Role *d = NULL;
    struct nspiomsg *msg = NULL;

    while (!list_empty(head)) {
	if (!(d = lbp->loadbalance_send(NULL)))
	    break;
	msg = list_first(head, struct nspiomsg, mq_node);
	list_del(&msg->mq_node);
	mq = d->Queue();
	mq->PushMsg(msg, el_head);
    }
    return 0;
}

int AppCtx::deliver_forward(struct list_head *head) {
    MQueue *mq = NULL;
    string rid;
    char msgss[HEADER_BUFFERLEN] = {};
    struct nspiomsg *next = NULL, *icmp = NULL;
    struct list_head el_head = {}, icmp_head = {}, drop_head = {};

    INIT_LIST_HEAD(&el_head);
    INIT_LIST_HEAD(&icmp_head);
    INIT_LIST_HEAD(&drop_head);

    __deliver_forward(head, &el_head);

    make_dsicmp_for_each_appmsg(head, &icmp_head, SPIO_ENODISPATCHER);
    list_splice(head, &drop_head);

    make_dsicmp_for_each_appmsg(&el_head, &icmp_head, SPIO_EQUEUEFULL);
    list_splice(&el_head, &drop_head);

    list_for_each_nspiomsg_safe(icmp, next, &icmp_head) {
	list_del(&icmp->mq_node);
	route_getid(icmp->route, rid);
	if ((mq = mqp.Find(rid)))
	    mq->PushMsg(icmp, &drop_head);
	else {
	    header_parse(&icmp->hdr, msgss);
	    mem_free(icmp, MSGPKGLEN(icmp) + RTLEN);
	    NSPIOLOG_WARN("%s found %s(q) doesn't exist", cid(), rid.c_str());
	}
    }
    drop_for_each_msg(&drop_head, SPIO_EQUEUEFULL);
    return 0;
}


int AppCtx::__deliver_backward(struct list_head *head) {
    MQueue *mq = NULL;
    string rid;
    struct nspiomsg *msg = NULL, *next = NULL;
    struct list_head el_head = {}, icmp_head = {}, drop_head = {};

    INIT_LIST_HEAD(&el_head);
    INIT_LIST_HEAD(&icmp_head);
    INIT_LIST_HEAD(&drop_head);

    list_for_each_nspiomsg_safe(msg, next, head) {
	list_del(&msg->mq_node);
	route_getid(msg->route, rid);
	if ((mq = mqp.Find(rid))) {
	    mq->PushMsg(msg, &el_head);
	    make_dsicmp_for_each_appmsg(&el_head, &icmp_head, SPIO_EQUEUEFULL);
	    mq->BatchPushMsg(&icmp_head, &drop_head);
	    list_splice(&el_head, &drop_head);
	    drop_for_each_msg(&drop_head, SPIO_EQUEUEFULL);
	} else {
	    mem_free(msg, MSGPKGLEN(msg) + RTLEN);
	    NSPIOLOG_WARN("%s found %s(q) doesn't exist", cid(), rid.c_str());
	}
    }
    return 0;
}


int AppCtx::__generic_role_recv(Role *r, struct lock_list *ll) {
    int rn = 0, msg_balance_cnt = 0;
    struct nspiomsg *msg = NULL;
    struct list_head head = {};

    INIT_LIST_HEAD(&head);
    msg_balance_cnt = IS_PIOROLE(r->Type()) ? conf.msg_balance_factor : 10;
    if (HAS_MSGERROR(r))
	msg_balance_cnt = conf.msg_queue_size;
    while (msg_balance_cnt > 0) {
	if (r->Recv(&msg) < 0)
	    break;
	rn++;
	msg_balance_cnt--;
	list_add_tail(&msg->mq_node, &head);
    }
    if (rn > 0)
	NSPIOLOG_INFO("%s %s(%s) total recv %d request", cid(), r->cid(), ROLESTR(r), rn);
    lock_list_splice(&head, ll);
    return rn;
}

static int __running_iotask(void *data) {
    struct iotask_env *tv = (struct iotask_env *)data;
    AppCtx *ctx = (AppCtx *)tv->task_data;
    Role *r = tv->r;
    struct lock_list *ll = tv->ll;
    WaitGroup *wg = tv->wg;

    if (tv->ev == MSGIN)
	tv->task_result = ctx->__generic_role_recv(r, ll);
    else if (tv->ev == MSGOUT)
	tv->task_result = ctx->__generic_role_send(r, ll);
    wg->Done();

    return 0;
}

#define __make_iotask(tv, _ctx, _r, _ev, _wg, _ll) do {	\
	tv->task_data = _ctx;				\
	tv->ev = _ev;					\
	tv->r = _r;					\
	tv->wg = _wg;					\
	tv->ll = _ll;					\
    } while (0)


int AppCtx::recv_massage(struct list_head *rhead, struct list_head *dhead) {
    struct iotask_env *tv = NULL;
    WaitGroup wg;
    Role *r = NULL;
    module_stat *rstat = NULL;
    struct list_head ghead = {};
    struct list_link *pos = NULL, *next = NULL;
    struct lock_list fw_head = {}, bw_head = {};

    INIT_LIST_HEAD(&ghead);
    if (rhead)
	list_splice(rhead, &ghead);
    if (dhead)
	list_splice(dhead, &ghead);
    INIT_LOCKLIST_HEAD(&fw_head);
    INIT_LOCKLIST_HEAD(&bw_head);

    list_for_each_list_link_safe(pos, next, &ghead) {
	r = list_r(pos);
	CLEAR_MSGIN(r);
	r->resource_stat_module_timestamp_update(now_time);
	rstat = r->Stat();
	rstat->incrkey(POLLIN);
	tv = r->IOTask();
	if (IS_RECEIVER(r->Type())) {
	    astat.incrkey(RPOLLIN);
	    __make_iotask(tv, this, r, MSGIN, &wg, &fw_head);
	} else {
	    astat.incrkey(DPOLLIN);
	    __make_iotask(tv, this, r, MSGIN, &wg, &bw_head);
	}
	if (iothreadpool) {
	    wg.Add();
	    iothreadpool->Run(__running_iotask, tv);
	} else
	    __running_iotask(tv);
    }
    if (iothreadpool) {
	iothreadpool->BroadCast();
	wg.Wait();
    }
    deliver_forward(&fw_head.head);
    __deliver_backward(&bw_head.head);

    list_for_each_list_link_safe(pos, next, &ghead) {
	r = list_r(pos);
	r->detach_from_msgin_head();
	if (HAS_MSGERROR(r)) {
	    CLEAR_MSGOUT(r);
	    r->detach_from_msgout_head();
	    r->attach_to_msgerror_head(&err_roles);
	}
	tv = r->IOTask();
	if (tv->task_result > 0) {
	    if (IS_RECEIVER(r->Type()))
		astat.incrkey(RRCVPKG, tv->task_result);
	    else if (IS_DISPATCHER(r->Type()))
		astat.incrkey(DRCVPKG, tv->task_result);
	}
    }

    DESTROY_LOCKLIST_HEAD(&fw_head);
    DESTROY_LOCKLIST_HEAD(&bw_head);
    return 0;
}

int AppCtx::__generic_role_send(Role *r, struct lock_list *ll) {
    int sn = 0, msg_balance_cnt = 0;
    struct list_head to_head = {}, icmp_head = {};
    
    INIT_LIST_HEAD(&to_head);
    INIT_LIST_HEAD(&icmp_head);

    msg_balance_cnt = conf.msg_balance_factor;
    if ((sn = r->BatchSend(msg_balance_cnt, &to_head)) > 0)
	NSPIOLOG_INFO("%s %s(%s) total send %d response", cid(), r->cid(), ROLESTR(r), sn);
    make_dsicmp_for_each_appmsg(&to_head, &icmp_head, SPIO_ETIMEOUT);
    drop_for_each_msg(&to_head, SPIO_ETIMEOUT);
    lock_list_splice(&icmp_head, ll);
    return sn;
}

int AppCtx::sendto_massage(struct list_head *rhead, struct list_head *dhead) {
    struct iotask_env *tv = NULL;
    WaitGroup wg;
    Role *r = NULL;
    module_stat *rstat = NULL;
    struct list_head ghead = {};
    struct list_link *pos = NULL, *next = NULL;
    struct lock_list bw_head = {};

    INIT_LIST_HEAD(&ghead);
    if (rhead)
	list_splice(rhead, &ghead);
    if (dhead)
	list_splice(dhead, &ghead);
    INIT_LOCKLIST_HEAD(&bw_head);
    
    list_for_each_list_link_safe(pos, next, &ghead) {
	r = list_r(pos);
	CLEAR_MSGOUT(r);
	r->resource_stat_module_timestamp_update(now_time);
	rstat = r->Stat();
	rstat->incrkey(POLLOUT);
	tv = r->IOTask();
	__make_iotask(tv, this, r, MSGOUT, &wg, &bw_head);
	if (IS_RECEIVER(r->Type()))
	    astat.incrkey(RPOLLOUT);
	else
	    astat.incrkey(DPOLLOUT);
	if (iothreadpool) {
	    wg.Add();
	    iothreadpool->Run(__running_iotask, tv);
	} else
	    __running_iotask(tv);
    }
    if (iothreadpool) {
	iothreadpool->BroadCast();
	wg.Wait();
    }
    __deliver_backward(&bw_head.head);
    
    list_for_each_list_link_safe(pos, next, &ghead) {
	r = list_r(pos);
	r->detach_from_msgout_head();
	if (HAS_MSGERROR(r))
	    r->attach_to_msgerror_head(&err_roles);
	tv = r->IOTask();
	if (tv->task_result > 0) {
	    if (IS_RECEIVER(r->Type()))
		astat.incrkey(RSNDPKG, tv->task_result);
	    else
		astat.incrkey(DSNDPKG, tv->task_result);
	}
    }
	
    DESTROY_LOCKLIST_HEAD(&bw_head);
    return 0;
}

static int mark_msgin_when_role_has_cache(Role *r, void *data) {
    Conn *internconn = NULL;
    struct list_head *head = (struct list_head *)data;

    if (HAS_MSGIN(r))
	return 0;
    internconn = r->Connect();
    if (internconn->CacheSize(SO_READCACHE) > 0) {
	SET_MSGIN(r);
	r->attach_to_msgin_head(head);
    }
    return 0;
}
    


int AppCtx::wait_massage(struct list_head *rin, struct list_head *rout,
			 struct list_head *din, struct list_head *dout) {
    EpollEvent *ev = NULL;
    module_stat *rstat = NULL;
    Role *r = NULL;
    struct list_link *pos = NULL, *next = NULL;
    struct list_head io_head = {}, to_head = {};
    
    INIT_LIST_HEAD(&io_head);
    INIT_LIST_HEAD(&to_head);

    if (poller->Wait(&io_head, &to_head, conf.epoll_timeout_msec) < 0) {
	NSPIOLOG_ERROR("poller wait with errno  %d", errno);
	return -1;
    }
    list_for_each_list_link_safe(pos, next, &to_head) {
	ev = list_ev(pos);
	ev->detach();
	r = ROLEOFEV(ev);
	r->attach_to_msgerror_head(&err_roles);
	rstat = r->Stat();
	rstat->incrkey(POLLTIMEOUT);
    }
    list_for_each_list_link_safe(pos, next, &io_head) {
	ev = list_ev(pos);
	ev->detach();
	r = ROLEOFEV(ev);
	if (IS_RECEIVER(r->Type()))
	    r->AttachToMsgevHead(rin, rout, &err_roles);
	else if (IS_DISPATCHER(r->Type()))
	    r->AttachToMsgevHead(din, dout, &err_roles);
    }
    rom.WalkReceivers(mark_msgin_when_role_has_cache, rin);
    rom.WalkDispatchers(mark_msgin_when_role_has_cache, din);
    detach_for_each_poll_link(&io_head);
    detach_for_each_poll_link(&to_head);

    return 0;
}



int AppCtx::process_massage() {
    struct list_head receiver_ihead = {}, dispatcher_ihead = {};
    struct list_head receiver_ohead = {}, dispatcher_ohead = {};

    INIT_LIST_HEAD(&receiver_ihead);
    INIT_LIST_HEAD(&receiver_ohead);
    INIT_LIST_HEAD(&dispatcher_ihead);
    INIT_LIST_HEAD(&dispatcher_ohead);

    lbp->balance();

    wait_massage(&receiver_ihead, &receiver_ohead,
		 &dispatcher_ihead, &dispatcher_ohead);

    // Process Msgev IN/OUT first and then ERROR. !importance
    // EPOLLRDHUP maybe happend with EPOLLIN. so we should recv
    // all data from cache and then process EPOLLRDHUP error.
    recv_massage(&receiver_ihead, &dispatcher_ihead);
    sendto_massage(&receiver_ohead, &dispatcher_ohead);

    if (!list_empty(&receiver_ihead) || !list_empty(&receiver_ohead))
	NSPIOLOG_ERROR("impossible reach here: %d", errno);
    if (!list_empty(&dispatcher_ihead) || !list_empty(&dispatcher_ohead))
	NSPIOLOG_ERROR("impossible reach here: %d", errno);

    if (!list_empty(&err_roles))
	process_roles_error();
    return 0;
}



}    
