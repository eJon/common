// Copyright(c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// External probing hashset/map with reference counting
// Author: gejun@baidu.com
// Date: Dec.27 15:28:29 CST 2010
#pragma once
#ifndef _BASE_COW_HASH_MAP_H_
#define _BASE_COW_HASH_MAP_H_

#include <string>                      // std::string
#include "rc_memory_pool.h"            // RCMemoryPool
#include "st_shared_ptr.hpp"           // shared_ptr<1>

// "external probing" is obviously not the fastest hash mapping, however
// since items are chained in separate memory blocks, there is a chance
// that they can be shared between different maps. We observed that in
// our applications load factors are hardly over 1 which means lengths
// of chains are very short, most of them are just 1. We add a reference
// counter to each item to track number of owners and copy the shared
// item and its chained ancestors on write. This method exposes a great
// possibility between maps to share nodes that they don't change, which
// we really want to have a memory-wise implementation of Multiple Version
// Concurrency Control. We also utilize the reference counters outside
// this map, when another data structure wants to store internal address
// of an item inside this map, namely an inverted index, it must increase
// the corresponding reference counter(and dereference when it does not
// need the item). With the help of these counters, pointer joining is
// much safer.

namespace st {
const size_t CH_MIN_NBUCKET = 32;          // minimum number of buckets
const size_t CH_DEFAULT_NBUCKET = 1543;    // default number of buckets
const u_int  CH_MIN_LOAD_FACTOR = 10;      // minimum load factor
const u_int  CH_MAX_LOAD_FACTOR = 100;     // maximum load factor
const u_int  CH_DEFAULT_LOAD_FACTOR = 80;  // default load factor

class CowHashMapItemHandler {
public:
    virtual size_t hash_key(const void*) const = 0;
    virtual void get_key(void*, const void*) const = 0;
    virtual bool eq_key_item(const void*, const void*) const = 0;
    virtual void copy_item(void*, const void*) const = 0;
    virtual void copy_construct_item(void*, const void*) const = 0;
    virtual void destruct_item(void*) const = 0;
};

// this is external probing hash mapping, items are stored in
// separate nodes managed by a pool
struct Bucket {
    Bucket* p_next_;
    // In c++11, directly dereferencing flexible array memeber
    // will break strict-aliasing rules. This warning can be
    // avoided by dereferencing via a temporary variable
    char item_[];
};
static Bucket* const END_BUCKET = (Bucket*)-1;

// interfaces of BaseCowHashMap    
class BaseCowHashMap {
public:    
    typedef RCMemoryPool Alloc;                      // type of allocator

    BaseCowHashMap();
    ~BaseCowHashMap();

    // Construct from another BaseCowHashMap. Triggered by "BaseCowHashMap x = y; 
    //(where y is typed BaseCowHashMap). It's hard to decide sharing allocator
    // or not during a copy-construction. Sharing is not thread-safe while 
    // not-sharing disables copy-on-write. We take a flexible solution here:
    // If the source map has local allocator, the allocator is not shared;
    // otherwise the allocator is shared
    BaseCowHashMap(const BaseCowHashMap&);

    // Assign from another BaseCowHashMap. Triggered by "x = y; "(where x and y
    // are both typed BaseCowHashMap). If "init" is not called, this function 
    // works exactly as copy-constructor; else if allocators are same, this
    // function unifies number of buckets to improve sharing ratio; 
    // otherwise, this function works as a normal assignment: conservatively
    // decide if the entry array needs to be resized, and copy all items 
    // from source to destination.
    // 
    // Returns: *this to chain invocations
    BaseCowHashMap& operator=(const BaseCowHashMap&);
    
    // Initialize this set, must be called before using, except that
    // assignment(operator=) to an uninitialized set also does initialization
    // Params:
    //   nbucket    number of initial buckets
    //   load_factor a number inside [CH_MIN_LOAD_FACTOR, CH_MAX_LOAD_FACTOR]
    //               to indicate the intended ratio(multiplying 100) between
    //               number of items and buckets(to provide reasonable 
    //               performance)
    //   p_alloc     where chaining nodes are from, if you want multiple maps
    //               to share chaining nodes, you have to make sure that they
    //               use the same allocator, otherwise memory-wise sharing is
    //               disabled.
    // Returns:
    //   0           success
    //   ECONFLICT   already initialized
    //   ENOMEM      fail to new
    int init(u_int item_size,
             size_t nbucket,
             u_int load_factor,
             CowHashMapItemHandler* item_handler,
             std::string desc);
                    
    // Insert an item into BaseCowHashMap
    // Returns:
    //   NULL         insertion was failed
    //   otherwise    address of the inserted item
    void* insert(const void* key, const void* item);

    // Erase the value matching a key from BaseCowHashMap
    // Returns:
    //   0            nothing matched
    //   1            one item was erased
    int erase(const void* key);

    // Erase all items
    void clear();
        
    // Search for the item matching a key
    // Returns: address of the item
    void* seek(const void* key) const;

    // Swap contents of two instances
    void swap(BaseCowHashMap & rhs);

    // Change number of buckets
    // Params:
    //   nbucket2  intended number of buckets
    // Returns: resized or not
    bool resize(size_t nbucket2);
    
    // Get referenced count of an item in this map
    static bool local_item(const void* p_item)
    { return Alloc::is_local(
            static_cast<const char*>(p_item) - offsetof(Bucket, item_)); }

