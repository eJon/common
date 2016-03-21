// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// A tuple instantiated from a list of distinct attributes
// Author: gejun@baidu.com
// Date: Nov 17 20:51:36 CST 2010
#pragma once
#ifndef _NAMED_TUPLE_HPP_
#define _NAMED_TUPLE_HPP_

#include "common.h"              // Cons
#include "basic_tuple.hpp"       // BasicTuple, init_tuple
#include "st_hash.h"             // hash()
#include "default_value.h"       // DEFINE_DEFAULT_VALUE
#include "attribute.h"           // DEFINE_COLUMN

namespace st {
// copy _AttrS of _SrcTup to _DestTup
template <class _DestTup, class _SrcTup, class _AttrS> struct copy_attr_list;

template <class _AttrL, char _Sep> class shows_attr_list_with_sep;

template <typename _Tup> struct show_tuple_type {
    void to_string (StringWriter& sw) const
    {
        sw << '[';
        shows_attr_list_with_sep<typename _Tup::AttrS, ','>::call(sw);
        sw << ']';
    }
};

// Pack attribute parameters into a NamedTuple, at most 12 attributes
template <class _A0,        class _A1  = void, class _A2  = void,
          class _A3 = void, class _A4  = void, class _A5  = void,
          class _A6 = void, class _A7  = void, class _A8  = void,
          class _A9 = void, class _A10 = void, class _A11 = void>
struct make_tuple;

namespace st_detail {
// to merge hash values
struct hash_value_folder {
    template <typename _Any>
    size_t operator()(size_t v, const _Any& __restrict a) const
    {
        return v * 257ul + Hash<_Any>()(a);
    }
};
}  // namespace st_detail

// A tuple instantiated from an attribute set
// Params:
//   _AttrS  a list of attributes with no duplication
//   _Base   which users generally never modify
template <class _AttrS>
class NamedTuple : public BasicTuple<TCAP(list_map, get_attr_type, _AttrS)> {
    // make sure that attributes do not repeat
    C_ASSERT(!CAP(list_dup, _AttrS), duplicated_attributes);
public:
    // inherited basic tuple
    typedef BasicTuple<TCAP(list_map, get_attr_type, _AttrS)> Base;
    
    typedef _AttrS AttrS;  // the attribute set
    
    static const int N_ATTR = Base::N_ATTR;  // number of attributes
        
    // Get sub tuple 
    template <class _SubAttrL> struct sub : public c_show_base {
        typedef NamedTuple SrcTuple;
        typedef NamedTuple<_SubAttrL> SubTuple;
        
        SubTuple operator() (const SrcTuple& t) const {
            SubTuple t2;
            copy_attr_list<SubTuple, SrcTuple, _SubAttrL>::call(&t2, t);
            return t2;
        }

        static void c_to_string (std::ostream& os)
        {
            os << "sub{from=" << c_show(SrcTuple)
               << " to=" << c_show(SubTuple) << "}";
        }
    };   

    // Pack attribute parameters into a sub, at most 9 attributes
    template <ST_SYMBOLLD9(class _B, void)> struct make_sub {
        typedef sub< TCAP(list_filter, c_not_void,
                          ST_MAKE_LIST(ST_SYMBOLL9(_B))) > R;
    };
        
    // Get offset of an attribute in this tuple
    template <class _Attr> struct offset {
        static const size_t R =
            offsetof(TCAP(list_at,
                          CAP(list_seek_first, _Attr, _AttrS),
                          typename Base::ClassL), value_);
    };

    // Default constructor and destructor, do nothing
    NamedTuple () {}
    ~NamedTuple () {}

    // Copy and Assign trivially call Base's
    NamedTuple (const NamedTuple& rhs) : Base(rhs) {}
    NamedTuple& operator= (const NamedTuple& rhs)
    {
        Base::operator=(rhs);
        return *this;
    }

    // Get position (from 0) of an attribute in this tuple
    template <class _Attr> class attr_pos
        : public list_seek_first<_Attr, _AttrS> {};

    // Get type of the attribute at _N
    template <int _N> struct type_at_n
    { typedef TCAP(list_at, _N, _AttrS)::Type R; };

    // Get value of an attribute
    template <class _Attr> typename _Attr::Type& at()
    { return static_cast<TCAP(list_at, CAP(list_seek_first, _Attr, _AttrS),
                              typename Base::ClassL)*>(this)->value_; }
        
    // Get value of an attribute, const version
    template <class _Attr> const typename _Attr::Type& at() const
    { return static_cast<const TCAP(list_at,
                                    CAP(list_seek_first, _Attr, _AttrS),
                                    typename Base::ClassL)*>(this)->value_; }

    // overload the hashing function in fun/st_hash.h
    // Note!!!! MUST BE `0ul', `0' overflows!
    size_t hash_code() const
    { return this->foldl(st_detail::hash_value_folder(), 0ul); }

    // overload the comparing function in fun/compare.hpp
friend inline int valcmp (const NamedTuple& t1, const NamedTuple& t2)
    { return t1.compare_with(t2); }

