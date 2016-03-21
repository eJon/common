#include <sharelib/server/tcp_context.h>

#include <sharelib/server/epoll_server.h>
#include <event.h>
#include <errno.h>
#include <string.h>
#include <sharelib/util/sharelib_log.h>
#include <arpa/inet.h>
#include <string>
SHARELIB_BS;
using namespace std;
CTcpContext::CTcpContext(){
    Reset(); 
}
CTcpContext::~CTcpContext(){
    Close(TCP_CLOSE_NORMAL,"Normal Close");
}

void CTcpContext::Reset(){
    sock_ = -1;
    master_ = NULL;
    cport_ = -1;
    rcurr_ = rbytes_ = 0;
    rsize_ = ADDS_IOBUF_LEN;
    memset(rbuf_,'\0',sizeof(rbuf_));
    memset(cip_,'\0',sizeof(cip_));  
}

int CTcpContext::Open(const int sfd,int cport, const char*cip,CEpollServer *svr) {
    if((sfd < 0)||(cport <0)||(cip == NULL)||(svr==NULL)){
        SHARELIB_LOG_ERROR("CTcpContext::Open(), para error");
    }
  
    // start read event
    event_set(&ev_, sfd, EV_READ | EV_PERSIST ,&OnEventHandle,this);
    event_base_set(svr->evbase(), &ev_);
    int ret = event_add(&ev_, NULL);
    if(!ret){
        sock_ = sfd;
        master_ = svr;
        cport_ = cport;
        strncpy(cip_,cip,DEF_IDX_IP_LEN);
    }else{
        SHARELIB_LOG_ERROR("CTcpContext::Open(), Failed to add control event");
    }  
    return ret;
}
void CTcpContext::Close(int status, const char* reason) {
    SHARELIB_LOG_DEBUG("CTcpContext::Close(), Server Close Client Socket[fd=%d cip=%s cport=%d],"
                       "close reason[code=%d errmsg=%s]",sock_,cip_,cport_,status,reason);
    /* delete the event, the socket and the conn */
    if(master_ != NULL){
        event_del(&ev_);
        if (sock_ >= 0) {
            close(sock_);
            sock_ = -1;
        }
        master_->IdleConn(this);
    }
}

int CTcpContext::OnRecvData(void){
    char *prase_buf = rbuf_ + rcurr_;
    do {
        if(rbytes_ <= DEF_IDX_MSG_HEAD_SIZE) {
            SHARELIB_LOG_DEBUG("CTcpContext::OnRecvData(), msg head size not enough");
            return 0;
        }
        //prase msg head
        int iSize = ntohl(*(uint32_t*)prase_buf);
        SHARELIB_LOG_DEBUG("CTcpContext::OnRecvData(), msg_head_size=[%d]\n",iSize);
        if(iSize>=ADDS_IOBUF_LEN){
            SHARELIB_LOG_ERROR("CTcpContext::OnRecvData(), msg size[%u] >= [%d]",iSize,ADDS_IOBUF_LEN);
            Close(TCP_CLOSE_UNACCEPTABLE_DATA,"send msg too long");
            return -1;
        }
        if((iSize+DEF_IDX_MSG_HEAD_SIZE) > rbytes_){
            SHARELIB_LOG_DEBUG("CTcpContext::OnRecvData(), msg body size not enough");
            return 0;
        }
    
        rcurr_ += (DEF_IDX_MSG_HEAD_SIZE+iSize); 
        prase_buf += (DEF_IDX_MSG_HEAD_SIZE+iSize);
        rbytes_ -= (DEF_IDX_MSG_HEAD_SIZE+iSize);

        std::string pkg(prase_buf - iSize, iSize);
        //SHARELIB_LOG_DEBUG("CTcpContext::OnRecvData(), pkg is [%s]", pkg.c_str());
        HandlePkg(pkg);
        
    }while(1);
    return 0;
}
void CTcpContext::OnEventHandle(int fd, short event, void* arg){
    CTcpContext *ctx = (CTcpContext *)arg;
    if(ctx == NULL) {
        SHARELIB_LOG_ERROR("CTcpContext::OnRead(), recv ctx == NULL");
        return;
    }
    if (fd != ctx->GetFd()) {
        SHARELIB_LOG_ERROR("CTcpContext::OnRead(), recv fd[=%d] != ctx->fd_[=%d]",
                           fd,ctx->GetFd());
        ctx->Close(TCP_CLOSE_UNEXPECTED,"client socket does not match");
        return;
    }
    ctx->OnRead();
}
void CTcpContext::OnRead(){
    if(rcurr_ != 0) {
        if(rbytes_ != 0) {
            memmove(rbuf_, rbuf_+rcurr_, rbytes_);
        }
        rcurr_ = 0;
    }
    SHARELIB_LOG_DEBUG("CTcpContext::OnRead(), recv from client event coming,"
                       "buffer[rcurr=%d rbytes_=%d rsize_=%d]",
                       rcurr_,rbytes_,rsize_);

    int avail = rsize_ - rbytes_;
    if(avail <= 0){
        SHARELIB_LOG_ERROR("CTcpContext::OnRead(), recv buf is overflow");
        OnRecvData();
        return;
    }

    int res = read(sock_, rbuf_ + rbytes_, avail);
    if (res > 0) {
        rbytes_ += res;
        if (res == avail) {
            SHARELIB_LOG_INFO("CTcpContext::OnRead(), socket data has not readed");
        }
    }

    if (res == 0) {
        SHARELIB_LOG_ERROR("CTcpContext::OnRead(), Client closed connection");
        Close(TCP_CLOSE_GOING_AWAY,"client closed connection");
        return;
    }
    if(res == -1){
        if(!(errno == EAGAIN || errno == EWOULDBLOCK)) {
            SHARELIB_LOG_ERROR("CTcpContext::ReadFromFd(), Reading from client error: %s", strerror(errno));
            Close(TCP_CLOSE_UNEXPECTED,"read from client socket error");
        }
        return;
    }

    OnRecvData();
    return;
}

int CTcpContext::SendMessage(int iSocket, const char *msg, int size) {
    //SHARELIB_LOG_DEBUG("CTcpContext::SendMessage %s", msg);
    int newsize = size + DEF_IDX_MSG_HEAD_SIZE;
    char *buf  = (char*)calloc(newsize,1);
    if(buf == NULL){
        SHARELIB_LOG_ERROR("calloc error");
        return -1;
    }
    (*(uint32_t*)buf) = htonl(size);
    memmove(buf + DEF_IDX_MSG_HEAD_SIZE, msg, size);
    
    int sendlen = 0;
    while(sendlen < newsize){
        int ret = write(iSocket, buf + sendlen, newsize-sendlen);
        if (ret < 0){
            delete buf;
            SHARELIB_LOG_ERROR("CTcpContext::SendMessage, socket error, write ret is %d", ret);
            return -1;
        }
        sendlen += ret;
    }
    delete buf;
    
    return 0;
}
SHARELIB_ES;

