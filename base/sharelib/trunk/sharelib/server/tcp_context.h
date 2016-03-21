#ifndef SHARELIB_TCPCONTEXT_H_
#define SHARELIB_TCPCONTEXT_H_

#include <event.h>
#include <string>
#include <sharelib/common.h>
SHARELIB_BS;
enum WebSocketStatusCode {
    TCP_CLOSE_NORMAL = 1000,
    TCP_CLOSE_GOING_AWAY = 1001,
    TCP_CLOSE_PROTOCOL_ERROR = 1002,
    TCP_CLOSE_UNACCEPTABLE_DATA = 1003,
    TCP_CLOSE_UNEXPECTED = 1004
};
#define DEF_IDX_MSG_HEAD_SIZE 4
#define ADDS_IOBUF_LEN  1024000
#define DEF_IDX_IP_LEN	64
class CEpollServer;
class CTcpContext {
public:
    CTcpContext();
    virtual ~CTcpContext();
public:
    static void OnEventHandle(int fd, short event, void*);
    virtual void HandlePkg(std::string& pkg) = 0;
    void Reset();
public:
    void OnRead(void);
    int OnRecvData(void);
public:
    int Open(const int sfd,int cport, const char*cip,sharelib::CEpollServer *svr);
    void Close(int status, const char* reason);
    

    static int SendMessage(int iSocket, const char *msg, int size) ;
    int GetFd(){return sock_;} 
    struct event *event(){return &ev_;}
protected:
    struct event ev_;
   
    int sock_;
    int cport_;
    char cip_[DEF_IDX_IP_LEN];
 
    sharelib::CEpollServer* master_;

    char rbuf_[ADDS_IOBUF_LEN];   /** buffer to read commands into */
    int rcurr_;  /** but if we parsed some already, this is where we stopped */
    int rsize_;   /** total allocated size of rbuf */
    int rbytes_;  /** how much data, starting from rcur, do we have unparsed */
protected:
    CTcpContext(const CTcpContext&);
    const CTcpContext &operator=(const CTcpContext&);
};
SHARELIB_ES;

#endif
