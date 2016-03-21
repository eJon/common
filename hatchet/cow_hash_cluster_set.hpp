// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Map keys to clusters which are groups of items ordered by another key
// Author: gejun@baidu.com
// Date: Wed Aug 11 14:57:47 2010
#pragma once
#ifndef _COW_HASH_CLUSTER_MAP_HPP_
#define _COW_HASH_CLUSTER_MAP_HPP_

#include "cow_hash_set.hpp"             // CowHashSet
#include "cowbass4.hpp"                 // Cowbass4
#include "common_function_objects.hpp"  // f_identity
#include "st_utility.h"                 // std_pair_first, std_pair_second
#include "combined_tuple.hpp"           // CombinedTuple

namespace st {
// CowHashClusterSet has two keys, the first key hash-maps to a cluster 
// which is a group of items ordered by the second key. With this structure,
// a large amount of data can be quickly narrowed to a cluster which is 
// much shorter. The cluster is specially designed for as-fast-as-array
// traversal, fast search, mediocre insertions and erasures, so this 
// structure is especially good at finding and traversing a cluster, however
// point operations are generally acceptable as well. Currently the mapping
// is CowHashSet and the cluster is Cowbass4, both supporting reference
// counting copy-on-write, making this structure have the property naturally
// to save considerable memory

// Iterator of CowHashClusterSet
template <class _Map> class HashClusterIterator {
    typedef typename _Map::Cluster::Iterator ClusterIterator;
    typedef typename _Map::HMap::Iterator MapIterator;
public:
    typedef _Map Map;
    typedef typename _Map::Pointer Pointer;
    typedef typename _Map::Reference Reference;
    
    HashClusterIterator () {}
    explicit HashClusterIterator (const MapIterator& map_iter)
        : mit_(map_iter)
    { find_valid_node(); }

    // it1 == it2
    bool operator== (const HashClusterIterator& rhs) const
    { return it_ == rhs.it_ && mit_ == rhs.mit_; }
            
    // it1 != it2
    bool operator!= (const HashClusterIterator& rhs) const
    { return it_ != rhs.it_ || mit_ != rhs.mit_; }
        
    // ++ it
    HashClusterIterator& operator++ ()
    {
        ++ it_;
        if (!it_) {
            ++ mit_;
            find_valid_node();
        }
        return *this;
    }

    // it ++
    HashClusterIterator operator++ (int)
    {
        HashClusterIterator tmp = *this;
        this->operator++();
        return tmp;
    }

    Pointer operator-> () const
    { return Pointer(&(mit_->hkey_), it_.operator->()); }
    
    Reference operator* () const
    { return Reference(mit_->hkey_, it_.operator*()); }

    // Test end, true/false for end or not
    operator bool () const { return it_ && mit_; }
    
    void set_end ()
    {
        it_.set_end();
        mit_.set_end();
    }

    // TODO: this is temporary
    const typename _Map::HKey& key() const { return mit_->hkey_; }
    const typename _Map::HVal& val() const { return *it_; }

    void to_string (StringWriter& sw) const
    {
        sw << "CowHashClusterIter{it=" << it_ << " mit=" << mit_ << "}";
    }
 
private:
    void find_valid_node ()
    {
        for ( ; mit_; ++mit_) {
            it_ = mit_->cluster_.begin();
            if (it_) {
                return;
            }
        }
    }
            
    ClusterIterator it_;
    MapIterator mit_;
};

template <class _Map> class HashOneClusterIterator {
public:
    typedef typename _Map::HKey HKey;
    typedef typename _Map::Cluster Cluster;
    typedef _Map Map;
    typedef typename _Map::Item Value;
    typedef typename _Map::Pointer Pointer;
    typedef typename _Map::Reference Reference;
    
    // Default constructor
    HashOneClusterIterator () {}

    // Construct from a map Iterator
    explicit HashOneClusterIterator (const HKey& hkey,
                                     const Cluster* p_cluster)
        : hkey_(hkey)
        , it_(p_cluster ? p_cluster->begin() : Cluster::end())
    {}

    bool operator== (const HashOneClusterIterator& rhs) const
    { return it_ == rhs.it; }
    
    bool operator!= (const HashOneClusterIterator& rhs) const
    { return it_ != rhs.it_; }
        
    HashOneClusterIterator& operator++ ()
    {
        ++ it_;
        return *this;
    }

    HashOneClusterIterator operator++ (int)
    {
        HashOneClusterIterator tmp = *this;
        operator++();
        return tmp;
    }

    Pointer operator-> () const { return Pointer(&hkey_, it_.operator->()); }
    Reference operator* () const { return Reference(hkey_, it_.operator*()); }

    // Test end, true for end, false for not end
    operator bool () const { return it_; }
    void set_end () { it_.set_end(); }
    
    // TODO: this is temporary
    const typename _Map::HKey& key() const { return hkey_; }
    const typename _Map::HVal& val() const { return *it_; }
    
    void to_string (StringWriter& sw) const
    {
        sw << "HashOneClusterIter{it=" << it_ << " hkey=" << hkey_ << "}";
    }

private:
    HKey hkey_;
    typename Cluster::Iterator it_;
};

template <typename _Item,
          class _Pointer,
          class _Reference,
          class _GetHashKey,
          class _GetHashVal,
          class _GetClusterKey =
                  f_identity<TCAP(ReturnType, _GetHashVal, _Item)>,
          class _Cmp = Compare<TCAP(ReturnType, _GetClusterKey,
                                      TCAP(ReturnType, _GetHashVal, _Item))>,
          u_int _MAX_FANOUT = COWBASS_DEFAULT_FANOUT,
          class _Hash = Hash<TCAP(ReturnType, _GetHashKey, _Item)>,
          class _Equal = Equal<TCAP(ReturnType, _GetHashKey, _Item)> >
class CowHashClusterSet {
public:
    typedef _Item Item;
    typedef TCAP(ReturnType, _GetHashKey, _Item) HKey;
    typedef TCAP(ReturnType, _GetHashVal, _Item) HVal;
    typedef TCAP(ReturnType, _GetClusterKey, HVal) CKey;  // clustering key
    typedef _Pointer Pointer;
    typedef _Reference Reference;

