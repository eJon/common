// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// A tuple combined from two NamedTuples, used in CowHashClusterMap to 
// mock tuple of hosting table with two tuples from CowHashMap and Cowbass4
// in the map.
// Author: gejun@baidu.com
// Date: Nov 17 20:51:36 CST 2010
#pragma once
#ifndef _COMBINED_TUPLE_HPP_
#define _COMBINED_TUPLE_HPP_

#include "named_tuple.hpp"       // NamedTuple

namespace st {
template <class _NamedTuple1, class _NamedTuple2>
class CombinedTuple {
public:
    typedef class _NamedTuple1::AttrS AttrS1;
    typedef class _NamedTuple2::AttrS AttrS2;
    typedef TCAP(list_concat, AttrS1, AttrS2) AttrS;
    static const int N_ATTR = _NamedTuple1::N_ATTR + _NamedTuple2::N_ATTR;

    CombinedTuple () : p_tup1_(NULL), p_tup2_(NULL) {}

    CombinedTuple (const _NamedTuple1* p_tup1, const _NamedTuple2* p_tup2)
        : p_tup1_(p_tup1), p_tup2_(p_tup2) {}

    ~CombinedTuple () {}

    CombinedTuple (const CombinedTuple& rhs)
        : p_tup1_(rhs.p_tup1_) , p_tup2_(rhs.p_tup2_) {}
    
    CombinedTuple& operator= (const CombinedTuple& rhs)
    {
        p_tup1_ = rhs.p_tup1_;
        p_tup2_ = rhs.p_tup2_;
        return *this;
    }

    // Get position (from 0) of an attribute in this tuple
    template <class _Attr> class attr_pos
        : public list_seek_first<_Attr, AttrS> {};

    // Get type of the attribute at _N
    template <int _N> struct type_at_n
    { typedef TCAP(list_at, _N, AttrS)::Type R; };

    // Get value of an attribute
    template <class _Attr> const typename _Attr::Type& at () const
    {
        enum { POS1 = CAP(list_seek_first, _Attr, AttrS1) };
        return CAP(if_, (POS1 >= 0),
                   value1_at_n<_Attr, POS1>,
                   value2_at<_Attr>)::call(p_tup1_, p_tup2_);
    }

    template <class _Attr, int _N> struct value1_at_n {
        static const typename _Attr::Type& call (
            const _NamedTuple1* p_tup1, const _NamedTuple2*)
        {
            return static_cast<
                const TCAP(list_at, _N,
                           class _NamedTuple1::Base::ClassL)*>(p_tup1)->value_;
        }
    };

    template <class _Attr> struct value2_at {
        static const typename _Attr::Type& call (
            const _NamedTuple1*, const _NamedTuple2* p_tup2)
        { return p_tup2->at<_Attr>(); }
    };

    // overload the hashing function in fun/st_hash.h
friend inline size_t hash(const CombinedTuple& t)
    { return hash(*t.p_tup1_) * 257 + hash(*t.p_tup2_); }

    // overload the comparing function in fun/compare.hpp
friend inline int valcmp (const CombinedTuple& t1, const CombinedTuple& t2)
    {
        int ret = valcmp(*t1.p_tup1_, *t2.p_tup1_);
        return ret != 0 ? ret : valcmp(*t1.p_tup2_, *t2.p_tup2_);
    }

    bool operator== (const CombinedTuple& rhs) const
    { return *p_tup1_ == *rhs.p_tup1_ && *p_tup2_ == *rhs.p_tup2_; }

    bool operator!= (const CombinedTuple& rhs) const
    { return *p_tup1_ != *rhs.p_tup1_ || *p_tup2_ != *rhs.p_tup2_; }

    bool operator< (const CombinedTuple& rhs) const
    { return valcmp(*this, rhs) < 0; }

    bool operator> (const CombinedTuple& rhs) const
    { return valcmp(*this, rhs) > 0; }
    
    bool operator>= (const CombinedTuple& rhs) const
    { return rhs.operator<(*this); }

    bool operator<= (const CombinedTuple& rhs) const
    { return rhs.operator>(*this); }
        
    // write to ostream
    template <class _Stream>
    _Stream& generic_to_string (_Stream& s) const
    {
        s << '(';
        if (p_tup1_) {
            s << *p_tup1_;
        } else {
            s << "(nil)";
        }
        s << ',';
        if (p_tup2_) {
            s << *p_tup2_;
        } else {
            s << "(nil)";
        }
        s << ')';
        return s;
    }

friend std::ostream& operator<< (std::ostream& os, const CombinedTuple& t)
    { return t.generic_to_string<std::ostream>(os); }
    
    // convert to string
    void to_string (StringWriter& sw) const
    { return this->generic_to_string<StringWriter>(sw); }

    bool valid () const { return NULL != p_tup1_ && NULL != p_tup2_; }

private:
    const _NamedTuple1* p_tup1_;
    const _NamedTuple2* p_tup2_;
};

// compile-time printing
template <class _Tup1, class _Tup2>
struct c_show_impl<CombinedTuple<_Tup1, _Tup2> > {
    static void c_to_string (std::ostream& os)
    { os  << '{' << c_show(_Tup1) << ',' << c_show(_Tup2) << '}'; }
};

}  // namespace st

#endif  //_COMBINED_TUPLE_HPP_
