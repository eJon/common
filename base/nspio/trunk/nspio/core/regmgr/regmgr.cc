// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/regmgr/regmgr.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <nspio/errno.h>
#include <map>
#include "log.h"
#include "mem_status.h"
#include "os/os.h"
#include "os/time.h"
#include "runner/thread.h"
#include "role.h"
#include "timer.h"
#include "rgmh_tb.h"
#include "net/tcp.h"

NSPIO_DECLARATION_START

enum {
    RGM_ACCEPTER = 0x01,
    RGM_REGISTER = 0x02,
    RGM_CONNECTOR = 0x03,
};

enum {
    RT_MAGIC = 0x85755,
};


extern spio_mem_status_t spio_mem_stats;

static mem_stat_t *rgm_mem_stats = &spio_mem_stats.rgm;
static mem_stat_t *rgm_accepter_mem_stats = &spio_mem_stats.rgm_accepter;
static mem_stat_t *rgm_register_mem_stats = &spio_mem_stats.rgm_register;
static mem_stat_t *rgm_connector_mem_stats = &spio_mem_stats.rgm_connector;


RgmHandler::RgmHandler() {
    INIT_LIST_LINK(&rgm_node);
}    

int RgmHandler::attach_to_rgm_head(struct list_head *head) {
    if (!rgm_node.linked) {
	rgm_node.linked = 1;
	list_add(&rgm_node.node, head);
	return 0;
    }
    return -1;
}

int RgmHandler::detach_from_rgm_head() {
    if (rgm_node.linked) {
	list_del(&rgm_node.node);
	rgm_node.linked = 0;
	return 0;
    }
    return -1;
}


    
class rgm_register;
class rgm_accepter;
class rgm_connector;

class rgm_accepter_hook {
public:
    int accept_new(Conn *conn);
};

class rgm_register_hook {
public:
    int register_done();
};

    
class RegisterManager : public Rgm {
public:
    RegisterManager();
    ~RegisterManager();
    friend class rgm_accepter;
    
    int Init(SpioConfig *_conf);
    int InsertHandler(RgmHandler *handler);
    int RemoveHandler(RgmHandler *handler);
    RgmHandler *Find(const struct spioreg *rgh);
    int Listen(const string &laddr);
    int ConnectTo(const string &rid, const string &app, const string &raddr, RgmHandler *h);
    int Start();
    void Stop();
    int StartThread();
    void StopThread();
    bool Running() {
	return stopping ? false : true;
    }

private:
    int inited;
    bool stopping;
    Thread thread;
    Epoller *poller;
    SpioConfig conf;
    rgmh_tb htb;

    TDQueue timewait_connectors;
    rgm_connector *pop_tw_connector();
    int push_tw_connector(rgm_connector *con);

    pmutex lock;
    struct list_head waiting_connectors;
    struct list_head rgm_accepters;
    struct list_head rgm_registers;
    struct list_head rgm_connectors;

    rgm_connector *pop_waiting_connector();
    int push_waiting_connector(rgm_connector *con);
    rgm_connector *pop_rgm_connector();
    int push_rgm_connector(rgm_connector *con);
    rgm_accepter *pop_rgm_accepter();
    int push_rgm_accepter(rgm_accepter *acc);
    rgm_register *pop_rgm_register();
    int push_rgm_register(rgm_register *reg);
    
    int process_rgm_registers();
    int process_rgm_connectors();
};

Rgm *NewRegisterManager(SpioConfig *_spioconf) {
    RegisterManager *rgm = NULL;
    if ((rgm = new (std::nothrow) RegisterManager()) != NULL)
	rgm->Init(_spioconf);
    return rgm;
}

    
class rgm_eventhandler {
public:
    virtual ~rgm_eventhandler() {}
    virtual int handle_event(Epoller *poller, EpollEvent *ev, void *data) = 0;
    virtual int enable_event(Epoller *poller, int to_msec) = 0;
    virtual int disable_event(Epoller *poller) = 0;
    virtual int attach_to_rgm_head(struct list_head *head) = 0;
    virtual int detach_from_rgm_head() = 0;
};

