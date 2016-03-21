// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// A helper class to wrap a single pointer into an iterator
// Author: gejun@baidu.com
// Date: Dec.7 14:10:24 CST 2010
#pragma once
#ifndef _POINTER_ITERATOR_HPP_
#define _POINTER_ITERATOR_HPP_

#include "string_writer.hpp"   // to_string

namespace st {
template <typename _Value,
          typename _Pointer = const _Value*,
          typename _Reference = const _Value&>
class PointerIterator {
public:
    typedef _Value Value;
    typedef _Pointer Pointer;
    typedef _Reference Reference;
    
    // Default constructor
    PointerIterator () { p_value_ = Pointer(); }

    // Construct from a const pointer
    explicit PointerIterator (Pointer p_value) : p_value_(p_value) {}

    // *it
    Reference operator* () const { return *p_value_; }

    // it->
    Pointer operator-> () const { return p_value_; }

    // ++it
    PointerIterator& operator++ ()
    {
        p_value_ = Pointer();
        return *this;
    }

    // it++
    PointerIterator& operator++ (int)
    {
        PointerIterator tmp = *this;
        p_value_ = Pointer();
        return tmp;
    }

    // Cast to const _Value* implicitly
    operator Pointer () const { return p_value_; }

    // ugly stuff
    operator bool () const { return p_value_; }
    void set_end () { p_value_ = Pointer(); }

    // Write to StringWriter
    void to_string (StringWriter& sw) const
    { sw << "PtrIter{p_value=" << p_value_ << "}"; } 

    void set_value (Pointer p_value)
    { p_value_ = p_value; }
        
private:
    Pointer p_value_;  // the wrapped pointer
};

}  // namespace st

#endif  //_POINTER_ITERATOR_HPP_

