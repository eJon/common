// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/proto/proto.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_PROTOCOL_H_
#define _H_PROTOCOL_H_

#include <stdio.h>
#include <uuid/uuid.h>
#include <sstream>
#include <string.h>
#include <nspio/api.h>
#include "base/slice.h"
#include "base/list.h"
#include "base/crc.h"
#include "os/memalloc.h"

NSPIO_DECLARATION_START



enum ICMPFLAG {
    PIOICMPMSG = FLAG_BIT1,
    PIOICMP_ROLEATTR = FLAG_BIT2 | PIOICMPMSG,
    PIOICMP_APPSTATUS = FLAG_BIT3 | PIOICMPMSG,
    PIOICMP_ROLESTATUS = FLAG_BIT4 | PIOICMPMSG,
    SYNCAPIMSG = FLAG_BIT5,
    ASYNCAPIMSG = FLAG_BIT6,
    APPICMPMSG = FLAG_BIT7,
    APPICMP_DELIVERERROR = FLAG_BIT8 | APPICMPMSG,
};

#define IS_PIOICMP(hdr) (((hdr)->flags & PIOICMPMSG))
#define IS_APPICMP(hdr) (((hdr)->flags & APPICMPMSG))
#define IS_NORMALMSG(hdr) ((hdr)->flags & (SYNCAPIMSG|ASYNCAPIMSG))

enum {
    HEADER_BUFFERLEN = 1024,
    SPIORT_WARNING_LEN = 40
};
#define RHDRLEN ((int)sizeof(struct spiohdr))


struct spiort {
    union {
	struct {
	    uuid_t uuid;
	    uint8_t ip[4];
	    uint16_t port;
	    uint16_t gocost;
	    uint16_t gostay;
	    uint32_t go;
	} env;
	char __padding[SPIORT_WARNING_LEN];
    } u;
};
#define RTLEN ((int)sizeof(struct spiort))


#define MAX_APPNAME_LEN 64
enum ROLETYPE {
    ROLE_APPRECEIVER = 0x01,
    ROLE_PIORECEIVER = 0x02,
    ROLE_APPDISPATCHER = 0x04,
    ROLE_PIODISPATCHER = 0x08,
    ROLE_RECEIVER = ROLE_APPRECEIVER|ROLE_PIORECEIVER,
    ROLE_DISPATCHER = ROLE_APPDISPATCHER|ROLE_PIODISPATCHER,
    ROLE_MONITOR = 0x10,
};
#define IS_PIOROLE(_rt) (!!(_rt & (ROLE_PIORECEIVER|ROLE_PIODISPATCHER)))
#define IS_RECEIVER(_rt) (!!(_rt & ROLE_RECEIVER))
#define IS_PIORECEIVER(_rt) (!!(_rt & ROLE_PIORECEIVER))
#define IS_DISPATCHER(_rt) (!!(_rt & ROLE_DISPATCHER))
#define IS_PIODISPATCHER(_rt) (!!(_rt & ROLE_PIODISPATCHER))
#define ROLESTR(_r) (IS_RECEIVER(_r->Type()) ? "r" : "d")

struct spioreg {
    uint32_t version;
    uint32_t rtype;
    int64_t timeout;
    uuid_t rid;
    char appname[MAX_APPNAME_LEN];
};

#define REGHDRLEN ((int)sizeof(spioreg))



static inline int header_parse(struct spiohdr *hdr, char *out) {
    stringstream ss;
    string fmt_msg;
    uint32_t fmt_len = HEADER_BUFFERLEN - 1;

    if (IS_PIOICMP(hdr))
	ss << "PIOICMP ";
    else if (IS_APPICMP(hdr))
	ss << "APPICMP ";
    ss << "{version:" << (int)hdr->version << " ttl:" << (int)hdr->ttl;
    ss << " seqid:" << (int)hdr->seqid;
    ss << " flags:" << (uint32_t)hdr->flags << " size:" << hdr->size;
    ss << " sendstamp:" << hdr->timestamp << " hdrsum:" << hdr->hdrcheck << "}";
    fmt_msg = ss.str();
    if (fmt_len > fmt_msg.size())
	fmt_len = fmt_msg.size();
    memcpy(out, fmt_msg.data(), fmt_len);
    out[fmt_len] = '\0';
    return 0;
}

 
struct appmsg_rt {
    uint8_t ttl;
    struct spiort rts[0];
};

static inline uint32_t appmsg_rtlen(int ttl) {
    return sizeof(struct appmsg_rt) + ttl * RTLEN;
}

static inline struct appmsg_rt *appmsg_rtdup(struct appmsg_rt *rt) {
    struct appmsg_rt *drt = NULL;
    if ((drt = (struct appmsg_rt *)mem_zalloc(appmsg_rtlen(rt->ttl))) != NULL)
	memcpy((char *)drt, (char *)rt, appmsg_rtlen(rt->ttl));
    return drt;
}

