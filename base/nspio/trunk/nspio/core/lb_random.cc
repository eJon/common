// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/lb_random.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "log.h"
#include "lb_random.h"
#include "mem_status.h"


NSPIO_DECLARATION_START

extern spio_mem_status_t spio_mem_stats;

static mem_stat_t *lb_random_mem_stats = &spio_mem_stats.lb_random;


lb_random::lb_random() :
    numbers(0), cap(1), backend_servers(NULL)
{
    do {
	if (!(backend_servers = (Role **)mem_zalloc(cap * sizeof(Role *))))
	    NSPIOLOG_ERROR("create lb_random with errno %d", errno);
    } while (!backend_servers);
    lb_random_mem_stats->alloc++;
    lb_random_mem_stats->alloc_size += sizeof(lb_random);
}


lb_random::~lb_random() {
    if (backend_servers)
	mem_free(backend_servers, cap * sizeof(Role *));
    lb_random_mem_stats->alloc--;
    lb_random_mem_stats->alloc_size -= sizeof(lb_random);
}


int lb_random::size() {
    return numbers;
}

int lb_random::balance() {
    return 0;
}

int lb_random::add(Role *r) {
    Role **__new_backend_servers = NULL;

    if ((numbers == cap) &&
	(__new_backend_servers =
	 (Role **)mem_realloc(backend_servers, cap * 2 * sizeof(Role *)))) {
	backend_servers = __new_backend_servers;
	cap = cap * 2;
    }
    if (numbers >= cap) {
	NSPIOLOG_ERROR("realloc lb_random with errno %d", errno);
	return -1;
    }
    NSPIOLOG_INFO("lb_random add %s", r->cid());
    backend_servers[numbers] = r;
    numbers++;
    return 0;
}

int lb_random::del(Role *r) {
    int i = 0;

    for (i = 0; i < numbers; i++) {
	if (backend_servers[i] != r)
	    continue;
	backend_servers[i] = backend_servers[numbers - 1];
	numbers--;
	NSPIOLOG_ERROR("lb_random del %s", r->cid());
	return 0;
    }
    NSPIOLOG_ERROR("lb_random del %s not found", r->cid());
    return -1;
}


Role *lb_random::loadbalance_recv() {
    Role *r = NULL;
    
    return r;
}


Role *lb_random::__loadbalance_send(struct nspiomsg *msg) {
    Role *r = NULL;

    if (!backend_servers || numbers <= 0) {
	errno = SPIO_EINTERN;
	goto out;
    }
    if (backend_servers && numbers > 0)
	r = backend_servers[rand() % numbers];
 out:
    return r;
}

Role *lb_random::loadbalance_send(struct nspiomsg *msg) {
    Role *r = NULL;
    int i, retries_cnt = size();

    for (i = 0; i < retries_cnt; i++) {
	r = __loadbalance_send(msg);
	if (ROLECANW(r))
	    return r;
    }
    return NULL;
}



}