    // a list of HVal clustered together and ordered by CKey
    typedef Cowbass4<HVal, _GetClusterKey, _MAX_FANOUT, _Cmp> Cluster;
        
private:
    // We may need to add customized tag or something later so I
    // use CowHashSet rather than CowHMap to store the relationship
    // between HKey and Clusters
    struct ClusterNode : public c_show_base {
        void to_string (StringWriter& sw) const
        { sw << '(' << hkey_ << ":" << cluster_ << ')'; }

        static void c_to_string (std::ostream& os) { os << c_show(Cluster); }  

        HKey hkey_;          // key
        Cluster cluster_;    // cluster, Note: this is not a pointer
    };

    // get the key part from ClusterNode
    struct cluster_node_key {
        HKey operator() (const ClusterNode& cn) const { return cn.hkey_; }
    };
public:
    // mapping from hashing key to cluster
    typedef CowHashSet<ClusterNode, cluster_node_key, _Hash, _Equal> HMap;
    typedef typename HMap::Allocator MapAlloc;
    typedef typename Cluster::LeafAlloc LeafAlloc;
    typedef typename Cluster::BranchAlloc BranchAlloc;
    typedef HashClusterIterator<CowHashClusterSet> Iterator;
    typedef HashOneClusterIterator<CowHashClusterSet> OneClusterIterator;

    // Default constructor and destructor
    CowHashClusterSet () : n_item_(0) {}

    // Initialize this container, must be called before using
    // Params:
    //   n_bucket/load_factor  passed to the map from hashing key to cluster
    //   get_hkey/get_ckey     used by erase_by_item/seek_by_item
    int init (size_t n_bucket                = CH_DEFAULT_NBUCKET,
              u_int load_factor              = CH_DEFAULT_LOAD_FACTOR,
              const _GetHashKey& get_hkey    = _GetHashKey(),
              const _GetHashVal& get_hval    = _GetHashVal(),
              const _GetClusterKey& get_ckey = _GetClusterKey());
        
    // Note: We don' need following three methods because default versions
    //       already works.
    // ~CowHashClusterSet () {}
    // CowHashClusterSet (const CowHashClusterSet& other) {}
    // CowHashClusterSet& operator= (const CowHashClusterSet& other) {}

    // Insert an item into this mapping
    // Returns: address of inserted item
    const Pointer insert (const _Item& item);

    // Erase the item matching both hkey and ckey
    // Returns: erased or not
    bool erase (const HKey& hkey, const CKey& ckey);

    // Erase the cluster matching the hkey
    // Returns: number of erased items
    size_t erase (const HKey& hkey);

