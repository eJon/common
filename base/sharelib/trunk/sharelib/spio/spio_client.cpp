
#include "spio_client.h"


namespace sharelib {

SpioClient::SpioClient(): net_agent_ptr_(NULL),
    initialized_(NET_NOT_INIT) {
    // TODO Auto-generated constructor stub
    pthread_rwlock_init(&net_stat_locker_, NULL);
    pthread_rwlock_wrlock(&net_stat_locker_);
    net_failed_send_cnt_ = 0;
    net_failed_recv_cnt_ = 0;
    net_succ_cnt_ = 0;
    net_stat_time_begin_ = time(NULL);
    pthread_rwlock_unlock(&net_stat_locker_);
}

SpioClient::~SpioClient() {
    // TODO Auto-generated destructor stub
    if (NULL != net_agent_ptr_) {
        delete net_agent_ptr_;
        net_agent_ptr_ = NULL;
        initialized_ = NET_NOT_INIT;
    }

    pthread_rwlock_destroy(&net_stat_locker_);
}

int SpioClient::Initialize(const int timeout,
                           const string &net_conf_file,
                           const string &net_group_name) {
    if (NULL != net_agent_ptr_ ) {
        delete net_agent_ptr_ ;
        net_agent_ptr_ = NULL;
    }

    net_agent_ptr_  = new CSpioApi();

    if (net_agent_ptr_->init(net_conf_file)) {
        return SPIO_CLIENT_ERR_INIT_SPIO_CFG;
    }

    if (net_agent_ptr_->join_client(net_group_name)) {
        return SPIO_CLIENT_ERR_INIT_JOIN_CLIENT;
    }

    net_conf_file_ = net_conf_file;
    net_group_name_ = net_group_name;

    if (timeout < 0) {
        net_timeout_ = 0;
    } else {
        net_timeout_ = timeout;
    }

    initialized_ = NET_IS_INIT;
    return SPIO_CLIENT_OK;
}
int SpioClient::WaitResponse(std::string &ret_str){
        
    int ret = 0;
    if ((ret = net_agent_ptr_->recv(ret_str, net_timeout_)) != 0) {
        pthread_rwlock_wrlock(&net_stat_locker_);
        net_failed_recv_cnt_++;
        pthread_rwlock_unlock(&net_stat_locker_);
        return ret;
    }


    pthread_rwlock_wrlock(&net_stat_locker_);
    net_succ_cnt_++;
    pthread_rwlock_unlock(&net_stat_locker_);
    
    return ret;
}

int SpioClient::Request(const string &qr) {
    if (NET_NOT_INIT == initialized_) {
        return SPIO_CLIENT_ERR_REQ_NOT_INIT;
    }

    int ret = 0;

    if ((ret = net_agent_ptr_->send(qr)) != 0) {
        pthread_rwlock_wrlock(&net_stat_locker_);
        net_failed_send_cnt_++;
        pthread_rwlock_unlock(&net_stat_locker_);
        return SPIO_CLIENT_ERR_REQ_SEND;
    }


    return SPIO_CLIENT_OK;
}
int SpioClient::Statistics(std::string &dt, int64_t &succ, int64_t &err_send, int64_t &err_recv) {
    pthread_rwlock_rdlock(&net_stat_locker_);
    char buff[SPIO_CLIENT_TMP_BUFF_SIZE] = {0};
    tm *tmp = localtime(&net_stat_time_begin_);
    strftime(buff, sizeof(buff), "%Y-%m-%d %H-%M-%S", tmp);
    dt = buff;
    succ = net_succ_cnt_;
    err_send = net_failed_send_cnt_;
    err_recv = net_failed_recv_cnt_;
    //clear statistics information
    net_succ_cnt_ = 0;
    net_failed_send_cnt_ = 0;
    net_failed_recv_cnt_ = 0;
    net_stat_time_begin_ = time(NULL);
    pthread_rwlock_unlock(&net_stat_locker_);
    return SPIO_CLIENT_OK;
}

}
