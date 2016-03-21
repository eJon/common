// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/base/hashtable.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _HASH_TABLE_H_INCLUDE_
#define _HASH_TABLE_H_INCLUDE_
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sync/pmutex.h"


NSPIO_DECLARATION_START


/*
 * multipthread or multiprocess safe hash table
 */

typedef void *(*m_alloc)(uint32_t size);
typedef void (*m_free)(void * p);
typedef uint32_t (*hash_func)(void * key, uint32_t key_len); 

typedef struct hash_table_slot_t hash_table_slot_t;
typedef struct hash_table_t hash_table_t;

struct hash_table_slot_t
{
    char    *key;
    uint32_t key_len;
    uint64_t data;
    hash_table_slot_t *next;
};

struct hash_table_t 
{
    pmutex_t lock;
    uint32_t elts;
    uint32_t slot_num_;
    hash_table_slot_t **slots_;
    m_alloc  alloc_func_;
    m_free   free_func_;
    hash_func  hash_func_;
};

hash_table_t *hash_table_create(uint32_t slot_num, m_alloc alloc_func,
				m_free free_func, hash_func hash);
int hash_table_destroy(hash_table_t *hash_table);
int hash_table_clear(hash_table_t *hash_table);
int hash_table_incr(hash_table_t *hash_table, void *key, uint32_t key_len, uint64_t data);
int hash_table_set(hash_table_t *hash_table, void *key, uint32_t key_len, uint64_t data);
int hash_table_get(hash_table_t *hash_table, void *key, uint32_t key_len, uint64_t *data);
int hash_table_mulincr(hash_table_t *hash_table, char *buffer, uint32_t *buff_len);
int hash_table_assign_all(hash_table_t *hash_table, char *bufer, uint32_t *buff_len);
uint32_t hash_table_get_size(hash_table_t *hash_table);


}

 
#endif //_HASH_TABLE_H_INCLUDE_
