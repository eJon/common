#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include "alert_socket.h"

#define MAXDATASIZE 256
#define BACKLOG 10
#define ALERT_SOCKET_CREATE_ERROR 4100
#define ALERT_SOCKET_SETOPT_ERROR 4101
#define ALERT_SOCKET_BIND_ERROR 4102
#define ALERT_SOCKET_LISTEN_ERROR 4103
#define ALERT_SOCKET_ACCEPT_ERROR 4104
#define ALERT_SOCKET_RECV_ERROR 4105


int CAlertSocket::init(int alertPort , int alertTimeout) {
  struct sockaddr_in my_addr;
  if ((m_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    return ALERT_SOCKET_CREATE_ERROR;
  }
  int on = 1;
  if ((setsockopt(m_sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {
    return ALERT_SOCKET_SETOPT_ERROR;
  }
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(alertPort);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  bzero(&(my_addr.sin_zero), 8);
  if (bind(m_sock_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
    return ALERT_SOCKET_BIND_ERROR;
  }
  if (listen(m_sock_fd, BACKLOG) == -1) {
    return ALERT_SOCKET_LISTEN_ERROR;
  }
  m_stat = true;

  m_timeout = alertTimeout;
  return 0;
}

void CAlertSocket::setTimeUse(int timeuse) {
  if (timeuse / 1000 >= m_timeout) {
    m_timeoutTimes++;
  }
  m_timeuse += timeuse;
  m_times++;
}

int CAlertSocket::sendTimeUse() {
  int client_fd;
  socklen_t sin_size = sizeof(struct sockaddr_in);
  if ((client_fd = accept(m_sock_fd, (struct sockaddr *)&m_remote_addr, &sin_size)) == -1) {
    return ALERT_SOCKET_ACCEPT_ERROR;
  }
  char buffer[MAXDATASIZE];
  bzero(buffer, MAXDATASIZE);
  int length = recv(client_fd, buffer, MAXDATASIZE, 0);
  if (length < 0) {
    return ALERT_SOCKET_RECV_ERROR;
  }

  bzero(buffer, MAXDATASIZE);
  sprintf(buffer, "{'totalTime':%d,'requestTimes':%d,'timeoutTimes':%d}", m_timeuse / 1000, m_times, m_timeoutTimes);

  if (send(client_fd, buffer, strlen(buffer), 0) == -1) {
  }

  close(client_fd);

  m_timeuse = 0;
  m_times = 0;
  m_timeoutTimes = 0;
  return 0;
}

bool CAlertSocket::getStat() {
  return m_stat;
}