    bool operator>= (const NamedTuple& rhs) const { return rhs < *this; }
    bool operator<= (const NamedTuple& rhs) const { return rhs > *this; }
        
    // write to ostream
friend std::ostream& operator<< (std::ostream& os, const NamedTuple& t)
    {
        os << '(';
        static_cast<const Base&>(t).foldl(show_with_pos<std::ostream>(&os), 0);
        os << ')';
        return os;
    }
        
    // convert to string
    void to_string (StringWriter& sw) const
    {
        sw << '(';
        Base::foldl(show_with_pos<StringWriter>(&sw), 0);
        sw << ')';
    }

    // We need following constructors to create tuples more conveniently.
    // Although they are not evaluated until user actually calls them, they
    // are really annoying when modifying code, so I put them here bottom
    // of the class
    // Note: at most 9 parameters
#define DEFINE_NAMED_TUPLE_VARIADIC_CONSTRUCTOR(_n_)           \
    template <ST_SYMBOLL##_n_(typename _T)>                    \
    explicit NamedTuple (ST_PARAML##_n_(const _T, & a))        \
    { init_tuple(this, ST_SYMBOLL##_n_(a)); }

    DEFINE_NAMED_TUPLE_VARIADIC_CONSTRUCTOR(1)
    DEFINE_NAMED_TUPLE_VARIADIC_CONSTRUCTOR(2)
    DEFINE_NAMED_TUPLE_VARIADIC_CONSTRUCTOR(3)
    DEFINE_NAMED_TUPLE_VARIADIC_CONSTRUCTOR(4)
    DEFINE_NAMED_TUPLE_VARIADIC_CONSTRUCTOR(5)
    DEFINE_NAMED_TUPLE_VARIADIC_CONSTRUCTOR(6)
    DEFINE_NAMED_TUPLE_VARIADIC_CONSTRUCTOR(7)
    DEFINE_NAMED_TUPLE_VARIADIC_CONSTRUCTOR(8)
    DEFINE_NAMED_TUPLE_VARIADIC_CONSTRUCTOR(9)    
};

template <typename _Attr, class _AttrL, char _Sep>
struct shows_attr_list_with_sep<Cons<_Attr, _AttrL>, _Sep> {
    static void call (StringWriter& sw)
    {
        sw << _Attr::name() << _Sep;
        shows_attr_list_with_sep<_AttrL, _Sep>::call(sw);
    }
};

template <typename _Attr, char _Sep>
struct shows_attr_list_with_sep<Cons<_Attr, void>, _Sep> {
    static void call (StringWriter& sw) { sw << _Attr::name(); }
};

template <char _Sep> struct shows_attr_list_with_sep<void, _Sep>
{ static void call (StringWriter&) {} };

// Implement copy_attr_list
template <class _DestTup, class _SrcTup, class _Attr, class _AttrS>
struct copy_attr_list<_DestTup, _SrcTup, Cons<_Attr, _AttrS> > {
    static void call (_DestTup* p_t1, const _SrcTup& t2)
    {
        p_t1->template at<_Attr>() = t2.template at<_Attr>();
        copy_attr_list<_DestTup, _SrcTup, _AttrS>::call (p_t1, t2);
    }
};

template <class _DestTup, class _SrcTup>
struct copy_attr_list<_DestTup, _SrcTup, void>
{ static void call (_DestTup* , const _SrcTup&) {} };

// A tuple based on attributes as parameters
// number of paramters could be easily extended by copy-n-paste
template <class _A0, class _A1, class _A2, class _A3, class _A4, class _A5,
          class _A6, class _A7, class _A8, class _A9, class _A10, class _A11>
struct make_tuple {
    typedef NamedTuple<TCAP(list_filter,c_not_void,
                            Cons<_A0, Cons<_A1, Cons<_A2,
                            Cons<_A3, Cons<_A4, Cons<_A5,
                            Cons<_A6, Cons<_A7, Cons<_A8,
                            Cons<_A9,Cons<_A10,Cons<_A11,
                            void> > > > > > > > > > > > )> R;
};

// compile-time printing
template <class _AttrS> struct c_show_impl<NamedTuple<_AttrS> > {
    static void c_to_string (std::ostream& os) { os << c_show(_AttrS); }
};

struct set_input_to_default_value {
    template <typename _T> void operator() (_T& v) const
    { v = ST_DEFAULT_VALUE_OF(_T); }
};

template <class _AttrS> struct default_value_of<NamedTuple<_AttrS> > {
    typedef NamedTuple<_AttrS> Tup;
    static Tup R() {
        Tup tup;
        tup.do_map(set_input_to_default_value());
        return tup;
    }
};

template <class _T> struct GetAttrS { typedef class _T::AttrS R; };

template <class _AttrS>
struct Hash<NamedTuple<_AttrS> >
    : public BaseFunctionObject<size_t(NamedTuple<_AttrS>)> {
    size_t operator()(const NamedTuple<_AttrS>& v) const
    {
        return v.hash_code();
    } 
};

}  // namespace st

#endif  //_NAMED_TUPLE_HPP_
