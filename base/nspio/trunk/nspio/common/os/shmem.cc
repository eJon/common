// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/os/shmem.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "os/shmem.h"


NSPIO_DECLARATION_START

shm_t *shm_create(uint64_t size, char *name, mempool_t *p) {
    if (0 == size || NULL == p)
        return NULL;
    shm_t *shm = NULL;
    shm = (shm_t *)mp_alloc(p, sizeof(shm_t));
    memset(shm, 0, sizeof(shm_t));
    shm->size = size;
    shm->addr = 0;
    shm->fd = -1;
    if (NULL != name) {
	shm->name = strdup(name);
        // why ??
	// shm->name = (char *)mp_alloc(p, strlen(name) + 1);
        memcpy(shm->name, name, strlen(name));
    }

    return shm;
}

int shm_alloc(shm_t *shm, char *addr, int is_create_or_clear) {
    if (NULL == shm){
        return -1;
    }
    if (NULL == shm->name) {
        shm_free(shm);
        return -1;
    }
    if (is_create_or_clear) {
        shm->fd = open(shm->name, O_RDWR|O_CREAT|O_TRUNC, 0600);
        if (-1 == shm->fd) {
            shm_free(shm);
            return -1;
        }
        if (-1 == ftruncate(shm->fd, shm->size)) {
            shm_free(shm);
            return -1;
        }
        shm->addr = (char *) mmap(addr, shm->size, PROT_READ|PROT_WRITE, MAP_SHARED, shm->fd, 0);
    } else {
        shm->fd = open(shm->name, O_RDWR);
        if (-1 == shm->fd) {
            shm_free(shm);
            return -1;
        }
        shm->addr = (char *) mmap(addr, shm->size, PROT_READ|PROT_WRITE, MAP_SHARED, shm->fd, 0);
    }
    if (MAP_FAILED == shm->addr) {
        shm_free(shm);
        return -1;
    }

    return 0;
}

int shm_free(shm_t *shm) {
    if (NULL == shm)
        return -1;
    if (-1 != shm->fd) {
        close(shm->fd);
        shm->fd = -1;
    }
    if (NULL != shm->addr) {
        munmap((void *) shm->addr, shm->size);
        shm->addr = NULL;
    }
    if (NULL != shm->name) {
	free(shm->name);
    }

    return 0;
}

}
