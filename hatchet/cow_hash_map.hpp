// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// External probing hashset/map with reference counting
// Author: gejun@baidu.com
// Date: Dec.27 15:28:29 CST 2010
#pragma once
#ifndef _COW_HASH_MAP_HPP_
#define _COW_HASH_MAP_HPP_

#include <sstream>                          // ostringstream
#include "base_cow_hash_map.h"              // BaseCowHashMap
#include "st_hash.h"                        // Hash
#include "compare.hpp"                      // Equal

namespace st {

template <typename _Key, typename _Val,
          class _Hash = Hash<_Key>,
          class _Equal = Equal<_Key> >
class CowHashMap {
private:
    typedef std::pair<_Key, _Val> Pair;
public:
    typedef BaseCowHashMap::Alloc Allocator;
    typedef Pair Item;
    typedef _Key Key;
    typedef _Val Value;
    typedef _Val* Pointer;
    typedef _Val& Reference;
    typedef const _Val& ConstReference;
    typedef BaseCowHashIterator<Item> Iterator;

    class ItemHandler : public CowHashMapItemHandler {
    public:
        size_t hash_key(const void* key) const
        { return _Hash()(*static_cast<const Key*>(key)); }
            
        void get_key(void* key, const void* pair) const
        { 
            new (key) Key(static_cast<const Pair*>(pair)->first);
        }
        
        bool eq_key_item(const void* key, const void* pair) const
        { return _Equal()(*static_cast<const Key*>(key),
                          static_cast<const Pair*>(pair)->first); }

        void copy_item(void* dst_pair, const void* src_pair) const
        { *static_cast<Pair*>(dst_pair) =
                *static_cast<const Pair*>(src_pair); }

        void copy_construct_item(void* dst_pair, const void* src_pair) const
        { ST_NEW_ON(dst_pair,
                    Pair, *static_cast<const Pair*>(src_pair)); }
            
        void destruct_item(void* pair) const
        { reinterpret_cast<Pair*>(pair)->~Pair(); }
    };
    
    CowHashMap () {}
    ~CowHashMap () {}

    int init(size_t nbucket         = CH_DEFAULT_NBUCKET,
             u_int load_factor      = CH_DEFAULT_LOAD_FACTOR)
    {
        std::ostringstream oss;
        static ItemHandler ih;

        oss << c_show(_Key) << "->" << c_show(_Val);
        return base_.init(sizeof(Pair), nbucket, load_factor, &ih, oss.str());
    }

    CowHashMap(const CowHashMap& rhs) : base_(rhs.base_) {}
    
    CowHashMap& operator=(const CowHashMap& rhs)
    {
        base_ = rhs.base_;
        return *this;
    }
        
    void swap (CowHashMap & rhs) { base_.swap(rhs.base_); }

    // Insert an item into CowHashMap
    // Returns: address of the inserted item, NULL means insertion was failed
    Pointer insert (const Key& key, const Value& value)
    {
        Pair pair(key, value);
        return &(static_cast<Pair*>(base_.insert(&key, &pair))->second);
    }

    // Erase the value matching a key from CowHashMap
    // Returns: erased or not
    bool erase (const Key& key) { return base_.erase(&key); }

    // Erase all items
    void clear () { base_.clear(); }
        
    // Search for the item matching a key
    // Returns: address of the item
    Pointer seek (const Key& key) const
    {
        const size_t bkt = _Hash()(key) % base_.bucket_num();
        Bucket* p_node = base_.bucket_array()[bkt];

        while (NULL != p_node) {
            Pair* pair = reinterpret_cast<Pair*>(p_node->item_);
            if (_Equal()(key, pair->first)) {
                return &(pair->second);
            }
            p_node = p_node->p_next_;
        }
        return NULL;
    }

    Iterator find (const Key& key) const
    {
        const size_t bkt = _Hash()(key) % base_.bucket_num();
        Bucket* const* pp_entry = base_.bucket_array() + bkt;

        if (NULL != *pp_entry) {
            Bucket* p_node = *pp_entry;
            do {
                if (_Equal()(key, reinterpret_cast<Pair*>(p_node->item_)->first)) {
                    return Iterator(p_node, pp_entry);
                }
                p_node = p_node->p_next_;
            } while (p_node != NULL);
        }
        return NULL;
    }

    bool resize(size_t nbucket2) { return base_.resize(nbucket2); }

    Iterator begin () const { return Iterator(base_.bucket_array()); }
    Iterator end () const
    { return Iterator(base_.bucket_array() + base_.bucket_num()); }

    bool not_init() const { return base_.not_init(); }
    bool empty() const { return base_.empty(); }
    size_t size() const { return base_.size(); }
    u_int load_factor () const { return base_.load_factor(); }
    size_t mem() const { return base_.mem(); }
    size_t bucket_num() const { return base_.bucket_num(); }
    void bucket_info(size_t* p_max_len, size_t* p_min_len, double* p_avg_len) const
    { base_.bucket_info(p_max_len, p_min_len, p_avg_len);}

    // Allocator information
    const Allocator* alloc () const { return base_.alloc(); }
    long alloc_use_count ()  const { return base_.alloc_use_count(); }
    bool alloc_owner () const { return base_.alloc_owner(); }

    // Print internal info to StringWriter
    void to_string (StringWriter& sw) const { base_.to_string(sw); }

    static bool local_item(const void* p_item)
    { return BaseCowHashMap::local_item(p_item); }
    
private:
    BaseCowHashMap base_;
};

}  // namespace st

#endif  //_COW_HASH_MAP_HPP_
