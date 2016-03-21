// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// External probing hashset/map with reference counting
// Author: gejun@baidu.com
// Date: Dec.27 15:28:29 CST 2010
#pragma once
#ifndef _COW1_HASH_MAP_HPP_
#define _COW1_HASH_MAP_HPP_

#include "utility.hpp"           // std_pair_first
#include "debug.h"               // logging
#include "fun/common_function_objects.hpp"  // f_identity
#include "fun/st_hash.h"          // f_hash
#include "fun/compare.hpp"       // Equal
#include "rc_memory_pool.h"      // RCMemoryPool
#include "boost/shared_ptr.hpp"  // shared_ptr<1>
#include <sstream>               // ostringstream

namespace st {
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
// the corresponding reference counter (and dereference when it does not
// need the item). With the help of these counters, pointer joining is
// much safer.
template <typename _Item, 
          class _GetKey = f_identity<_Item>, 
          class _Hash = Hash<TCAP(ReturnType, _GetKey, _Item)>, 
          class _Equal = Equal<TCAP(ReturnType, _GetKey, _Item)>, 
          class _Alloc = RCMemoryPool>
class Cow1HashSet;

// wrapper over Cow1HashSet to provide normal interfaces as a hash_map
template <typename _Key, 
          typename _Value, 
          class _Hash = Hash<_Key>, 
          class _Equal = Equal<_Key>, 
          class _Alloc = RCMemoryPool, 
          class _Pair = std::pair<_Key, _Value>, 
          class _Base = Cow1HashSet<_Pair, std_pair_first<_Pair>, 
                                   _Hash, _Equal, _Alloc> >
class Cow1HashMap;

// Iterate Cow1HashSet/Map
// Params:
//   _F  filtering function object accepting _Item and returning true for keep,
//       false for skip
template <class _Map> class Cow1HashIterator;

class Cow1HashSetTag {};
class Cow1HashMapTag {};

template <typename _Cont>
struct is_tagged_cow1_hash {
    static const bool R =
        CAP(c_same, Cow1HashSetTag, class _Cont::Tag)
        || CAP(c_same, Cow1HashMapTag, class _Cont::Tag);
};

const u_int CHS_MIN_NBUCKET = 32;          // minimum number of buckets
const u_int CHS_DEFAULT_NBUCKET = 1543;    // default number of buckets
const u_int CHS_MIN_LOAD_FACTOR = 10;      // minimum load factor
const u_int CHS_MAX_LOAD_FACTOR = 100;     // maximum load factor
const u_int CHS_DEFAULT_LOAD_FACTOR = 80;  // default load factor

// interfaces of Cow1HashSet    
template <typename _Item, class _GetKey, class _Hash,
          class _Equal, class _Alloc>
class Cow1HashSet {
template <class _Map> friend class Cow1HashIterator;
    // this is external probing hash mapping, items are stored in
    // separate nodes managed by a pool
    struct Node {
        _Item item_;
        Node* p_next_;
    };

    static const size_t ITEM_OFFSET_IN_NODE = offsetof(Node, item_);

public:
    typedef Cow1HashSetTag Tag;                        // tag this class
    typedef _Item Item;                               // type of stored item
    typedef TCAP(ReturnType, _GetKey, _Item) Key;  // type of key
    typedef _Alloc Alloc;                             // type of allocator
    typedef Cow1HashIterator<Cow1HashSet> iterator;     // type of iterator

    // Default constructor
    explicit Cow1HashSet ()
        : n_item_(0)
        , n_bucket_(CHS_DEFAULT_NBUCKET)
        , ap_entry_(NULL)
        , load_factor_(CHS_DEFAULT_LOAD_FACTOR)
        , alloc_creator_(false)
    {}