#define RGMOFEV(ev) ((rgm_eventhandler *)(ev)->ptr)
#define list_rgmeh(link) ((rgm_eventhandler *)link->self)    

#define list_accepter(link) ((rgm_accepter *)link->self) 
#define list_first_accepter(head)					\
    ({struct list_link *__pos =						\
	    list_first(head, struct list_link, node);  list_accepter(__pos);})

#define list_register(link) ((rgm_register *)link->self) 
#define list_first_register(head)					\
    ({struct list_link *__pos =						\
	    list_first(head, struct list_link, node);  list_register(__pos);})

#define list_connector(link) ((rgm_connector *)link->self) 
#define list_first_connector(head)					\
    ({struct list_link *__pos =						\
	    list_first(head, struct list_link, node);  list_connector(__pos);})

    
class rgm_accepter : public rgm_eventhandler {
public:
    rgm_accepter(Listener *_listener);
    ~rgm_accepter();

    int enable_event(Epoller *poller, int to_msec);
    int disable_event(Epoller *poller);
    int handle_event(Epoller *poller, EpollEvent *ev, void *data);

    int attach_to_rgm_head(struct list_head *head);
    int detach_from_rgm_head();

private:    
    int rgmtype;    
    Listener *listener;
    EpollEvent ee;
    rgm_accepter_hook *hook;
    struct list_link rgm_node;
};


class rgm_register : public rgm_eventhandler {
public:
    rgm_register(Conn *conn);
    ~rgm_register();

    int enable_event(Epoller *poller, int to_msec);
    int disable_event(Epoller *poller);
    int handle_event(Epoller *poller, EpollEvent *ev, void *data);    

    int attach_to_rgm_head(struct list_head *head);
    int detach_from_rgm_head();

private:    
    int rgmtype;
    Conn *internconn;
    EpollEvent ee;
    struct spioreg reghdr;
    SectionReadWriter sr;
    struct list_link rgm_node;
};


class rgm_connector : public rgm_eventhandler {
public:
    rgm_connector(const string &raddr, struct spioreg *hdr, RgmHandler *h);
    ~rgm_connector();

    int tryconnect();
    int enable_event(Epoller *poller, int to_msec);
    int disable_event(Epoller *poller);
    int handle_event(Epoller *poller, EpollEvent *ev, void *data);    

    int attach_to_rgm_head(struct list_head *head);
    int detach_from_rgm_head();

private:
    int rgmtype;
    string raddr;
    Conn *internconn;
    EpollEvent ee;
    struct spioreg reghdr;
    RgmHandler *handler;
    SectionReadWriter sr;
    struct list_link rgm_node;
};


rgm_accepter::rgm_accepter(Listener *_listener) :
    rgmtype(RGM_ACCEPTER), listener(_listener), hook(NULL)
{
    INIT_LIST_LINK(&rgm_node);
    rgm_accepter_mem_stats->alloc++;
    rgm_accepter_mem_stats->alloc_size += sizeof(rgm_accepter);
}

rgm_accepter::~rgm_accepter() {
    if (listener)
	delete listener;
    rgm_accepter_mem_stats->alloc--;
    rgm_accepter_mem_stats->alloc_size -= sizeof(rgm_accepter);
}

int rgm_accepter::enable_event(Epoller *poller, int to_msec) {
    ee.SetEvent(listener->Fd(), EPOLLIN, this);
    ee.to_nsec = (int64_t)to_msec * 1000000;
    return poller->CtlAdd(&ee);
}


int rgm_accepter::disable_event(Epoller *poller) {
    return poller->CtlDel(&ee);
}

