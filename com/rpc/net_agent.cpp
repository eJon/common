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
#include <mscom/rpc/net_agent.h>

MsNetAgent::MsNetAgent(): net_agent_ptr_(NULL),
    initialized_(NET_NOT_INIT) {
    // TODO Auto-generated constructor stub
    pthread_rwlock_init(&net_stat_locker_, NULL);

}

MsNetAgent::~MsNetAgent() {
    // TODO Auto-generated destructor stub
    if(NULL != net_agent_ptr_) {
        delete net_agent_ptr_;
        net_agent_ptr_ = NULL;
        initialized_ = NET_NOT_INIT;
    }

    pthread_rwlock_destroy(&net_stat_locker_);

}

int MsNetAgent::Initialize(const int timeout,
                           const string &net_conf_file,
                           const string &net_group_name) {

    DEBUG(LOGROOT, "begin initialize...");

    if(NULL != net_agent_ptr_) {
        delete net_agent_ptr_ ;
        net_agent_ptr_ = NULL;
    }

    net_agent_ptr_  = new CSpioApi();

    if(net_agent_ptr_->init(net_conf_file)) {
        ERROR(LOGROOT, "spio initialize failed");
        return NET_AGENT_ERR;
    }

    if(net_agent_ptr_->join_client(net_group_name)) {
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

int MsNetAgent::Request(const string &qr) {
    DEBUG(LOGROOT, "begin request");
    if(NET_NOT_INIT == initialized_) {
        WARN(LOGROOT, "net agent is not intialized");
        return 1;
    }
    int ret = 0;
    if((ret = net_agent_ptr_->send(qr)) != 0) {
	    return 2;
    }
    DEBUG(LOGROOT, "request return ok");
    return NET_AGENT_OK;
}

int MsNetAgent::Request(const string &qr, string &ret_str) {
    DEBUG(LOGROOT, "begin request");
    if(NET_NOT_INIT == initialized_) {
        WARN(LOGROOT, "net agent is not intialized");
        return 1;
    }
    int ret = 0;
    if((ret = net_agent_ptr_->send(qr)) != 0) {
	    return 2;
    }
    if((ret = net_agent_ptr_->recv(ret_str, net_timeout_)) != 0) {
	    return 3;
    }
    DEBUG(LOGROOT, "request return ok");
    return NET_AGENT_OK;
}
/********************* inline functions **************************/

void MsNetAgent::Statistics(int trace /*==0*/) {
    pthread_rwlock_rdlock(&net_stat_locker_);
    DEBUG(LOGROOT, "statistics info {time:%s (succ=%ld,err_send=%ld,err_recv=%ld)}",
          net_statistics_.dt_str.c_str(), net_statistics_.succ,
          net_statistics_.err_send, net_statistics_.err_recv);
    pthread_rwlock_unlock(&net_stat_locker_);
}
void MsNetAgent::TryResetStat() {

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
