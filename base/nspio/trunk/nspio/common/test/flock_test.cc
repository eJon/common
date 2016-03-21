// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/test/flock_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <nspio/errno.h>
#include "runner/thread.h"
#include "sync/flock.h"
#include "os/memalloc.h"

using namespace nspio;


static int test_flock_single() {
    int ret;
    char lock_file[] = "/tmp/mem_flock.file";
    flock_t *fl;

    fl = (flock_t *)mem_alloc(sizeof(flock_t));
    if (!fl) {
	printf("mem_alloc flock_t with errno %d\n", errno);
	return -1;
    }

    if ((ret = flock_create(fl, lock_file)) != 0) {
	printf("flock_create with errno %d\n", errno);
	return -1;
    }
    
    // first, lock this file lock
    if ((ret = flock_lock(fl)) != 0) {
	printf("flock_lock with errno %d\n", errno);
	return -1;
    }


    // in the same process, relock should be ok
    if ((ret = flock_lock(fl)) != 0) {
	printf("flock_relock with errno %d\n", errno);
	return -1;
    }

    if ((ret = flock_trylock(fl)) != 0) {
	printf("flock_trylock with errno %d\n", errno);
	return -1;
    }

    if ((ret = flock_unlock(fl)) != 0) {
	printf("flock_unlock with errno %d\n", errno);
	return -1;
    }

    flock_destroy(fl);    
    mem_free(fl);
    return 0;
}



static int test_flock_thread_worker(void *arg_) {
    int ret;
    flock_t *fl = (flock_t *)arg_;

    if ((ret = flock_lock(fl)) != 0) {
	printf("flock_lock in thread with errno %d\n", errno);
	return -1;
    }
    return 0;
}

static int test_flock_multipthread() {
    int ret;
    char lock_file[] = "/tmp/mem_flock.file";
    flock_t *fl;
    Thread thread;

    fl = (flock_t *)mem_alloc(sizeof(flock_t));
    if (!fl) {
	printf("mem_alloc flock_t with errno %d\n", errno);
	return -1;
    }

    if ((ret = flock_create(fl, lock_file)) != 0) {
	printf("flock_create with errno %d\n", errno);
	return -1;
    }
    
    // first, lock this file lock
    if ((ret = flock_lock(fl)) != 0) {
	printf("flock_lock with errno %d\n", errno);
	return -1;
    }


    thread.Start(test_flock_thread_worker, fl);
    if ((ret = thread.Stop()) != 0) {
	printf("flock_lock in thread with errno %d\n", errno);
	return -1;
    }

    if ((ret = flock_unlock(fl)) != 0) {
	printf("flock_unlock with errno %d\n", errno);
	return -1;
    }

    flock_destroy(fl);    
    mem_free(fl);
    return 0;
}



static int test_flock_process_worker(void *arg_) {
    int ret;
    flock_t *fl = (flock_t *)arg_;

    if ((ret = flock_trylock(fl)) == 0) {
	printf("flock_trylock in process failed. it should be trylock failed!! \n");
	return -1;
    }
    return 0;
}

static int test_flock_multiprocess() {
    int ret;
    char lock_file[] = "/tmp/mem_flock.file";
    flock_t *fl;
    pid_t pid;

    fl = (flock_t *)mem_alloc(sizeof(flock_t));
    if (!fl) {
	printf("mem_alloc flock_t with errno %d\n", errno);
	return -1;
    }

    if ((ret = flock_create(fl, lock_file)) != 0) {
	printf("flock_create with errno %d\n", errno);
	return -1;
    }
    
    // first, lock this file lock
    if ((ret = flock_lock(fl)) != 0) {
	printf("flock_lock with errno %d\n", errno);
	return -1;
    }

    if ((pid = fork()) == 0) {
	// child
	int ret = test_flock_process_worker(fl);
	flock_destroy(fl);
	mem_free(fl);
	exit(ret);
    } else if (pid > 0) {
	waitpid(pid, &ret, 0);
    }

    if (ret != 0) {
	printf("flock_lock in process failed: %d\n", ret);
	return -1;
    }

    if ((ret = flock_unlock(fl)) != 0) {
	printf("flock_unlock with errno %d\n", errno);
	return -1;
    }

    flock_destroy(fl);
    mem_free(fl);
    return 0;
}



static int test_flock_multiprocess_ex() {
    int ret;
    char lock_file[] = "/tmp/mem_flock.file";
    flock_t *fl;
    pid_t pid;

    fl = (flock_t *)mem_alloc(sizeof(flock_t));
    if (!fl) {
	printf("mem_alloc flock_t with errno %d\n", errno);
	return -1;
    }

    if ((ret = flock_create(fl, lock_file)) != 0) {
	printf("flock_create with errno %d\n", errno);
	return -1;
    }


    if ((ret = flock_lock(fl)) != 0) {
	printf("flock_lock with errno %d\n", errno);
	return -1;
    }
    
    if ((pid = fork()) == 0) {
	// child
	flock_unlock(fl);
	flock_destroy(fl);
	mem_free(fl);
	exit(0);
    }

    if ((ret = flock_unlock(fl)) != 0) {
	printf("flock_unlock with errno %d\n", errno);
	return -1;
    }
    if ((ret = flock_lock(fl)) != 0) {
	printf("flock_lock with errno %d\n", errno);
	return -1;
    }
    if ((pid = fork()) == 0) {
	// child
	if ((ret = flock_trylock(fl)) == 0) {
	    printf("flock_lock error, it should be lock failed in other process");
	    exit(-1);
	}
	flock_destroy(fl);
	mem_free(fl);
	exit(0);
    } else if (pid > 0) {
	waitpid(pid, &ret, 0);
    }
    if (ret != 0) {
	return -1;
    }
    if ((ret = flock_unlock(fl)) != 0) {
	printf("flock_unlock with errno %d\n", errno);
	return -1;
    }
    if ((ret = flock_destroy(fl)) != 0) {
	printf("flock_destroy with errno %d\n", errno);
	return -1;
    }

    mem_free(fl);
    return 0;
}





TEST(flock, single) {
    EXPECT_EQ(0, test_flock_single());
}

/*
 thread A,B
 A: lock(fl)
 B: EXPECT(lock(fl) == 0)
 A: unlock(fl)
 */  

TEST(flock, multipthread) {
    EXPECT_EQ(0, test_flock_multipthread());
}

/*
 process A,B
 A: lock(fl)
 B: EXPECT(lock(fl) == -1)
 A: unlock(fl)
 */  

TEST(flock, multiprocess) {
    return;
    EXPECT_EQ(0, test_flock_multiprocess());
}



TEST(flock, multiprocess_ex) {
    return;
    EXPECT_EQ(0, test_flock_multiprocess_ex());
}

