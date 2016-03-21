// copyright:
//            (C) SINA Inc.
//
//           file: nspio/api/sync_comsumer.cc
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


Comsumer *NewComsumer() {
    __comsumer *svr = new (std::nothrow) __comsumer();
    return svr;
}

__comsumer::__comsumer() :
    internconn(NULL), api_stats(API_MODULE_STATITEM_KEYRANGE, NULL),
    timeout_msec(0), options(0), st_timestamp(rt_mstime())
{
    route_genid(_roleid);
}

__comsumer::~__comsumer() {
    if (internconn)
	delete internconn;
}

int __comsumer::Fd() {
    if (internconn)
	return internconn->Fd();
    return -1;
}

int __comsumer::CacheSize() {
    if (!internconn) {
	errno = SPIO_EINTERN;
	return -1;
    }
    return internconn->CacheSize(SO_READCACHE);
}

int __comsumer::FlushCache() {
    int ret = 0;

    if (!internconn) {
	errno = SPIO_EINTERN;
	return -1;
    }
    while ((ret = internconn->Flush()) < 0 && errno == EAGAIN) {}
    return ret;
}


int __comsumer::Connect(const string &appname, const string &apphost) {
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
    ret = __registe_role(internconn, appname, _roleid, ROLE_APPDISPATCHER);
    if (ret < 0) {
	Close();
	errno = SPIO_EREGISTRY;
	return -1;
    }
    _appname = appname;
    _apphost = apphost;
    return 0;
}

int __comsumer::Close() {
    pp._reset_recv();
    pp._reset_send();
    if (internconn) {
	internconn->Close();
	delete internconn;
    }
    internconn = NULL;
    return 0;
}


int __comsumer::SetOption(int opt, ...) {
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


int __comsumer::GetOption(int opt, ...) {

    return 0;
}

int __comsumer::Recv(string &msg, string &rt) {
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
	    mem_free(raw.rt, appmsg_rtlen(raw.rt->ttl));
	    errno = EAGAIN;
	    api_stats.incrkey(API_RECV_ICMPMSG);
	    return -1;
	}
	api_stats.incrkey(API_RECV_NORMALMSG);
	msg.clear();
	rt.clear();
	if (raw.s.len > 0) {
	    msg.assign(raw.s.data, raw.s.len);
	    mem_free(raw.s.data, raw.s.len);
	}
	rt.assign((char *)&raw.hdr, sizeof(raw.hdr));
	rt.append((char *)raw.rt, appmsg_rtlen(raw.rt->ttl));
	mem_free(raw.rt, appmsg_rtlen(raw.rt->ttl));
    }
    return ret;
}


int __comsumer::Send(const string &msg, const string &rt) {
    return Send(msg.data(), msg.size(), rt);
}


int __comsumer::Recv(Msghdr *hdr, string &msg, string &rt) {
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
	    if (raw.s.len)
		mem_free(raw.s.data, raw.s.len);
	    mem_free(raw.rt, appmsg_rtlen(raw.rt->ttl));
	    errno = EAGAIN;
	    if (!IS_NORMALMSG(&raw.hdr))
		api_stats.incrkey(API_RECV_ICMPMSG);
	    return -1;
	}
	api_stats.incrkey(API_RECV_NORMALMSG);
	msg.clear();
	rt.clear();
	memcpy((char *)hdr, raw.s.data, HDRLEN);
	if (raw.s.len > HDRLEN)
	    msg.assign(raw.s.data + HDRLEN, raw.s.len - HDRLEN);
	rt.assign((char *)&raw.hdr, sizeof(raw.hdr));
	rt.append((char *)raw.rt, appmsg_rtlen(raw.rt->ttl));
	mem_free(raw.s.data, raw.s.len);
	mem_free(raw.rt, appmsg_rtlen(raw.rt->ttl));
    }
    return ret;
}


int __comsumer::Send(const Msghdr *hdr, const string &msg, const string &rt) {
    string tmpmsg;
    
    if (!hdr) {
	errno = EINVAL;
	return -1;
    }
    tmpmsg.assign((char *)hdr, HDRLEN);
    tmpmsg += msg;
    return Send(tmpmsg.data(), tmpmsg.size(), rt);
}



int __comsumer::Send(const char *data, uint32_t len, const string &rt) {
    int rtlen = 0;
    struct appmsg raw = {};

    if (!internconn) {
	errno = SPIO_EINTERN;
	return -1;
    }
    
    if (!data) {
	errno = EINVAL;
	return -1;
    }
    rtlen = rt.size() - sizeof(*raw.rt) - sizeof(raw.hdr);
    if (rtlen < 2 * RTLEN || (rtlen % RTLEN != 0)) {
	errno = EINVAL;
	return -1;
    }
    raw.s.len = len;
    raw.s.data = const_cast<char *>(data);
    raw.hdr = *((struct spiohdr *)rt.data());
    raw.rt = (struct appmsg_rt *)(rt.data() + sizeof(raw.hdr));
    return Send(&raw);
}

int __comsumer::Recv(struct appmsg *raw) {
    int ret = 0;
    int64_t rtt = 0;
    struct appmsg_rt *rt = NULL;

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
	if (!(rt = (struct appmsg_rt *)mem_zalloc(appmsg_rtlen(raw->hdr.ttl)))) {
	    mem_free(raw->s.data, raw->s.len);
	    errno = EAGAIN;
	    return -1;
	}
	raw->s.len -= RTLEN * raw->hdr.ttl;
	rt->ttl = raw->hdr.ttl;
	memcpy(rt->rts, raw->s.data + raw->s.len, RTLEN * raw->hdr.ttl);
	raw->rt = rt;
	if (!raw->s.len) {
	    mem_free(raw->s.data, RTLEN * raw->hdr.ttl);
	    raw->s.data = NULL;
	}
	rtt = rt_mstime() - raw->hdr.timestamp;
	rt->rts[rt->ttl - 1].u.env.gocost = rtt - rt->rts[rt->ttl - 1].u.env.go;
    } else if (ret < 0 && errno != EAGAIN)
	pp._reset_recv();
    return ret;
}


int __comsumer::Send(const struct appmsg *raw) {
    int ret = 0;
    struct slice s[2];
    struct spiohdr hdr = {};
    
    if (!internconn) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (!raw || !raw->rt) {
	errno = EINVAL;
	return -1;
    }

    hdr = raw->hdr;
    hdr.ttl = raw->rt->ttl;
    hdr.size = hdr.ttl * RTLEN + raw->s.len;
    s[0] = raw->s;
    s[1].len = hdr.ttl * RTLEN;
    s[1].data = (char *)raw->rt->rts;
    if ((ret = pp._send_massage(internconn, &hdr, 2, (struct slice *)&s)) < 0 && errno != EAGAIN)
	pp._reset_send();
    else if (ret == 0)
	api_stats.incrkey(API_SEND_PACKAGES);
    return ret;    
}


}
