// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/role_attr_evh.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_ROLEATTREVH_
#define _H_ROLEATTREVH_

#include "role.h"


NSPIO_DECLARATION_START



class rattr_ev_monitor : public rattr_event_change_handler {
 public:
    rattr_ev_monitor();
    ~rattr_ev_monitor();

    int role_enable_r_event(Role *r);
    int role_disable_r_event(Role *r);
    int role_enable_w_event(Role *r);
    int role_disable_w_event(Role *r);

    inline int count_canr_receivers() {
	return canr_receivers;
    }
    inline int count_canw_receivers() {
	return canw_receivers;
    }
    inline int count_canr_dispatchers() {
	return canr_dispatchers;
    }
    inline int count_canw_dispatchers() {
	return canw_dispatchers;
    }
    
 private:
    int canr_receivers, canw_receivers, canr_dispatchers, canw_dispatchers;
};


}

#endif // _H_RATTR_EVH_
