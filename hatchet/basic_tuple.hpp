// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// A tuple instantiated from a list of types
// Author: gejun@baidu.com
// Date: Nov 17 20:51:36 CST 2010
#pragma once
#ifndef _BASIC_TUPLE_HPP_
#define _BASIC_TUPLE_HPP_

#include "string_writer.hpp"      // to_string
#include "c_list.hpp"             // list_* templates
#include "compare.hpp"            // valcmp(2)
#include "c_common.hpp"           // ST_SYMBOLL, ST_PARAML

namespace st {
// A primitive tuple based on a list of types and implemented by
// recursive inheritance.
// The prefix 'R' means storage of values are reversed from _TypeL. Namely
// Cons<int, Cons<long, Cons<short, void> > > is stored similarly as
// struct { short; long; int; };
template <class _TypeL> struct RBasicTuple;

// BasicTuple inherits reversed RBasicTuple so that storage order of values
// is consistent with _TypeL
template <class _TypeL> struct BasicTuple;

// Note: RBasicTuple is used extensively to instantiate a list of types
//       into values, so I intended to make it simple and quick to compile
//       Keep this in mind before adding new features.

// Implement RBasicTuple
// Only one type
template <typename _Type> struct RBasicTuple<Cons<_Type, void> > {
    // NamedTuple needs this to get ancestors
    typedef Cons<RBasicTuple,void> ClassL;  

    static const int N_ATTR = 1;  // Number of attributes

    // Get type of a position
    template <int _N> struct type_at_n {
        C_ASSERT (_N == 0, _N_must_be_zero);
        typedef _Type R;
    };

    // We need default constructor/copy constructor/operator= to make
    // RBasicTuple work correctly with bitwise fields
    RBasicTuple () {}
    ~RBasicTuple () {}
    
    RBasicTuple (const RBasicTuple& rhs) : value_(rhs.value_) {}

    RBasicTuple& operator= (const RBasicTuple& rhs)
    {
        value_ = rhs.value_;
        return *this;
    }
    
    // tup1 == tup2
    bool operator== (const RBasicTuple& rhs) const
    { return value_ == rhs.value_; }

    // tup1 != tup2
    bool operator!= (const RBasicTuple& rhs) const
    { return value_ != rhs.value_; }

    // tup1 < tup2
    bool operator< (const RBasicTuple& rhs) const
    { return compare_with(rhs) < 0; }

    // tup1 > tup2
    bool operator> (const RBasicTuple& rhs) const
    { return compare_with(rhs) > 0; }
        
    // Compare two tuples
    int compare_with (const RBasicTuple& rhs) const
    { return valcmp(value_, rhs.value_); }

    // Get value of a position
    template <int _N> TCAP(type_at_n, _N)& at_n () { return value_; }

    // Get value of a position, const version
    template <int _N> const TCAP(type_at_n, _N)& at_n () const
    { return value_; }
        
    // Apply a function object to each attribute
    template <class _F> void do_map (const _F& f) { f (value_); }

    // Apply a function object to each attribute, const version
    template <class _F> void do_map (const _F& f) const { f (value_); }

    // Fold all attributes from left to right
    template <class _F, typename _State>
    _State foldl (const _F& f, const _State& s) { return f (s, value_); }
    
    // Fold all attributes from left to right, const version
    template <class _F, typename _State>
    _State foldl (const _F& f, const _State& s) const { return f (s, value_); }

    // Fold all attributes from right to left
    template <class _F, typename _State>
    _State foldr (const _F& f, const _State& s) { return f (s, value_); }
    
    // Fold all attributes from right to left, const version
    template <class _F, typename _State>
    _State foldr (const _F& f, const _State& s) const { return f (s, value_); }

public:
    // offsetof used in NamedTuple needs value_ to be public
    _Type value_;  // data at this level of inheritance
};
    
// Multiple types
template <typename _Type, class _TypeL>
struct RBasicTuple<Cons<_Type, _TypeL> > : public RBasicTuple<_TypeL> {
    // Specialization does not allow default template parameters, so
    // _Base has to be additionally defined
    typedef RBasicTuple<_TypeL> _Base;

    static const int N_ATTR = 1 + _Base::N_ATTR;  // number of attributes

    // NamedTuple needs this to know types of its ancestors
    typedef Cons<RBasicTuple, typename _Base::ClassL> ClassL;

    // We need default constructor/copy constructor/operator= to make
    // RBasicTuple work correctly with bitwise structures
    RBasicTuple () {}
    ~RBasicTuple () {}

    RBasicTuple (const RBasicTuple& rhs)
        : _Base(rhs), value_(rhs.value_)
    {}

    RBasicTuple& operator= (const RBasicTuple& rhs)
    {
        _Base::operator= (rhs);
        value_ = rhs.value_;
        return *this;
    }

    // tup1 == tup2
    bool operator== (const RBasicTuple& rhs) const
    { return _Base::operator==(rhs) && value_ == rhs.value_; }

    // tup1 != tup2
    bool operator!= (const RBasicTuple& rhs) const
    { return _Base::operator!=(rhs) || value_ != rhs.value_; }

    // tup1 < tup2
    bool operator< (const RBasicTuple& rhs) const
    { return compare_with(rhs) < 0; }

    // tup1 > tup2
    bool operator> (const RBasicTuple& rhs) const
    { return compare_with(rhs) > 0; }
        
