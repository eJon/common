// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// I'M NOT COMPLETED, DONT USE ME UNTIL THIS LINE IS REMOVED
// Author: gejun@baidu.com
// Date: Thu Nov 25 14:07:39 CST 2010
#pragma once
#ifndef _DODGE_HASH_MAP_HPP_
#define _DODGE_HASH_MAP_HPP_

#include "st_hash.h"                     // f_hash
#include "compare.hpp"                  // Equal
#include "common.h"                     // Logging and StringWriter
#include "st_utility.h"                 // find_near_prime

namespace st {
const uint32_t CHM_MIN_CAP = 1024;
const uint32_t CHM_DEFAULT_LOAD_FACTOR = 95;
const uint32_t CELLAR_RATIO = 20;

// mark unused nodes
enum DodgeHashMapNodeStat {
    DHM_EMPTY = 0,
    DHM_HASHED = 1,
    DHM_CHAINED = 2
};

template <class _Map> class DodgeHashMapIterator {
    typedef class _Map::Node Node;    
public:
    DodgeHashMapIterator () : this_(NULL), idx_(0) {}
                              
    DodgeHashMapIterator (const _Map* p_map, uint32_t idx)
        : idx_(idx)
        , this_(p_map)
    { skip_null (); }
    
    const Node& operator* () const { return this_->a_node_[idx_]; }
    const Node* operator-> () const { return &(this_->a_node_[idx_]); }
        
    bool operator!= (const DodgeHashMapIterator& other) const
    { return idx_ != other.idx_; }

    // test equivalence of two iterators
    bool operator== (const DodgeHashMapIterator& other) const
    { return idx_ == other.idx_; }

    // move iterator ahead for one step
    DodgeHashMapIterator& operator++ ()
    {
        //ST_TRACE ("go from %u to %u", idx_, this_->a_node_[idx_].next);
        ++ idx_;
        skip_null();
        return *this;
    }

private:
    void skip_null ()
    {
        const uint32_t c = this_->cap_ + this_->cellar_cap_;
        for ( ; idx_ < c && DHM_EMPTY == this_->a_node_[idx_].stat; ++ idx_);
    }
            
    uint32_t idx_;
    const _Map* this_;
};

template <typename _Key, typename _Value>
struct DodgeHashMapNode {
    uint32_t free : 1;   // in free_list_ or not
    uint32_t stat : 2;   // NodeType
    uint32_t next : 29;  // 2^29 = 0.5 billion nodes
    _Key key;
    _Value value;

    // pretty print this node
    void to_string(StringWriter& sw) const
    {
        if (free) {
            sw << '\'';
        }
        if (DHM_HASHED == stat) {
            sw << "(" << key << "," << value << "," << next << ")"; 
        } else if (DHM_EMPTY == stat) {
            sw << "(unused)";
        } else {  // CHAINED
            sw << "[" << key << "," << value << "," << next << "]"; 
        }
    }
};


// dodge hash map
template <typename _Key,
          typename _Value,
          typename _Hash = Hash<_Key>,
          typename _Equal = Equal<_Key> >
class DodgeHashMap {
template <class> friend class DodgeHashMapIterator;
    
public:
    typedef DodgeHashMapNode<_Key, _Value> Node;
    typedef DodgeHashMapIterator<DodgeHashMap> const_iterator;
    typedef const_iterator iterator;

    // Default constructor
    DodgeHashMap()
        : cap_(0)
        , load_factor_(0)
        , free_idx_(0)
        , n_item_(0)
        , a_node_(NULL)
    {}
        
    // Destroy this map
    ~DodgeHashMap()
    {
        if (NULL != a_node_) {
            delete [] a_node_;
            a_node_ = NULL;
        }
    }

    DodgeHashMap (const DodgeHashMap& rhs);
    
    DodgeHashMap& operator= (const DodgeHashMap& rhs);

        
    // Init this hashmap
    // Params:
    //   cap         rounded power of 2
    //   load_factor if element_count*100 > cap*load_factor, resizing occurs
    // Returns:
    //   0       success
    //   ENOMEM  fail to new a_node_
    int init (uint32_t cap = CHM_MIN_CAP,
              uint32_t load_factor = CHM_DEFAULT_LOAD_FACTOR);
    
