// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/mq.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_MSGQUEUE_
#define _H_MSGQUEUE_


#include "base/skrb.h"
#include "base/list.h"
#include "base/mq.h"
#include "proto/proto.h"
#include "mq_modstat.h"


using namespace std;

NSPIO_DECLARATION_START

class msg_queue_monitor {
 public:
    msg_queue_monitor();
    virtual ~msg_queue_monitor() {}
    
    virtual int mq_empty_callback_func() = 0;
    virtual int mq_nonempty_callback_func() = 0;

    int __attach_to_mqmonitor_head(struct list_head *head);
    int __detach_from_mqmonitor_head();
    
 private:
    struct list_link link;
};

#define list_monitor(link) ((msg_queue_monitor *)link->self) 
#define list_first_monitor(head)					\
    ({struct list_link *__pos =						\
	    list_first(head, struct list_link, node); (msg_queue_monitor *)__pos->self;})

#define list_mq(link) ((MQueue *)link->self) 
#define list_first_mq(head)						\
    ({struct list_link *__pos =						\
	    list_first(head, struct list_link, node);  list_mq(__pos);})


DEFINE_MQ(msg_queue, struct nspiomsg, mq_node);

class MQueue : public msg_queue {
 public:
    MQueue(const string &_appid, const string &_id);
    ~MQueue();
    friend class MQueuePool;

    inline module_stat *Stat() {
	return &qstat;
    }
    inline module_stat_trigger *StatTrigger() {
	return &__mq_sm;
    }
    inline string Id() {
	return qid;
    }
    inline const char *cid() {
	return qid.c_str();
    }
    inline const char *cappid() {
	return appid.c_str();
    }
    int MonitorsNum();
    int AddMonitor(msg_queue_monitor *monitor);
    int DelMonitor(msg_queue_monitor *monitor);

    int Setup(int cap, int tw_msec, int to_msec);
    int TimeWaitTime() {
	return time_wait_msec;
    }
    int PushMsg(struct nspiomsg *msg, struct list_head *el_head = NULL);
    struct nspiomsg *PopMsg(struct list_head *to_head = NULL);
    int BatchPushMsg(struct list_head *head, struct list_head *el_head = NULL);

    int attach_to_mqpool_head(struct list_head *head);
    int detach_from_mqpool_head();
    
 private:
    string qid, appid;
    int msg_to_msec, time_wait_msec, monitors_cnt;
    module_stat qstat;
    __mq_module_stat_trigger __mq_sm;
    skrb_node_t tw_timeout_node;
    struct list_head monitors;
    struct list_link mqp_node;

    bool msg_is_timeout(struct nspiomsg *msg);
};



}



#endif   // _H_MSGQUEUE_
