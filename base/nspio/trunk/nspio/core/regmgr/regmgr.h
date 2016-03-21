// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/regmgr/regmgr.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_REGISTER_
#define _H_REGISTER_

#include "proto/proto.h"
#include "config.h"

using namespace std;


NSPIO_DECLARATION_START

struct rgmh_stats {
    int at_receivers; 
    int at_dispatchers;
    int tw_receivers;
    int tw_dispatchers;
    int nr_receivers;
    int nr_dispatchers;
};



#define __INCR_COUNTER(__r, __st, typ)		\
    {						\
	if (IS_RECEIVER((__r)->Type()))		\
	    (__st)->typ##receivers++;		\
	else if (IS_DISPATCHER((__r)->Type()))	\
	    (__st)->typ##dispatchers++;		\
    }

#define __DECR_COUNTER(__r, __st, typ)		\
    {						\
	if (IS_RECEIVER((__r)->Type()))		\
	    (__st)->typ##receivers--;		\
	else if (IS_DISPATCHER((__r)->Type()))	\
	    (__st)->typ##dispatchers--;		\
    }
 
#define INCR_AT_COUNTER(_r, _st) __INCR_COUNTER(_r, _st, at_)
#define INCR_TW_COUNTER(_r, _st) __INCR_COUNTER(_r, _st, tw_)
#define INCR_NR_COUNTER(_r, _st) __INCR_COUNTER(_r, _st, nr_)
#define DECR_AT_COUNTER(_r, _st) __DECR_COUNTER(_r, _st, at_)
#define DECR_TW_COUNTER(_r, _st) __DECR_COUNTER(_r, _st, tw_)
#define DECR_NR_COUNTER(_r, _st) __DECR_COUNTER(_r, _st, nr_)
#define RECEIVER_COUNTER(_st)						\
    ({int __all = (_st)->at_receivers + (_st)->tw_receivers + (_st)->nr_receivers; __all;})
#define DISPATCHER_COUNTER(_st)						\
    ({int __all = (_st)->at_dispatchers + (_st)->tw_dispatchers + (_st)->nr_dispatchers; __all;})

 
class RgmHandler {
 public:
    RgmHandler();
    virtual ~RgmHandler() {}

    virtual const char *cappid() = 0;
    virtual int stats(struct rgmh_stats *info) = 0;
    virtual bool reg_keep_session(const struct spioreg *reghdr) = 0;
    virtual int Register(struct spioreg *header, Conn *conn) = 0;

    int attach_to_rgm_head(struct list_head *head);
    int detach_from_rgm_head();

 private:
    struct list_link rgm_node;
};

#define list_rgmh(link) ((RgmHandler *)link->self) 
#define list_first_rgmh(head)						\
    ({struct list_link *__pos =						\
	    list_first(head, struct list_link, node);  list_rgmh(__pos);})

class Rgm {
 public:
    virtual ~Rgm() {}
    virtual int InsertHandler(RgmHandler *handler) = 0;
    virtual int RemoveHandler(RgmHandler *handler) = 0;
    virtual int Listen(const string &laddr) = 0;
    virtual int ConnectTo(const string &rid, const string &app,
			  const string &raddr, RgmHandler *h) = 0;
    virtual int Start() = 0;
    virtual void Stop() = 0;
    virtual bool Running() = 0;
    virtual int StartThread() = 0;
    virtual void StopThread() = 0;
};

Rgm *NewRegisterManager(SpioConfig *_conf);
 
}




#endif  // _H_RECEIVER_H_
