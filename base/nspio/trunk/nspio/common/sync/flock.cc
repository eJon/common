// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/sync/flock.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <nspio/errno.h>
#include "sync/flock.h"
#include "os/memalloc.h"


NSPIO_DECLARATION_START

int trylock_fd(int fd)
{
    struct flock  fl;

    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_pid = 0;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    if (fcntl(fd, F_SETLK, &fl) == -1)
        return errno;

    return 0;
}

int lock_fd(int fd)
{
    struct flock  fl;

    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_pid = 0;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    if (fcntl(fd, F_SETLKW, &fl) == -1)
        return errno;
    return 0;
}

int unlock_fd(int fd)
{
    struct flock  fl;

    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_pid = 0;
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    if (fcntl(fd, F_SETLK, &fl) == -1)
        return errno;
    return 0;
}

int flock_create(flock_t *fl, const char *name) {
    int namelen;
    if (NULL == fl || NULL == name){
        return 0;
    }
    fl->fd = open(name, O_RDWR|O_CREAT|O_TRUNC, 0600);
    if (fl->fd == -1) {
	return -1;
    }
    namelen = strlen(name) + 1;
    fl->name = (char *)mem_alloc(namelen);
    memset(fl->name, 0, namelen);
    strcpy(fl->name, name);
    return 0;
}

int flock_destroy(flock_t *fl)
{
    if (NULL == fl || -1 == fl->fd){
        return 0;
    }
    close(fl->fd);
    remove(fl->name);
    mem_free(fl->name);
    return 0;
}

int flock_trylock(flock_t *fl)
{
    if (NULL == fl || -1 == fl->fd){
        return 0;
    }
    return trylock_fd(fl->fd);
}

int flock_lock(flock_t *fl)
{
    if (NULL == fl || -1 == fl->fd){
        return 0;
    }
    return lock_fd(fl->fd);
}

int flock_unlock(flock_t *fl)
{
    if (NULL == fl || -1 == fl->fd){
        return 0;
    }
    return unlock_fd(fl->fd);
}


}
