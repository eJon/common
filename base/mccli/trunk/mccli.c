
#include "mccli.h"
void __mcInitReply(mcContext *c) {
  if (c == NULL) {
    return;
  }
  memset(c->m_recvbuf, '\0', sizeof(c->m_recvbuf)); //Recv Buffer
  c->m_currpos = 0;      //current recv pos
  c->m_parseflag = 0;    //0:parse start
  c->m_datapos = 0;      //datablock start pos
  c->m_size = 0;         //reply msg size
  c->m_flag = 0;         //reply msg flag
}
void __mcSetError(mcContext *c, const char *fmt, ...) {
  c->err = 1;
  char buf[768] = {'\0'};
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  strcpy(c->errstr, buf);
}

void __mcSetlog(mcContext *c, const char *fmt, ...) {
  c->err = -1;
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(c->errstr, sizeof(c->errstr), fmt, ap);
  va_end(ap);
  c->errstr[767] = '\0';
}

static void __mcErrorErrorno(mcContext *c, const char *prefix) {
  char buf[128];
  size_t len = 0;
  if (prefix != NULL) {
    len = snprintf(buf, sizeof(buf), "%s: ", prefix);  // len not include null
  }
  char *p = strerror_r(errno, buf + len, sizeof(buf) - len);
  strncpy(buf + len, p, sizeof(buf) - len);
  buf[127] = '\0';
  if (strlen(c->errstr)) {
    __mcSetError(c, "%s %s [%d]", c->errstr, buf, errno);
  } else {
    __mcSetError(c, "%s [%d]", buf, errno);
  }
}

void freeItem(item_data *item) {
  free(item->_buffer);
  free(item);
}

static char *seekNewline(const char *s, size_t len) {
  int pos = 0;
  int _len = len - 1;

  while (pos < _len) {
    while (pos < _len && s[pos] != '\r') {
      pos++;
    }
    if (s[pos] != '\r') {
      return NULL;
    } else {
      if (s[pos + 1] == '\n') {
        return (char *)(s + pos);
      } else {
        pos++;
      }
    }
  }
  return NULL;
}

int changetoslv(mcContext *c) {
  if (c->fd) {
    close(c->fd);
    c->fd = 0;
  }
  if (c->masternow == 0) {
    if (mcContextConnectTcp(c, c->slvip, c->slvport, &(c->conntimeout)) == MCCLI_OK) {
      c->masternow = 1;
      __mcSetlog(c, "change to %s:%d,rc:%d,sr:%d,m:%d,fd:%d", c->slvip, c->slvport, c->rctimes, c->srtimes, c->masternow, c->fd);
      c->rctimes = 0;
      c->srtimes = 0;
      mcContextSetTimeout(c, c->socktimeout);
      return MCCLI_OK;
    } else {
      c->masternow = 0;
      c->rctimes = 1;
      c->srtimes = c->maxsrtimes;
      __mcSetError(c, "change to %s:%d failed:%s", c->slvip, c->slvport, c->errstr);
      return MCCLI_ERR;
    }
  } else {
    if (mcContextConnectTcp(c, c->mip, c->mport, &(c->conntimeout)) == MCCLI_OK) {
      c->masternow = 0;
      __mcSetlog(c, "change to %s:%d,rc:%d,sr:%d,m:%d,fd:%d", c->mip, c->mport, c->rctimes, c->srtimes, c->masternow, c->fd);
      c->rctimes = 0;
      c->srtimes = 0;
      mcContextSetTimeout(c, c->socktimeout);
      return MCCLI_OK;
    } else {
      c->masternow = 1;
      c->rctimes = 1;
      c->srtimes = c->maxsrtimes;
      __mcSetError(c, "change to %s:%d failed:%s", c->mip, c->mport, c->errstr);
      return MCCLI_ERR;
    }
  }
}