int rgm_accepter::handle_event(Epoller *poller, EpollEvent *ev, void *data) {
    RegisterManager *rgm = (RegisterManager *)data;
    rgm_register *reg = NULL;
    Conn *conn = NULL;

    if (!(ev->happened & EPOLLIN)) {
	NSPIOLOG_ERROR("invalid epoll_events %d", ev->happened);
	return -1;
    }
    if (!(conn = listener->Accept())) {
	NSPIOLOG_ERROR("accept a null new connection");
	return EAGAIN;
    }
    if (!(reg = new (std::nothrow) rgm_register(conn))) {
	NSPIOLOG_ERROR("unexpect out of memory");
	delete conn;
	return EAGAIN;
    }
    if (reg->enable_event(poller, rgm->conf.register_timeout_msec) < 0) {
	NSPIOLOG_ERROR("enable registe worker epollevent");
	delete reg;
	return EAGAIN;
    }
    rgm->push_rgm_register(reg);
    return EAGAIN;
}

int rgm_accepter::attach_to_rgm_head(struct list_head *head) {
    if (!rgm_node.linked) {
	rgm_node.linked = 1;
	list_add(&rgm_node.node, head);
	return 0;
    }
    return -1;
}

int rgm_accepter::detach_from_rgm_head() {
    if (rgm_node.linked) {
	list_del(&rgm_node.node);
	rgm_node.linked = 0;
	return 0;
    }
    return -1;
}



rgm_register::rgm_register(Conn *conn) :
    rgmtype(RGM_REGISTER), internconn(conn)
{
    INIT_LIST_LINK(&rgm_node);
    memset(&reghdr, 0, sizeof(reghdr));
    sr.InitReader(REGHDRLEN);

    // Disable Nagle's algorithm
    internconn->SetSockOpt(SO_NODELAY, 1);
    internconn->SetSockOpt(SO_NONBLOCK, 1);

    rgm_register_mem_stats->alloc++;
    rgm_register_mem_stats->alloc_size += sizeof(rgm_register);
}

rgm_register::~rgm_register() {
    if (internconn)
	delete internconn;
    rgm_register_mem_stats->alloc--;
    rgm_register_mem_stats->alloc_size -= sizeof(rgm_register);
}

int rgm_register::enable_event(Epoller *poller, int to_msec) {
    ee.SetEvent(internconn->Fd(), EPOLLIN, this);
    ee.to_nsec = (int64_t)to_msec * 1000000;
    return poller->CtlAdd(&ee);
}


int rgm_register::disable_event(Epoller *poller) {
    return poller->CtlDel(&ee);
}

int rgm_register::handle_event(Epoller *poller, EpollEvent *ev, void *data) {
    RegisterManager *rgm = (RegisterManager *)data;
    RgmHandler *handler = NULL;

    if (!(ev->happened & (EPOLLIN)))
	return -1;
    if (-1 == sr.ReadSection(internconn, (char *)&reghdr) && errno == EAGAIN)
	return EAGAIN;

    // Register this role into corresponse app. chekced is used for
    // indicate event have been processed.
    // hit = crc16((const char *)reghdr.rid, sizeof(reghdr.rid));
    if ((handler = rgm->Find(&reghdr)) == NULL)
	return -1;
    disable_event(poller);
    if (handler->Register(&reghdr, internconn) < 0) {
	delete internconn;
    }
    internconn = NULL;
    return 0;
}

int rgm_register::attach_to_rgm_head(struct list_head *head) {
    if (!rgm_node.linked) {
	rgm_node.linked = 1;
	list_add(&rgm_node.node, head);
	return 0;
    }
    return -1;
}

int rgm_register::detach_from_rgm_head() {
    if (rgm_node.linked) {
	list_del(&rgm_node.node);
	rgm_node.linked = 0;
	return 0;
    }
    return -1;
}

