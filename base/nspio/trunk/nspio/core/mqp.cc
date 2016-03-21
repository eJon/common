// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/mqp.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "os/time.h"
#include "log.h"
#include "mem_status.h"
#include "mqp.h"


NSPIO_DECLARATION_START
    
extern spio_mem_status_t spio_mem_stats;
static mem_stat_t *msgqueuepool_mem_stats = &spio_mem_stats.msgqueuepool;



MQueuePool::MQueuePool() :
    cfg(NULL)
{
    skrb_init(&tw_timeout_tree);
    INIT_LIST_HEAD(&time_wait_queues);
    msgqueuepool_mem_stats->alloc++;
    msgqueuepool_mem_stats->alloc_size += sizeof(MQueuePool);
}

MQueuePool::~MQueuePool() {
    MQueue *mq = NULL;
    map<string, MQueue *>::iterator it;
    struct list_link *pos = NULL, *next = NULL;

    for (it = queues.begin(); it != queues.end(); ++it)
	delete it->second;
    list_for_each_list_link_safe(pos, next, &time_wait_queues) {
	mq = list_mq(pos);
	delete mq;
    }
    msgqueuepool_mem_stats->alloc--;
    msgqueuepool_mem_stats->alloc_size -= sizeof(MQueuePool);
}


int MQueuePool::Setup(const string &_appid, CtxConf *conf) {
    appid = _appid;
    cfg = conf;
    return 0;
}


inline void MQueuePool::clean_tw_queues() {
    skrb_node_t *node = NULL;
    MQueue *mq = NULL;
    int64_t cur_ms = rt_mstime();

    while (!skrb_empty(&tw_timeout_tree)) {
	node = skrb_min(&tw_timeout_tree);
	if (node->key > cur_ms)
	    break;
	mq = (MQueue *)node->data;
	skrb_delete(&tw_timeout_tree, node);
	mq->detach_from_mqpool_head();
	NSPIOLOG_NOTICE("%s delete time_wait queue: %s", cappid(), mq->cid());
	delete mq;
    }
}


inline void MQueuePool::insert_tw_queue(MQueue *mq) {
    mq->attach_to_mqpool_head(&time_wait_queues);
    mq->tw_timeout_node.key = mq->TimeWaitTime() + rt_mstime();
    mq->tw_timeout_node.data = mq;
    skrb_insert(&tw_timeout_tree, &mq->tw_timeout_node);
}


// find and erase from list
inline MQueue *MQueuePool::find_tw_queue(const string &qid) {
    MQueue *mq = NULL;
    struct list_link *pos = NULL, *next = NULL;

    list_for_each_list_link_safe(pos, next, &time_wait_queues) {
	mq = list_mq(pos);
	if (mq->Id() == qid) {
	    mq->detach_from_mqpool_head();
	    skrb_delete(&tw_timeout_tree, &mq->tw_timeout_node);
	    break;
	}
	mq = NULL;
    }
    return mq;
}

MQueue *MQueuePool::Set(const string &qid) {
    MQueue *mq;
    map<string, MQueue *>::iterator it;

    clean_tw_queues();
    if ((it = queues.find(qid)) != queues.end()) {
	mq = it->second;
	NSPIOLOG_NOTICE("%s found an exist queue: %s", cappid(), mq->cid());
	mq->Ref();
	return mq;
    }
    if ((mq = find_tw_queue(qid)) != NULL) {
	NSPIOLOG_NOTICE("%s found an time_wait queue: %s", cappid(), mq->cid());
	queues.insert(make_pair(qid, mq));
	mq->Ref();
	return mq;
    }
    if (!(mq = new (std::nothrow) MQueue(appid, qid))) {
	NSPIOLOG_ERROR("%s out of memory", cappid());
	return NULL;
    }
    mq->Setup(cfg->msg_queue_size, cfg->reconnect_timeout_msec, cfg->msg_timeout_msec);
    queues.insert(make_pair(qid, mq));
    mq->Ref();
    return mq;
}

int MQueuePool::unSet(const string &qid) {
    int ret;
    MQueue *mq = NULL;
    map<string, MQueue *>::iterator it;

    if ((it = queues.find(qid)) == queues.end()) {
	NSPIOLOG_WARN("%s queue %s doesn't exists", cappid(), qid.c_str());
	return -1;
    }
    mq = it->second;
    if ((ret = mq->unRef()) == 0) {
	queues.erase(it);
	if (mq->TimeWaitTime() == 0) {
	    NSPIOLOG_NOTICE("%s queue %s deleted", cappid(), mq->cid());
	    delete mq;
	} else {
	    NSPIOLOG_NOTICE("%s queue %s enter time_wait", cappid(), mq->cid());
	    insert_tw_queue(mq);
	}
    }
    return ret;
}


MQueue *MQueuePool::Find(const string &qid) {
    map<string, MQueue *>::iterator it;
    MQueue *mq = NULL;

    if ((it = queues.find(qid)) != queues.end())
	mq = it->second;

    return mq;
}

int MQueuePool::Walk(mq_walkfn walkfn, void *data) {
    map<string, MQueue *>::iterator it;

    for (it = queues.begin(); it != queues.end(); ++it)
	walkfn(it->second, data);
    return 0;
}

    

// Output all massage queue status
int MQueuePool::OutputQueueStatus(FILE *fp) {

    MQueue *mq = NULL;
    map<string, MQueue *>::iterator it;
    struct list_link *pos = NULL, *next = NULL;
    string uid;
    
    fprintf(fp, "------------------------------------------------------------\n");
    fprintf(fp,
	    "%15s %10s %10s %10s %10s %10s %10s\n\n", "qid", "cap",
	    "size", "passed", "timeout", "residence", "monitor");

#define __output_queue_status(__uid, __mq)				\
    do {								\
	module_stat_trigger *__tg = __mq->StatTrigger();		\
	fprintf(fp, "%15s %10u %10"PRId64" %10"PRId64" %10"PRId64" %10"PRId64" %10d\n",	\
		__uid.c_str(),						\
		__mq->Cap(),						\
		__tg->getlast_s(SIZE),					\
		__tg->getlast_s(PASSED),				\
		__tg->getlast_s(TIMEOUTED),				\
		__tg->getlast_s(RESI_MSEC),				\
		__mq->MonitorsNum());					\
    } while (0)

    
    for (it = queues.begin(); it != queues.end(); ++it) {
	mq = it->second;
	uid.clear();
	uid.assign(mq->cid(), 8);
	__output_queue_status(uid, mq);
    }
    list_for_each_list_link_safe(pos, next, &time_wait_queues) {
	mq = list_mq(pos);
	uid.clear();
	uid.assign(mq->cid(), 8);
	uid += "(tw)";
	__output_queue_status(uid, mq);
    }
    fprintf(fp, "------------------------------------------------------------\n");

    return 0;
}


}
