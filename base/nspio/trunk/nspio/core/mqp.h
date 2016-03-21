// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/mqp.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_MSGQUEUEEX_
#define _H_MSGQUEUEEX_

#include <map>
#include "mq.h"
#include "config.h"

NSPIO_DECLARATION_START


typedef int (*mq_walkfn) (MQueue *mq, void *data);
 
class MQueuePool {
 public:
    MQueuePool();
    ~MQueuePool();

    inline const char *cappid() {
	return appid.c_str();
    }
    int Setup(const string &_appid, CtxConf *cfg);
    MQueue *Set(const string &qid);
    int unSet(const string &qid);
    MQueue *Find(const string &qid);
    int Walk(mq_walkfn walkfn, void *data);
    int OutputQueueStatus(FILE *fp);
    
 private:
    CtxConf *cfg;
    string appid;
    map<string, MQueue *> queues;
    struct list_head time_wait_queues;
    skrb_t tw_timeout_tree;

    inline void clean_tw_queues();
    inline void insert_tw_queue(MQueue *mq);
    inline MQueue *find_tw_queue(const string &qid);
};


}

#endif   // _H_MSGQUEUEEX_
