// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/mem_status.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_MEM_STATUS_
#define _H_MEM_STATUS_

#include "os/memalloc.h"


NSPIO_DECLARATION_START

typedef struct spio_mem_status {
    mem_stat_t app_context;
    mem_stat_t spio_config, app_config;
    mem_stat_t rolemanager, dispatchers, receivers;
    mem_stat_t lb_iphash, lb_random, lb_rrbin, lb_fair;
    mem_stat_t msgpoller;
    mem_stat_t msgqueuepool, msgqueue;
    mem_stat_t rgm, rgm_accepter, rgm_connector, rgm_register;
    mem_stat_t tdqueue, timer;
} spio_mem_status_t;



}

















#endif  // _H_MEM_STATUS_