    // Initialize this set, must be called before using, except that
    // assignment(operator=) to an uninitialized set also does initialization
    // Params:
    //   n_bucket    number of initial buckets
    //   load_factor a number inside [CHS_MIN_LOAD_FACTOR, CHS_MAX_LOAD_FACTOR]
    //               to indicate the intended ratio (multiplying 100) between
    //               number of items and buckets (to provide reasonable 
    //               performance)
    //   p_alloc     where chaining nodes are from, if you want multiple maps
    //               to share chaining nodes, you have to make sure that they
    //               use the same allocator, otherwise memory-wise sharing is
    //               disabled.
    // Returns: success or not
    int init (u_int n_bucket         = CHS_DEFAULT_NBUCKET,
              u_int load_factor      = CHS_DEFAULT_LOAD_FACTOR,
              const _GetKey& get_key = _GetKey(),
              const _Hash& hash      = _Hash(),
              const _Equal& eq       = _Equal());
    
    // Destroy this mapping and release internal resources
    ~Cow1HashSet ()
    {
        clear();
        ST_DELETE_ARRAY(ap_entry_);
    }

    // Construct from another Cow1HashSet. Triggered by "Cow1HashSet x = y; " 
    // (where y is typed Cow1HashSet). It's hard to decide sharing allocator
    // or not during a copy-construction. Sharing is not thread-safe while 
    // not-sharing disables copy-on-write. We take a flexible solution here:
    // If the source map has local allocator, the allocator is not shared;
    // otherwise the allocator is shared
    Cow1HashSet (const Cow1HashSet& rhs);

    // Assign from another Cow1HashSet. Triggered by "x = y; " (where x and y
    // are both typed Cow1HashSet). If "init" is not called, this function 
    // works exactly as copy-constructor; else if allocators are same, this
    // function unifies number of buckets to improve sharing ratio; 
    // otherwise, this function works as a normal assignment: conservatively
    // decide if the entry array needs to be resized, and copy all items 
    // from source to destination.
    // 
    // Returns: *this to chain invocations
    Cow1HashSet& operator= (const Cow1HashSet& rhs);
        
    // Swap contents of two instances
    void swap (Cow1HashSet & rhs);

    // Reserve number of buckets
    void reserve (const u_int n_bucket) { resize(n_bucket); }
        
    // Insert an item into Cow1HashSet
    // Returns: address of the inserted item, NULL means insertion was failed
    _Item* insert (const _Item& item);

    // Erase the value matching a key from Cow1HashSet
    // Returns: erased or not
    bool erase (const Key& key);

    // Erase the item matching key of the given item
    // Returns: erased or not
    bool erase_by_item (const _Item& item) { return erase(_GetKey()(item)); }

    // Erase all items
    void clear ();
        
    // Search for the item matching a key
    // Returns: address of the item
    inline _Item* seek (const Key& key) const;

    // Search the item with same key of the given item
    // Returns: address of the item
    _Item* seek_by_item (const _Item& item) const
    { return seek(_GetKey()(item)); }

    // Change number of buckets
    // Params:
    //   n_bucket2  intended number of buckets
    // Returns: resized or not
    bool resize (u_int n_bucket2);

    // Get beginning position of this mapping
    iterator begin () const { return iterator(ap_entry_, this); }

    // Get ending position of this mapping
    iterator end () const { return iterator(ap_entry_+n_bucket_, this); }

    // Get referenced count of an item in this map
    static bool is_local_item (const _Item* p_item)
    { return _Alloc::is_local((char*)p_item - ITEM_OFFSET_IN_NODE); }

    // Know initialized or not
    bool not_init () const { return NULL == ap_entry_; }
        
    // Know empty or not
    bool empty () const { return 0 == n_item_; }
        
    // Get number of items
    size_t size () const { return n_item_; }

    // Get number of buckets
    size_t bucket_num () const { return n_bucket_; }

    // Get memory taken by this instance
    size_t mem () const
    {
        return sizeof(*this)
            + (sizeof(Node*) * n_bucket_)
            + (alloc_creator_ ? sp_alloc_->mem() : 0);
    }

