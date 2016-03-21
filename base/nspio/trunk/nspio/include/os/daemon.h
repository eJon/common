// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/os/daemon.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef __SPIO_DAEMON_h_INCLUDED__
#define __SPIO_DAEMON_h_INCLUDED__

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "decr.h"


NSPIO_DECLARATION_START

int init_daemon()
{
    int  fd;
    switch (fork()) {
    case -1:
        return -1;
    case 0:
        break;
    default:
        exit(0);
    }

    if (setsid() == -1) {
        return -1;
    }

    umask(0);

    fd = open("/dev/null", O_RDWR);
    if (fd == -1) {
        return -1;
    }

    if (dup2(fd, STDIN_FILENO) == -1) {
        return -1;
    }

    if (dup2(fd, STDOUT_FILENO) == -1) {
        return -1;
    }

    /*
    if (dup2(fd, STDERR_FILENO) == -1) {
    return SPIO_ERR;
    }
    */

    if (fd > STDERR_FILENO) {
        if (close(fd) == -1) {
            return -1;
        }
    }

    return 0;
}


}
 
#endif