    bool not_init() const { return NULL == ap_entry_; }
    bool empty() const { return 0 == n_item_; }
    size_t size() const { return n_item_; }
    size_t bucket_num() const { return nbucket_; }
    size_t mem() const;       // resident memory

    size_t node_size() const { return item_size_ + sizeof(Bucket); }

    // Get internal array storing entries of buckets
    Bucket* const* bucket_array() const { return ap_entry_; }

    // Get pointer to internal allocator
    const Alloc* alloc() const { return sp_alloc_.get(); }

    // Number of users of the allocator
    long alloc_use_count()  const { return sp_alloc_.use_count(); }

    // An allocator belongs to at-most one container
    bool alloc_owner() const
    { return alloc_creator_ || sp_alloc_.use_count() == 1; }

    // Get max/min/average length of chains
    // input pointers must be non-NULL
    void bucket_info(size_t* p_max_len, size_t* p_min_len,
                     double* p_avg_len) const;

    // Get the actual load factor
    u_int load_factor() const
    { return static_cast<u_int>(n_item_ * 100 / nbucket_); }

    // Print internal info to StringWriter
    void to_string(StringWriter& sw) const;

friend std::ostream& operator<<(std::ostream& os, const BaseCowHashMap& m);
    
    std::string desc() const { return desc_; }

private:
    inline static void inc_node_ref_(Bucket* p_node)
    { Alloc::inc_ref(p_node); }

    struct DestructBucket {
        void operator()(void* node) const
        { item_handler_->destruct_item(static_cast<Bucket*>(node)->item_); }
        
        CowHashMapItemHandler* item_handler_;
    };
    
    inline void dec_node_ref_(Bucket* p_node) const
    {
        DestructBucket dn = { item_handler_ };
        Alloc::dec_ref(p_node, sp_alloc_.get(), dn);
    }

    inline Bucket* alloc_node_(const void* item) const
    {
        Bucket* p_node = static_cast<Bucket*>(sp_alloc_->alloc_mem());
        item_handler_->copy_construct_item(p_node->item_, item);
        return p_node;
    }

    inline static bool local_node(Bucket* p_node)
    { return Alloc::is_local(p_node); }
    
    void chain_reference(Bucket* p_begin, Bucket* p_end);
    void chain_dereference(Bucket* p_begin, Bucket* p_end);
    Bucket* chain_copy(Bucket* p_begin, Bucket* p_end);

    bool resize_if_necessary()
    {
        if ((nbucket_ * load_factor_) <(n_item_ * 100)) {
            // ratios between neighbor primes in utility.hpp/find_next_prime
            // are generally near but less than 2, using 190 rather than 200
            // here guarantees primes in the sequence are not skipped.
            return resize(n_item_ * 190 / load_factor_);
        }
        return false;
    }

    size_t n_item_;                       // number of items
    size_t nbucket_;                     // number of buckets
    u_int item_size_;                     // size of item
    u_int load_factor_;                   // 100*n_item_/nbucket_
    Bucket** ap_entry_;                     // array of buckets
    CowHashMapItemHandler* item_handler_;
    st_boost::shared_ptr<Alloc> sp_alloc_; // pointer to allocator
    bool alloc_creator_;                    
    std::string desc_;
};

// Iterate BaseCowHashMap
template <typename _Value> class BaseCowHashIterator {
public:
    typedef _Value Value;
    
    // Default constructor
    BaseCowHashIterator() : p_node_(NULL), pp_entry_(NULL)
    {}

    // Construct from a position in a map
    BaseCowHashIterator(Bucket* const* pp_entry) : pp_entry_(pp_entry)
    {
        find_valid_node();
    }

    BaseCowHashIterator(const Bucket* p_node, Bucket* const* pp_entry)
        : p_node_(p_node), pp_entry_(pp_entry)
    {}

    // *this == rhs
    bool operator==(const BaseCowHashIterator& rhs) const
    { return p_node_ == rhs.p_node_; }

    // *this != rhs
    bool operator!=(const BaseCowHashIterator& rhs) const
    { return p_node_ != rhs.p_node_; }
        
    // ++ it
    BaseCowHashIterator& operator++()
    {
        if (NULL == p_node_->p_next_) {
            ++ pp_entry_;
            find_valid_node();
        } else {
            p_node_ = p_node_->p_next_;
        }

        return *this;
    }

    // it ++
    BaseCowHashIterator operator++(int)
    {
        BaseCowHashIterator tmp = *this;
        this->operator++();
        return tmp;
    }

    Value& operator*() const  
    {
        Value* ret = (Value*)p_node_->item_;
        return *ret;
    }
    Value* operator->() const { return (Value*)(p_node_->item_); }
    operator const void*() const    { return p_node_; }
    void set_end()            { p_node_ = NULL; }
    void to_string(StringWriter& sw) const
    {
        sw << "CowHashIter{p_item=" <<(p_node_ ? operator*() : NULL)
           << "}";
    }
    
private:
    void find_valid_node()
    {
        for( ; NULL == *pp_entry_; ++pp_entry_);
        p_node_ = (*pp_entry_ != END_BUCKET ? *pp_entry_ : NULL);
    }
            
    const Bucket* p_node_;
    Bucket* const* pp_entry_;
};

    
}  // namespace st

#endif  //_BASE_COW_HASH_MAP_H_