rgm_connector::rgm_connector(const string &_raddr, struct spioreg *hdr, RgmHandler *h) :
    rgmtype(RGM_CONNECTOR), raddr(_raddr), internconn(NULL), reghdr(*hdr), handler(h)
{
    INIT_LIST_LINK(&rgm_node);
    rgm_connector_mem_stats->alloc++;
    rgm_connector_mem_stats->alloc_size += sizeof(rgm_connector);
}

rgm_connector::~rgm_connector() {
    if (internconn)
	delete internconn;
    rgm_connector_mem_stats->alloc--;
    rgm_connector_mem_stats->alloc_size -= sizeof(rgm_connector);
}

int rgm_connector::tryconnect() {
    if (internconn) {
	if (internconn->Reconnect() < 0) {
	    NSPIOLOG_ERROR("appid: %s connect to remote app: %s", reghdr.appname, raddr.c_str());
	    return -1;
	}
    } else if (!(internconn = DialTCP("tcp", "", raddr))) {
	NSPIOLOG_ERROR("appid: %s connect to remote app: %s", reghdr.appname, raddr.c_str());
	return -1;
    }
    internconn->SetSockOpt(SO_NODELAY, 1);
    internconn->SetSockOpt(SO_NONBLOCK, 1);
    sr.InitWriter(REGHDRLEN);
    return 0;
}

int rgm_connector::enable_event(Epoller *poller, int to_msec) {
    ee.SetEvent(internconn->Fd(), EPOLLOUT, this);
    ee.to_nsec = (int64_t)to_msec * 1000000;
    return poller->CtlAdd(&ee);
}


int rgm_connector::disable_event(Epoller *poller) {
    return poller->CtlDel(&ee);
}

int rgm_connector::handle_event(Epoller *poller, EpollEvent *ev, void *data) {

    if (!(ev->happened & (EPOLLOUT))) {
	errno = ev->happened;
	return -1;
    }

    if (-1 == sr.WriteSection(internconn, (char *)&reghdr) && errno == EAGAIN)
	return EAGAIN;
    // ... i am a dispatcher, connect to a remote reciever
    reghdr.rtype = ROLE_PIODISPATCHER;
    disable_event(poller);
    if (handler->Register(&reghdr, internconn) < 0)
	delete internconn;
    internconn = NULL;
    return 0;
}
    
int rgm_connector::attach_to_rgm_head(struct list_head *head) {
    if (!rgm_node.linked) {
	rgm_node.linked = 1;
	list_add(&rgm_node.node, head);
	return 0;
    }
    return -1;
}

int rgm_connector::detach_from_rgm_head() {
    if (rgm_node.linked) {
	list_del(&rgm_node.node);
	rgm_node.linked = 0;
	return 0;
    }
    return -1;
}


    
    
void rgm_connector_tdqueue_clean_func(void *data) {
    rgm_connector *con = (rgm_connector *)data;
    delete con;
}

RegisterManager::RegisterManager() :
    inited(0), stopping(true), poller(NULL)
{
    INIT_LIST_HEAD(&waiting_connectors);
    INIT_LIST_HEAD(&rgm_accepters);
    INIT_LIST_HEAD(&rgm_registers);
    INIT_LIST_HEAD(&rgm_connectors);

    pmutex_init(&lock);
    timewait_connectors.SetCleanFunc(rgm_connector_tdqueue_clean_func);
    rgm_mem_stats->alloc++;
    rgm_mem_stats->alloc_size += sizeof(RegisterManager);
}

RegisterManager::~RegisterManager() {
    rgm_eventhandler *h = NULL;
    map<string, struct list_head *>::iterator it;
    struct list_head head = {};
    struct list_link *pos = NULL, *next = NULL;

    if (poller)
	delete poller;
    pmutex_destroy(&lock);
    INIT_LIST_HEAD(&head);
    list_splice(&waiting_connectors, &head);
    list_splice(&rgm_accepters, &head);
    list_splice(&rgm_registers, &head);
    list_splice(&rgm_connectors, &head);
    list_for_each_list_link_safe(pos, next, &head) {
	h = list_rgmeh(pos);
	delete h;
    }
    rgm_mem_stats->alloc--;
    rgm_mem_stats->alloc_size -= sizeof(RegisterManager);
}