static mcContext *mcContextInit(const char *mip, int mport, const char *slvip, int slvport) {
  mcContext *c;

  c = calloc(1, sizeof(mcContext));
  if (c == NULL) {
    return NULL;
  }
  c->err = 0;
  c->errstr[0] = '\0';
  if ((mip) && (30 > strlen(mip))) {
    strcpy(c->mip, mip);
  }
  c->mport = mport;
  c->masternow = 0;
  if ((slvip) && (30 > strlen(slvip))) {
    strcpy(c->slvip, slvip);
  }
  c->slvport = slvport;
  return c;
}

static int getValue(const char *key, mcContext *c, void **reply) {
  char *start = NULL;
  switch (c->m_parseflag) {
    case 0: //prase head tag
      //seek \r\n
      start = seekNewline(c->m_recvbuf, (size_t)c->m_currpos);
      if (start) {
        /*check key*/
        char *p = strstr(c->m_recvbuf, " ");
        if (p == NULL) {
          return MCCLI_ERR;
        }
        if (strncmp(p + 1, key, strlen(key)) != 0) {
          return MCCLI_ERR;
        }
        /*get flag*/
        p = strstr(p + 1, " ");
        if (p == NULL) {
          return MCCLI_ERR;
        }
        c->m_flag = atoi(p);
        /*get length*/
        p = strstr(p + 1, " ");
        if (p == NULL) {
          return MCCLI_ERR;
        }
        c->m_size = atoi(p);

        //set head parse ok
        c->m_parseflag = 1;
        //set datablock pos
        c->m_datapos = start - c->m_recvbuf + 2;

        //continue to prase datablock
      } else {
        //wait recv buffer to prase head
        return MCCLI_OK;
      }
    case 1:  //prase datablock
      if (((c->m_currpos - c->m_datapos) >= c->m_size)
          && (seekNewline(c->m_recvbuf + c->m_datapos + c->m_size, (size_t)c->m_currpos))) {
        item_data *data = malloc(sizeof(item_data));
        if (data == NULL) {
          return MCCLI_ERR;
        }
        strncpy(data->_key, key, sizeof(data->_key)); //assert key <256
        data->_buffer = malloc(c->m_size);
        if (data->_buffer == NULL) {
          free(data);
          data = NULL;
          return MCCLI_ERR;
        }
        memcpy(data->_buffer, c->m_recvbuf + c->m_datapos, c->m_size);
        data->_flag = c->m_flag;
        data->_size = c->m_size;
        data->_buffer[data->_size] = '\0';
        *reply = data;
        c->m_parseflag = 2;

        //continue to prase end tag
      } else {
        return MCCLI_OK;
      }
    case 2:  //prase end tag
      //get the last line[="END \r\n"] of reply
      start = seekNewline(c->m_recvbuf + c->m_datapos + c->m_size + 2, (size_t)c->m_currpos);
      if (start) {
        c->m_parseflag = 3;
      }
      return MCCLI_OK;
    case 3:
    default:
      return MCCLI_ERR;
      break;
  }
}

void mcFree(mcContext *c) {
  if (c->fd > 0) {
    close(c->fd);
  }
  free(c);
}

mcContext *mcConnectWithTimeout(const char *mip, int mport, const char *slvip, int slvport, struct timeval conntimeout, struct timeval srtimeout, int maxsrtimes, int maxrctimes) {
  mcContext *c = mcContextInit(mip, mport, slvip, slvport);
  c->flags |= MCCLI_BLOCK;
  if ((mcContextConnectTcp(c, mip, mport, &conntimeout) == MCCLI_ERR) && (c->slvip)) {
    if (mcContextConnectTcp(c, slvip, slvport, &conntimeout) == MCCLI_ERR) {
      mcFree(c);
      c = NULL;
    } else {
      c->masternow = 1;
    }
  } else {
    c->masternow = 0;
  }
  if (c) {
    c->socktimeout = srtimeout;
    c->conntimeout = conntimeout;
    c->maxsrtimes = maxsrtimes;
    c->maxrctimes = maxrctimes;
    c->rctimes = 0;
    c->srtimes = 0;
    mcContextSetTimeout(c, srtimeout);

    __mcInitReply(c);
  }
  return c;
}

