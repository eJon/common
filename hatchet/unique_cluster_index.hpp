// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// The index corresponding to ST_UNIQUE_KEY(..., ST_UNIQUE_CLUSTER_KEY(...))
// Author: gejun@baidu.com
// Date: Aug 2 20:57:03 CST 2011
#pragma once
#ifndef _UNIQUE_CLUSTER_INDEX_H_
#define _UNIQUE_CLUSTER_INDEX_H_

#include "index_traits.hpp"           // SeekInfo
#include "cow_hash_cluster_set.hpp"   // CowHashClusterSet

namespace st {

template <class _Tup, class _HashAttrS, class _ClusterAttrS,
          int _MAX_FANOUT, bool _REVERSED>
class UniqueClusterIndex : public c_show_base {
private:
    C_ASSERT(CAP(is_valid_index, UniqueClusterIndex), miss_interface);
    typedef _HashAttrS KeyAttrS1;
    typedef TCAP(list_concat, _HashAttrS, _ClusterAttrS) KeyAttrS2;
    typedef NamedTuple<KeyAttrS1> Key1;
    typedef NamedTuple<KeyAttrS2> Key2;
    typedef Key1 HKey;
    typedef TCAP(set_minus, class _Tup::AttrS, _HashAttrS) HValAttrS;
    typedef NamedTuple<HValAttrS> HVal;
    typedef CombinedTuple<HKey, HVal> CombinedTup;
    typedef NamedTuple<_ClusterAttrS> CKey;

public:
    class Reference : public CombinedTup {
    public:
        Reference () {}
        Reference (const HKey& hkey, const HVal& hval)
            : CombinedTup(&hkey, &hval) {}
        Reference (const HKey* hkey, const HVal* hval)
            : CombinedTup(hkey, hval) {}

        operator const _Tup () const
        {
            _Tup t;
            copy_attr_list<_Tup, CombinedTup, class _Tup::AttrS>
                ::call(&t, *this);
            return t;
        }
    };

    class Pointer {
    public:
        Pointer () {}
        
        Pointer (const HKey* p_hkey, const HVal* p_hval)
            : ref_(p_hkey, p_hval) {}
        
        operator bool () const { return ref_.valid(); }
        const Reference* operator-> () const { return &ref_; }
        const Reference& operator* () const { return ref_; }
        
    private:
        Reference ref_;
    };
    
    typedef CowHashClusterSet<_Tup, Pointer, Reference,
                              typename _Tup::template sub<_HashAttrS>,
                              typename _Tup::template sub<HValAttrS>,
                              typename HVal::template sub<_ClusterAttrS>,
                              TCAP(if_, _REVERSED,
                                   ReversedCompare<CKey>,
                                   Compare<CKey>),
                              _MAX_FANOUT> Base;
    
    //typedef typename Base::ConstReference ConstReference;
    typedef typename Base::Iterator Iterator;

    typedef typename Base::OneClusterIterator SeekIterator1;
    typedef PointerIterator<_Tup, Pointer, Reference> SeekIterator2;

    typedef PartialIterator<SeekIterator1> PartialSeekIterator1;
    typedef PartialIterator<SeekIterator2> PartialSeekIterator2;

    typedef Cons<SeekInfo<KeyAttrS1, SeekIterator1, COW_HASH_CLUSTER1_SCORE,
                          PartialSeekIterator1, MAYBE_COW_HASH_CLUSTER1_SCORE>,
                 Cons<SeekInfo<KeyAttrS2, SeekIterator2, COW_HASH_CLUSTER2_SCORE,
                               PartialSeekIterator2, MAYBE_COW_HASH_CLUSTER2_SCORE>,
                      void> > SeekInfoL;
    
    //unique cluster index中用于建立索引hash的Key
    typedef KeyAttrS1 IndexHashKey;

    typedef KeyAttrS2 Uniqueness;
    
    UniqueClusterIndex () {}
    ~UniqueClusterIndex () {}

    int init (size_t n_bucket        = CH_DEFAULT_NBUCKET,
              u_int load_factor      = CH_DEFAULT_LOAD_FACTOR)
    {
        return base_.init(n_bucket, load_factor);
    }

    UniqueClusterIndex (const UniqueClusterIndex& rhs) : base_(rhs.base_) {}

    UniqueClusterIndex& operator= (const UniqueClusterIndex& rhs)
    {
        base_ = rhs.base_;
        return *this;
    }
        
