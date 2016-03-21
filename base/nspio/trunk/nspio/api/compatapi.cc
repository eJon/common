// copyright:
//            (C) SINA Inc.
//
//           file: nspio/api/compatapi.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/compat_api.h>
#include "os/time.h"
#include "sync_api.h"

NSPIO_DECLARATION_START


CSpioApi::CSpioApi() :
    m_joined(false), m_msgid(0), m_to_msec(0),
    cr(NULL), pr(NULL)
{

}

CSpioApi::~CSpioApi() {
    if (cr)
	delete cr;
    if (pr)
	delete pr;
}

int CSpioApi::init(const string &grouphost) {
    if (m_joined) {
	errno = SPIO_EINTERN;
	return -1;
    }
    m_grouphost = grouphost;
    return 0;
}

#if defined(__NSPIO_UT__)
int CSpioApi::fd() {
    if (!m_joined) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (cr)
	return cr->Fd();
    else if (pr)
	return pr->Fd();

    return 0;
}
#endif


Comsumer *CSpioApi::get_comsumer() {
    return cr;
}

Producer *CSpioApi::get_producer() {
    return pr;
}



int CSpioApi::join_client(const string &groupname) {
    if (m_joined || cr) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (pr) {
	if (m_groupname == groupname)
	    return 0;
    } else {
	if (!(pr = NewProducer())) {
	    errno = ENOMEM;
	    return -1;
	}
    }
    m_groupname = groupname;
    return rejoin();
}

int CSpioApi::join_server(const string &groupname) {
    if (m_joined || pr) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (cr) {
	if (m_groupname == groupname)
	    return 0;
    } else {
	if (!(cr = NewComsumer())) {
	    errno = ENOMEM;
	    return -1;
	}
    }
    m_groupname = groupname;
    return rejoin();
}

int CSpioApi::rejoin() {

    if (cr) {
	cr->Close();
	while (cr->Connect(m_grouphost, m_groupname) != 0)
	    rt_usleep(100000);
	if (m_to_msec)
	    cr->SetOption(OPT_TIMEOUT, m_to_msec);
	m_joined = true;
    } else if (pr) {
	pr->Close();
	while (pr->Connect(m_grouphost, m_groupname) != 0)
	    rt_usleep(100000);
	if (m_to_msec)
	    pr->SetOption(OPT_TIMEOUT, m_to_msec);
	m_joined = true;
	pr->SetOption(OPT_KEEPORDER);
    }
    return 0;
}


int CSpioApi::recv(string &msg, int timeout) {
    int ret = 0;
    __producer *client = NULL;
    __comsumer *server = NULL;
    uint64_t end_tt = timeout ? (uint64_t)rt_mstime() + timeout : ~(0);

    if (!m_joined) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (cr) {
	server = (__comsumer *)cr;
	if (!m_to_msec || m_to_msec != timeout) {
	    m_to_msec = timeout;
	    server->SetOption(OPT_TIMEOUT, timeout);
	}
	do {
	    ret = server->Recv(msg, cur_rt);
	} while (ret < 0 && errno == EAGAIN && (uint64_t)rt_mstime() < end_tt);
    } else if (pr) {
	client = (__producer *)pr;
	if (!m_to_msec || m_to_msec != timeout) {
	    m_to_msec = timeout;
	    client->SetOption(OPT_TIMEOUT, timeout);
	}
	do {
	    ret = client->Recv(msg);
	} while (ret < 0 && (errno == EAGAIN && (uint64_t)rt_mstime() < end_tt));
    }
    if (ret < 0 && errno == EPIPE) {
	rejoin();
	errno = SPIO_ESPIODOWN;
    }
    return ret;
}


int CSpioApi::send(const string &msg, int timeout) {
    int ret = 0;
    __producer *client = NULL;
    __comsumer *server = NULL;
    uint64_t end_tt = timeout ? ((uint64_t)rt_mstime() + timeout) : ~(0);

    if (!m_joined) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (cr && cur_rt.empty()) {
	errno = SPIO_EINTERN;
	return -1;
    }
    if (cr) {
	server = (__comsumer *)cr;
	if (!m_to_msec || m_to_msec != timeout) {
	    m_to_msec = timeout;
	    server->SetOption(OPT_TIMEOUT, timeout);
	}
	do {
	    ret = server->Send(msg, cur_rt);
	} while (ret < 0 && errno == EAGAIN && (uint64_t)rt_mstime() < end_tt);
	cur_rt.clear();
    } else if (pr) {
	client = (__producer *)pr;
	if (!m_to_msec || m_to_msec != timeout) {
	    m_to_msec = timeout;
	    client->SetOption(OPT_TIMEOUT, timeout);
	}
	do {
	    ret = client->Send(msg);
	} while (ret < 0 && errno == EAGAIN && (uint64_t)rt_mstime() < end_tt);
    }
    if (ret < 0 && errno == EPIPE) {
	rejoin();
	errno = SPIO_ESPIODOWN;
    }
    return ret;
}




void CSpioApi::terminate() {
    if (cr) {
	cr->Close();
	delete cr;
	cr = NULL;
    } else if (pr) {
	pr->Close();
	delete pr;
	pr = NULL;
    }
    m_joined = false;
}


}
