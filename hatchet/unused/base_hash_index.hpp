// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// (smalltable1) base class of hash indexes
// Author: gejun@baidu.com
// Date: Wed Aug 11 14:57:47 2010
#pragma once
#ifndef _BASE_HASH_INDEX_HPP_
#define _BASE_HASH_INDEX_HPP_

#include "common.h"
#include "header.hpp"
#include "predicate.hpp"

namespace st {

// The default hash_size to initialize indexes.
// Indexes in smalltable are safe to automatically resize, so this
// default value is mainly to reduce frequent resizing with same sizes
static const int DEFAULT_HASH_SIZE = 10007;

enum { HI_NONE,
       HI_SEEK,
       HI_COWBASS,
       HI_COWBASS_PTR,
       HI_SLOWER,
       HI_BG_COWBASS,
       HI_BG_COWBASS_PTR,
       HI_TRAVERSE,
       HI_BG_TRAVERSE };

#define HI_MARK_SLOWER(_type_) (((_type_) << 16) | HI_SLOWER)

#define HI_SUB_TYPE(_type_) ((_type_) >> 16)

#define HI_TYPE(_type_) ((_type_) & 0xFFFF)


template <typename _Tup, typename _RefTup=typename _Tup::RefTuple>
class BaseHashIndex {
public:
    typedef _Tup Tup;
    
    typedef _RefTup RefTup;
    
    typedef BaseHashIndex<_Tup, _RefTup> Self;
        
    BaseHashIndex() {}

    virtual ~BaseHashIndex () {}
        
    // Get element number of the buffer
    virtual size_t size (int buf_no) const = 0;
    
    size_t fg_size () const { return size (fg_no()); }

    size_t bg_size () const { return size (bg_no()); }
    
    // the buffer has element or not
    virtual bool empty (int buf_no) const = 0;
        
    // internal information
    virtual void to_string (StringWriter& sb) const = 0;
        
    // insert an element into the index
    virtual std::pair<const Tup*, const Tup*>
    insert (const Tup& tuple) = 0;

    // erase an element from the index
    virtual const Tup* erase (const Tup& tuple) = 0;

    virtual void forward_slower_iterator(void* p_it, int slower_type) const = 0;

    virtual const std::vector<const Tup*>*
    erase_by (const Tup& tuple, const std::vector<Predicate*>* pa_filter) = 0;

    virtual const std::vector<const Tup*>*
    erase_ref_by (const ObjectHanger& oh, const RefTup& tuple,
                  const std::vector<Predicate*>* pa_filter) = 0;

    virtual const std::vector<const Tup*>*
    erase_all_by (const std::vector<Predicate*>* pa_filter) = 0;

    virtual void clear () = 0;
        
    virtual int begin_update () = 0;
        
    virtual int end_update () = 0;

    virtual void recycle_delayed_memory () = 0;

    //----------non-virtual methods -----------//
    void create (const SimpleVersion* p_ver) { p_ver_ = p_ver; }

    int fg_no () const { return p_ver_->version() & 1; }
        
    int bg_no () const { return (p_ver_->version() + 1) & 1; }

    virtual size_t mem () const = 0;

public:
    // test if the index is primary or not
    bool primary() const { return primary_; }
        
    const Header& header() const { return header_; }

    const Header& table_header() const { return table_header_; }
                            
protected:
    Header header_;
    Header table_header_;
    const SimpleVersion* p_ver_;
    bool primary_;
};

}

#endif  //_BASE_HASH_INDEX_HPP_
