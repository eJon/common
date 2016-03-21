/**
 * @desc: alert
 * @auth: zibin
 * @mail: zibin@staff.sina.com.cn
 * @date: 2011-10-09
 */

#include <sys/socket.h>
#include <netinet/in.h>

#ifndef __ALERT_SOCKET_H__
#define __ALERT_SOCKET_H__

class CAlertSocket {
  public:
    CAlertSocket() {
      m_stat = false;
      m_timeuse = 0;
      m_times = 0;
      m_timeoutTimes = 0;
    };
    virtual ~CAlertSocket() {};

    int init(int alertPort, int alertTimeout);

    void setTimeUse(int timeuse);

    int sendTimeUse();

    bool getStat();
  private:
    int m_timeuse;

    int m_times;

    int m_timeout;
    int m_timeoutTimes;

    int m_sock_fd;

    bool m_stat;

    struct sockaddr_in m_remote_addr;

};
#endif