    // Erase the item matching both hkey and ckey of the given item
    // Returns: erased or not
    bool erase_by_item (const _Item& item)
    { return erase(get_hkey_(item), get_ckey_(get_hval_(item))); }

    // Remove all items from this container
    void clear ()
    {
        // Since we store struct of Cluster directly in Map (not
        // pointers), clear in map_ should consequently clear and
        // deallocate all clusters
        map_.clear();
        n_item_ = 0;
    }

    // Resize/Reserve internal map
    bool resize (size_t n_bucket2) { return map_.resize(n_bucket2); }
    bool reserve (size_t n_bucket2) { return map_.reserve(n_bucket2); }

    // Find the item matching both hkey and ckey
    // Returns: `Pointer' to the item
    const Pointer seek (const HKey& hkey, const CKey& ckey) const
    {
        typename HMap::Pointer p_cn = map_.seek(hkey);
        return (NULL != p_cn)
            ? Pointer(&(p_cn->hkey_), p_cn->cluster_.seek(ckey))
            : Pointer();
    }
    
    // Search for item matching both hkey and ckey of the given item
    // Returns: address of the item
    const Pointer seek_by_item (const _Item& item) const
    { return seek(get_hkey_(item), get_ckey_(get_hval_(item))); }

    // Seek the cluster matching hkey
    // Returns: address of the cluster
    Cluster* seek (const HKey& hkey) const
    {
        typename HMap::Pointer p_cn = map_.seek(hkey);
        return (NULL != p_cn) ? &(p_cn->cluster_) : NULL;
    }
    
    OneClusterIterator find (const HKey& hkey) const
    {
        typename HMap::Pointer p_cn = map_.seek(hkey);
        return OneClusterIterator(
            hkey, (p_cn ? &(p_cn->cluster_) : NULL));
    }
    
    // Get the hash map from hashing keys to clusters
    const HMap* hash_map_ptr () const { return &map_; }

    // Know empty or not
    bool empty () const { return 0 == n_item_; }

    // Get number of items
    size_t size () const { return n_item_; }

    // Get memory taken by this container
    size_t mem () const
    {
        const long div = map_.alloc_use_count();
        return sizeof(*this) + 
            map_.mem() - sizeof(map_) +
            (div >= 1 ? (leaf_alloc()->mem() + branch_alloc()->mem()) / div : 0);
    }
    
    const MapAlloc* map_alloc() const { return map_.alloc(); }
    const LeafAlloc* leaf_alloc() const { return seed_cluster_.leaf_alloc(); }
    const BranchAlloc* branch_alloc() const
    { return seed_cluster_.branch_alloc(); }
    
    // Know initialized or not
    bool not_init () const
    { return map_.not_init() || seed_cluster_.not_init(); }

    Iterator begin () const { return Iterator(map_.begin()); }
    Iterator end () const { return Iterator(map_.end()); }

    // Get max/min/average length of chains
    // Note: input pointers must be non-NULL
    void cowbass_info (size_t* p_max_len,
                       size_t* p_min_len,
                       double* p_avg_len) const;
    
    // Write to StringWriter
    void to_string (StringWriter& sw) const;

