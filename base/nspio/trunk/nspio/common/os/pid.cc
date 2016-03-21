// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/os/pid.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


/**
 * File: pid_ctrl_t.cpp
 * User:
 * 通过PID文件控制进程的启动、退出
 */
#include "os/pid.h"
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
NSPIO_DECLARATION_START


/**
 * 初始化
 * @param exe   程序名，可以传argv[0]
 * @param name  一个用于区分pid文件的名称。
 *              如果有，则pid文件名为"<exe>_<name>.pid";
 *              如果为NULL，则文件名为"<exe>.pid"
 */
pid_ctrl_t::pid_ctrl_t(const char* exe, const char* name, const char* basedir) : m_inited(false)
{
    if ( basedir != NULL) {
        m_pidfilename = basedir;
    } else {
        m_pidfilename = "";
    }
    if (exe == NULL)
        return;
    else
        m_pidfilename += exe;

    if (name != NULL)
        m_pidfilename += string("_") + name + ".pid";
    m_inited = true;
}

pid_ctrl_t::~pid_ctrl_t()
{
}

// 合成pid文件名
string
pid_ctrl_t::getPidFilename()
{
    if (!m_inited) return "";

    return m_pidfilename;
}

/**
 * 读取pid的值
 */
bool
pid_ctrl_t::readPid(pid_t& pid)
{
    string fpid = getPidFilename();
    if (fpid == "") return false;
    FILE *fp = fopen(fpid.c_str(), "r");
    if (fp == NULL)
        return false;

    char pbuf[100];
    if(fgets(pbuf,100,fp) == NULL) {
        fclose(fp);
        return false;
    }

    fclose(fp);
    pid = atoi(pbuf);

    return true;
}

/**
 * 检查是否有相同的实例正在运行
 * @param reason  (out)当返回值为true时，保存原因; 返回值为false时无效
 * @return 如果存在，返回true; 否则false
 */
bool
pid_ctrl_t::exists(EPidCtrlResult& reason)
{
    if (!m_inited) return false;

    // 读pid
    pid_t pid;
    if (!readPid(pid)) return false;

    // 检查pid对应的进程是否存在
    if (::kill(pid, 0) == -1) {
        switch(errno) {
        case EINVAL:
        case ESRCH:
            // no such process, that's ok
            break;
        case EPERM:
            reason = PCR_PERMISSION_DENIED;
            return true;
        }
    } else {
        reason = PCR_RUNNING_ALREADY;
        return true;
    }

    return false;
}

/**
 * 保存pid文件
 */
void
pid_ctrl_t::savePid()
{
    if (!m_inited) return;

    string fpid = this->getPidFilename();
    if (fpid == "") return;
    FILE *fp = fopen(fpid.c_str(), "w");
    if (fp == NULL)
        return;

    pid_t pid = getpid();
    fprintf(fp,"%d",pid);
    fclose(fp);
}

void
pid_ctrl_t::delPidFile()
{
    if (!m_inited) return;

    string fpid = getPidFilename();
    if (fpid == "") return;

    remove(fpid.c_str());
}

/**
 * 用某个信号量杀掉已运行的进程实例
 * @param sig   需要传递给已运行的进程实例的信号量
 * @return true:成功; false:没有权限传递信号量sig
 */
bool
pid_ctrl_t::kill(int sig)
{
    if (!m_inited) {
        return true;
    }

    // 读pid
    pid_t pid;
    if (!readPid(pid)) {
        return true;
    }


    if (::kill(pid, sig) == -1) {
        switch (errno) {
        case EINVAL:
        case ESRCH:
            // no such process, that's ok
            printf("no such process\n");
            return true;
        case EPERM:
            // permission denied
            printf("permission denied\n");
            return false;
        }
    }
    //delete the pid file
    delPidFile();
    return true;
}


}