mcContext *mcConnectNonBlock(const char *mip, int mport, const char *slvip, int slvport) {
  mcContext *c = mcContextInit(mip, mport, slvip, slvport);
  c->flags &= ~MCCLI_BLOCK;
  if ((mcContextConnectTcp(c, mip, mport, NULL) == MCCLI_ERR) && (c->slvip)) {
    mcContextConnectTcp(c, slvip, slvport, NULL);
  }
  return c;
}

static int get_reply(mcContext *c, unsigned int *value ) {
  char msg[256] = {'\0'};
  int buflen = 0;
  int ret = 0;
  do {
    ret = recv(c->fd, msg + buflen, sizeof( msg ) - 1 - buflen, 0);
    if (0 < ret) { // recv data
      buflen += ret;
      c->srtimes = 0;
      c->err = 0;

      if (seekNewline(msg, buflen)) {
        break;
      }
    } else if (0 == ret) {
      c->srtimes = c->maxsrtimes;
      return MEM_SYSTEM_ERROR;
    } else {
      __mcErrorErrorno(c, "read");
      if ( errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
        c->srtimes += 1;
      } else { // connect abnormal, please reconnect
        c->srtimes = c->maxsrtimes;
      }
      return MEM_SYSTEM_ERROR;
    }
  } while (1);

  msg[buflen] = '\0';
  CHECK_REPLY(msg, "ERROR\r\n", MEM_ERROR);
  CHECK_REPLY(msg, "CLIENT_ERROR\r\n", MEM_CLIENT_ERROR);
  CHECK_REPLY(msg, "SERVER_ERROR\r\n", MEM_SERVER_ERROR);
  CHECK_REPLY(msg, "STORED\r\n", MEM_STORED);
  CHECK_REPLY(msg, "NOT_STORED\r\n", MEM_NOT_STORED);
  CHECK_REPLY(msg, "EXISTS\r\n", MEM_EXISTS);
  CHECK_REPLY(msg, "NOT_FOUND\r\n", MEM_NOT_FOUND);
  CHECK_REPLY(msg, "DELETED\r\n", MEM_DELETED);
  CHECK_REPLY(msg, "OK\r\n", MEM_OK);

  if (value != 0) {
    *value = atoi(msg);
  }
  return MEM_OK;
}

int mc_reconnect(mcContext *c) {
  if (c->fd) {
    close(c->fd);
    c->fd = 0;
  }
  int connresult = -1;
  if (1 == c->masternow) {
    connresult = mcContextConnectTcp(c, c->slvip, c->slvport, &(c->conntimeout));
  } else if (0 == c->masternow) {
    connresult = mcContextConnectTcp(c, c->mip, c->mport, &(c->conntimeout));
  }
  if (MCCLI_OK != connresult) {
    c->rctimes++;
    if (1 == c->masternow) {
      __mcSetError(c, "connect to %s:%d failed:%s", c->slvip, c->slvport, c->errstr);
    } else if (0 == c->masternow) {
      __mcSetError(c, "connect to %s:%d failed:%s", c->mip, c->mport, c->errstr);
    }
    c->fd = 0;
    return MCCLI_ERR;
  } else {
    __mcSetError(c, "reconnect %d times to %d", c->rctimes, c->masternow);
    mcContextSetTimeout(c, c->socktimeout);
    c->rctimes = 0;
    c->srtimes = 0;
    return MCCLI_OK;
  }
}

