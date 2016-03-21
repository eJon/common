// Copyright(c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Disguise a hash-cluster index in a CowTable as a table with one hash index
// mapping from hashing key to cluster of the source index
// Author: jiangrujie@baidu.com
// Date: Jan 11 15:38:21 CST 2011
#pragma once
#ifndef _GROUP_VIEW_HPP
#define _GROUP_VIEW_HPP

#include "cow_table.hpp"

namespace st {

// This class should only work on a CowHashClusterSet index.
// It behaves like a CowTable with only one UniqueKey whose Key is HKey 
// and item is NamedTuple<HKey + Cluster *>. 
// GroupView can also be used by a Selector or a Connector as a CowTable.
// This structure does not store data of source index
template<class _Table, int _INDEX_POS = 0>
class GroupView;

// This iterator wraps the iterator of a CowHashClusterSet.
// Its item is the combination of the hkey and the pointer to the 
// corresponding cluster.
template<class _Index, class _Item, class _ClusterAttr>
class GroupViewIterator;

// This is a wrapper of CowHashClusterSet.
// It somehow upgrades the index to a CowHashSet with HKey as 
// Key and <HKey + Cluster *> as Item.
// Now I only implement the seek function because it's used in
// Selector2.
// It also has an iterator to traverse the read data.
template<class _Index, class _Item, class _ClusterAttr>
class IndexWrapper;

// Commonly used function objects to check if the cluster of a groupview
// is empty or not
struct not_empty_cluster {
    template <class _Cluster> bool operator() (const _Cluster* p_cluster) const
    { return !p_cluster->empty(); }
};

// --------------------------------------------
// Implementations
// --------------------------------------------

template<class _Index, class _Item, class _ClusterAttr>
class IndexWrapper {
    typedef class _Index::HMap Map;
    typedef class Map::Item ClusterNode;
    
public:
    typedef class _Index::HKey Key1;
    typedef _Item Item;
    typedef GroupViewIterator<_Index, _Item, _ClusterAttr> Iterator;
    typedef PointerIterator<_Item> SeekIterator1;
    typedef SeekIterator1 PartialSeekIterator1;

    typedef Cons<SeekInfo<class Key1::AttrS, SeekIterator1, COW_HASH_SCORE,
                          PartialSeekIterator1, MAYBE_COW_HASH_SCORE>,
                 void> SeekInfoL;
    static const int UNIQUE_SEEK_INFO = 0;
    
    IndexWrapper() : p_map_(NULL) {}

    void init(const _Index *src) 
    {
        p_map_ = src->hash_map_ptr(); 
    }

    SeekIterator1 seek(const Key1& key) const
    {
        ClusterNode *p_node = p_map_->seek(key);
        if (p_node != NULL) {
            copy_attr_list<_Item, Key1, class Key1::AttrS>::call(&item_, key);
            item_.template at<_ClusterAttr>() = &p_node->cluster_;
            return SeekIterator1(&item_);
        } 
        return SeekIterator1(NULL);
    }

    void maybe_seek (PartialSeekIterator1* p_it, const Key1& key) const
    {
        ClusterNode *p_node = p_map_->seek(key);
        copy_attr_list<_Item, Key1, class Key1::AttrS>::call(&item_, key);
        item_.template at<_ClusterAttr>() = (p_node ? &p_node->cluster_ : NULL);
        *p_it = SeekIterator1(&item_);
    }

    void exclude_seek (PartialSeekIterator1* p_it, const Key1& key) const
    {
        ClusterNode *p_node = p_map_->seek(key);
        if (NULL == p_node) {
            copy_attr_list<_Item, Key1, class Key1::AttrS>::call(&item_, key);
            item_.template at<_ClusterAttr>() = NULL;
            *p_it = SeekIterator1(&item_);
        } else {
            *p_it = SeekIterator1(NULL);
        }
    }

    bool copy_key_part (_Item* p_dst, const _Item& src) const
    {
        copy_attr_list<_Item, _Item, class Key1::AttrS>::call(p_dst, src);
        return true;
    }


    Iterator begin() { return Iterator(p_map_->begin()); }

    Iterator end() { return Iterator(p_map_->end()); }

private:
    const Map *p_map_;
    mutable Item item_;
};
    

template<class _Index, class _Item, class _ClusterAttr>
class GroupViewIterator {
    typedef class _Index::HMap::Iterator MapIter;
    
public:
    GroupViewIterator() {}

    explicit GroupViewIterator(const MapIter& map_iter)
        : mit_(map_iter) { construct_tuple(); }

    GroupViewIterator& operator++()
    {
        ++mit_;
        construct_tuple();
        return *this;
    }

    GroupViewIterator& operator++(int)
    {
        GroupViewIterator tmp = *this;
        this->operator++();
        return tmp;
    }

    const _Item& operator*() const { return item_; }
    
