// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// A helper iterator switching between iteration and default value
// Author: gejun@baidu.com
// Date: Mon. Feb. 28 20:44:57 CST 2011
#pragma once
#ifndef _PARTIAL_ITERATOR_HPP_
#define _PARTIAL_ITERATOR_HPP_

#include "pointer_iterator.hpp"       // PointerIterator
#include "string_writer.hpp"          // to_string
#include "default_value.h"            // ST_DEFAULT_VALUE_OF
#include "cow_hash_cluster_set.hpp"   // HashOneClusterIterator

namespace st {

template <class _Iterator> class PartialIterator {
public:
    typedef _Iterator Iterator;
    typedef typename Iterator::Value Value;
    typedef typename Iterator::Pointer Pointer;
    typedef typename Iterator::Reference Reference;
    
    C_ASSERT_NOT_SAME(_Iterator, Value,
                      iterator_and_value_must_be_different_types);
    
    PartialIterator () : value_(ST_DEFAULT_VALUE_OF(Value)) {}

    Iterator& iterator() { return it_; }
    Value& value() { return value_; }
    
    void point_to_iterator () { ptr_ = it_.operator->(); }
    void point_to_value () { ptr_ = &value_; }

    Reference operator* () const { return *ptr_; }
    Pointer operator-> () const { return ptr_; }

    PartialIterator& operator++ ()
    {
        if (ptr_ == &value_) {
            set_end();
        } else {
            ++ it_;
            ptr_ = it_.operator->();
        }
        return *this;
    }

    PartialIterator operator++ (int)
    {
        PartialIterator tmp = *this;
        operator++();
        return tmp;
    }

    operator bool () const { return ptr_ != NULL; }
    void set_end () { ptr_ = NULL; }

protected:
    Pointer ptr_;
    Iterator it_;
    Value value_;
};

template <class _Map> class PartialIterator<HashOneClusterIterator<_Map> > {
public:
    typedef typename _Map::Cluster Cluster;
    typedef HashOneClusterIterator<_Map> Iterator;
    typedef typename Iterator::Value Value;
    typedef typename Iterator::Pointer Pointer;
    typedef typename Iterator::Reference Reference;
    
    C_ASSERT_NOT_SAME(Iterator, Value,
                      iterator_and_value_must_be_different_types);
    
    PartialIterator () {}

    typename Cluster::Iterator& iterator() { return cit_.iterator(); }
    typename _Map::HKey& hkey() { return hkey_; }
    
    void point_to_iterator () { cit_.point_to_iterator(); }
    void point_to_value () { cit_.point_to_value(); }

    Reference operator* () const { return Reference(hkey_, cit_.operator*()); }
    Pointer operator-> () const { return Pointer(&hkey_, cit_.operator->()); }

    PartialIterator& operator++ ()
    {
        ++ cit_;
        return *this;
    }

    PartialIterator operator++ (int)
    {
        PartialIterator tmp = *this;
        operator++();
        return tmp;
    }

    operator bool () const { return cit_; }
    void set_end () { cit_.set_end(); }

protected:
    typename _Map::HKey hkey_;
    PartialIterator<typename Cluster::Iterator> cit_;
};

// Specialize for PointerIterator because ptr_ could be removed and logic
// could be simplified
template <typename _Item> class PartialIterator<PointerIterator<_Item> > {
public:
    typedef PointerIterator<_Item> Iterator;
    typedef typename Iterator::Value Value;
    typedef typename Iterator::Pointer Pointer;
    typedef typename Iterator::Reference Reference;
    
    PartialIterator () : value_(ST_DEFAULT_VALUE_OF(Value)) {}

    Iterator& iterator() { return it_; }
    Value& value() { return value_; }
    
    void point_to_iterator () {}
    void point_to_value () { it_.set_value(&value_); }

    Reference operator* () const { return it_.operator*(); }
    Pointer operator-> () const { return it_.operator->(); }

    PartialIterator& operator++ ()
    {
        it_.set_end();
        return *this;
    }

    PartialIterator operator++ (int)
    {
        PartialIterator tmp = *this;
        operator++();
        return tmp;
    }

    operator bool () const { return it_; }
    void set_end () { it_.set_end(); }

protected:
    Iterator it_;
    Value value_;
};


}  // namespace st

#endif  //_PARTIAL_ITERATOR_HPP_