int readforget(mcContext *c) {
  int ret = 0;
  int buflen = 0;
  ret = recv(c->fd, c->m_recvbuf + c->m_currpos, MCCLI_MAX_PACKET - c->m_currpos, 0);
  if (0 < ret) { // recv data
    buflen += ret;
    c->srtimes = 0;
    c->err = 0;

    //appent the recv buf to context
    if ( (c->m_currpos + ret) > MCCLI_MAX_PACKET ) {
      __mcErrorErrorno(c, "buffer overflowing!");
      return MCCLI_ERR;
    }
    c->m_currpos += ret;

    return buflen;
  } else if (0 == ret) {
    c->srtimes = c->maxsrtimes;
    return MCCLI_ERR;
  } else {
    __mcErrorErrorno(c, "read");
    if ( errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
      c->srtimes += 1;
    } else { // connect abnormal, please reconnect
      c->srtimes = c->maxsrtimes;
    }
    return MCCLI_ERR;
  }
}

int sendforget(mcContext *c, const char *cmd ) {
  int ret = 0;
  int cmdlen = strlen(cmd);
  int writenlen = 0;
  while (1) {
    ret = send(c->fd, cmd + writenlen, cmdlen - writenlen, 0);
    if (ret <= 0) {
      __mcErrorErrorno(c, "send");
      if ( errno != EINTR  && errno != EAGAIN && errno != EWOULDBLOCK) {
        c->srtimes = c->maxsrtimes;
      } else {
        c->srtimes += 1;
      }
      return MCCLI_ERR;
    } else {
      writenlen += ret;
      if (writenlen == cmdlen) {
        c->srtimes = 0;
        break;
      } else {
        continue;
      }
    }
  }
  return MCCLI_OK;
}


int senddata(mcContext *c, const char *data, int len ) {
  int ret = 0;
  int writenlen = 0;
  while (1) {
    ret = send(c->fd, data + writenlen, len - writenlen, 0);
    if (ret <= 0) {
      __mcErrorErrorno(c, "send");
      if ( errno != EINTR  && errno != EAGAIN && errno != EWOULDBLOCK) {
        c->srtimes = c->maxsrtimes;
      } else {
        c->srtimes += 1;
      }
      return MCCLI_ERR;
    } else {
      writenlen += ret;
      if (writenlen == len) {
        c->srtimes = 0;
        break;
      } else {
        continue;
      }
    }
  }
  return MCCLI_OK;
}


void *mc_get(mcContext *c, const char *key) {
  char cmd[1024];
  strcpy(cmd, "get ");
  strcat(cmd, key);
  strcat(cmd, "\r\n");
  void *result = NULL;
  c->errstr[0] = '\0';

  //socket send/recv failed count over, reconnect
  if (c->maxsrtimes <= c->srtimes) {
    if (MCCLI_OK != mc_reconnect(c)) {
      //socket reconnect failed count over
      if (c->maxrctimes <= c->rctimes) {
        if (MCCLI_OK != changetoslv(c)) {
          //change mc-server failed, continue connect to old server
          return result;
        }
      }
    }
  }

  if (!(c->fd)) {
    return result;
  }

  if (MCCLI_OK == sendforget(c, cmd)) {
    __mcInitReply(c);
    //read from socket until there is a reply
    do {
      if (readforget(c) == MCCLI_ERR) {
        return NULL;
      }
      if (getValue(key, c, &result) == MCCLI_ERR) {
        return NULL;
      }
    } while ((result == NULL) || (c->m_parseflag != 3));
  }
  return result;
}