    // Swap internal data with another instance, after swapping,
    // both instances do not change their storage addresses while
    // data are exchanged, this is useful for resizing
    void swap (DodgeHashMap& rhs);
        
    // Insert an element
    // Returns:
    //   true   the key does not exist
    //   false  the key exists and value gets updated
    bool insert (const _Key& key, const _Value& value);

    // Erase an element
    // Returns:
    //   true   a node is erased
    //   false  no node is erased
    bool erase (const _Key& key);

    // Seek an element
    // Returns: storage pointer to the value, NULL for not found
    inline _Value* seek (const _Key& key) const;
    
    // Erase all elements
    void clear ();

    const_iterator begin () const { return const_iterator(this, 0); }
    const_iterator end () const
    { return const_iterator(this, cap_ + cellar_cap_); }
    
    size_t size () const { return n_item_; }
    uint32_t capacity () const { return cap_; }
    uint32_t load_factor () const { return load_factor_; }

    // memory consumption
    size_t mem () const
    { return sizeof(*this) + (sizeof (Node) * capacity()); }

    // print internal info of this hashmap
    void to_string(StringWriter& sw) const;

    bool resize (uint32_t cap2);
    
private:
    // change the node to be free
    void push_free (const uint32_t idx);

    // get a free node
    uint32_t pop_free ();
    
