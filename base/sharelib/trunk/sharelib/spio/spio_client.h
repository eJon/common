#ifndef SHARELIB_SPIO_CLIENT_H_
#define SHARELIB_SPIO_CLIENT_H_

#include <pthread.h>
#include <string>
#include <sys/timeb.h>
#include <stdint.h>
#include <spio/SpioApi.h>

namespace sharelib {
#define NET_NOT_INIT 0
#define NET_IS_INIT 1
#define SPIO_CLIENT_TMP_BUFF_SIZE 1024
enum SPIO_CLIENT_RET_CODE {
    SPIO_CLIENT_OK,
    SPIO_CLIENT_NOT_INIT,
    SPIO_CLIENT_ERR_INIT_SPIO_CFG,
    SPIO_CLIENT_ERR_INIT_JOIN_CLIENT,
    SPIO_CLIENT_ERR_REQ_NOT_INIT,
    SPIO_CLIENT_ERR_REQ_SEND,
    SPIO_CLIENT_ERR_REQ_RECV
};
class SpioClient {
    public:
        SpioClient();
        virtual ~SpioClient();
    public:
        /*all public function for interface*/
        /*
         **desc:initialize spio with key parameters
         **param[in]timeout,for recv timeout of spio
         **param[in]net_conf_file,spio configure file full path(absolute)
         **param[in]net_group_name,spio group name in configure file,keep the same
         **return,0-OK
         */
        virtual int Initialize(const int timeout,
                               const std::string &net_conf_file,
                               const std::string &net_group_name);
        /*
         **desc:handle spio comunication,send message 
         **param[in]qr,request string to spio server
         
         **return,0-OK
         */
        virtual int Request(const std::string &qr);


         /*
         **desc:handle spio comunication,recieve message
         **param[out]ret,response string from spio server
         **return,0-OK
         */
        virtual int WaitResponse(std::string &ret);
    
        /*
         **desc:statistic information of spio client ,when you invoke this function,it will clear
                and reset all statistic information
         **param[out]dt,request statistic time string
         **param[out]succ,record successful spio request from last invoke this function
         **param[out]err_send,record failed send spio request from last invoke this function
         **param[out]err_recv,record failed recv  spio request from last invoke this function
         **return,0-OK
         */
        virtual int Statistics(std::string &dt, int64_t &succ, int64_t &err_send, int64_t &err_recv) ;

#ifdef UTEST_SPIO_CLIENT
    public:
#else
    private:
#endif
        CSpioApi *net_agent_ptr_;
        bool initialized_;
        int net_timeout_;
        std::string  net_conf_file_;
        std::string  net_group_name_;
        int64_t net_succ_cnt_;
        int64_t net_failed_send_cnt_;
        int64_t net_failed_recv_cnt_;
        time_t net_stat_time_begin_;

        pthread_rwlock_t net_stat_locker_;

};

} /* namespace matchserver */
#endif /* MS_SPIO_CLIENT_H_ */
