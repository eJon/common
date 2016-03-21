// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Hashmap with a dynamic layout
// Author: gejun@baidu.com
// Date: Dec.27 15:28:29 CST 2010
#pragma once
#ifndef _DYN_UNIQUE_INDEX_H_
#define _DYN_UNIQUE_INDEX_H_

#include "dyn_index.h"                      // DynIndex, DynIterator
#include "base_cow_hash_map.h"              // BaseCowHashMap

namespace st {

class DynUniqueIndexIterator : public DynIterator {
public:
    DynUniqueIndexIterator(const BaseCowHashMap* m)
        : p_node_(NULL)
        , pp_entry_(m->bucket_array())
        , pp_entry_end_(m->bucket_array() + m->bucket_num())
    {}
    
    const void* setup()
    {
        find_valid_node();
        return p_node_ ? p_node_->item_ : NULL;
    }

    const void* forward()
    {
        if (NULL == p_node_->p_next_) {
            ++ pp_entry_;
            find_valid_node();
        } else {
            p_node_ = p_node_->p_next_;
        }
        return p_node_ ? p_node_->item_ : NULL;
    }

private:
    void find_valid_node()
    {
        for ( ; pp_entry_ != pp_entry_end_; ++pp_entry_) {
            if (NULL != *pp_entry_) {
                p_node_ = *pp_entry_;
                return;
            }
        }
        p_node_ = NULL;
    }

    Bucket* p_node_;
    Bucket* const* pp_entry_;
    Bucket* const* pp_entry_end_;
};

class DynUniqueIndexSeekIterator : public DynIterator {
public:
    DynUniqueIndexSeekIterator(const void* item) : item_(item) {}
    const void* setup() { return item_; }
    const void* forward() { return NULL; }

private:
    const void* item_;
};

class DynUniqueIndexPartialSeekIterator : public DynIterator {
public:
    DynUniqueIndexPartialSeekIterator(const void* item) : item_(item) {}
    const void* setup() { return item_; }
    const void* forward() { return NULL; }

private:
    const void* item_;
};

class DynUniqueIndex : public DynIndex {
public:
    DynUniqueIndex();
    ~DynUniqueIndex();
    DynUniqueIndex(const DynUniqueIndex&);
    DynUniqueIndex& operator=(const DynUniqueIndex&);

    int copy_from(const DynIndex&);
    int try_copy_from(const DynIndex& base_rhs) const;
    DynIndex* clone() const;
    
    // ST_TABLE(int8 UNIT_ID, int16 SITE_ID, int32 BID,
    //          ST_UNIQUE_KEY(UNIT_ID, SITE_ID))
    // Returns: 0, EINVAL, ENOTEXIST, what BaseCowHashMap::init(5) returns
    int init (
        const DynTupleSchema* schema,
        std::vector<std::string> key_names,
        size_t nbucket        = CH_DEFAULT_NBUCKET,
        u_int load_factor      = CH_DEFAULT_LOAD_FACTOR);
        
    // Insert an item into DynUniqueIndex
    // Returns: address of the inserted item, NULL means insertion was failed
    void* insert(const DynTuple& tup);

    // Erase the value matching a key from DynUniqueIndex
    // Returns: erased or not
    size_t erase(int, const DynTuple& key);

    // Get DynSeekInfos
    std::vector<DynSeekInfo> seek_info_list() const;
    
    // Search tuples matching `key' of `si_pos'th seekinfo
    // Returns: An iterator to found tuples
    size_t seek_mem_size() const { return sizeof(DynUniqueIndexSeekIterator); }
    DynIterator* seek(int si_pos, const DynTuple& key) const;
    DynIterator* seek(int si_pos, const DynTuple& key, void* mem) const;
    
    DynIterator* maybe_seek(int, const DynTuple& key) const;
    DynIterator* exclude_seek(int, const DynTuple& key) const;
    size_t key_num(int) const;
    
    bool copy_key_part(DynTuple* p_dst, const DynTuple& src) const;

    DynSeekInfo uniqueness() const;
    void* seek_tuple(const DynTuple&) const;
    bool erase_tuple (const DynTuple&);
    
    DynIterator* all() const;
    void serialize(std::ostream&) const;

    void swap(DynUniqueIndex& rhs)        { base_.swap(rhs.base_); }
    void clear()                          { base_.clear(); }
    bool resize(size_t nbucket2)          { return base_.resize(nbucket2); }
    bool not_init() const                 { return base_.not_init(); }
    bool empty() const                    { return base_.empty(); }
    size_t size() const                   { return base_.size(); }
    size_t capacity() const               { return base_.bucket_num(); }
    size_t mem() const                    { return base_.mem(); }

    const DynTupleSchema* key_schema() const { return &key_schema_; }
    const DynTupleSchema* schema() const     { return schema_; }

private:
    class ItemHandler : public CowHashMapItemHandler {
    public:
        ItemHandler() : index_(NULL) {}
        size_t hash_key(const void* key) const;
        void get_key(void* key, const void* item) const;
        bool eq_key_item(const void* key, const void* item) const;
        void copy_item(void* dst_item, const void* src_item) const;
        void copy_construct_item(void* dst_item, const void* src_item) const;
        void destruct_item(void*) const;
        
        DynUniqueIndex* index_;
    };
    
    BaseCowHashMap base_;
    std::vector<int> key_pos_;
    DynTupleSchema key_schema_;
    ItemHandler ih_;
};

}  // namespace st

#endif  //_DYN_UNIQUE_INDEX_H_