    bool resize_if_necessary ()
    {
        uint32_t demand_cap = (n_item_+1) * 100 / load_factor_;
        if (demand_cap <= cap_) {
            return false;
        }
        return resize(std::max(demand_cap, static_cast<uint32_t>(cap_*1.95)));
    }

private:
    uint32_t cap_;                         // length of a_node_-1
    uint32_t cellar_cap_;
    // an integer in [1,100] which is percentage of maximum elements
    // stored in buckets comparing to capacity of buckets
    uint32_t load_factor_;
    uint32_t free_idx_;                    // first possibly unused position
    uint32_t n_item_;                      // number of elements
    Node* a_node_;                         // node array
    std::vector<uint32_t> free_list_;      // erased nodes
};

template <typename _Key, typename _Value, typename _Hash, typename _Equal>
int DodgeHashMap<_Key, _Value, _Hash, _Equal>::
init (uint32_t cap, uint32_t load_factor)
{
    if (load_factor <= 0 || load_factor >= 100) {
        ST_FATAL ("param[load_factor] should be an integer inside"
                  " [1,100], use default value %u"
                  , CHM_DEFAULT_LOAD_FACTOR);
        load_factor_ = CHM_DEFAULT_LOAD_FACTOR;
    } else {
        load_factor_ = load_factor;
    }

    cap_ = find_near_prime(cap);
    cellar_cap_ = cap_ * CELLAR_RATIO / 100;
    // last node is reserved to ease iteration
    a_node_ = ST_NEW_ARRAY(Node, cap_ + cellar_cap_); 
    if (NULL == a_node_) {
        ST_FATAL ("Fail to new a_node_");
        return ENOMEM;
    }

    clear();
            
    ST_TRACE ("init done, cap=%u", cap_);
    return 0;
}

template <typename _Key, typename _Value, typename _Hash, typename _Equal>
void DodgeHashMap<_Key, _Value, _Hash, _Equal>::
swap (DodgeHashMap& rhs)
{
    std::swap(cap_, rhs.cap_);
    std::swap(cellar_cap_, rhs.cellar_cap_);
    std::swap(load_factor_, rhs.load_factor_);
    std::swap(free_idx_, rhs.free_idx_);
    std::swap(n_item_, rhs.n_item_);
    std::swap(a_node_, rhs.a_node_);
    std::swap(free_list_, rhs.free_list_);
}

template <typename _Key, typename _Value, typename _Hash, typename _Equal>
bool DodgeHashMap<_Key, _Value, _Hash, _Equal>::
insert (const _Key& key, const _Value& value)
{
    resize_if_necessary();

    _Hash hash;
    _Equal eq;
    uint32_t p0, p;
    uint32_t n;
    uint32_t idx = hash(key) % cap_;
        
    switch (a_node_[idx].stat) {
    case DHM_HASHED:
        //test if the node has a equal key
        if (eq(a_node_[idx].key, key)) {
            // equal, set the value and we're done
            a_node_[idx].value = value;
            return false;  // a node is overwritten
        }
                
        // scan the chain to find if there's an equal node
        p0 = idx;
        p = a_node_[idx].next;
        for ( ; idx != p && !eq(a_node_[p].key, key);
              p0 = p, p = a_node_[p].next);

        if (idx != p) {
            // p has an equal key
            a_node_[p].value = value;
            return false;
        }

        // no equal node, get a free node and append
        // this works for p0==p where the hashing address
        // is not chained and the node is self-dodge
        n = pop_free();
        a_node_[n].stat = DHM_CHAINED;
        a_node_[n].next = idx;
        a_node_[n].key = key;
        a_node_[n].value = value;
        a_node_[p0].next = n;

        //increase counter
        ++ n_item_;
        return true;
    case DHM_EMPTY:
        a_node_[idx].stat = DHM_HASHED;
        a_node_[idx].next = idx;  // reference self
        a_node_[idx].key = key;
        a_node_[idx].value = value;
        ++ n_item_;
        return true;
    case DHM_CHAINED:
        // find parent of this node
        p0 = idx;
        p = a_node_[idx].next;
        for ( ; idx != p; p0 = p, p = a_node_[p].next);

        // move the node to another free node
        n = pop_free();
        a_node_[n].stat = a_node_[idx].stat;
        a_node_[n].next = a_node_[idx].next;
        a_node_[n].key = a_node_[idx].key;
        a_node_[n].value = a_node_[idx].value;
            
        a_node_[p0].next = n;
            
        // change the node to be hashed
        a_node_[idx].stat = DHM_HASHED;
        a_node_[idx].next = idx;  // reference self
        a_node_[idx].key = key;
        a_node_[idx].value = value;

        ++ n_item_;
        return true;
    }

    return true;
}

template <typename _Key, typename _Value, typename _Hash, typename _Equal>
bool DodgeHashMap<_Key, _Value, _Hash, _Equal>::
erase (const _Key& key)
{
    _Hash hash;
    _Equal eq;

    uint32_t idx = hash(key) % cap_;

    if (DHM_HASHED != a_node_[idx].stat) {
        return false;
    }
    // check the first node
    if (eq(a_node_[idx].key, key)) {
        if (idx == a_node_[idx].next) {
            // self referenced
            push_free(idx);
            -- n_item_;
            return true;
        } else {
            // copy next to idx and free next
            uint32_t p = a_node_[idx].next;
                
            a_node_[idx].next = a_node_[p].next;
            a_node_[idx].key = a_node_[p].key;
            a_node_[idx].value = a_node_[p].value;
                
            push_free(p);
            -- n_item_;
            return true;
        }
    } else {
        // walk through the chain to find an equal node
        uint32_t p0 = idx;  // parent node of p
        uint32_t p = a_node_[idx].next;
        for ( ; idx != p && !eq (a_node_[p].key, key);
              p0 = p, p = a_node_[p].next);
        if (idx == p) {
            // no equal node
            return false;
        }
                    
        // skip the to-be-erased node
        a_node_[p0].next = a_node_[p].next;
                
        // append the skipped node to free queue
        push_free(p);

        -- n_item_;
        return true;
    }
}

template <typename _Key, typename _Value, typename _Hash, typename _Equal>
_Value* DodgeHashMap<_Key, _Value, _Hash, _Equal>::
seek (const _Key& key) const
{
    _Hash hash;
    _Equal eq;

    uint32_t idx = hash(key) % cap_;
    Node* p_node = a_node_ + idx;
            
    if (DHM_HASHED == p_node->stat) {
        if (eq(p_node->key, key)) {
            return &(p_node->value);
        }
                
        uint32_t p = p_node->next;
        // walk through the chain
        for ( ; p != idx; p = p_node->next) {
            p_node = a_node_ + p;
            if (eq(p_node->key, key)) {
                return &(p_node->value);
            }
        }
    }
    return NULL;
}

template <typename _Key, typename _Value, typename _Hash, typename _Equal>
void DodgeHashMap<_Key, _Value, _Hash, _Equal>::
clear ()
{
    free_list_.clear();
    free_idx_ = cap_+1;
    n_item_ = 0;        
        
    // chain all nodes
    for (uint32_t i = 0; i < (cap_ + cellar_cap_); ++i) {
        a_node_[i].stat = DHM_EMPTY;
    }
}

template <typename _Key, typename _Value, typename _Hash, typename _Equal>
void DodgeHashMap<_Key, _Value, _Hash, _Equal>::
to_string(StringWriter& sw) const
{
    uint32_t n_hashed = 0, n_chained = 0, n_empty = 0;
    for (uint32_t i=0; i<(cap_+cellar_cap_); ++i) {
        switch (a_node_[i].stat) {
        case DHM_HASHED:
            ++ n_hashed;
            break;
        case DHM_CHAINED:
            ++ n_chained;
            break;
        case DHM_EMPTY:
            ++ n_empty;
            break;
        }
    }
    
    sw << "{cap=" << cap_
       << " cellar=" << cellar_cap_
       << " n=" << n_item_
       << " n_hashed/chained/empty="
       << n_hashed << '/' << n_chained << '/' << n_empty
       << " load_factor=" << load_factor_
       << " free_num=" << free_list_.size()
       << " mem=" << mem()
       << " content=";
    shows_range (sw, begin(), end());
    sw << "}";
}

template <typename _Key, typename _Value, typename _Hash, typename _Equal>
void DodgeHashMap<_Key, _Value, _Hash, _Equal>::
push_free (const uint32_t idx)
{
    if (a_node_[idx].stat == DHM_EMPTY) {
        ST_FATAL("push in a empty node, idx=%u", idx);
    }
        
    a_node_[idx].stat = DHM_EMPTY;
    if (0 == a_node_[idx].free) {
        a_node_[idx].free = 1;
        free_list_.push_back(idx);
    }
}

template <typename _Key, typename _Value, typename _Hash, typename _Equal>
uint32_t DodgeHashMap<_Key, _Value, _Hash, _Equal>::
pop_free ()
{
    // Try free_list_ first
    while (!free_list_.empty()) {
        // pop one node
        uint32_t idx = free_list_.back();
        free_list_.pop_back();

        if (DHM_EMPTY == a_node_[idx].stat) {
            a_node_[idx].free = 0;
            //ST_TRACE("Popped %d", idx);
            return idx;
        }
        // ST_TRACE("Here, free_num=%lu, cap=%u, n=%u",
        //          free_list_.size(), cap_, n_item_);
    }
        
    for ( ; free_idx_ != cap_; ) {
        const uint32_t orig_idx = free_idx_;
        free_idx_ = (free_idx_ + 1) % (cap_ + cellar_cap_);
        
        if (a_node_[orig_idx].stat == DHM_EMPTY) {
            a_node_[orig_idx].free = 0;
            return orig_idx;
        }
    }
        
    ST_FATAL("Impossible");
    return cap_;
}

template <typename _Key, typename _Value, typename _Hash, typename _Equal>
bool DodgeHashMap<_Key, _Value, _Hash, _Equal>::
resize (uint32_t cap2)
{
    DodgeHashMap tmp;
    if (tmp.init(cap2, load_factor_) < 0) {
        return false;
    }
    
    uint32_t c = 0;
    ST_TRACE ("before resizing");
    for (const_iterator it=begin(); it != end(); ++it, ++c) {
        //ST_TRACE ("insert %u -> %u", it->key, it->value);
        tmp.insert (it->key, it->value);
    }
    if (c != size()) {
        ST_FATAL ("c=%u, size=%lu", c, size());
    }

    swap(tmp);
    return true;
}


}  // namespace st

#endif  //_DODGE_HASH_MAP_HPP_
