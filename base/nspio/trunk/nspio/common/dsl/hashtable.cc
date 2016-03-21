// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/dsl/hashtable.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS 1

#include <ctype.h>
#include "base/hashtable.h"
#include "os/memalloc.h"


NSPIO_DECLARATION_START

static void _hash_table_slot_assign_all(hash_table_slot_t *hash_table_slot,
					char *buffer, uint32_t *buff_len);

uint32_t 
hash_func_default(void *key, uint32_t key_len)
{
    uint32_t sum = 0, count;

    if (NULL == key){
        return 0;
    }
    for (count = 0; count < key_len; count++){
        sum += *((char*)key + count) * 33;
    }
    return sum;
}

hash_table_t * 
hash_table_create( uint32_t slot_num, m_alloc alloc_func, m_free free_func, hash_func hash)
{
    hash_table_t *hash_table = NULL;
    if (0 == slot_num){
        return hash_table;
    }
    if (NULL != alloc_func && NULL != free_func){
        hash_table = (hash_table_t *)alloc_func(sizeof(hash_table_t));
        hash_table->alloc_func_ = alloc_func;
        hash_table->free_func_ = free_func;
    }else{
        hash_table = (hash_table_t *)malloc(sizeof(hash_table_t));
        hash_table->alloc_func_ = mem_alloc;
        hash_table->free_func_ = mem_free;
    }
    if (NULL == hash_table){
        return NULL;
    }
    hash_table->slot_num_ = slot_num;
    if (NULL != hash){
        hash_table->hash_func_ = hash;
    }else{
        hash_table->hash_func_ = hash_func_default;
    }
    
    hash_table->slots_ = (hash_table_slot_t **) hash_table->alloc_func_(sizeof(hash_table_slot_t *) * slot_num);
    if (NULL == hash_table->slots_){
        if (NULL != hash_table){
            hash_table->free_func_(hash_table);
            hash_table = NULL;
        }
        return NULL;
    }
    
    memset(hash_table->slots_, 0, sizeof(hash_table_slot_t *) * slot_num);
    pmutex_init(&hash_table->lock);
    hash_table->elts = 0;
    return hash_table;
}

hash_table_slot_t * 
_hash_table_slot_find(hash_table_slot_t *head, void *key, uint32_t len)
{
    hash_table_slot_t *cur = head;

    if (!cur || !key || len == 0){
        return NULL;
    }

    while (cur) {
	if (cur->key && memcmp(cur->key, key, len) == 0)
	    return cur;
	cur = cur->next;
    }
    return cur;
}

int 
hash_table_incr( hash_table_t *hash_table, void *key, uint32_t key_len, uint64_t data )
{
    if (NULL == hash_table || NULL == key || 0 == key_len || 
            NULL == hash_table->slots_ || NULL == hash_table->hash_func_){
        return -1;
    }
    if (0 != pmutex_lock(&hash_table->lock)){
        return -1;
    }
    uint32_t slot = hash_table->hash_func_(key, key_len) % hash_table->slot_num_;
    hash_table_slot_t *hash_table_slot = NULL;
    hash_table_slot = _hash_table_slot_find(hash_table->slots_[slot], key, key_len);
    if (NULL != hash_table_slot){
        hash_table_slot->data += data;
        goto Exit0;
    }else{
        hash_table_slot = (hash_table_slot_t *)hash_table->alloc_func_(sizeof(hash_table_slot_t));
        hash_table_slot->key = (char *)hash_table->alloc_func_(key_len);
        memcpy(hash_table_slot->key, key, key_len);
        hash_table_slot->key_len = key_len;
        hash_table_slot->data = data;
        hash_table_slot->next = hash_table->slots_[slot];
        hash_table->slots_[slot] = hash_table_slot;
        hash_table->elts++;
        goto Exit0;
    }
Exit0:
    pmutex_unlock(&hash_table->lock);
    return 0;
}   

int 
hash_table_set( hash_table_t *hash_table, void *key, uint32_t key_len, uint64_t data )
{
    if (NULL == hash_table || NULL == key || 0 == key_len || 
        NULL == hash_table->slots_ || NULL == hash_table->hash_func_){
            return -1;
    }
    if (0 != pmutex_lock(&hash_table->lock)){
        return -1;
    }
    uint32_t slot = hash_table->hash_func_(key, key_len) % hash_table->slot_num_;
    hash_table_slot_t *hash_table_slot = NULL;
    hash_table_slot = _hash_table_slot_find(hash_table->slots_[slot], key, key_len);
    if (NULL != hash_table_slot){
        hash_table_slot->data = data;
        goto Exit0;
    }else{
        hash_table_slot = (hash_table_slot_t *)hash_table->alloc_func_(sizeof(hash_table_slot_t));
        hash_table_slot->key = (char *)hash_table->alloc_func_(key_len);
        memcpy(hash_table_slot->key, key, key_len);
        hash_table_slot->key_len = key_len;
        hash_table_slot->data = data;
        hash_table_slot->next = hash_table->slots_[slot];
        hash_table->slots_[slot] = hash_table_slot;
        hash_table->elts++;
        goto Exit0;
    }
Exit0:
    pmutex_unlock(&hash_table->lock);
    return 0;
}