    // strcmp(2) style comparison between two tuples
    int compare_with (const RBasicTuple& rhs) const
    {
        register int ret = _Base::compare_with(rhs);
        return ret ? ret : valcmp(value_, rhs.value_);
    }

    // Apply a function object to each attribute
    template <class _F> void do_map (const _F& f)
    {
        _Base::do_map(f);
        f(value_);
    }
    // Apply a function object to each attribute, const version
    template <class _F> void do_map (const _F& f) const
    {
        _Base::do_map(f);
        f(value_);
    }

    // Fold all attributes from left to right
    template <class _F, typename _State>
    _State foldl (const _F& f, const _State& s)
    { return f(_Base::foldl(f, s), value_); }
    
    // Fold all attributes from left to right, const version
    template <class _F, typename _State>
    _State foldl (const _F& f, const _State& s) const
    { return f(_Base::foldl(f, s), value_); }

    // Fold all attributes from right to left
    template <class _F, typename _State>
    _State foldr (const _F& f, const _State& s)
    { return _Base::foldl(f, f(s, value_)); }

    // Fold all attributes from right to left, const version
    template <class _F, typename _State>
    _State foldr (const _F& f, const _State& s) const
    { return _Base::foldl(f, f(s, value_)); }

public:
    // offsetof used in NamedTuple needs value_ to be public
    _Type value_;  // data at this level of inheritance
};

// Implement BasicTuple
template <class _TypeL> struct BasicTuple
    : public RBasicTuple<TCAP(list_reverse, _TypeL)> {
    typedef RBasicTuple<TCAP(list_reverse, _TypeL)> _Base;

    // list of inherited RBasicTuples, reversed to be consistent with _TypeL
    typedef TCAP(list_reverse, typename _Base::ClassL) ClassL;

    // Number of attributes
    static const int N_ATTR = _Base::N_ATTR;

    // Get type at a position
    template <int _N> struct type_at_n {
        C_ASSERT (_N >= 0 && _N < N_ATTR, _N_is_too_big_or_negative);
        typedef TCAP(list_at, _N, _TypeL) R;
    };

    // Default constructor and destructor, do nothing
    BasicTuple () {}
    ~BasicTuple () {}

    // Copy-constructor and operator= trivially invoke _Base's
    BasicTuple (const BasicTuple& rhs) : _Base(rhs) {}
    BasicTuple& operator= (const BasicTuple& rhs)
    {
        _Base::operator= (rhs);
        return *this;
    }
    
    // Get value at a position.
    // Note: storage layout in RBasicTuple is different from NamedTuple where
    //       at_n<0>() returns the attribute with lowest address while here
    //       at_n<0>() returns the highest address
    template <int _N> TCAP(type_at_n, _N) & at_n ()
    { return static_cast<TCAP(list_at, _N, ClassL)*>(this)->value_; }

    // Get value of an attribute at a position, const version
    template <int _N> const TCAP(type_at_n, _N)& at_n () const
    { return static_cast<const TCAP(list_at, _N, ClassL)*>(this)->value_; }
};

// BasicTuple is trivial if input is void
template <> struct BasicTuple<void> {
    template <int> struct type_at_n { typedef void R; };
    template <int _N> void at_n () const {}
};

// compile-time printing
template <class _TypeL> struct c_show_impl<BasicTuple<_TypeL> > {
    static void c_to_string (std::ostream& os)
    { os << "BasicTuple" << c_show(_TypeL); }
};


// Print RBasicTuple into StringWriter
template <class _TypeL>
StringWriter& operator<< (StringWriter& sw, const BasicTuple<_TypeL>& tup)
{
    sw << '(';
    tup.foldl (show_with_pos<StringWriter>(&sw), 0);
    sw << ')';
    return sw;
}

// Print RBasicTuple into std::ostream
template <class _TypeL>
std::ostream& operator<< (std::ostream& os, const BasicTuple<_TypeL>& tup)
{
    os << '(';
    tup.foldl (show_with_pos<std::ostream>(&os), 0);
    os << ')';
    return os;
}


// Initialize BasicTuple or NamedTuple conveniently with at most 9 parameters
#define ST_DEFINE_VARIADIC_INIT_TUPLE_AUX(n)    \
    p_tup->template at_n<n>() = a##n

#define ST_DEFINE_VARIADIC_INIT_TUPLE(n)                                \
    template <class _Tup, ST_SYMBOLL##n(typename _T)>                   \
    inline void init_tuple (_Tup* p_tup, ST_PARAML##n(const _T, & a))   \
    {                                                                   \
        C_ASSERT(_Tup::N_ATTR == n, number_of_attributes_is_not_##n);   \
        ST_APPL##n(ST_DEFINE_VARIADIC_INIT_TUPLE_AUX);                  \
    }

ST_DEFINE_VARIADIC_INIT_TUPLE(1)
ST_DEFINE_VARIADIC_INIT_TUPLE(2)
ST_DEFINE_VARIADIC_INIT_TUPLE(3)
ST_DEFINE_VARIADIC_INIT_TUPLE(4)
ST_DEFINE_VARIADIC_INIT_TUPLE(5)
ST_DEFINE_VARIADIC_INIT_TUPLE(6)
ST_DEFINE_VARIADIC_INIT_TUPLE(7)
ST_DEFINE_VARIADIC_INIT_TUPLE(8)
ST_DEFINE_VARIADIC_INIT_TUPLE(9)

}  // namespace st

#endif  //_BASIC_TUPLE_HPP_