    friend std::ostream& operator<< (
        std::ostream& os, const CowHashClusterSet& index)
    {
        size_t max_len = 0, min_len = 0;
        double avg_len = 0;
        size_t cb_max_len = 0, cb_min_len = 0;
        double cb_avg_len = 0;
        
        index.map_.bucket_info(&max_len, &min_len, &avg_len);
        index.cowbass_info(&cb_max_len, &cb_min_len, &cb_avg_len); 
        return os << c_show(CowHashClusterSet)
                  << " cb=" << cb_max_len
                  << "/" << cb_min_len
                  << "/" << cb_avg_len
                  << " map=" << max_len
                  << "/" << min_len
                  << "/" << avg_len
                  << "/" << index.map_.bucket_num(); 
    }

private:
    size_t n_item_;            // number of items
    HMap map_;                 // the map from hashing-keys to clusters
    Cluster seed_cluster_;     // all clusters are created by coping this 
                               // cluster which holds an reference to 
                               // internal shared allocators so that even
                               // if all clusters in map_ are removed, 
                               // allocators are still referenced and not 
                               // deallocated
    _GetHashKey get_hkey_;     // get key for map_ from _Item
    _GetHashVal get_hval_;     // get value from _Item
    _GetClusterKey get_ckey_;  // get key for cluster from _Item
};

template <typename _Item, class _Pointer, class _Reference,
          class _GetHashKey, class _GetHashVal,
          class _GetClusterKey, class _Cmp, u_int _MAX_FANOUT,
          class _Hash, class _Equal>
int CowHashClusterSet<_Item, _Pointer, _Reference,
                      _GetHashKey, _GetHashVal,
                      _GetClusterKey, _Cmp, _MAX_FANOUT, _Hash, _Equal>::
init (size_t n_bucket, u_int load_factor,
      const _GetHashKey& get_hkey,
      const _GetHashVal& get_hval,
      const _GetClusterKey& get_ckey)
{
    if (!not_init()) {
        ST_FATAL("This set is already initialized");
        return -1;
    }
            
    int ret = map_.init(n_bucket, load_factor);
    if (0 != ret) {
        ST_FATAL("Fail to init map_");
        return ret;
    }

    ret = seed_cluster_.init();
    if (0 != ret) {
        ST_FATAL("Fail to init seed_cluster_");
        return ret;
    }
        
    get_hkey_ = get_hkey;
    get_hval_ = get_hval;
    get_ckey_ = get_ckey;
            
    return 0;
}

template <typename _Item, class _Pointer, class _Reference,
          class _GetHashKey, class _GetHashVal,
          class _GetClusterKey, class _Cmp, u_int _MAX_FANOUT,
          class _Hash, class _Equal>
const _Pointer
CowHashClusterSet<_Item, _Pointer, _Reference,
                  _GetHashKey, _GetHashVal,
                  _GetClusterKey, _Cmp, _MAX_FANOUT, _Hash, _Equal>::
insert (const _Item& item)
{
    const HKey hkey = get_hkey_(item);
    typename HMap::Pointer p_cn = map_.seek(hkey);
    typename HMap::Pointer p_new_cn = NULL;
    if (NULL == p_cn) {
        ClusterNode cn;
        cn.hkey_ = hkey;
        cn.cluster_ = seed_cluster_;
        p_new_cn = map_.insert(cn);
    } else {  // p_cn != NULL
        if (! HMap::local_item(p_cn)) {  // make p_cn local
            p_cn = map_.insert(*p_cn);
        }
        p_new_cn = p_cn;
    }
    Cluster* p_clu = &(p_new_cn->cluster_);
    size_t n_pre = p_clu->size();
    typename Cluster::Pointer p_value = p_clu->insert(get_hval_(item));
    n_item_ += (p_clu->size() - n_pre);
    return Pointer(&(p_new_cn->hkey_), p_value);
}

template <typename _Item, class _Pointer, class _Reference,
          class _GetHashKey, class _GetHashVal,
          class _GetClusterKey, class _Cmp, u_int _MAX_FANOUT,
          class _Hash, class _Equal>
bool
CowHashClusterSet<_Item, _Pointer, _Reference,
                  _GetHashKey, _GetHashVal,
                  _GetClusterKey, _Cmp, _MAX_FANOUT, _Hash, _Equal>::
erase (const HKey& hkey, const CKey& ckey)
{
    typename HMap::Pointer p_cn = map_.seek(hkey);
    if (NULL == p_cn) {
        return false;
    }  // p_cn != NULL
        
    assert(hkey == p_cn->hkey_);
    if (! HMap::local_item(p_cn)) {  // make p_cn local
        p_cn = map_.insert(*p_cn);
    }
    Cluster* p_clu = &(p_cn->cluster_);
    bool erased = p_clu->erase(ckey);
    if (erased && p_clu->empty()) {
        map_.erase(hkey);
    }
    n_item_ -= erased;
    return erased;
}

template <typename _Item, class _Pointer, class _Reference,
          class _GetHashKey, class _GetHashVal,
          class _GetClusterKey, class _Cmp, u_int _MAX_FANOUT,
          class _Hash, class _Equal>
size_t
CowHashClusterSet<_Item, _Pointer, _Reference,
                  _GetHashKey, _GetHashVal,
                  _GetClusterKey, _Cmp, _MAX_FANOUT, _Hash, _Equal>::
erase (const HKey& hkey)
{
    typename HMap::Pointer p_cn = map_.seek(hkey);
    if (NULL == p_cn) {
        return 0;
    }  // p_cn != NULL

    const size_t n_erased = p_cn->cluster_.size();
    map_.erase(hkey);
    n_item_ -= n_erased;
    return n_erased;
}

template <typename _Item, class _Pointer, class _Reference,
          class _GetHashKey, class _GetHashVal,
          class _GetClusterKey, class _Cmp, u_int _MAX_FANOUT,
          class _Hash, class _Equal>
void
CowHashClusterSet<_Item, _Pointer, _Reference,
                  _GetHashKey, _GetHashVal,
                  _GetClusterKey, _Cmp, _MAX_FANOUT, _Hash, _Equal>::
cowbass_info (size_t* p_max_len, size_t* p_min_len, double* p_avg_len) const
{
    if (not_init()) {
        *p_max_len = 0;
        *p_min_len = 0;
        *p_avg_len = 0;
        return;
    }
            
    size_t max_len=0, min_len=UINT_MAX, sum_len=0;
    for (typename HMap::Iterator it=map_.begin(), it_e=map_.end();
         it != it_e; ++ it) {
        const size_t len = it->cluster_.size();
        if (len > max_len) {
            max_len = len;
        }
        if (len < min_len) {
            min_len = len;
        }
        sum_len += len;
    }
    size_t n_non_empty = map_.size();
    *p_max_len = n_non_empty > 0 ? max_len : 0;
    *p_min_len = n_non_empty > 0 ? min_len : 0;
    *p_avg_len = n_non_empty > 0 ? (sum_len/(double)n_non_empty) : 0;
}

template <typename _Item, typename _Pointer, typename _Reference,
          class _GetHashKey, class _GetHashVal,
          class _GetClusterKey, class _Cmp, u_int _MAX_FANOUT,
          class _Hash, class _Equal>
void
CowHashClusterSet<_Item, _Pointer, _Reference,
                  _GetHashKey, _GetHashVal,
                  _GetClusterKey, _Cmp, _MAX_FANOUT, _Hash, _Equal>::
to_string (StringWriter& sw) const
{
    size_t cb_max_len, cb_min_len;
    double cb_avg_len;
    cowbass_info(&cb_max_len, &cb_min_len, &cb_avg_len);

    size_t max_len, min_len;
    double avg_len;
    map_.bucket_info(&max_len, &min_len, &avg_len);
    
    std::ostringstream oss;
    oss << c_show(HKey) << "->" << c_show(Cluster);
    
    sw << "CowHashCluster{" << oss.str()
       << " mem=" << mem()
       << " n_item=" << n_item_
       << " cb_max/min/avg="
       << cb_max_len << '/' << cb_min_len << '/' << cb_avg_len
       << " map_item/bkt/max/min/avg="
       << map_.size() << '/' << map_.bucket_num()
       << '/' << max_len << '/' << min_len << '/' << avg_len;
    sw << " cb_alloc="
       << map_.alloc_use_count() << '/' << (void*)map_.alloc();
    if (map_.alloc_owner()) {
        sw << '/' << map_.alloc();
    }
        
    sw << " leaf_alloc=" << seed_cluster_.leaf_alloc_use_count()
       << '/' << (void*)seed_cluster_.leaf_alloc();
    if (seed_cluster_.leaf_alloc_owner()) {
        sw << '/' << seed_cluster_.leaf_alloc();
    };

    sw << " branch_alloc=" << seed_cluster_.branch_alloc_use_count()
       << '/' << (void*)seed_cluster_.branch_alloc();
    if (seed_cluster_.branch_alloc_owner()) {
        sw << '/' << seed_cluster_.branch_alloc();
    };

    // if (!not_init()) {
    //     sw << " items=";
    //     shows_range(sw, begin(), end());
    // }
    sw << "}";
}

// compile-time printing
template <typename _Item, class _Pointer, class _Reference,
          class _GetHashKey, class _GetHashVal, class _GetClusterKey,
          class _Cmp, u_int _MAX_FANOUT, class _Hash, class _Equal>
struct c_show_impl<
    CowHashClusterSet<_Item, _Pointer, _Reference,
                      _GetHashKey, _GetHashVal, _GetClusterKey,
                      _Cmp, _MAX_FANOUT, _Hash, _Equal> > {
    typedef CowHashClusterSet<_Item, _Pointer, _Reference,
                              _GetHashKey, _GetHashVal, _GetClusterKey,
                              _Cmp, _MAX_FANOUT, _Hash, _Equal> T;
    static void c_to_string (std::ostream& os)
    {
        os << c_show(class T::HKey) << "->" << c_show(class T::Cluster);
    }
};

}  // namespace st    

#endif  // _COW_HASH_CLUSTER_MAP_HPP_