int RegisterManager::Init(SpioConfig *_conf) {
    if (inited)
	return -1;
    if (_conf)
	conf = *_conf;
    inited = 1;
    return 0;
}

int RegisterManager::InsertHandler(RgmHandler *handler) {
    return htb.insert(handler);
}

int RegisterManager::RemoveHandler(RgmHandler *handler) {
    return htb.remove(handler);;
}


int RegisterManager::Listen(const string &laddr) {
    int ret;
    TCPListener *listen;
    rgm_accepter *acc;

    // First. create the poller if it is NULL
    if (!poller && !(poller = EpollCreate(1024, 500))) {
	NSPIOLOG_ERROR("regmgr listen %s out of memory", laddr.c_str());
	return -1;
    }
    if (!(listen = ListenTCP("tcp", laddr, 1024))) {
	NSPIOLOG_ERROR("regmgr listen on %s with errno %d", laddr.c_str(), errno);
	return -1;
    }
    // Importance for fast restart
    listen->SetReuseAddr(true);
    if (!(acc = new (std::nothrow) rgm_accepter(listen))) {
	NSPIOLOG_ERROR("regmgr listen on %s out of memory", laddr.c_str());
	delete listen;
	return -1;
    }
    if ((ret = acc->enable_event(poller, 0)) < 0) {
	NSPIOLOG_ERROR("regmgr listen on %s with errno %d", laddr.c_str(), errno);
	delete acc;
	return -1;
    }
    push_rgm_accepter(acc);
    return 0;
}


int RegisterManager::ConnectTo(const string &roleid, const string &appid,
			       const string &raddr, RgmHandler *handler) {
    struct spioreg reghdr = {};
    rgm_connector *con = NULL;

    reghdr.rtype = ROLE_PIORECEIVER;
    uuid_parse(roleid.c_str(), reghdr.rid);
    strncpy(reghdr.appname, appid.c_str(), MAX_APPNAME_LEN);

    if (!(con = new (std::nothrow) rgm_connector(raddr, &reghdr, handler))) {
	NSPIOLOG_ERROR("unexpect out of memory");
	return -1;
    }
    if (con->tryconnect() < 0)
	push_tw_connector(con);
    else
	push_waiting_connector(con);
    return 0;
}

rgm_connector *RegisterManager::pop_tw_connector() {
    rgm_connector *con = NULL;
    timewait_connectors.Lock();
    con = (rgm_connector *)timewait_connectors.PopTD();
    timewait_connectors.unLock();
    return con;
}
    
int RegisterManager::push_tw_connector(rgm_connector *con) {
    timewait_connectors.Lock();
    timewait_connectors.PushTD(con, conf.register_interval_msec);
    timewait_connectors.unLock();
    return 0;
}
    
rgm_connector *RegisterManager::pop_waiting_connector() {
    rgm_connector *connector = NULL;

    pmutex_lock(&lock);
    if (!list_empty(&waiting_connectors)) {
	connector = list_first_connector(&waiting_connectors);
	connector->detach_from_rgm_head();
    }
    pmutex_unlock(&lock);
    return connector;
}

int RegisterManager::push_waiting_connector(rgm_connector *connector) {
    pmutex_lock(&lock);
    connector->attach_to_rgm_head(&waiting_connectors);
    pmutex_unlock(&lock);
    return 0;
}


rgm_connector *RegisterManager::pop_rgm_connector() {
    rgm_connector *connector = NULL;

    if (!list_empty(&rgm_connectors)) {
	connector = list_first_connector(&rgm_connectors);
	connector->detach_from_rgm_head();
    }
    return connector;
}

