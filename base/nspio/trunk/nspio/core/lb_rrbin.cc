// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/lb_rrbin.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "lb_rrbin.h"
#include "log.h"
#include "mem_status.h"

NSPIO_DECLARATION_START


extern spio_mem_status_t spio_mem_stats;

static mem_stat_t *lb_rrbin_mem_stats = &spio_mem_stats.lb_rrbin;


lb_rrbin::lb_rrbin() :
    idx(0), numbers(0), cap(1), backend_servers(NULL)
{
    do {
	if (!(backend_servers = (Role **)mem_zalloc(cap * sizeof(Role *))))
	    NSPIOLOG_ERROR("create lb_rrbin with errno %d", errno);
    } while (!backend_servers);
    lb_rrbin_mem_stats->alloc++;
    lb_rrbin_mem_stats->alloc_size += sizeof(lb_rrbin);
}

lb_rrbin::~lb_rrbin() {
    if (backend_servers)
	mem_free(backend_servers, cap * sizeof(Role *));
    lb_rrbin_mem_stats->alloc--;
    lb_rrbin_mem_stats->alloc_size -= sizeof(lb_rrbin);
}

int lb_rrbin::size() {
    return numbers;
}

int lb_rrbin::balance() {
    return 0;
}

int lb_rrbin::add(Role *r) {
    Role **__new_backend_servers = NULL;

    if ((numbers == cap) &&
	(__new_backend_servers =
	 (Role **)mem_realloc(backend_servers, cap * 2 * sizeof(Role *)))) {
	backend_servers = __new_backend_servers;
	cap = cap * 2;
    }
    if (numbers >= cap) {
	NSPIOLOG_ERROR("realloc lb_rrbin with errno %d", errno);
	return -1;
    }
    NSPIOLOG_INFO("lb_rrbin add %s", r->cid());
    backend_servers[numbers] = r;
    numbers++;
    return 0;
}

int lb_rrbin::del(Role *r) {
    int i = 0;

    for (i = 0; i < numbers; i++) {
	if (backend_servers[i] != r)
	    continue;
	backend_servers[i] = backend_servers[numbers - 1];
	numbers--;
	if (numbers > 0 && idx >= numbers)
	    idx = numbers - 1;
	else if (numbers == 0)
	    idx = 0;
	NSPIOLOG_ERROR("lb_rrbin del %s", r->cid());
	return 0;
    }
    NSPIOLOG_ERROR("lb_rrbin del %s not found", r->cid());
    return -1;
}

Role *lb_rrbin::loadbalance_recv() {
    Role *r = NULL;

    
    return r;
}

Role *lb_rrbin::__loadbalance_send(struct nspiomsg *msg) {
    Role *r = NULL;

    if (!backend_servers || numbers <= 0) {
	errno = SPIO_EINTERN;
	goto out;
    }
    if (backend_servers && numbers > 0) {
	if (idx == numbers)
	    idx = 0;
	r = backend_servers[idx];
	idx++;
    }
 out:
    return r;
}


Role *lb_rrbin::loadbalance_send(struct nspiomsg *msg) {
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