    const _Item *operator->() const
    { 
        this->operator*();
        return &item_; 
    }

    operator bool () const { return mit_; }

    void set_end() { mit_.set_end(); }
    
private:
    void construct_tuple()
    {
        if (mit_) {
            copy_attr_list<_Item, class _Index::HKey,
                class _Index::HKey::AttrS>::call(&item_, mit_->hkey_);
            item_.template at<_ClusterAttr>() = &mit_->cluster_;
        }
    }
    
    MapIter mit_;
    _Item item_;
};

template<class _Table, int _INDEX_POS>
class GroupView {
    C_ASSERT(_INDEX_POS >= 0 && _INDEX_POS < _Table::N_INDEX, 
            position_out_of_range);

    // The origin index which must be a CowHashClusterSet
    typedef TCAP(_Table::template IndexAt, _INDEX_POS) Index2;
    C_ASSERT(CAP(is_unique_cluster_index, Index2), hash_cluster_index_expected);
    typedef typename Index2::Base Index;
   
    typedef class Index::HKey::AttrS KeyAttrS;

public:
    // Attribute of the cluster
    DEFINE_COLUMN(CLUSTER_ATTR, const class Index::Cluster *);

private:
    typedef TCAP(list_concat, KeyAttrS, Cons<CLUSTER_ATTR, void>) AttrS;
    typedef class Index::HKey Key;
    typedef class Index::Cluster Cluster;
    typedef class Index::Item SrcTup;

public:
    typedef NamedTuple<AttrS> Tup;
    
private:
    // The index that pretends to be a CowHashSet.
    // It will be used to form IndexL and exposed by IndexAt.
    typedef IndexWrapper<Index, Tup, CLUSTER_ATTR> WrappedIndex;

    // Construct a tuple of GroupView which has an attribute of 
    // CLUSTER_ATTR according to the src_tup.
    struct build_tuple {
        static void call(Tup *tup, const SrcTup& src_tup, const Index *p_index)
        {
            Key hkey;
            copy_attr_list<Key, SrcTup, KeyAttrS>::call(&hkey, src_tup);
            copy_attr_list<Tup, SrcTup, KeyAttrS>::call(tup, src_tup);
            tup->template at<CLUSTER_ATTR>() = p_index->seek(hkey);
        }
    };
    
    // Extract the HKey of src_tup and then get the item(Cluster *).
    // Return true if the the item is empty.
    struct cluster_empty {
        static bool call(const SrcTup& src_tup, const Index *p_index)
        {
            Key hkey;
            copy_attr_list<Key, SrcTup, KeyAttrS>::call(&hkey, src_tup);
            Cluster *p_node = p_index->seek(hkey);
            return (p_node == NULL || p_node->empty());
        }
    };
    
    // Extract the HKey of src_tup and then get the item(Cluster *).
    // Return true if the there is only one item in the cluster.
    struct cluster_one_item {
        static bool call(const SrcTup& src_tup, const Index *p_index)
        {
            Key hkey;
            copy_attr_list<Key, SrcTup, KeyAttrS>::call(&hkey, src_tup);
            Cluster *p_node = p_index->seek(hkey);
            return (p_node != NULL && p_node->size() == 1);
        }
    };
    
public:
    
    typedef Cons<WrappedIndex, void> IndexL;

    template<int _N> struct IndexAt
    { 
        C_ASSERT(_N == 0, index_out_of_range);
        typedef TCAP(list_at, _N, IndexL) R; 
    };

    static const int N_INDEX = 1;

    typedef GroupViewIterator<Index, Tup, CLUSTER_ATTR> Iterator;
    typedef PointerIterator<Tup> SeekIterator1;
    typedef PartialIterator<SeekIterator1> PartialSeekIterator1;
    typedef Cons<IndexInfo<0, SeekInfo<KeyAttrS, SeekIterator1, COW_HASH_SCORE, PartialSeekIterator1, MAYBE_COW_HASH_SCORE> >, void> IndexInfoL;

    struct InsertObserver: public BaseObserver<const SrcTup&> {
        void on_event(const SrcTup& src_tup)
        {
            bool is_one_item = cluster_one_item::
                call(src_tup, p_src_index_);
            
            if ((!p_view_->post_replace_event.empty() && !is_one_item)
                || (!p_view_->insert_event.empty() && is_one_item)) {
                Tup tuple;
                build_tuple::call(&tuple, src_tup, p_src_index_);
                
                if (is_one_item) {
                    // A new cluster has just been created.
                    p_view_->insert_event.notify(tuple);
                } else if (p_view_->enable_replace_event_) {
                    p_view_->post_replace_event.notify(tuple);
                }
            }
        }

        const Index *p_src_index_;
        GroupView *p_view_;
    };