int 
hash_table_get( hash_table_t *hash_table, void *key, uint32_t key_len, uint64_t *data )
{
    if (NULL == hash_table || NULL == key || 0 == key_len || 
        NULL == hash_table->slots_ || NULL == hash_table->hash_func_){
            return -1;
    }
    if (0 != pmutex_lock(&hash_table->lock)){
        return -1;
    }
    uint32_t slot = hash_table->hash_func_(key, key_len) % hash_table->slot_num_;
    hash_table_slot_t *hash_table_slot = NULL;
    hash_table_slot = _hash_table_slot_find(hash_table->slots_[slot], key, key_len);
    if (NULL != hash_table_slot){
        *data = hash_table_slot->data;
        pmutex_unlock(&hash_table->lock);
	return 0;
    }
    pmutex_unlock(&hash_table->lock);
    return -1;
}

int 
hash_table_mulincr( hash_table_t *hash_table, char *buffer, uint32_t *buff_len )
{
    if (NULL == hash_table || NULL == buffer || NULL == buff_len || 0 == *buff_len){
        return -1;
    }
    char *key = NULL;
    uint32_t key_len =0;
    char *value = NULL;
    uint32_t value_len = 0;
    char *p = NULL;
    for (p = strchr(buffer, '='); p; p = strchr(p, '=')) {
        if (p == buffer) {
            ++p;
            continue;
        }
        for (key = p-1; isspace(*key); --key);
        key_len = 0;
        while (isalnum(*key) || '_' == *key || '\\' == *key || '/' == *key || ':' == *key) {
            /* don't parse backwards off the start of the string */
            if (key == buffer) {
                --key;
                ++key_len;
                break;
            }
            --key;
            ++key_len;
        }
        ++key;
        *(buffer + (key - buffer) + key_len) = '\0';
        for (value = p+1; isspace(*value); ++value);
        value_len = strcspn(value, "\t; |,");
        p = value + value_len;
        if ('\0' != *p){
            *(value + value_len) = '\0';
            p = value + value_len + 1;    
        }else{
            p = value + value_len;
        }
        if (0 != hash_table_incr(hash_table, key, key_len, atol(value))){
            return -1;
        }
    }
    return 0;
}

void 
_hash_table_slot_destroy(hash_table_t *hash_table, uint32_t slot)
{
    if (NULL != hash_table && slot < hash_table->slot_num_ && NULL != hash_table->slots_[slot]){
        hash_table_slot_t *temp = hash_table->slots_[slot]->next;
        if (NULL != hash_table->slots_[slot]->key){
            hash_table->free_func_(hash_table->slots_[slot]->key);
            hash_table->slots_[slot]->key = NULL;
        }
        if (NULL != hash_table->slots_[slot]){
            hash_table->free_func_(hash_table->slots_[slot]);
            hash_table->slots_[slot] = NULL;
        }
        hash_table->slots_[slot] = temp;
        _hash_table_slot_destroy(hash_table, slot);
    }
}

int 
hash_table_destroy( hash_table_t *hash_table )
{
    if (NULL == hash_table){
        return -1;
    }
    hash_table_clear(hash_table);
    pmutex_destroy(&hash_table->lock);
    if (NULL != hash_table->slots_){
        hash_table->free_func_(hash_table->slots_);
        hash_table->slots_ = NULL;
    }
    if (NULL != hash_table){
        hash_table->free_func_(hash_table);
        hash_table = NULL;
    }
    return 0;
}

void 
_hash_table_slot_assign_all(hash_table_slot_t *hash_table_slot, char *buffer, uint32_t *buff_len)
{
    if (NULL != hash_table_slot && NULL != buffer && NULL != buff_len && 0 != *buff_len){
        uint32_t used = 0;
        uint32_t un_used = 0;
        if (used < *buff_len){
            un_used = *buff_len - used;
            used += snprintf(buffer + used, un_used, "%s: %"PRIu64,
			     hash_table_slot->key, hash_table_slot->data);
            if (used < *buff_len){
                un_used = *buff_len - used;
                _hash_table_slot_assign_all(hash_table_slot->next, buffer + used, &un_used);
                *buff_len = used + un_used;
            }
        }
    }else{
        *buff_len = 0;
    }
}

int 
hash_table_assign_all( hash_table_t *hash_table, char *buffer, uint32_t *buff_len )
{
    if (NULL == hash_table || NULL == buffer || NULL == buff_len || 0 == *buff_len){
        return -1;
    }
    if( 0 != pmutex_lock(&hash_table->lock)){
        return -1;
    }
    uint32_t used = 0;
    uint32_t un_used = 0;
    uint32_t count = 0;
    for (count = 0; count < hash_table->slot_num_; count++){
        if (used < *buff_len){
            un_used = *buff_len - used;
            _hash_table_slot_assign_all(hash_table->slots_[count], buffer + used, &un_used);
            used += un_used;
        }
    }
    *buff_len = used;
    pmutex_unlock(&hash_table->lock);
    return 0;
}

int
hash_table_clear( hash_table_t *hash_table )
{
    if (NULL == hash_table){
        return -1;
    }
    if( 0 != pmutex_lock(&hash_table->lock)){
        return -1;
    }
    uint32_t count = 0;
    for (count = 0; count < hash_table->slot_num_; count++){
        _hash_table_slot_destroy(hash_table, count);
    }
    pmutex_unlock(&hash_table->lock);
    return 0;
}

uint32_t 
hash_table_get_size( hash_table_t *hash_table )
{
    if (NULL == hash_table){
        return -1;
    }
    if( 0 != pmutex_lock(&hash_table->lock)){
        return -1;
    }
    uint32_t size = hash_table->elts;
    pmutex_unlock(&hash_table->lock);
    return size;
}


}
