// Copyright(c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Map keys to clusters which are groups of items ordered by another key
// Author: gejun@baidu.com
// Date: Wed Aug 11 14:57:47 2010
#pragma once
#ifndef _COW_HASH_CLUSTER_SET_HPP_
#define _COW_HASH_CLUSTER_SET_HPP_

#include "cow_hash_cluster_set.hpp"

namespace st {

// Wrap over CowHashClusterSet to provide commonly key-value style interfaces
template <typename _HKey,
          typename _CKey,
          typename _CVal,
          class _Cmp = Compare<_CKey>,
          u_int _MAX_FANOUT = COWBASS_DEFAULT_FANOUT,
          class _Hash = Hash<_HKey>,
          class _Equal = Equal<_HKey> >
class CowHashClusterMap {
public:
    typedef std::pair<_CKey, _CVal> HVal;
    typedef _HKey HKey;
    typedef _CKey CKey;
    typedef _CVal CVal;
    typedef std::pair<_HKey, HVal> Item;

    // TODO: We can't make a HashClusterMap work similar with a hashmap now
    class Reference {
    public:
        Reference(const HKey&, const HVal& hval)
            : cval_(hval.second) {}

        operator CVal() const { return cval_; }
        
    private:
        const CVal& cval_;
    };

    class Pointer {
    public:
        Pointer() : p_cval_(NULL) {}
        
        Pointer(const HKey*, const HVal* p_hval)
            : p_cval_(p_hval ? &(p_hval->second) : NULL) {}
        
        // const CVal& operator*() const { return *p_cval_; }
        // const CVal* operator->() const { return p_cval_; }
        operator const CVal*() const { return p_cval_; }
        
    private:
        const CVal* p_cval_;
    };

    typedef CowHashClusterSet<Item,
                              Pointer,
                              Reference,
                              std_pair_first<Item>,
                              std_pair_second<Item>,
                              std_pair_first<HVal>,
                              _Cmp,
                              _MAX_FANOUT,
                              _Hash,
                              _Equal> Base;

    typedef typename Base::Cluster Cluster;
    typedef typename Base::HMap HMap;
    typedef typename Base::Iterator Iterator;
    typedef typename Base::MapAlloc MapAlloc;
    typedef typename Base::LeafAlloc LeafAlloc;
    typedef typename Base::BranchAlloc BranchAlloc;

    CowHashClusterMap() {}

    int init(size_t n_bucket    = CH_DEFAULT_NBUCKET,
              u_int load_factor  = CH_DEFAULT_LOAD_FACTOR)
    { return base_.init(n_bucket, load_factor); }

    // Pack key/value into pair and insert into this container
    const Pointer insert(const HKey& hkey, const CKey& ckey, const CVal& cval)
    { return base_.insert(Item(hkey, HVal(ckey, cval))); }

    bool erase(const HKey& hkey, const CKey& ckey)
    { return base_.erase(hkey, ckey); }

    size_t erase(const HKey& hkey) { return base_.erase(hkey); }

    const Pointer seek(const HKey& hkey, const CKey& ckey) const
    { return base_.seek(hkey, ckey); }

    Cluster* seek(const HKey& hkey) const { return base_.seek(hkey); }

    void clear() { base_.clear(); }

    bool resize(size_t n_bucket2) { return base_.resize(n_bucket2); }

    const HMap* hash_map_ptr() const { return base_.hash_map_ptr(); }
    bool empty() const { return base_.empty(); }
    size_t size() const { return base_.size(); }
    size_t mem() const { return base_.mem(); }

    const MapAlloc* map_alloc() const { return base_.map_alloc(); }
    const LeafAlloc* leaf_alloc() const { return base_.leaf_alloc(); }
    const BranchAlloc* branch_alloc() const { return base_.branch_alloc(); }

    Iterator begin() const { return base_.begin(); }
    Iterator end() const { return base_.end(); }

    bool not_init() const { return base_.not_init(); }
    
    void cowbass_info(size_t* p_max_len,
                       size_t* p_min_len,
                       double* p_avg_len) const
    { base_.cowbass_info(p_max_len, p_min_len, p_avg_len); }

    void to_string(StringWriter& sw) const { base_.to_string(sw); }

private:
    Base base_;
};

}  // namespace st    

#endif  // _COW_HASH_CLUSTER_SEt_HPP_