    // Get internal array storing entries of buckets
    Node* const* bucket_array () const { return ap_entry_; }

    // Get pointer to internal allocator
    const _Alloc* alloc () const { return sp_alloc_.get(); }

    // Number of users of the allocator
    long alloc_use_count ()  const { return sp_alloc_.use_count(); }

    // Is this map creator of the allocator
    bool alloc_creator () const { return alloc_creator_; }

    // Get max/min/average length of chains
    // input pointers must be non-NULL
    void bucket_info (size_t* p_max_len, size_t* p_min_len,
                      double* p_avg_len) const;

    // Get the actual load factor
    u_int load_factor () const { return n_item_ * 100 / n_bucket_; }

    // Print internal info to StringWriter
    void to_string (StringWriter& sw) const;
        
private:
    void chain_reference (Node* p_begin, Node* p_end)
    {
        for (; p_begin != p_end; p_begin = p_begin->p_next_) {
            _Alloc::inc_ref(p_begin);
        }
    }

    void chain_dereference (Node* p_begin, Node* p_end)
    {
        while (p_begin != p_end) {
            Node* p_node = p_begin;
            p_begin = p_begin->p_next_;
            _Alloc::dec_ref(p_node, sp_alloc_.get());
        }
    }

    Node* chain_copy (Node* p_begin, Node* p_end)
    {
        Node* p_head = NULL;
        Node** pp_prior_next = &p_head;
        for (; p_begin != p_end; p_begin = p_begin->p_next_) {
            Node* p_node = sp_alloc_->template alloc_object<Node>();
            p_node->item_ = p_begin->item_;
            p_node->p_next_ = NULL;
            *pp_prior_next = p_node;
            pp_prior_next = &(p_node->p_next_);
        }
        return p_head;
    }

    bool resize_if_necessary ()
    {
        if ((n_bucket_ * load_factor_) < (n_item_ * 100)) {
            // ratios between neighbor primes in utility.hpp/find_next_prime
            // are generally near but less than 2, using 190 rather than 200
            // here guarantees primes in the sequence are not skipped.
            return resize(n_item_ * 190 / load_factor_);
        }
        return false;
    }

    u_int n_item_;                        // number of items
    u_int n_bucket_;                      // number of buckets
    Node** ap_entry_;                     // array of buckets
    boost::shared_ptr<_Alloc> sp_alloc_;  // pointer to allocator
    u_int load_factor_;                   // 100*n_item_/n_bucket_
    bool alloc_creator_;                  // is this creator of sp_alloc_
    _GetKey get_key_;                     // get key from _Item
    _Hash hash_;                          // compute hash value from Key
    _Equal eq_;                           // test equivalence between keys
};

// -------------------
//   Implementations
// -------------------

// Wrap over Cow1HashSet to provide normal interfaces as a hash_map
template <typename _Key, typename _Value, class _Hash, class _Equal,
          class _Alloc, class _Pair, class _Base>
class Cow1HashMap : public _Base {
public:
    typedef Cow1HashMapTag Tag;     // Tag this class
        
    // Insert the pair of key and value into the map
    _Pair* insert (const _Key& key, const _Value& value)
    { return _Base::insert(_Pair(key, value)); }

