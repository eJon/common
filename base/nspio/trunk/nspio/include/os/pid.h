// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/os/pid.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _PIDCTRL_H_
#define _PIDCTRL_H_

#include <string>
#include <unistd.h>
#include "decr.h"


NSPIO_DECLARATION_START

class pid_ctrl_t
{
public:
    // checkInstance返回true的原因
    enum EPidCtrlResult {
        PCR_PERMISSION_DENIED, // 检查进程的权限不够
        PCR_RUNNING_ALREADY,   // 进程已经存在
        TOTAL_PCR
    };

    /**
    * init
    * @param exe   execute name argv[0]
    * @param name  一个用于区分pid文件的名称。
    *              如果有，则pid文件名为"<exe>_<name>.pid";
    *              如果为NULL，则文件名为"<exe>.pid"
    */
    pid_ctrl_t(const char* exe, const char* name = NULL, const char* rootdir = NULL);
    virtual ~pid_ctrl_t();

    /**
    * 检查是否有相同的实例正在运行
    * @param reason  (out)当返回值为true时，保存原因; 返回值为false时无效
    * @return 如果存在，返回true; 否则false
    */
    bool exists(EPidCtrlResult& reason);

    /**
    * 用某个信号量杀掉已运行的进程实例
    * @param sig   需要传递给已运行的进程实例的信号量
    * @return true:成功; false:没有权限传递信号量sig
    */
    bool kill(int sig);
    void savePid();
    void delPidFile();

private:
    std::string getPidFilename();
    bool readPid(pid_t& pid);

    std::string m_pidfilename;
    bool m_inited;
};

}

#endif
