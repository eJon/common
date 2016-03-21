// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/receiver.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include "log.h"
#include "os/time.h"
#include "mem_status.h"
#include "receiver.h"
#include "proto/icmp.h"

NSPIO_DECLARATION_START


extern spio_mem_status_t spio_mem_stats;

static mem_stat_t *receivers_mem_stats = &spio_mem_stats.receivers;


Receiver::Receiver(const string &_appid, const string &roleid) :
    Role(_appid, roleid, ROLE_RECEIVER)
{
    receivers_mem_stats->alloc++;
    receivers_mem_stats->alloc_size += sizeof(Receiver);
}


Receiver::Receiver(int rt, const string &_appid, const string &roleid) :
    Role(_appid, roleid, rt)
{
    receivers_mem_stats->alloc++;
    receivers_mem_stats->alloc_size += sizeof(Receiver);
}

Receiver::~Receiver() {
    Conn *conn = NULL;
    unBind(&conn);
    if (conn)
	delete conn;
    receivers_mem_stats->alloc--;
    receivers_mem_stats->alloc_size -= sizeof(Receiver);
}

int Receiver::process_normal_massage(struct nspiomsg *msg) {
    uint32_t size = 0;
    int64_t rtt = 0;
    struct spiort *rt = NULL;
    module_stat *rstat = Stat();
    
    size = msg->hdr.size;
    rt = msg->route = (struct spiort *)(msg->data + size - RTLEN);
    pp._reset_recv();
    if ((rtt = rt_mstime() - msg->hdr.timestamp) > 0)
	rstat->incrkey(RRTT, rtt);
    rtt = rtt > 0 ? rtt : 0;
    rt->u.env.gocost = rtt - rt->u.env.go;
    return 0;
}

int Receiver::process_self_icmp(struct nspiomsg *msg) {

    return -1;
}

int Receiver::Recv(struct nspiomsg **req) {
    int ret = 0;
    module_stat *rstat = Stat();
    Conn *internconn = Connect();
    struct nspiomsg *nmsg = NULL;
    
    if ((ret = pp._recv_massage(internconn, &nmsg, RTLEN)) == 0) {
	rstat->incrkey(RECV_BYTES, MSGPKGLEN(nmsg));
	if (IS_PIOICMP(&nmsg->hdr)) {
	    if (process_self_icmp(nmsg) < 0)
		PushRCVICMP(nmsg);
	    return -1;
	}
	process_normal_massage(nmsg);
	*req = nmsg;
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

int Receiver::Send(struct nspiomsg *msg) {
    int ret = 0;
    module_stat *rstat = Stat();
    Conn *internconn = Connect();

    if ((ret = pp._send_massage(internconn, msg)) == 0) {
	if (IS_NORMALMSG(&msg->hdr))
	    rstat->incrkey(SEND_PACKAGES);
	else
	    rstat->incrkey(SEND_ICMPS);	    
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


int Receiver::BatchSend(int max_send, struct list_head *to_head) {
    MQueue *mq = Queue();
    Conn *internconn = Connect();
    struct nspiomsg *resp = NULL;
    char hlog[HEADER_BUFFERLEN] = {};
    int ret = 0, idx;

    if (!mq) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (max_send <= 0)
	max_send = mq->Size();
    for (idx = 0; idx < max_send; idx++) {
	if (!(resp = mq->PopMsg(to_head)))
	    break;
	if ((ret = Send(resp)) < 0) {
	    header_parse(&resp->hdr, hlog);
	    mem_free(resp, MSGPKGLEN(resp) + RTLEN);
	    NSPIOLOG_ERROR("%s [r:%s:%s] send response %s with errno %d",
			   cappid(), cid(), remoteip(), hlog, errno);
	    break;
	}
	mem_free(resp, MSGPKGLEN(resp) + RTLEN);
	resp = NULL;
    }
    while ((ret = internconn->Flush()) < 0 && errno == EAGAIN) {
	/* flush cache */
    }
    if (ret < 0 && errno != EAGAIN)
	NSPIOLOG_ERROR("%s %s(r) flush cache with errno %d", cappid(), cid(), errno);
    return idx;
}

int Receiver::send_icmp() {
    Conn *internconn = Connect();
    struct spiohdr *hdr = NULL;
    struct nspiomsg *icmp_msg = NULL;

    while ((icmp_msg = PopSNDICMP()) != NULL) {
	hdr = &icmp_msg->hdr;
	if (Send(icmp_msg) < 0) {
	    NSPIOLOG_ERROR("%s %s(r) send icmp %d with errno %d",
			   cappid(), cid(), hdr->flags, errno);
	    mem_free(icmp_msg, MSGPKGLEN(icmp_msg));
	    break;
	}
	NSPIOLOG_INFO("%s %s(r) send icmp %d", cappid(), cid(), hdr->flags);
	mem_free(icmp_msg, MSGPKGLEN(icmp_msg));
    }
    while (internconn->Flush() < 0 && errno == EAGAIN) {
	/* flush cache */
    }
    return 0;
}

}