    void swap (UniqueClusterIndex& rhs) { base_.swap(rhs.base_); }

    // Insert an item into UniqueClusterIndex
    // Returns: address of the inserted item, NULL means insertion was failed
    Pointer insert (const _Tup& tup) { return base_.insert(tup); }

    // Erase all items
    void clear () { base_.clear(); }
        
    // Search for the item matching a key
    // Returns: address of the item
    SeekIterator1 seek (const Key1& key) const { return base_.find(key); }

    void maybe_seek (PartialSeekIterator1* p_it, const Key1& key) const
    {
        typename Base::Cluster* p_cluster = base_.seek(key);
        p_it->hkey() = key;
        if (NULL == p_cluster) {
            p_it->point_to_value();
        } else {
            p_it->iterator() = p_cluster->begin();
            p_it->point_to_iterator();
        }
    }

    void exclude_seek (PartialSeekIterator1* p_it, const Key1& key) const
    {
        class Base::Cluster* p_cluster = base_.seek(key);
        p_it->hkey() = key;
        if (NULL == p_cluster) {
            p_it->point_to_value();
        } else {
            p_it->iterator().set_end();
            p_it->point_to_iterator();
        }
    }
    
    // Erase the value matching a key from UniqueClusterIndex
    // Returns: erased or not
    bool erase (const Key1& key) { return base_.erase(key); }

    size_t key_num (const Key1&) const { return base_.hash_map_ptr()->size(); }
    
    SeekIterator2 seek (const Key2& key) const
    {
        HKey hkey;
        CKey ckey;
        copy_attr_list<HKey, Key2, _HashAttrS>::call(&hkey, key);
        copy_attr_list<CKey, Key2, _ClusterAttrS>::call(&ckey, key);
        return SeekIterator2(base_.seek(hkey, ckey));
    }

    bool erase (const Key2& key)
    {
        HKey hkey;
        CKey ckey;
        copy_attr_list<HKey, Key2, _HashAttrS>::call(&hkey, key);
        copy_attr_list<CKey, Key2, _ClusterAttrS>::call(&ckey, key);
        return base_.erase(hkey, ckey);
    }

    size_t key_num (const Key2&) const { return base_.size(); }

    
    bool copy_key_part (_Tup* p_dst, const _Tup& src) const
    {
        copy_attr_list<_Tup, _Tup, _HashAttrS>::call(p_dst, src);

        HKey hkey;
        copy_attr_list<HKey, _Tup, _HashAttrS>::call(&hkey, src);
        class Base::Cluster* p_clu = base_.seek(hkey);
        return (NULL != p_clu && p_clu->size() == 1ul);
    }

    Pointer seek_tuple (const _Tup& tup) const
    { return base_.seek_by_item(tup); }

    bool erase_tuple (const _Tup& tup)
    { return base_.erase_by_item(tup); }

    bool resize (size_t n_bucket2) { return base_.resize(n_bucket2); }

    Iterator begin() const { return base_.begin(); }
    Iterator end() const { return base_.end(); }
    bool not_init() const { return base_.not_init(); }
    bool empty() const { return base_.empty(); }
    size_t size() const { return base_.size(); }
    size_t mem() const { return base_.mem(); }
    const Base* base() const { return &base_; }
    
    // Print internal info to StringWriter
    void to_string (StringWriter& sw) const
    { base_.to_string(sw); }

friend std::ostream& operator<< (std::ostream& os, const UniqueClusterIndex& uci)
    { return os << uci.base_; }
    
    static void c_to_string (std::ostream& os)
    {
        os << (_REVERSED ? "UniqueReversedCluster(" : "UniqueCluster(")
           << c_show(_HashAttrS)
           << "->" << c_show(_ClusterAttrS)
           << "->" << c_show(HVal) << ")";
    }

private:
    Base base_;
};

template <typename _Index> struct is_unique_cluster_index {
    static const bool R = false;
};

template <class _Tup, class _HashAttrS, class _ClusterAttrS,
          int _MAX_FANOUT, bool _REVERSED>
struct is_unique_cluster_index<
    UniqueClusterIndex<_Tup, _HashAttrS, _ClusterAttrS,
                       _MAX_FANOUT, _REVERSED> > {
    static const bool R = true;
};

}  // namespace st

#endif  //_UNIQUE_CLUSTER_INDEX_H_