    struct EraseObserver: public BaseObserver<const SrcTup&> {
        void on_event(const SrcTup& src_tup)
        {
            bool is_one_item = cluster_one_item::
                call(src_tup, p_src_index_);
            
            if ((!p_view_->pre_replace_event.empty() && !is_one_item)
                    || (!p_view_->erase_event.empty() && is_one_item)) {
                Tup tuple;
                build_tuple::call(&tuple, src_tup, p_src_index_);

                if (is_one_item) {
                    // A cluster is going to be deleted.
                    p_view_->erase_event.notify(tuple);
                } else if (p_view_->enable_replace_event_) {
                    p_view_->pre_replace_event.notify(tuple);
                }
            }
        }

        const Index *p_src_index_;
        GroupView *p_view_;
    };
    
    // If the cluster is not empty before the insertion, it 
    // means a replace event to the GroupView.
    // Note: We do not dispatch the event to pre_insert_event of
    // GroupView because it costs too much to create a temporary
    // cluster. Since no one else uses pre_insert_event except 
    // GroupView, we omit this now.
    struct PreInsertObserver: public BaseObserver<const SrcTup&> {
        void on_event(const SrcTup& src_tup)
        {
            if (p_view_->enable_replace_event_ &&
                !p_view_->pre_replace_event.empty() &&
                !cluster_empty::call(src_tup, p_src_index_)) {
                Tup tuple;
                build_tuple::call(&tuple, src_tup, p_src_index_);
                p_view_->pre_replace_event.notify(tuple);
            }
        }

        const Index *p_src_index_;
        GroupView *p_view_;
    };
    
    // If the cluster is not empty after the erasion, it 
    // means a replace event to the GroupView.
    // Note: We do not dispatch the event to post_erase_event of
    // GroupView because it costs too much to create a temporary
    // cluster. Since no one else uses post_erase_event except 
    // GroupView, we omit this now.
    struct PostEraseObserver: public BaseObserver<const SrcTup&> {
        void on_event(const SrcTup& src_tup)
        {
            if (p_view_->enable_replace_event_ &&
                !p_view_->post_replace_event.empty() &&
                !cluster_empty::call(src_tup, p_src_index_)) {
                Tup tuple;
                build_tuple::call(&tuple, src_tup, p_src_index_);
                p_view_->post_replace_event.notify(tuple);
            }
        }

        const Index *p_src_index_;
        GroupView *p_view_;
    };
    
    explicit GroupView(const _Table *src)
    {
        enable_replace_event_ = true;
        
        p_src_index_ = src->template index<_INDEX_POS>().base();

        insert_ob_.p_src_index_ = p_src_index_;
        erase_ob_.p_src_index_ = p_src_index_;
        pre_insert_ob_.p_src_index_ = p_src_index_;
        post_erase_ob_.p_src_index_ = p_src_index_;
        
        insert_ob_.p_view_ = this;
        erase_ob_.p_view_ = this;
        pre_insert_ob_.p_view_ = this;
        post_erase_ob_.p_view_ = this;

        src->insert_event.subscribe(&insert_ob_);
        src->erase_event.subscribe(&erase_ob_);
        src->pre_replace_event.subscribe(&erase_ob_);
        src->post_replace_event.subscribe(&insert_ob_);
        src->pre_insert_event.subscribe(&pre_insert_ob_);
        src->post_erase_event.subscribe(&post_erase_ob_);

        windex_.init(p_src_index_);    
    }
    
    template <int _N> const TCAP(list_at, _N, IndexL)& index() const
    { 
        C_ASSERT(_N == 0, index_out_of_range);
        return windex_;
    }

    Iterator begin() const
    { return Iterator(p_src_index_->hash_map_ptr()->begin()); }

    Iterator end() const
    { return Iterator(p_src_index_->hash_map_ptr()->end()); }

    size_t size() const
    { return p_src_index_->hash_map_ptr()->size(); }
        
    bool empty() const
    { return p_src_index_->hash_map_ptr()->empty(); }

    bool not_init() const
    { return NULL == p_src_index_ || p_src_index_->not_init(); }

    // Turn on/off sending pre_replace/new_event
    void enable_replace_event () { enable_replace_event_ = true; }
    void disable_replace_event () { enable_replace_event_ = false; }
    
private:
    bool enable_replace_event_;
    const Index *p_src_index_;
    WrappedIndex windex_;
    InsertObserver insert_ob_;
    PreInsertObserver pre_insert_ob_;
    EraseObserver erase_ob_;
    PostEraseObserver post_erase_ob_;

public:
    mutable Event<const Tup&> insert_event;
    mutable Event<const Tup&> pre_insert_event;
    mutable Event<const Tup&> erase_event;
    mutable Event<const Tup&> post_erase_event;
    mutable Event<const Tup&> pre_replace_event;
    mutable Event<const Tup&> post_replace_event;
    mutable Event<> clear_event;
    mutable Event<const Tup&> clear_erase_event;
};

}  // namespace st

#endif  // _GROUP_VIEW_HPP