    // Seek the map with a key
    // Returns: pointer of the value rather than _Item* as in _Base::seek(1)
    _Value* seek_value (const _Key& key) const
    {
        _Pair * p_pair = _Base::seek(key);
        return p_pair ? &(p_pair->second) : NULL;
    }
};


template <class _Map> class Cow1HashIterator {
public:
    typedef typename _Map::Item value_type;
    
    // Default constructor
    Cow1HashIterator () : p_node_(NULL), pp_entry_(NULL), pp_entry_end_(NULL)
    {}

    // Construct from a position in a map
    Cow1HashIterator (typename _Map::Node* const* pp_entry,
                     const _Map* p_this)
        : pp_entry_(pp_entry)
        , pp_entry_end_(p_this->bucket_array() + p_this->bucket_num())
    { find_valid_node (); }

    // *this == rhs
    bool operator== (const Cow1HashIterator& rhs) const
    { return p_node_ == rhs.p_node_; }

    // *this != rhs
    bool operator!= (const Cow1HashIterator& rhs) const
    { return p_node_ != rhs.p_node_; }
        
    // ++ it
    Cow1HashIterator& operator++ ()
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
    Cow1HashIterator operator++ (int)
    {
        Cow1HashIterator tmp = *this;
        this->operator++();
        return tmp;
    }

    // *it
    value_type& operator* () const { return p_node_->item_; }

    // it->
    value_type* operator-> () const { return &(p_node_->item_); }

    // ugly stuff
    bool not_end () const { return p_node_ != NULL; }
    void set_end () { p_node_ = NULL; }

    void to_string (StringWriter& sw) const
    {
        sw << "Cow1HashIter{p_item=" << (p_node_ ? &(p_node_->item_) : NULL)
           << "}";
    }
    
private:
    void find_valid_node ()
    {
        for ( ; pp_entry_ != pp_entry_end_; ++pp_entry_) {
            if (NULL != *pp_entry_) {
                p_node_ = *pp_entry_;
                return;
            }
        }
        p_node_ = NULL;
    }
            
    typename _Map::Node* p_node_;
    typename _Map::Node* const* pp_entry_;
    typename _Map::Node* const* pp_entry_end_;
};

// init
template <typename _Item, class _GetKey, class _Hash,
          class _Equal, class _Alloc>
int
Cow1HashSet<_Item, _GetKey, _Hash, _Equal, _Alloc>::
init (u_int n_bucket, u_int load_factor,
      const _GetKey& get_key, const _Hash& hash, const _Equal& eq)
{
    get_key_ = get_key;
    hash_ = hash;
    eq_ = eq;

    if (NULL != ap_entry_) {
        ST_FATAL("init an initialized map"
                  ", force destruction first");
        this->~Cow1HashSet();
    }
            
    n_bucket_ = find_near_prime
        ((n_bucket < CHS_MIN_NBUCKET) ? CHS_MIN_NBUCKET : n_bucket);
            
    load_factor_ = (load_factor < CHS_MIN_LOAD_FACTOR)
        ? CHS_MIN_LOAD_FACTOR
        : ((load_factor > CHS_MAX_LOAD_FACTOR)
           ? CHS_MAX_LOAD_FACTOR
           : load_factor);

    sp_alloc_.reset(ST_NEW(_Alloc, sizeof(Node)));
    if (NULL == sp_alloc_.get()) {
        ST_FATAL("Fail to new allocator");
        return ENOMEM;
    }
    alloc_creator_ = true;

    n_item_ = 0;
    ap_entry_ = ST_NEW_ARRAY(Node*, n_bucket_);
    if (NULL == ap_entry_) {
        ST_FATAL("Fail to new ap_entry_");
        return ENOMEM;
    }
    memset(ap_entry_, 0, sizeof(Node*) * n_bucket_);
    return 0;
}

// copy construct
template <typename _Item, class _GetKey, class _Hash,
          class _Equal, class _Alloc>
Cow1HashSet<_Item, _GetKey, _Hash, _Equal, _Alloc>::
Cow1HashSet (const Cow1HashSet<_Item, _GetKey, _Hash, _Equal, _Alloc>& rhs)
{
    if (rhs.not_init()) {
        ST_FATAL("source is not initialized");
        // and we keep ourself uninitialized
        return;
    }
            
    ap_entry_ = ST_NEW_ARRAY(Node*, rhs.n_bucket_);
    if (NULL == ap_entry_) {
        ST_FATAL("Fail to new ap_entry_");
        return;
    }
            
    sp_alloc_ = rhs.sp_alloc_;
    alloc_creator_ = false;
    for (size_t i=0; i<rhs.n_bucket_; ++i) {
        ap_entry_[i] = rhs.ap_entry_[i];
        chain_reference(ap_entry_[i], NULL);
    }

    n_bucket_ = rhs.n_bucket_;
    load_factor_ = rhs.load_factor_;
    n_item_ = rhs.n_item_;
}

// operator=
template <typename _Item, class _GetKey, class _Hash,
          class _Equal, class _Alloc>
Cow1HashSet<_Item, _GetKey, _Hash, _Equal, _Alloc>&
Cow1HashSet<_Item, _GetKey, _Hash, _Equal, _Alloc>::
operator= (const Cow1HashSet<_Item, _GetKey, _Hash, _Equal, _Alloc>& rhs)
{
    // load_factor is not changed in this function
            
    if (&rhs == this) {  // assign to self, we do nothing here
        return *this;
    }
            
    if (not_init()) {
        // just call copy-constructor
        ST_NEW_ON(this, Cow1HashSet, rhs);
        return *this;
    }
            
    if (rhs.not_init()) {  // source is not initialized 
        this->~Cow1HashSet();  // destroy self
        return *this;
    }

    if (sp_alloc_ == rhs.sp_alloc_) {
        // sharing alloctor
        // making numbers of buckets same improves sharing ratio
        if (n_bucket_ != rhs.n_bucket_) {
            Node** ap_entry2 = ST_NEW_ARRAY(Node*, rhs.n_bucket_);
            if (NULL == ap_entry2) {
                ST_FATAL("Fail to new ap_entry2");
                return *this;
            }

            clear();
                    
            delete [] ap_entry_;
            ap_entry_ = ap_entry2;
                    
            n_bucket_ = rhs.n_bucket_;
                    
            for (size_t i=0; i<n_bucket_; ++i) {
                ap_entry_[i] = rhs.ap_entry_[i];
                chain_reference(ap_entry_[i], NULL);
            }
        } else {
            for (size_t i=0; i<n_bucket_; ++i) {
                if (ap_entry_[i] != rhs.ap_entry_[i]) {
                    chain_dereference(ap_entry_[i], NULL);
                    ap_entry_[i] = rhs.ap_entry_[i];
                    chain_reference(ap_entry_[i], NULL);
                }
            }
        }
        n_item_ = rhs.n_item_;
    } else {
        // separate allocators, we should be conservative with memory
        if ((n_bucket_ * load_factor_) <= (rhs.n_item_ * 100)) {
            u_int n_bucket2 = find_near_prime
                (rhs.n_item_ * 100 / load_factor_);
            Node** ap_entry2 = ST_NEW_ARRAY(Node*, n_bucket2);
            if (NULL == ap_entry2) {
                ST_FATAL("Fail to new ap_entry2");
                return *this;
            }
            memset(ap_entry2, 0, sizeof(Node*) * n_bucket2);

            clear();
                    
            delete [] ap_entry_;
            ap_entry_ = ap_entry2;
            n_bucket_ = n_bucket2;
        } else {
            clear();
        }

        for (iterator it=rhs.begin(), it_e=rhs.end();
             it != it_e;
             ++it) {
            insert(*it);
        }
    }
            
    return *this;
}

template <typename _Item, class _GetKey, class _Hash,
          class _Equal, class _Alloc>
void
Cow1HashSet<_Item, _GetKey, _Hash, _Equal, _Alloc>::
swap (Cow1HashSet& rhs)
{
    std::swap(rhs.n_item_, n_item_);
    std::swap(rhs.n_bucket_, n_bucket_);
    std::swap(rhs.ap_entry_, ap_entry_);
    rhs.sp_alloc_.swap(sp_alloc_);
    std::swap(rhs.load_factor_, load_factor_);
    std::swap(rhs.alloc_creator_, alloc_creator_);
    std::swap(rhs.get_key_, get_key_);
    std::swap(rhs.hash_, hash_);
    std::swap(rhs.eq_, eq_);
}


template <typename _Item, class _GetKey, class _Hash,
          class _Equal, class _Alloc>
_Item*
Cow1HashSet<_Item, _GetKey, _Hash, _Equal, _Alloc>::
insert (const _Item& item)
{
    const Key key = get_key_(item);
    u_int bkt = hash_(key) % n_bucket_;

    // find equal item
    Node* p_item_node = ap_entry_[bkt];
    while (NULL != p_item_node && !eq_(key, get_key_(p_item_node->item_))) {
        p_item_node = p_item_node->p_next_;
    }

    if (NULL == p_item_node) {
        // no equal item, append a new node to the bucket
        if (resize_if_necessary()) {
            bkt = hash_(key) % n_bucket_;  // re-calculate bucket
        }
        Node* p_head_node = sp_alloc_->template alloc_object<Node>();
        p_head_node->item_ = item;
        p_head_node->p_next_ = ap_entry_[bkt];
        ap_entry_[bkt] = p_head_node;
        ++ n_item_;
        return &(p_head_node->item_);
    }

    // p_item_node->item_ equal item
    if (_Alloc::is_local(p_item_node)) {
        // local node, modify item_ in-place
        p_item_node->item_ = item;
        return &(p_item_node->item_);
    }
    // p_item_node is shared

    // insert the new node
    Node* p_new_node = sp_alloc_->template alloc_object<Node>();
    p_new_node->item_ = item;
    p_new_node->p_next_ = ap_entry_[bkt];
    Node** pp_prior_next = &(p_new_node->p_next_);

    // find first shared
    Node* p_shared_node = ap_entry_[bkt];
    while (p_shared_node != p_item_node && _Alloc::is_local(p_shared_node)) {
        pp_prior_next = &(p_shared_node->p_next_);
        p_shared_node = p_shared_node->p_next_;
    }

    // copy until p_item_node
    while (p_shared_node != p_item_node) {
        Node* p_cow1_node = sp_alloc_->template alloc_object<Node>();
        p_cow1_node->item_ = p_shared_node->item_;
        // p_shared_node->n_ref_ must be >= 2
        _Alloc::dec_ref(p_shared_node, sp_alloc_.get()); 
                
        *pp_prior_next = p_cow1_node;
        pp_prior_next = &(p_cow1_node->p_next_);

        p_shared_node = p_shared_node->p_next_;
    }
            
    // skip p_item_node
    *pp_prior_next = p_item_node->p_next_;
    // p_item_node->n_ref_ must be >=2
    _Alloc::dec_ref(p_item_node, sp_alloc_.get());
            
    ap_entry_[bkt] = p_new_node;

    return &(p_new_node->item_);
}

template <typename _Item, class _GetKey, class _Hash,
          class _Equal, class _Alloc>
bool
Cow1HashSet<_Item, _GetKey, _Hash, _Equal, _Alloc>::
erase (const Key& key)
{
    const u_int bkt = hash_(key) % n_bucket_;
    Node* p_item_node = ap_entry_[bkt];
    Node** pp_prior_next = &ap_entry_[bkt];
    while (NULL != p_item_node && !eq_(key, get_key_(p_item_node->item_))) {
        pp_prior_next = &(p_item_node->p_next_);
        p_item_node = p_item_node->p_next_;
    }

    if (NULL == p_item_node) {  // no equal item
        return false;
    }

    // p_item_node equal item
    if (_Alloc::is_local(p_item_node)) {
        // local node, skip and deallocate p_item_node
        // all nodes before p_item_node must be local and safe to edit
        *pp_prior_next = p_item_node->p_next_; 
        _Alloc::dec_ref(p_item_node, sp_alloc_.get());
        -- n_item_;
        return true;
    }

    // p_item_node is shared
    // find first shared
    // we don't want to change ap_entry_ directly
    Node* p_head_node = ap_entry_[bkt];
    pp_prior_next = &p_head_node;
    Node* p_shared_node = ap_entry_[bkt];
    while (p_shared_node != p_item_node && _Alloc::is_local(p_shared_node)) {
        pp_prior_next = &(p_shared_node->p_next_);
        p_shared_node = p_shared_node->p_next_;
    }

    // copy until p_item_node
    while (p_shared_node != p_item_node) {
        Node* p_cow1_node = sp_alloc_->template alloc_object<Node>();
        p_cow1_node->item_ = p_shared_node->item_;
        // p_shared_node->n_ref_ must be >= 2
        _Alloc::dec_ref(p_shared_node, sp_alloc_.get());
                
        *pp_prior_next = p_cow1_node;
        pp_prior_next = &(p_cow1_node->p_next_);

        p_shared_node = p_shared_node->p_next_;
    }
    // skip p_item_node
    *pp_prior_next = p_item_node->p_next_;
    // p_item_node->n_ref_ must be >= 2
    _Alloc::dec_ref(p_item_node, sp_alloc_.get()); 
    -- n_item_;
            
    ap_entry_[bkt] = p_head_node;
    return true;
}

template <typename _Item, class _GetKey, class _Hash,
          class _Equal, class _Alloc>
void
Cow1HashSet<_Item, _GetKey, _Hash, _Equal, _Alloc>::
clear ()
{
    if (NULL != ap_entry_) {
        for (size_t i=0; i<n_bucket_; ++i) {
            chain_dereference(ap_entry_[i], NULL);
            ap_entry_[i] = NULL;
        }
    }
    n_item_ = 0;
}

template <typename _Item, class _GetKey, class _Hash,
          class _Equal, class _Alloc>
_Item*
Cow1HashSet<_Item, _GetKey, _Hash, _Equal, _Alloc>::
seek (const Key& key) const
{
    const u_int bkt = hash_(key) % n_bucket_;
    Node* p_item_node = ap_entry_[bkt];
    while (NULL != p_item_node) {
        if (eq_(key, get_key_(p_item_node->item_))) {
            return &(p_item_node->item_);
        }
        p_item_node = p_item_node->p_next_;
    }
    return NULL;
}

template <typename _Item, class _GetKey, class _Hash,
          class _Equal, class _Alloc>
bool
Cow1HashSet<_Item, _GetKey, _Hash, _Equal, _Alloc>::
resize (u_int n_bucket2)
{            
    n_bucket2 = find_near_prime(n_bucket2);
            
    if (n_bucket_ == n_bucket2) {
        return false;
    }

    {   // Resizing is rare so using ostringstream is OK
        std::ostringstream oss;
        oss << c_show(Key) << "->" << c_show(_Item);
        ST_WARN("%s resized from %u to %u",
                oss.str().c_str(), n_bucket_, n_bucket2);
    }
            
    Node** ap_entry2 = ST_NEW_ARRAY(Node*, n_bucket2);
    if (NULL == ap_entry2) {
        ST_FATAL("Fail to new ap_entry2");
        return false;
    }
    memset(ap_entry2, 0, sizeof(Node*)*n_bucket2);

    if (ap_entry_) {
        for (size_t i=0; i<n_bucket_; ++i) {
            Node* p_node = ap_entry_[i];
            while (NULL != p_node) {
                Node* p_next_node = p_node->p_next_;
                    
                const u_int bkt = hash_(get_key_(p_node->item_)) % n_bucket2;
                if (_Alloc::is_local(p_node)) {
                    // ancestors of a local node must be all local, 
                    // it's safe to move local node anywhere
                    p_node->p_next_ = ap_entry2[bkt];
                    ap_entry2[bkt] = p_node;
                } else {  // p_node is shared
                    if (NULL == ap_entry2[bkt] && NULL == p_node->p_next_) {
                        // since we choose prime number as n_bucket, items are
                        // hardly hashed to same bucket (in the sense of offset)
                        // with different numbers of bucket. if that happens,
                        // (hash(key1)-hash(key2)) divides bucket1*bucket2
                        // (generally 2*bucket1^2 when Cow1HashSet grows), which
                        // is unlikely. so we take simple solution here, if
                        // next of the node and new_entry are both NULL, the
                        // node can be shared; otherwise the node is copied
                        ap_entry2[bkt] = p_node;
                    } else {
                        Node* p_cow1_node =
                            sp_alloc_->template alloc_object<Node>();
                        p_cow1_node->item_ = p_node->item_;
                        p_cow1_node->p_next_ = ap_entry2[bkt];
                        ap_entry2[bkt] = p_cow1_node;
                        _Alloc::dec_ref(p_node, sp_alloc_.get());
                    }
                }

                p_node = p_next_node;
            }
            //ap_entry_[i] = NULL;
        }
        delete [] ap_entry_;
    }

    n_bucket_ = n_bucket2;
    ap_entry_ = ap_entry2;
            
    return true;
}

template <typename _Item, class _GetKey, class _Hash,
          class _Equal, class _Alloc>
void
Cow1HashSet<_Item, _GetKey, _Hash, _Equal, _Alloc>::
bucket_info (size_t* p_max_len, size_t* p_min_len, double* p_avg_len) const
{
    if (not_init()) {
        *p_max_len = 0;
        *p_min_len = 0;
        *p_avg_len = 0;
        return;
    }
            
    size_t max_len=0, min_len=UINT_MAX, sum_len=0;
    size_t n_non_empty = 0;
    for (size_t bkt = 0; bkt < n_bucket_; bkt++) {
        Node* pp_entry = ap_entry_[bkt];
        if (NULL != pp_entry) {
            ++ n_non_empty;
            size_t len=1;
            for (Node* p_node = pp_entry->p_next_
                     ; p_node
                     ; p_node=p_node->p_next_, ++len);
            if (len > max_len) {
                max_len = len;
            }
            if (len < min_len) {
                min_len = len;
            }
            sum_len += len;
        }
    }

    *p_max_len = n_non_empty > 0 ? max_len : 0;
    *p_min_len = n_non_empty > 0 ? min_len : 0;
    *p_avg_len = n_non_empty > 0 ? (sum_len/(double)n_non_empty) : 0;
}

template <typename _Item, class _GetKey, class _Hash,
          class _Equal, class _Alloc>
void
Cow1HashSet<_Item, _GetKey, _Hash, _Equal, _Alloc>::
to_string (StringWriter& sw) const
{
    size_t max_len, min_len;
    double avg_len;
    bucket_info(&max_len, &min_len, &avg_len);

    ostringstream oss;
    oss << c_show(Key) << "->" << c_show(_Item);
    
    sw << "Hash{" << oss.str()
       << " n_item/bkt=" << n_item_ << '/' << n_bucket_
       << " max/min/avg=" << max_len << "/" << min_len << "/" << avg_len
       << " mem=" << mem()
        ;
    
    sw << " alloc=" << sp_alloc_.use_count()
       << '/' << (void*)sp_alloc_.get();
    if (alloc_creator_) {
        sw << '/' << sp_alloc_.get();
    }
    
    // if (!not_init()) {
    //     sw << " items=";
    //     shows_range(sw, begin(), end());
    // }
    sw << "}";
}

// Print type of this container
template <typename _Item, class _GetKey, class _Hash,
          class _Equal, class _Alloc>
struct c_show_impl<Cow1HashSet<_Item, _GetKey, _Hash, _Equal, _Alloc> > {
    static void c_to_string (std::ostream& os)
    {
        os << "Cow1HashSet(" << c_show(_Item)
           << ", " << c_show(TCAP(ReturnType, _GetKey, _Item))
           << ")";
    }
};
    
}

#endif  //_COW1_HASH_MAP_HPP_
