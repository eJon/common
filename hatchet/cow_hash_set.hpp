// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// External probing hashset/map with reference counting
// Author: gejun@baidu.com
// Date: Dec.27 15:28:29 CST 2010
#pragma once
#ifndef _COW_HASH_SET_HPP_
#define _COW_HASH_SET_HPP_

#include <sstream>                          // ostringstream
#include "base_cow_hash_map.h"              // BaseCowHashMap
#include "st_hash.h"                        // Hash
#include "compare.hpp"                      // Equal

namespace st {

template <class _Item,
          class _GetKey,
          class _Hash  = Hash<TCAP(ReturnType, _GetKey, _Item)>,
          class _Equal = Equal<TCAP(ReturnType, _GetKey, _Item)> >
class CowHashSet {
public:
    typedef TCAP(ReturnType, _GetKey, _Item) Key;
    typedef _Item Item;
    typedef _Item* Pointer;
    typedef _Item& Reference;
    typedef const _Item& ConstReference;
    typedef BaseCowHashMap::Alloc Allocator;
    typedef BaseCowHashIterator<Item> Iterator;
    
    class ItemHandler : public CowHashMapItemHandler {
    public:
        size_t hash_key(const void* key) const
        { return _Hash()(*static_cast<const Key*>(key)); }
            
        void get_key(void* key, const void* item) const
        { 
            new (key) Key(_GetKey()(*static_cast<const _Item*>(item)));
        }
        
        bool eq_key_item(const void* key, const void* item) const
        { return _Equal()(*static_cast<const Key*>(key),
                          _GetKey()(*static_cast<const _Item*>(item))); }

        void copy_item(void* dst_item, const void* src_item) const
        { *static_cast<_Item*>(dst_item) = *static_cast<const _Item*>(src_item); }

        void copy_construct_item(void* dst_item, const void* src_item) const
        { ST_NEW_ON(dst_item, _Item, *static_cast<const _Item*>(src_item)); }
            
        void destruct_item(void* item) const
        { reinterpret_cast<_Item*>(item)->~_Item(); }
    };
    
    CowHashSet () {}
    ~CowHashSet () {}

    int init (size_t n_bucket, u_int load_factor)
    {
        std::ostringstream oss;
        static ItemHandler ih;

        oss << c_show(Key) << "->" << c_show(_Item);
        return base_.init(sizeof(_Item), n_bucket, load_factor, &ih, oss.str());
    }

    CowHashSet (const CowHashSet& rhs) : base_(rhs.base_) {}

    CowHashSet& operator= (const CowHashSet& rhs)
    {
        base_ = rhs.base_;
        return *this;
    }
        
    void swap (CowHashSet & rhs) { base_.swap(rhs.base_); }

    // Insert an item into CowHashSet
    // Returns: address of the inserted item, NULL means insertion was failed
    Pointer insert (const Item& item)
    {
        const Key k = _GetKey()(item);
        return static_cast<Pointer>(base_.insert(&k, &item));
    }

    // Erase the value matching a key from CowHashSet
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
            _Item* item = reinterpret_cast<_Item*>(p_node->item_);
            if (_Equal()(key, _GetKey()(*item))) {
                return item;
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
                if (_Equal()(
                        key, reinterpret_cast<_Item*>(p_node->item_))) {
                    return Iterator(p_node, pp_entry);
                }
                p_node = p_node->p_next_;
            } while (p_node != NULL);
        }
        return NULL;
    }

    // Change number of buckets
    // Params:
    //   nbucket2  intended number of buckets
    // Returns: resized or not
    bool resize (size_t nbucket2) { return base_.resize(nbucket2); }

    Iterator begin () const { return Iterator(base_.bucket_array()); }
    Iterator end () const
    { return Iterator(base_.bucket_array() + base_.bucket_num()); }

    bool not_init () const { return base_.not_init(); }
    bool empty () const { return base_.empty(); }
    size_t size () const { return base_.size(); }
    u_int load_factor () const { return base_.load_factor(); }
    size_t bucket_num () const { return base_.bucket_num(); }
    size_t mem () const { return base_.mem(); }

    const Allocator* alloc () const { return base_.alloc(); }
    long alloc_use_count ()  const { return base_.alloc_use_count(); }
    bool alloc_owner () const { return base_.alloc_owner(); }

    // Get max/min/average length of chains
    // input pointers must be non-NULL
    void bucket_info (size_t* p_max_len, size_t* p_min_len,
                      double* p_avg_len) const
    { base_.bucket_info(p_max_len, p_min_len, p_avg_len);}

    // Get the actual load factor

    // Print internal info to StringWriter
    void to_string (StringWriter& sw) const { base_.to_string(sw); }

friend std::ostream& operator<< (std::ostream& os, const CowHashSet& m)
    {
        size_t max_len = 0, min_len = 0;
        double avg_len = 0;
        m.bucket_info(&max_len, &min_len, &avg_len);
        return os << c_show(CowHashSet)
                  << " map=" << max_len
                  << "/" << min_len
                  << "/" << avg_len
                  << "/" << m.bucket_num(); 
    }

    static bool local_item (const void* p_item)
    { return BaseCowHashMap::local_item(p_item); }

    std::string desc () const { return base_.desc(); }
    
private:
    BaseCowHashMap base_;
};

// Print type of this container
template <typename _Item, class _GetKey, class _Hash, class _Equal>
struct c_show_impl<CowHashSet<_Item, _GetKey, _Hash, _Equal> > {
    static void c_to_string (std::ostream& os)
    {
        os << c_show(TCAP(ReturnType, _GetKey, _Item))
           << "->" << c_show(_Item);
    }
};

}  // namespace st

#endif  //_COW_HASH_SET_HPP_