static inline void free_appmsg_rt(struct appmsg_rt *rt) {
    if (rt)
	mem_free(rt, appmsg_rtlen(rt->ttl));
}


struct appmsg {
    struct spiohdr hdr;
    struct slice s;
    struct appmsg_rt *rt;
    struct list_head node;
};

static inline struct appmsg *new_appmsg(const char *data, uint32_t len) {
    struct appmsg *new_msg = NULL;
    
    if (len < 0) {
	errno = EINVAL;
	return NULL;
    }
    if (!(new_msg = (struct appmsg *)mem_zalloc(sizeof(*new_msg)))) {
	errno = ENOMEM;
	return NULL;
    }
    new_msg->s.len = len;
    if (new_msg->s.len && !(new_msg->s.data = (char *)mem_zalloc(new_msg->s.len))) {
	mem_free(new_msg, sizeof(*new_msg));
	errno = ENOMEM;
	return NULL;
    }
    if (data)
	memcpy(new_msg->s.data, data, len);
    return new_msg;
}

static inline void free_appmsg(struct appmsg *msg) {
    if (!msg)
	return;
    if (msg->s.len > 0)
	mem_free(msg->s.data, msg->s.len);
    mem_free(msg, sizeof(*msg));
}



#define list_first_appmsg(head)						\
    ({struct appmsg *__msg = list_first(head, struct appmsg, node); __msg;})
#define list_for_each_appmsg(pos, head)				\
    list_for_each_entry((pos), (head), struct appmsg, node)
#define list_for_each_appmsg_safe(pos, next, head)			\
    list_for_each_entry_safe((pos), (next), (head), struct appmsg, node)

struct nspiomsg {
    struct spiohdr hdr;
    char *data;
    int64_t resitime;
    struct spiort *route;
    struct list_head mq_node;
};

#define MSGHDRLEN ((int)sizeof(struct nspiomsg))
#define MSGPKGLEN(msg) (MSGHDRLEN + (msg)->hdr.size)

#define list_first_nspiomsg(head)		\
    list_first(head, struct nspiomsg, mq_node);
#define list_for_each_nspiomsg(pos, head)			\
    list_for_each_entry((pos), struct nspiomsg, mq_node)
#define list_for_each_nspiomsg_safe(pos, next, head)			\
    list_for_each_entry_safe((pos), (next), (head), struct nspiomsg, mq_node)

class Conn;
class proto_parser {
 public:
    proto_parser();
    ~proto_parser();

    inline void _set_recv_max(uint32_t max_size) {
	msg_max_size = max_size;
    }
    inline uint64_t _dropped_cnt() {
	return dropped_cnt;
    }
    int _reset_recv();
    int _reset_send();

    int _recv_massage(Conn *conn, struct spiohdr *hdr, struct slice *s);
    int _send_massage(Conn *conn, const struct spiohdr *hdr, int c, struct slice *s);
    int _send_massage_async(Conn *conn, const struct spiohdr *hdr, int c, struct slice *s);

    int _recv_massage(Conn *conn, struct nspiomsg **header, int reserve);
    int _send_massage(Conn *conn, struct nspiomsg *header);
    int _send_massage_async(Conn *conn, struct nspiomsg *header);
#ifndef __NSPIO_UT__
 private:
#endif
    uint64_t dropped_cnt;
    uint32_t msg_max_size;
    struct nspiomsg msg, *bufhdr;
    uint32_t recv_hdr_index;
    uint32_t recv_data_index;
    uint32_t send_hdr_index;
    uint32_t send_data_index;

    int __drop(Conn *conn, uint32_t size);
    int __recv_hdr(Conn *conn, struct spiohdr *hdr);
    int __recv_data(Conn *conn, int c, struct slice *s);
    int __send_hdr(Conn *conn, const struct spiohdr *hdr);
    int __send_data(Conn *conn, int c, struct slice *s);
};


static inline int package_validate(struct spiohdr *header, void *data) {
    struct spiohdr copyheader = *header;

    copyheader.hdrcheck = 0;
    if (crc16((char *)&copyheader, RHDRLEN) != header->hdrcheck)
	return -1;
    if (header->datacheck &&
	data && crc16((char *)data, header->size) != header->datacheck)
	return -1;
    return 0;
}


static inline int package_makechecksum(struct spiohdr *header, void *data, uint32_t len) {
    struct spiohdr copyheader = *header;
    copyheader.hdrcheck = 0;
    if (data && len) {
	copyheader.datacheck = header->datacheck = crc16((char *)data, len);
    }
    header->hdrcheck = crc16((char *)&copyheader, RHDRLEN);
    return 0;
}










}








#endif // _H_PROTOCOL_H_
