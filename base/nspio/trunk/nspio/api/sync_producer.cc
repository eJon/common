// copyright:
//            (C) SINA Inc.
//
//           file: nspio/api/sync_producer.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <uuid/uuid.h>
#include <string.h>
#include <stdarg.h>
#include <nspio/errno.h>
#include "os/time.h"
#include "net/tcp.h"
#include "proto/route.h"
#include "os/memalloc.h"
#include "sync_api.h"

NSPIO_DECLARATION_START

static int version = 0x8;

static int __registe_role(Conn *internconn,
			  const string &appname, const string &roleid, int rtype) {
    struct spioreg rgh = {};
    int nbytes = 0, ret = 0;

    rgh.version = version;
    rgh.rtype = rtype;
    rgh.timeout = 0;
    rgh_setid(&rgh, roleid);
    strncpy(rgh.appname, appname.data(), MAX_APPNAME_LEN);
    while (nbytes < REGHDRLEN) {
	ret = internconn->Write((char *)&rgh + nbytes, REGHDRLEN - nbytes);
	if (ret < 0)
	    return -1;
	nbytes += ret;
    }
    return 0;
}

Producer *NewProducer() {
    __producer *cli = new (std::nothrow) __producer();
    return cli;
}

__producer::__producer() :
    internconn(NULL), api_stats(API_MODULE_STATITEM_KEYRANGE, NULL),
    crsid(0), timeout_msec(0), options(0), st_timestamp(rt_mstime())
{
    route_genid(_roleid);
}

__producer::~__producer() {
    if (internconn)
	delete internconn;
}

int __producer::Fd() {
    if (internconn)
	return internconn->Fd();
    return -1;
}

int __producer::CacheSize() {
    if (!internconn) {
	errno = SPIO_EINTERN;
	return -1;
    }
    return internconn->CacheSize(SO_READCACHE);
}


int __producer::FlushCache() {
    int ret = 0;

    if (!internconn) {
	errno = SPIO_EINTERN;
	return -1;
    }
    while ((ret = internconn->Flush()) < 0 && errno == EAGAIN) {}
    return ret;
}


int __producer::Connect(const string &appname, const string &apphost) {
    int ret = 0;

    if (internconn) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (!(internconn = DialTCP("tcp", "", apphost))) {
	errno = SPIO_ENETUNREACH;
	return -1;
    }
    internconn->SetSockOpt(SO_NODELAY, 1);
    ret = __registe_role(internconn, appname, _roleid, ROLE_APPRECEIVER);
    if (ret < 0) {
	Close();
	errno = SPIO_EREGISTRY;
	return -1;
    }
    _appname = appname;
    _apphost = apphost;
    return 0;
}


int __producer::Close() {
    pp._reset_recv();
    pp._reset_send();
    if (internconn) {
	internconn->Close();
	delete internconn;
    }
    internconn = NULL;
    return 0;
}



int __producer::SetOption(int opt, ...) {
    va_list ap;
    
    if (!internconn) {
	errno = SPIO_EINTERN;
	return -1;
    }
    switch (opt) {
    case OPT_TIMEOUT:
	{
	    int to_msec = 0;
	    va_start(ap, opt);
	    to_msec = va_arg(ap, int);
	    va_end(ap);
	    if (to_msec < 0) {
		errno = EINVAL;
		return -1;
	    }
	    timeout_msec = to_msec;
	    return internconn->SetSockOpt(SO_TIMEOUT, timeout_msec);
	}
    case OPT_AUTORECONNECT:
	{
	    // deprecated option
	    options |= OPT_AUTORECONNECT;
	    break;
	}
    case OPT_NONBLOCK:
	{
	    int flags = 0;
	    va_start(ap, opt);
	    flags = va_arg(ap, int);
	    va_end(ap);
	    if (flags)
		internconn->SetSockOpt(SO_NONBLOCK, 1);
	    else
		internconn->SetSockOpt(SO_NONBLOCK, 0);
	    break;
	}
    case OPT_SOCKCACHE:
	{
	    internconn->SetSockOpt(SO_READCACHE, 0);
	    internconn->SetSockOpt(SO_WRITECACHE, 0);
	    break;
	}
    case OPT_KEEPORDER:
	{
	    options |= OPT_KEEPORDER;
	    break;
	}
    }
    return 0;
}


int __producer::GetOption(int opt, ...) {

    return 0;
}

int __producer::Send(const string &msg) {
    return Send(msg.data(), msg.size());
}

