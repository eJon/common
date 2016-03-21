// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Interfaces to form a dynamic index
// Author: gejun@baidu.com
// Date: Dec.27 15:28:29 CST 2010
#pragma once
#ifndef _DYN_INDEX_H_
#define _DYN_INDEX_H_

#include <vector>              // std::vector
#include <string>              // std::string
#include "dyn_tuple.h"         // DynTuple
#include "index_traits.hpp"    // compile-time index commons

namespace st {

struct DynSeekInfo {
    const DynTupleSchema* key_schema;
    int seek_score;
    int partial_seek_score;

friend std::ostream& operator<<(std::ostream& os, const DynSeekInfo& si)
    {
        os << "SeekInfo(" << si.seek_score
           << '/' << si.partial_seek_score << "):";
        if (si.key_schema) {
            os << *(si.key_schema);
        } else {
            os << "Nil";
        }
        return os;
    }
    
};

class DynIterator {
public:
    virtual ~DynIterator() {}
    virtual const void* setup() = 0;
    virtual const void* forward() = 0;
};

// TODO: Find better name
class DynNormalIterator {
public:
    DynNormalIterator(const DynTupleSchema* schema, DynIterator* it)
        : it_(it)
    {
        dt_.set_schema(schema);
        dt_.set_buf(it_ ? (void*)it_->setup() : NULL);
    }

    DynNormalIterator(const DynNormalIterator& rhs)
        : it_(rhs.it_)
    {
        dt_.set_schema(rhs.dt_.schema());
        dt_.set_buf((void*)rhs.dt_.buf());
    }

    DynNormalIterator& operator++()
    {
        dt_.set_buf((void*)it_->forward());
        return *this;
    }

    operator const void*() const { return (void*)dt_.valid(); }
    const DynTuple& operator*() const { return dt_; }
    const DynTuple* operator->() const { return &dt_; }

private:
    DynIterator* it_;
    DynTuple dt_;
};

class DynIndex {
public:
    DynIndex() : schema_(NULL) {}
    virtual ~DynIndex() {}

    // Copy content from `rhs' which should be same type/schema with this DynIndex
    // Returns:
    //   0             success
    //   EINVAL        type/schema of `rhs' does not match
    virtual int copy_from(const DynIndex& rhs) = 0;
    virtual int try_copy_from(const DynIndex& base_rhs) const = 0;
    
    // Clone an DynIndex with exactly same type/schema/content
    // Returns: NULL for not enough memory
    virtual DynIndex* clone() const = 0;
    
    // Insert a tuple
    // Returns: address of the inserted item, NULL if failed
    virtual void* insert(const DynTuple&) = 0;
         
    // Get DynSeekInfos
    // seek info describes which attributes are sped up by this index
    virtual std::vector<DynSeekInfo> seek_info_list() const = 0;

    // Erase tuples matching key of the seek info
    // Returns:
    //   Number of erased elements
    virtual size_t erase(int seek_info_idx, const DynTuple&) = 0;

    // Search tuples matching `key' of `si_pos'th seekinfo
    // Returns: an iterator to found tuples
    virtual size_t seek_mem_size() const = 0;
    virtual DynIterator* seek(int si_pos, const DynTuple& key) const = 0;
    virtual DynIterator* seek(int si_pos, const DynTuple& key, void*) const = 0;

    virtual DynIterator* maybe_seek(int, const DynTuple&) const = 0;
    virtual DynIterator* exclude_seek(int, const DynTuple&) const = 0;

    // Get number of different keys of the seek info
    virtual size_t key_num(int seek_info_idx) const = 0;
    
    // Get iterator to all tuples
    virtual DynIterator* all() const = 0;
    
    // Erase all tuples
    virtual void clear() = 0;
    
    virtual bool copy_key_part(DynTuple* p_dst, const DynTuple& src) const = 0;
    
    virtual DynSeekInfo uniqueness() const = 0;
    virtual void* seek_tuple(const DynTuple&) const = 0;
    virtual bool erase_tuple(const DynTuple&) = 0;
    
    virtual bool resize(size_t nbucket2) = 0;
    virtual bool not_init() const = 0;
    virtual bool empty() const = 0;
    virtual size_t size() const = 0;
    virtual size_t capacity() const = 0;
    virtual size_t mem() const = 0;
    virtual void serialize(std::ostream&) const = 0;
    const DynTupleSchema* schema() const { return schema_; }
    void set_schema(const DynTupleSchema* schema) { schema_ = schema; }
    
friend std::ostream& operator<<(std::ostream& os, const DynIndex& i)
    {
        i.serialize(os);
        return os;
    }    

friend std::ostream& operator<<(std::ostream& os, const DynIndex* i)
    {
        if (i) {
            i->serialize(os);
        } else {
            os << "(nil)";
        }
        return os;
    }

protected:
    const DynTupleSchema* schema_;
};

}  // namespace st

#endif  //_DYN_INDEX_H_