int mc_store(mcContext *c, const item_data *item, int need_response, const char *store_type) {
  int ret;
  char cmd[512] = {'\0'};
  u_char *data = item->_buffer ;
  size_t data_len = item->_size;

  if ((data_len > MCCLI_MAX_VALUESIZE) || (data_len <= 0)) {
    //value size overflowing
    return MEM_SYSTEM_ERROR;
  }
  //socket send/recv failed count over, reconnect
  if (c->maxsrtimes <= c->srtimes) {
    if (MCCLI_OK != mc_reconnect(c)) {
      //socket reconnect failed count over
      if (c->maxrctimes <= c->rctimes) {
        if (MCCLI_OK != changetoslv(c)) {
          //change mc-server failed, continue connect to old server
          return MEM_SYSTEM_ERROR;
        }
      }
    }
  }
  if (!(c->fd)) {
    return MEM_SYSTEM_ERROR;
  }

  if (need_response == 0) {
    sprintf(cmd, "%s %s %d %lu %lu noreply\r\n", store_type, item->_key, item->_flag, item->_expire, data_len);
  } else {
    sprintf(cmd, "%s %s %d %lu %lu\r\n", store_type, item->_key, item->_flag, item->_expire, data_len);
  }

  if (MCCLI_OK != senddata(c, cmd, (int)strlen(cmd))) {
    __mcErrorErrorno(c, "send cmd");
    return MEM_SYSTEM_ERROR;
  }

  if (MCCLI_OK != senddata(c, (char *)data, data_len)) {
    __mcErrorErrorno(c, "send datalen");
    return MEM_SYSTEM_ERROR;
  }

  if (MCCLI_OK != senddata(c, "\r\n", 2)) {
    __mcErrorErrorno(c, "send \\r\\n");
    return MEM_SYSTEM_ERROR;
  }

  if (need_response != 0) {
    ret = get_reply(c, 0);
  } else {
    ret = MEM_NO_RESPONSE;
  }

  return ret;
}

int mc_delete(mcContext *c, const char key[256], int need_response) {
  char cmd[512] = {'\0'};
  int ret;

  //socket send/recv failed count over, reconnect
  if (c->maxsrtimes <= c->srtimes) {
    if (MCCLI_OK != mc_reconnect(c)) {
      //socket reconnect failed count over
      if (c->maxrctimes <= c->rctimes) {
        if (MCCLI_OK != changetoslv(c)) {
          //change mc-server failed, continue connect to old server
          return MEM_SYSTEM_ERROR;
        }
      }
    }
  }
  if (!(c->fd)) {
    return MEM_SYSTEM_ERROR;
  }

  if (need_response == 0) {
    sprintf(cmd, "delete %s noreply\r\n", key);
  } else {
    sprintf(cmd, "delete %s\r\n", key);
  }

  if (MCCLI_OK != senddata(c, cmd, strlen(cmd))) {
    __mcErrorErrorno(c, "send cmd");
    return MEM_SYSTEM_ERROR;
  }

  if (need_response == 0) {
    ret = MEM_NO_RESPONSE;
  } else {
    ret = get_reply(c, 0);
  }

  return ret;
}

mcContext *mcConnect(const char *mip, int mport, const char *slvip, int slvport) {
  mcContext *c = mcContextInit(mip, mport, slvip, slvport);
  c->flags |= MCCLI_BLOCK;
  if ((mcContextConnectTcp(c, mip, mport, NULL) == MCCLI_ERR) && (c->slvip)) {
    mcContextConnectTcp(c, slvip, slvport, NULL);
  }
  return c;
}

int mc_inc_dec(mcContext *c, const char key[256], unsigned int value, unsigned int *new_value, const char *cmd_type) {
  char cmd[512] = {'\0'};
  int ret;

  //socket send/recv failed count over, reconnect
  if (c->maxsrtimes <= c->srtimes) {
    if (MCCLI_OK != mc_reconnect(c)) {
      //socket reconnect failed count over
      if (c->maxrctimes <= c->rctimes) {
        if (MCCLI_OK != changetoslv(c)) {
          //change mc-server failed, continue connect to old server
          return MEM_SYSTEM_ERROR;
        }
      }
    }
  }
  if (!(c->fd)) {
    return MEM_SYSTEM_ERROR;
  }

  if (new_value != 0) {
    sprintf(cmd, "%s %s %u\r\n", cmd_type, key, value);
  } else {
    sprintf(cmd, "%s %s %u noreply\r\n", cmd_type, key, value);
  }

  if (MCCLI_OK != senddata(c, cmd, strlen(cmd))) {
    __mcErrorErrorno(c, "send cmd");
    return MEM_SYSTEM_ERROR;
  }

  /* get reply - new value */
  if (new_value != 0) {
    ret = get_reply(c, new_value);
  } else {
    ret = MEM_NO_RESPONSE;
  }
  return ret;
}
