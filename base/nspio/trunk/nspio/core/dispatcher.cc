// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/dispatcher.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "os/time.h"
#include "mem_status.h"
#include "log.h"
#include "mqp.h"
#include "proto/icmp.h"
#include "dispatcher.h"


NSPIO_DECLARATION_START

extern spio_mem_status_t spio_mem_stats;
static mem_stat_t *dispatchers_mem_stats = &spio_mem_stats.dispatchers;

Dispatcher::Dispatcher(const string &_appid, const string &roleid) :
    Role(_appid, roleid, ROLE_DISPATCHER)
{
    dispatchers_mem_stats->alloc++;
    dispatchers_mem_stats->alloc_size += sizeof(Dispatcher);
}

Dispatcher::Dispatcher(int rt, const string &_appid, const string &roleid) :
    Role(_appid, roleid, rt)
{
    dispatchers_mem_stats->alloc++;
    dispatchers_mem_stats->alloc_size += sizeof(Dispatcher);
}


Dispatcher::~Dispatcher() {
    Conn *conn = NULL;
    unBind(&conn);
    if (conn)
	delete conn;
    dispatchers_mem_stats->alloc--;
    dispatchers_mem_stats->alloc_size -= sizeof(Dispatcher);
}

int Dispatcher::process_normal_massage(struct nspiomsg *msg) {
    int64_t rtt = 0;
    struct spiort *rt = NULL;
    struct spiohdr *hdr = NULL;
    module_stat *rstat = Stat();

    hdr = &msg->hdr;
    rt = (struct spiort *)(msg->data + hdr->size - RTLEN);
    if ((rtt = rt_mstime() - hdr->timestamp) > 0)
	rstat->incrkey(DRTT, rtt);
    hdr->size -= RTLEN;
    hdr->ttl--;
    if (hdr->ttl)
	msg->route = (struct spiort *)(msg->data + hdr->size - RTLEN);
    else
	msg->route = NULL;
    pp._reset_recv();
    return 0;
}

int Dispatcher::process_self_icmp(struct nspiomsg *icmp_msg) {
    struct spiohdr *hdr = &icmp_msg->hdr;

    if (hdr->flags & PIOICMP_ROLESTATUS) {
	process_role_status_icmp(icmp_msg);
	return 0;
    }
    return -1;
}
    
    
int Dispatcher::Recv(struct nspiomsg **resp) {
    int ret = 0;
    module_stat *rstat = Stat();
    Conn *internconn = Connect();
    struct nspiomsg *nmsg = NULL;
    
    if ((ret = pp._recv_massage(internconn, &nmsg, 0)) == 0) {
	rstat->incrkey(RECV_BYTES, MSGPKGLEN(nmsg));
	if (IS_PIOICMP(&nmsg->hdr)) {
	    rstat->incrkey(RECV_ICMPS);
	    if (process_self_icmp(nmsg) < 0)
		PushRCVICMP(nmsg);
	    return -1;
	}
	process_normal_massage(nmsg);
	*resp = nmsg;
	rstat->incrkey(RECV_PACKAGES);
    } else if (ret < 0 && errno != EAGAIN) {
	pp._reset_recv();
	SET_MSGERROR(this);
	if (errno == SPIO_ECHECKSUM)
	    rstat->incrkey(CHECKSUM_ERRORS);
	rstat->incrkey(RECV_ERRORS);
    }
    return ret;
}


static void __append_route_info(struct nspiomsg *msg, const string rid, const string rip) {
    int64_t rtt = 0;
    struct spiort rt = {}, *prev_rt = msg->route;

    route_setip(&rt, rip);
    route_setid(&rt, rid);
    rtt = rt_mstime() - msg->hdr.timestamp;
    rt.u.env.go = rtt > 0 ? rtt : 0;
    prev_rt->u.env.gostay = rtt - prev_rt->u.env.go - prev_rt->u.env.gocost;
    memcpy(msg->data + msg->hdr.size, (char *)&rt, RTLEN);
    msg->hdr.ttl++;
    msg->hdr.size += RTLEN;
}

