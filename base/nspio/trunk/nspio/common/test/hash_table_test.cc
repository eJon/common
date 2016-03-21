// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/test/hash_table_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <gtest/gtest.h>
#include "base/hashtable.h"
#include "runner/thread.h"

using namespace nspio;


static void *my_alloc(uint32_t size) {
    return malloc(size);
}

static void my_free(void *ptr) {
    return free(ptr);
}


#define cnt 1000
#define ht_key_size 1024
static char ht_key1[] = "hash_table_key";
static char ht_sharedkey[] = "hash_table_sharedkey";


static int test_hash_table_worker(void *arg_) {
    hash_table_t *hash_table = (hash_table_t *)arg_;
    int i = 0, ret;
    uint32_t buff_len;
    char ht_key[ht_key_size];
    uint64_t data, shareddata = 0, data_range = 20;

    for (i = 0; i < cnt; i++) {
	data = rand() % data_range;
	shareddata += data;

	snprintf(ht_key, ht_key_size, "%s_%d", ht_key1, i);
	if ((ret = hash_table_incr(hash_table, ht_key, strlen(ht_key), data)) != 0) {
	    printf("hash_table_incr %s failed: %d\n", ht_key, ret);
	    return -1;
	}

	snprintf(ht_key, ht_key_size, "%s_%d=%"PRIu64", %s=%"PRIu64, ht_key1, i, data, ht_sharedkey, data);
	buff_len = strlen(ht_key);
	if ((ret = hash_table_mulincr(hash_table, ht_key, &buff_len)) != 0) {
	    printf("hash_table_mulincr %s failed: %d\n", ht_key, ret);
	    return -1;
	}
    }

    return 0;
}



static int test_hash_table_single(int argc, char **argv) {
    int slot_size = 1024, i, ret;
    uint64_t data = 0, sum = 0;
    hash_table_t *ht;
    char ht_key[ht_key_size];

    ht = hash_table_create(slot_size, my_alloc, my_free, NULL);
    if (!ht) {
	printf("hash_table_create failed ...\n");
	return -1;
    }
    test_hash_table_worker(ht);


    // valid all the hash table op have been success done?

    for (i = 0; i < cnt; i++) {
	snprintf(ht_key, ht_key_size, "%s_%d", ht_key1, i);
	if ((ret = hash_table_get(ht, ht_key, strlen(ht_key), &data)) != 0) {
	    printf("hash_table_get %s failed: %d\n", ht_key, ret);
	    return -1;
	}
	sum += data;
    }
    if ((ret = hash_table_get(ht, ht_sharedkey, strlen(ht_sharedkey), &data)) != 0) {
	printf("hash_table_get %s failed: %d\n", ht_sharedkey, ret);
	return -1;
    }
    if (sum != data << 1) {
	printf("invalid all op because of sum incr error: %"PRIu64" != %"PRIu64"\n", sum, data);
	return -1;
    }


    hash_table_destroy(ht);
    return 0;
}

static int test_hash_table_multipthread(int argc, char **argv) {
    int ret, slot_size = 1024, thread_cnt = 2, i;
    hash_table_t *ht;
    char ht_key[ht_key_size];
    uint64_t data = 0, sum = 0;
    Thread thread[thread_cnt];

    
    ht = hash_table_create(slot_size, my_alloc, my_free, NULL);
    if (!ht) {
	printf("hash_table_create failed...\n");
	return -1;
    }

    for (i = 0; i < thread_cnt; i++) {
	thread[i].Start(test_hash_table_worker, ht);
    }

    for (i = 0; i < thread_cnt; i++) {
	thread[i].Stop();
    }

    // valid all the hash table op have been success done?

    for (i = 0; i < cnt; i++) {
	snprintf(ht_key, ht_key_size, "%s_%d", ht_key1, i);
	if ((ret = hash_table_get(ht, ht_key, strlen(ht_key), &data)) != 0) {
	    printf("hash_table_get %s failed: %d\n", ht_key, ret);
	    return -1;
	}
	sum += data;
    }
    if ((ret = hash_table_get(ht, ht_sharedkey, strlen(ht_sharedkey), &data)) != 0) {
	printf("hash_table_get %s failed: %d\n", ht_sharedkey, ret);
	return -1;
    }
    if (sum != data << 1) {
	printf("invalid all op because of sum incr error: %"PRIu64" != %"PRIu64"\n", sum, data << 1);
	return -1;
    }
    
    hash_table_destroy(ht);
    return 0;
}


TEST(hash_table, single_worker) {
    EXPECT_EQ(0, test_hash_table_single(1, NULL));
}

TEST(hash_table, multipthread) {
    EXPECT_EQ(0, test_hash_table_multipthread(1, NULL));
}


#undef cnt
