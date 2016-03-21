/***************************************************************************
 *
 * Copyright (c) 2013 staff.sina.com.cn, Inc. All Rights Reserved
 * $007$
 *
**************************************************************************/


/**
  * @file common/net_agent.h
  * @author Andrew(pengdong@staff.sina.com.cn)
  * @date 2013/03/20 15:53:29
  * @version 1.0
  * @brief to handle io request
  *
**/

#include <mscom/base/log.h>
#include <mscom/rpc/nspio_net_agent.h>

NspioNetAgent::NspioNetAgent():
    initialized_(NET_NOT_INIT) {
    // TODO Auto-generated constructor stub
    pthread_rwlock_init(&net_stat_locker_, NULL);

}

NspioNetAgent::~NspioNetAgent() {
    // TODO Auto-generated destructor stub
    pthread_rwlock_destroy(&net_stat_locker_);

}

int NspioNetAgent::Initialize(const int timeout,
                           const string &net_conf_file,
                           const string &net_group_name) {

    DEBUG(LOGROOT, "begin initialize... host: %s group: %s", net_conf_file.c_str(), net_group_name.c_str());

    if(client.init(net_conf_file)) {
        ERROR(LOGROOT, "spio initialize failed");
        return NET_AGENT_ERR;
    }

    if(client.join_client(net_group_name)) {
        ERROR(LOGROOT, "spio join client failed");
        return NET_AGENT_ERR;
    }

    net_conf_file_ = net_conf_file;
    net_group_name_ = net_group_name;

    if(timeout < 0) {
        net_timeout_ = 0;
    } else {
        net_timeout_ = timeout;
    }

    initialized_ = NET_IS_INIT;
    DEBUG(LOGROOT, "initialize ok");
    return NET_AGENT_OK;
}

int NspioNetAgent::Request(const string &qr) {

    DEBUG(LOGROOT, "begin request");

    if(NET_NOT_INIT == initialized_) {
        WARN(LOGROOT, "net agent is not intialized");
        return NET_AGENT_ERR;
    }

    TryResetStat();
#ifdef NET_CORE_STAT_PRINT
    Statistics();
#endif
    int ret = 0;

    if((ret = client.send(qr)) != 0) {
        pthread_rwlock_wrlock(&net_stat_locker_);
        net_statistics_.err_send++;
        pthread_rwlock_unlock(&net_stat_locker_);
        return NET_AGENT_ERR;
    }
    pthread_rwlock_wrlock(&net_stat_locker_);
    net_statistics_.succ++;
    pthread_rwlock_unlock(&net_stat_locker_);
    DEBUG(LOGROOT, "request return ok");
    return NET_AGENT_OK;
}

int NspioNetAgent::Request(const string &qr, string &ret_str) {

    DEBUG(LOGROOT, "begin request");

    if(NET_NOT_INIT == initialized_) {
        WARN(LOGROOT, "net agent is not intialized");
        return NET_AGENT_ERR;
    }

    TryResetStat();
#ifdef NET_CORE_STAT_PRINT
    Statistics();
#endif
    int ret = 0;

    if((ret = client.send(qr)) != 0) {
        pthread_rwlock_wrlock(&net_stat_locker_);
        net_statistics_.err_send++;
        pthread_rwlock_unlock(&net_stat_locker_);
        return NET_AGENT_ERR;
    }

    if((ret = client.recv(ret_str, net_timeout_)) != 0) {
        pthread_rwlock_wrlock(&net_stat_locker_);
        net_statistics_.err_recv++;
        pthread_rwlock_unlock(&net_stat_locker_);
        return NET_AGENT_ERR;
    }

    pthread_rwlock_wrlock(&net_stat_locker_);
    net_statistics_.succ++;
    pthread_rwlock_unlock(&net_stat_locker_);
    DEBUG(LOGROOT, "request return ok");
    return NET_AGENT_OK;
}


/********************* inline functions **************************/

void NspioNetAgent::Statistics(int trace /*==0*/) {
    pthread_rwlock_rdlock(&net_stat_locker_);
    DEBUG(LOGROOT, "statistics info {time:%s (succ=%ld,err_send=%ld,err_recv=%ld)}",
          net_statistics_.dt_str.c_str(), net_statistics_.succ,
          net_statistics_.err_send, net_statistics_.err_recv);
    pthread_rwlock_unlock(&net_stat_locker_);
}
void NspioNetAgent::TryResetStat() {

    pthread_rwlock_wrlock(&net_stat_locker_);

    if((time(NULL) - net_statistics_.stat_begin_time) % HOURE == 0) {
        DEBUG(LOGROOT, "reset statistics info,%d / %d",
              (int)(time(NULL) - net_statistics_.stat_begin_time) , HOURE);
        net_statistics_.succ = 0;
        net_statistics_.err_send = 0;
        net_statistics_.err_recv = 0;
        net_statistics_.stat_begin_time = time(NULL);
        char buff[NET_AGENT_TMP_BUFF_SIZE] = {0};
        tm *tmp = localtime(&net_statistics_.stat_begin_time);
        strftime(buff, sizeof(buff), "%Y-%m-%d %H-%M-%S", tmp);
        net_statistics_.dt_str = buff;
    }

    pthread_rwlock_unlock(&net_stat_locker_);

}