int __producer::Recv(string &msg) {
    int ret = 0;
    struct appmsg raw = {};
    
    if (!internconn) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if ((ret = Recv(&raw)) == 0) {
	if (!IS_NORMALMSG(&raw.hdr)) {
	    if (raw.s.len > 0)
		mem_free(raw.s.data, raw.s.len);
	    errno = EAGAIN;
	    api_stats.incrkey(API_RECV_ICMPMSG);
	    return -1;
	}
	api_stats.incrkey(API_RECV_NORMALMSG);
	msg.assign(raw.s.data, raw.s.len);
	if (raw.s.len > 0)
	    mem_free(raw.s.data, raw.s.len);
    }
    return ret;
}


int __producer::Send(const Msghdr *hdr, const string &msg) {
    string tmpmsg;

    if (!hdr) {
	errno = EINVAL;
	return -1;
    }
    
    tmpmsg.assign((char *)hdr, HDRLEN);
    tmpmsg += msg;
    return Send(tmpmsg.data(), tmpmsg.size());
}


int __producer::Recv(Msghdr *hdr, string &msg) {
    int ret = 0;
    struct appmsg raw = {};
    
    if (!internconn) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (!hdr) {
	errno = EINVAL;
	return -1;
    }
    
    if ((ret = Recv(&raw)) == 0) {
	if (raw.s.len < HDRLEN || !IS_NORMALMSG(&raw.hdr)) {
	    if (raw.s.len > 0)
		mem_free(raw.s.data, raw.s.len);
	    if (raw.s.len < HDRLEN)
		errno = SPIO_ECHECKSUM;
	    else {
		errno = EAGAIN;
		api_stats.incrkey(API_RECV_ICMPMSG);
	    }
	    return -1;
	}
	api_stats.incrkey(API_RECV_NORMALMSG);
	memcpy((char *)hdr, raw.s.data, HDRLEN);
	if (raw.s.len > HDRLEN)
	    msg.assign(raw.s.data + HDRLEN, raw.s.len - HDRLEN);
	mem_free(raw.s.data, raw.s.len);
    }
    return ret;
}



int __producer::Send(const char *data, uint32_t len) {
    struct appmsg raw = {};

    if (!data) {
	errno = EINVAL;
	return -1;
    }
    if (!internconn) {
	errno = SPIO_EINTERN;
	return -1;
    }
    raw.s.data = const_cast<char *>(data);
    raw.s.len = len;
    return Send(&raw);
}

int __producer::Send(const struct appmsg *raw) {
    int ret = 0;
    string rip;
    struct slice s[2];
    struct spiort rt = {};
    struct spiohdr hdr = {};
    
    if (!internconn) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (!raw) {
	errno = EINVAL;
	return -1;
    }
    internconn->RemoteAddr(rip);
    route_setip(&rt, rip);
    route_setid(&rt, _roleid);
    rt.u.env.go = 0;
    hdr = raw->hdr;
    if (!(hdr.flags & ASYNCAPIMSG) && (options & OPT_KEEPORDER))
	hdr.seqid = get_seqid();
    hdr.timestamp = rt_mstime();
    hdr.ttl = 1;
    hdr.size = RTLEN + raw->s.len;
    if (!(hdr.flags & ASYNCAPIMSG))
	hdr.flags |= SYNCAPIMSG;
    s[0] = raw->s;
    s[1].len = RTLEN;
    s[1].data = (char *)&rt;
    if ((ret = pp._send_massage(internconn, &hdr, 2, (struct slice *)&s)) < 0 && errno != EAGAIN)
	pp._reset_send();
    else if (ret == 0)
	api_stats.incrkey(API_SEND_PACKAGES);
    return ret;
}


int __producer::Recv(struct appmsg *raw) {
    int ret = 0;

    if (!internconn) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (!raw) {
	errno = EINVAL;
	return -1;
    }

    if ((ret = pp._recv_massage(internconn, &raw->hdr, &raw->s)) == 0) {
	api_stats.incrkey(API_RECV_PACKAGES);
	raw->s.len -= RTLEN * raw->hdr.ttl;
	raw->rt = NULL;
	if (!raw->s.len) {
	    mem_free(raw->s.data, raw->hdr.size);
	    raw->s.data = NULL;
	}
	if (!(raw->hdr.flags & ASYNCAPIMSG) && (options & OPT_KEEPORDER)
	    && raw->hdr.seqid != crsid) {
	    if (raw->s.len)
		mem_free(raw->s.data, raw->hdr.size);
	    ret = -1;
	    errno = EAGAIN;
	}
    } else if (ret < 0 && errno != EAGAIN)
	pp._reset_recv();
    return ret;
}



}