int RegisterManager::push_rgm_connector(rgm_connector *connector) {
    connector->attach_to_rgm_head(&waiting_connectors);
    return 0;
}

rgm_accepter *RegisterManager::pop_rgm_accepter() {
    rgm_accepter *accepter = NULL;

    if (!list_empty(&rgm_accepters)) {
	accepter = list_first_accepter(&rgm_accepters);
	accepter->detach_from_rgm_head();
    }
    return accepter;
}

int RegisterManager::push_rgm_accepter(rgm_accepter *accepter) {
    accepter->attach_to_rgm_head(&rgm_accepters);
    return 0;
}

rgm_register *RegisterManager::pop_rgm_register() {
    rgm_register *reg = NULL;

    if (!list_empty(&rgm_registers)) {
	reg = list_first_register(&rgm_registers);
	reg->detach_from_rgm_head();
    }
    return reg;
}

int RegisterManager::push_rgm_register(rgm_register *reg) {
    reg->attach_to_rgm_head(&rgm_accepters);
    return 0;
}

    

RgmHandler *RegisterManager::Find(const struct spioreg *rgh) {
    return htb.balance_find(rgh);
}


int RegisterManager::process_rgm_registers() {
    int ret;
    rgm_eventhandler *rgm_eh = NULL;
    EpollEvent *ev = NULL;
    struct list_head io_head, to_head;
    struct list_link *pos = NULL, *next = NULL;

    INIT_LIST_HEAD(&io_head);
    INIT_LIST_HEAD(&to_head);
    
    if ((ret = poller->Wait(&io_head, &to_head, conf.epoll_timeout_msec)) < 0)
	NSPIOLOG_ERROR("poller wait with errno %d", errno);

    list_for_each_list_link_safe(pos, next, &to_head) {
	ev = list_ev(pos);
	ev->detach();
	ev->attach(&io_head);
    }
    list_for_each_list_link_safe(pos, next, &io_head) {
	ev = list_ev(pos);
	ev->detach();
	rgm_eh = RGMOFEV(ev);
	if ((ret = rgm_eh->handle_event(poller, ev, this)) != EAGAIN) {
	    rgm_eh->disable_event(poller);
	    rgm_eh->detach_from_rgm_head();
	    delete rgm_eh;
	}
    }
    detach_for_each_poll_link(&io_head);
    detach_for_each_poll_link(&to_head);

    return 0;
}


int RegisterManager::process_rgm_connectors() {
    rgm_connector *con = NULL;

    if ((con = pop_tw_connector()) != NULL) {
	if (con->tryconnect() < 0)
	    push_tw_connector(con);
	else
	    push_waiting_connector(con);
    }
	
    // Handle waiting for connectto registe workers.
    if ((con = pop_waiting_connector()) != NULL) {
	if (con->enable_event(poller, conf.register_timeout_msec) == 0)
	    push_rgm_connector(con);
	else
	    push_tw_connector(con);
    }

    return 0;
}

void RegisterManager::Stop() {
    stopping = true;
}

int RegisterManager::Start() {
    pid_t pid = gettid();
    stopping = false;

    if (!poller) {
	errno = SPIO_EINTERN;
	NSPIOLOG_ERROR("regmgr start with errno %d", errno);
	return -1;
    }
    NSPIOLOG_NOTICE("regmgr start ok with pid %d", pid);
    while (!stopping) {
	process_rgm_registers();
	process_rgm_connectors();
    }
    NSPIOLOG_NOTICE("regmgr stop ok with pid %d", pid);    
    return 0;
}

void RegisterManager::StopThread() {
    bool __running = stopping ? false : true;
    Stop();
    if (__running)
	thread.Stop();
}

static int registe_manager_thread_worker(void *arg_) {
    RegisterManager *rm = (RegisterManager *)arg_;
    return rm->Start();
}

int RegisterManager::StartThread() {
    if (stopping)
	thread.Start(registe_manager_thread_worker, this);
    return 0;
}



}
