// Get statistics of a process
// the code is collected and cleaned from an anonymous post from web
// Author: gejun@baidu.com
// Date: Dec 8, 21:34:54 CST 2010
#pragma once
#ifndef _GET_PROC_STAT_H_
#define _GET_PROC_STAT_H_

#include <unistd.h>
#include <limits.h>

// the structure corresponding the content in /proc/<pid>/stat
struct proc_stat_t {
    int           pid;                      // The process id. 
    char          exName [_POSIX_PATH_MAX];  // The filename of the executable 
    char          state;   // 1             // R is running, S is sleeping,
                                            // D is sleeping in an uninterruptible wait,
                                            // Z is zombie, T is traced or stopped 
    unsigned int  euid;                     // effective user id 
    unsigned int  egid;                     // effective group id */
    int           ppid;                     // The pid of the parent. 
    int           pgrp;                     // The pgrp of the process. 
    int           session;                  // The session id of the process. 
    int           tty;                      // The tty the process uses 
    int           tpgid;                    // (too long) 
    unsigned int  flags;                    // The flags of the process. 
    unsigned int  minflt;                   // The number of minor faults 
    unsigned int  cminflt;                  // The number of minor faults with childs 
    unsigned int  majflt;                   // The number of major faults 
    unsigned int  cmajflt;                  // The number of major faults with childs 
    int           utime;                    // user mode jiffies 
    int           stime;                    // kernel mode jiffies 
    int           cutime;                   // user mode jiffies with childs 
    int           cstime;                   // kernel mode jiffies with childs 
    int           counter;                  // process's next timeslice 
    int           priority;                 // the standard nice value, plus fifteen 
    unsigned int  timeout;                  // The time in jiffies of the next timeout 
    unsigned int  itrealvalue;              // The time before the next SIGALRM is sent to the process 
    int           starttime;  // 20          // Time the process started after system boot 
    unsigned int  vsize;                    // Virtual memory size 
    unsigned int  rss;                      // Resident Set Size 
    unsigned int  rlim;                     // Current limit in bytes on the rss 
    unsigned int  startcode;                // The address above which program text can run 
    unsigned int  endcode;                  // The address below which program text can run 
    unsigned int  startstack;               // The address of the start of the stack 
    unsigned int  kstkesp;                  // The current value of ESP 
    unsigned int  kstkeip;                  // The current value of EIP 
    int           signal;                   // The bitmap of pending signals 
    int           blocked;  // 30            // The bitmap of blocked signals 
    int           sigignore;                // The bitmap of ignored signals 
    int           sigcatch;                 // The bitmap of catched signals 
    unsigned int  wchan;  // 33             // (too long) 
};

// Get statistics of the process according to pid
// Params:
//   p_stat  output structure
//   pid     process id
int get_proc_stat (proc_stat_t* p_stat, pid_t pid);

// Get statistics of the process where this function is called
// Params:
//   p_stat  output structure 
int get_current_proc_stat (proc_stat_t * p_info);

#endif  // _GET_PROC_STAT_H_
