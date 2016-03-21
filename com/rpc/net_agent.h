/***************************************************************************
 *
 * Copyright (c) 2013 staff.sina.com.cn, Inc. All Rights Reserved
 * $007$
 *
**************************************************************************/


/**
  * @file common/net_agent.h
  * @author Andrew(pengdong@staff.sina.com.cn)
  * @date 2013/04/19 10:53:29
  * @version 1.0
  * @brief to handle io request
  *
**/

#ifndef MS_NET_AGENT_H_
#define MS_NET_AGENT_H_

#ifndef NET_CORE_STAT_PRINT
#define NET_CORE_STAT_PRINT
#endif

#include <pthread.h>
#include <string>
#include <sys/timeb.h>
#include <stdint.h>
#include <spio/SpioApi.h>
#include <mscom/base/define_const.h>

struct net_core_statistics {
    time_t stat_begin_time;
    std::string dt_str;
    int64_t succ;
    int64_t err_send;
    int64_t err_recv;
    net_core_statistics() {
        stat_begin_time = time(NULL);
        dt_str = "";
        succ = 0;
        err_send = 0;
        err_recv = 0;
    }
};
class MsNetAgent {
  public:
    MsNetAgent();
    virtual ~MsNetAgent();
  public:
    /*************  all public function for interface  **************/
    /*
    **@bref initialize the parameters and initialize net core obj
    **@param[in]timeout,spio net communication timeout
    **@param[in]net_conf_file,absolute net configure file name
    **@param[in]net_group_name,group name to join in
    **@return,0-ok
    */
    virtual int Initialize(const int timeout,
                           const std::string &net_conf_file,
                           const std::string &net_group_name);
    /*
    **@bref,handle net request
    **@param[in]req,request string to server
    **@param[out]resp,response string from server
    **@return ,0-OK
    */
    virtual int Request(const std::string &req, std::string &resp);
    virtual int Request(const std::string &req);
    
    void Statistics(int trace = 0);
    int Stat(std::string &dt, int64_t &succ, int64_t &err_send, int64_t &err_recv) {
        pthread_rwlock_rdlock(&net_stat_locker_);
        char buff[NET_AGENT_TMP_BUFF_SIZE] = {0};
        tm *tmp = localtime(&net_statistics_.stat_begin_time);
        strftime(buff, sizeof(buff), "%Y-%m-%d %H-%M-%S", tmp);
        dt = buff;
        succ = net_statistics_.succ;
        err_send = net_statistics_.err_send;
        err_recv = net_statistics_.err_recv;
        pthread_rwlock_unlock(&net_stat_locker_);
        return NET_AGENT_OK;
    }

#ifdef UTEST_NET_AGENT
  public:
#else
  private:
#endif

    void TryResetStat();

  private:
    CSpioApi *net_agent_ptr_;
    bool initialized_;
    int net_timeout_;
    std::string  net_conf_file_;
    std::string  net_group_name_;
    net_core_statistics net_statistics_;
    pthread_rwlock_t net_stat_locker_;

};


#endif /* MS_NET_AGENT_H_ */
