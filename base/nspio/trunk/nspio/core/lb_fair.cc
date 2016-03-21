// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/lb_fair.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include <stdlib.h>
#include "log.h"
#include "lb_fair.h"
#include "mem_status.h"


NSPIO_DECLARATION_START

extern spio_mem_status_t spio_mem_stats;
static mem_stat_t *lb_fair_mem_stats = &spio_mem_stats.lb_fair;


lb_fair::lb_fair() :
    idx(0), numbers(0), cap(1), backend_servers(NULL)
{
    do {
	if (!(backend_servers = (Role **)mem_zalloc(cap * sizeof(Role *))))
	    NSPIOLOG_ERROR("create lb_fair with errno %d", errno);
    } while (!backend_servers);
    lb_fair_mem_stats->alloc++;
    lb_fair_mem_stats->alloc_size += sizeof(lb_fair);
}

lb_fair::~lb_fair() {
    if (backend_servers)
	mem_free(backend_servers, cap * sizeof(Role *));
    lb_fair_mem_stats->alloc--;
    lb_fair_mem_stats->alloc_size -= sizeof(lb_fair);
}



static int backend_servers_qsort_cmp_of_role(Role *a, Role *b) {
    int64_t a_snd = 0, b_snd = 0;
    module_stat *a_rstat = NULL;
    module_stat *b_rstat = NULL;    

    a_rstat = a->Stat();
    b_rstat = b->Stat();
    a_snd = a_rstat->getkey(SEND_PACKAGES) - a_rstat->getkey(RECV_PACKAGES);
    b_snd = b_rstat->getkey(SEND_PACKAGES) - b_rstat->getkey(RECV_PACKAGES);
    return a_snd - b_snd;
}


static int backend_servers_qsort_cmp(const void *a, const void *b) {
    return backend_servers_qsort_cmp_of_role(*(Role **)a, *(Role **)b);
}


int lb_fair::size() {
    return numbers;
}

int lb_fair::balance() {
    if (!backend_servers || numbers <= 0) {
	errno = SPIO_EINTERN;
	return -1;
    }
    qsort(backend_servers, numbers, sizeof(Role *), backend_servers_qsort_cmp);
    return 0;
}

int lb_fair::add(Role *r) {
    Role **__new_backend_servers = NULL;

    if ((numbers == cap) &&
	(__new_backend_servers =
	 (Role **)mem_realloc(backend_servers, cap * 2 * sizeof(Role *)))) {
	backend_servers = __new_backend_servers;
	cap = cap * 2;
    }
    if (numbers >= cap) {
	NSPIOLOG_ERROR("realloc lb_fair with errno %d", errno);
	return -1;
    }
    NSPIOLOG_INFO("lb_fair add %s", r->cid());
    backend_servers[numbers] = r;
    numbers++;
    return 0;
}

int lb_fair::del(Role *r) {
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
	NSPIOLOG_ERROR("lb_fair del %s", r->cid());
	return 0;
    }
    NSPIOLOG_ERROR("lb_fair del %s not found", r->cid());
    return -1;
}

Role *lb_fair::loadbalance_recv() {
    Role *r = NULL;

    
    return r;
}


Role *lb_fair::__loadbalance_send(struct nspiomsg *msg) {
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


Role *lb_fair::loadbalance_send(struct nspiomsg *msg) {
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
