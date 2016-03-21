#include <sharelib/server/epoll_server.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
SHARELIB_BS;
using namespace std;
// Fixed-size (one-byte) messages communicated via control pipe.
const char kControlMessageShutdown[] = { '.' };

// Returns true on success.
static bool SetNonBlock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return flags >= 0 && fcntl(fd, F_SETFL, flags | O_NONBLOCK) == 0;
}

CEpollServer::CEpollServer()
    : listening_sock_(-1),
      shutdown_requested_(false),
      evbase_(NULL) {
    control_descriptor_[0] = -1;
    control_descriptor_[1] = -1;
    cache_queue_.SetCacheSize(32);
      }

CEpollServer::~CEpollServer() {
    UnInit();
}

int CEpollServer::Init(int port,const char *address) {
    if (evbase_ || shutdown_requested_) {
        SHARELIB_LOG_ERROR("CEpollServer::Init: has already inited");
        return -1;
    }

    evbase_ = event_init();
    if (!evbase_) {
        SHARELIB_LOG_ERROR("CEpollServer::Init: Couldn't create libevent base");
        return -1;
    }

    if (pipe(control_descriptor_) ||
        !SetNonBlock(control_descriptor_[0]) ||
        !SetNonBlock(control_descriptor_[1])) {
        SHARELIB_LOG_ERROR("CEpollServer::Init: Failed to create control pipe");
        return -1;
    }

    listening_sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_sock_ < 0) {
        SHARELIB_LOG_ERROR("CEpollServer::Init: Failed to create socket");
        return -1;
    }

    {
        int on = 1;
        setsockopt(listening_sock_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    // Let the OS allocate a port number.
    addr.sin_port = htons(port);
    if (strlen(address) <= 0) {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        addr.sin_addr.s_addr = inet_addr(address);
    }

    if (bind(listening_sock_,
             reinterpret_cast<struct sockaddr*>(&addr),
             sizeof(addr))) {
        SHARELIB_LOG_ERROR("CEpollServer::Init: Failed to bind server socket");
        return -1;
    }
    if (listen(listening_sock_, 512)) {
        SHARELIB_LOG_ERROR("CEpollServer::Init: Failed to listen server socket");
        return -1;
    }
    if (!SetNonBlock(listening_sock_)) {
        SHARELIB_LOG_ERROR("CEpollServer::Init: Failed to go non block");
        return -1;
    }

    event_set(&connection_event_, listening_sock_, EV_READ | EV_PERSIST,
              &OnConnect, this);
    event_base_set(evbase_, &connection_event_);
    if (event_add(&connection_event_, NULL)) {
        SHARELIB_LOG_ERROR("CEpollServer::Init: Failed to add listening event");
        return -1;
    }

    event_set(&control_event_, control_descriptor_[0], EV_READ | EV_PERSIST,
              &OnControlRequest, this);
    event_base_set(evbase_, &control_event_);
    if (event_add(&control_event_, NULL)) {
        SHARELIB_LOG_ERROR("CEpollServer::Init: Failed to add control event");
        return -1;
    }
    return 0;
}

void CEpollServer::UnInit() {
    if (shutdown_requested_)
        SHARELIB_LOG_ERROR("Event dispatch loop terminated upon request");
    else
        SHARELIB_LOG_ERROR("Event dispatch loop terminated unexpectedly");
    CloseAll();
}
void CEpollServer::Shutdown() {
    write(control_descriptor_[1], kControlMessageShutdown, 1);
}

void CEpollServer::CloseAll() {
    CtxMap::iterator rit = ctx_map_.begin();

    for(;rit != ctx_map_.end(); rit++){
        rit->second->Close(TCP_CLOSE_NORMAL, "close mandy");
        //ctx_map_.erase(rit);
    }
    ctx_map_.clear();
    if (listening_sock_ >= 0) {
        close(listening_sock_);
        listening_sock_ = -1;
    }
    for (int i = 0; i < 2; ++i) {
        if (control_descriptor_[i] >= 0) {
            close(control_descriptor_[i]);
            control_descriptor_[i] = -1;
        }
    }
    event_del(&control_event_);
    event_del(&connection_event_);
    if (evbase_) {
        event_base_free(evbase_);
        evbase_ = NULL;
    }
}

void CEpollServer::IdleConn(CTcpContext* cs) {
    CtxMap::iterator rit = ctx_map_.find((uint64_t)cs);
    if (rit != ctx_map_.end()) {
        ctx_map_.erase(rit);
        cache_queue_.AddTail(cs);
    }
    //cs->Reset();
}

CTcpContext* CEpollServer::GetFreshConn() {
    CTcpContext * cs = cache_queue_.PopHead();
    if(cs == NULL){
        cs = CreateTcpContext();
    }
    if(cs != NULL){
        ctx_map_[(uint64_t)cs] = cs;
    }
    cs->Reset();
    return cs;
}

// static
void CEpollServer::OnConnect(int listening_sock, short event, void* ctx) {
    CEpollServer* self = static_cast<CEpollServer*>(ctx);
    CTcpContext* cs = self->GetFreshConn();
    struct sockaddr_in stSockAddr;
    int sockAddrSize;
    int sock = accept(listening_sock, 
                      (struct sockaddr *)&stSockAddr,
                      (socklen_t *)&sockAddrSize);
    if (sock < 0 || !SetNonBlock(sock)) {
        //failed to accept a connection
        SHARELIB_LOG_ERROR("CEpollServer::OnConnect: Failed to accept a new connection");
        self->IdleConn(cs);
        return;
    }
    int cport = stSockAddr.sin_port;
    char *cip = inet_ntoa(stSockAddr.sin_addr); 
    SHARELIB_LOG_DEBUG("CEpollServer::OnConnect: client socket connect coming[fd=%d cport=%d cip=%s]",
                       sock,cport,cip);
    if(cs->Open(sock,cport,cip,self)){
        self->IdleConn(cs);
        SHARELIB_LOG_ERROR("CEpollServer::OnConnect: client socket connect failed[fd=%d cport=%d cip=%s]",
                           sock,cport,cip);
        return;
    }
    SHARELIB_LOG_DEBUG("CEpollServer::OnConnect: client socket connect success[fd=%d cport=%d cip=%s]",
                       sock,cport,cip);
}

// static
void CEpollServer::OnControlRequest(int fd, short event, void* ctx) {
    CEpollServer* self = static_cast<CEpollServer*>(ctx);

    char c;
    if (1 == read(fd, &c, 1) && c == *kControlMessageShutdown) {
        self->shutdown_requested_ = true;
        event_base_loopbreak(self->evbase_);
    }
}

SHARELIB_ES;