int Dispatcher::Send(struct nspiomsg *msg) {
    int ret = 0;
    module_stat *rstat = Stat();
    Conn *internconn = Connect();

    if ((ret = pp._send_massage(internconn, msg)) == 0) {
	rstat->incrkey(SEND_PACKAGES);
	rstat->incrkey(SEND_BYTES, MSGPKGLEN(msg));
    } else if (ret < 0 && errno != EAGAIN) {
	pp._reset_send();
	SET_MSGERROR(this);
	if (errno == SPIO_ECHECKSUM)
	    rstat->incrkey(CHECKSUM_ERRORS);
	rstat->incrkey(SEND_ERRORS);
    }
    return ret;
}

int Dispatcher::BatchSend(int max_send, struct list_head *to_head) {
    int ret = 0, idx = 0;
    MQueue *mq = Queue();
    Conn *internconn = Connect();
    char hlog[HEADER_BUFFERLEN] = {};
    struct nspiomsg *req = NULL, copyheader = {};
    
    if (!mq) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (!ROLECANW(this))
	return 0;
    if (max_send <= 0)
	max_send = mq->Size();
    while (mq->Size()) {
	if (!(req = mq->PopMsg(to_head)))
	    break;
	memcpy(&copyheader, req, sizeof(copyheader));
	__append_route_info(&copyheader, Id(), remoteip());
	if ((ret = Send(&copyheader)) < 0) {
	    header_parse(&req->hdr, hlog);
	    mem_free(req, MSGPKGLEN(req) + RTLEN);
	    NSPIOLOG_ERROR("%s [r:%s:%s] send request %s with errno %d",
			   cappid(), cid(), remoteip(), hlog, errno);
	    break;
	}
	idx++;
	mem_free(req, MSGPKGLEN(req) + RTLEN);
    }
    while ((ret = internconn->Flush()) < 0 && errno == EAGAIN) {
	/* flushing cache */
    }
    if (ret < 0 && errno != EAGAIN)
	NSPIOLOG_ERROR("%s %s(d) flush cache with errno %d", cappid(), cid(), errno);
    return idx;
}

int Dispatcher::send_icmp() {
    Conn *internconn = Connect();
    struct spiohdr *hdr = NULL;
    struct nspiomsg *icmp_msg = NULL;

    while ((icmp_msg = PopSNDICMP()) != NULL) {
	if (Send(icmp_msg) < 0) {
	    hdr = &icmp_msg->hdr;
	    NSPIOLOG_ERROR("%s [d:%s:%s] send icmp %d with errno %d",
			   cappid(), cid(), remoteip(), hdr->flags, errno);
	    mem_free(icmp_msg, MSGPKGLEN(icmp_msg));
	    break;
	}
	mem_free(icmp_msg, MSGPKGLEN(icmp_msg));
    }
    while (internconn->Flush() < 0 && errno == EAGAIN) {
	/* flush cache */
    }
    return 0;
}

int Dispatcher::process_role_status_icmp(struct nspiomsg *msg) {
    MQueue *mq = Queue();
    struct role_status_icmp *other_peer = NULL;

    other_peer = (struct role_status_icmp *)msg->data;
    if (other_peer->dispatchers > 0 && !ROLECANW(this)) {
	EnablePOLLOUT();
	mq->AddMonitor(this);
	SETROLE_W(this);
	NSPIOLOG_WARN("%s [d:%s:%s] open write", cappid(), cid(), remoteip());
    } else if (other_peer->dispatchers <= 0 && ROLECANW(this)) {
	DisablePOLLOUT();
	mq->DelMonitor(this);
	detach_from_msgout_head();
	UNSETROLE_W(this);
	NSPIOLOG_WARN("%s [d:%s:%s] shutdown write", cappid(), cid(), remoteip());
    }
    mem_free(msg, MSGPKGLEN(msg));

    return 0;
}


}
