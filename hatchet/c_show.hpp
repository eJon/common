// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Make types printable to std::ostream
// Author: gejun@baidu.com
// Date: Dec 4 15:08:43 CST 2010
#pragma once
#ifndef _C_SHOW_HPP_
#define _C_SHOW_HPP_

#include "is_base_of.hpp"
#include <iostream>

namespace st {
// There're 3 ways to make a type printable, say the type is call _T :
// 1) Specialize c_show_impl:
//    template <...> struct c_show_impl<_T> {
//        static void c_to_string (std::ostream& os)
//        { os << ... }
//    };
// 2) Inherit c_show_base and implement a static method "c_to_string(ostream&)"
//    struct _T : public c_show_base {
//        ...
//        static void c_to_string (std::ostream& os)
//        { os << ... }
//    };
// 3) Inherit c_show_as
//    struct _T : public c_show_as<...> {
//        ...
//    };
//
// To actually send a type to ostream, just wrap the type with c_show
// e.g. std::cout << c_show(the_type) ...

struct c_show_base {};

template <typename _A> struct c_show_impl;

struct CShowNone : public c_show_base
{ static void c_to_string (std::ostream&) {} };

template <typename _A1 = CShowNone, typename _A2 = CShowNone,
          typename _A3 = CShowNone, typename _A4 = CShowNone,
          typename _A5 = CShowNone, typename _A6 = CShowNone,
          typename _A7 = CShowNone, typename _A8 = CShowNone,
          typename _A9 = CShowNone, typename _A10 = CShowNone,
          typename _A11 = CShowNone, typename _A12 = CShowNone,
          typename _A13 = CShowNone, typename _A14 = CShowNone,
          typename _A15 = CShowNone, typename _A16 = CShowNone>
struct c_show_as;

#define c_show_multiple(...) st::c_make_show<c_show_as< __VA_ARGS__ > >()

#define c_show(_type_) st::c_make_show< _type_ >()


// -------------- Implementation -------------------


template <typename, bool> struct c_show_impl_showable;

template <typename _A>
struct c_show_impl_showable<_A, true> : public c_show_base
{ static void c_to_string (std::ostream& os) { _A::c_to_string (os); } };

template <typename _A>
struct c_show_impl_showable<_A, false> : public c_show_base
{ static void c_to_string (std::ostream& os) { os << typeid(_A).name(); } };

template <typename _A> struct c_show_impl
    : public c_show_impl_showable<_A, is_base_of<c_show_base, _A>::R > {};

template <typename _A1, typename _A2, typename _A3, typename _A4,
          typename _A5, typename _A6, typename _A7, typename _A8,
          typename _A9, typename _A10, typename _A11, typename _A12,
          typename _A13, typename _A14, typename _A15, typename _A16>
struct c_show_as : public c_show_base {
    static void c_to_string (std::ostream& os)
    {
        c_show_impl<_A1>::c_to_string (os);
        c_show_impl<_A2>::c_to_string (os);
        c_show_impl<_A3>::c_to_string (os);
        c_show_impl<_A4>::c_to_string (os);
        c_show_impl<_A5>::c_to_string (os);
        c_show_impl<_A6>::c_to_string (os);
        c_show_impl<_A7>::c_to_string (os);
        c_show_impl<_A8>::c_to_string (os);
        c_show_impl<_A9>::c_to_string (os);
        c_show_impl<_A10>::c_to_string (os);
        c_show_impl<_A11>::c_to_string (os);
        c_show_impl<_A12>::c_to_string (os);
        c_show_impl<_A13>::c_to_string (os);
        c_show_impl<_A14>::c_to_string (os);
        c_show_impl<_A15>::c_to_string (os);
        c_show_impl<_A16>::c_to_string (os);
    }
};

template <typename _A> struct c_make_show {
friend std::ostream& operator<< (std::ostream& os, const c_make_show&)
    {
        c_show_impl<_A>::c_to_string (os);
        return os;
    }
};

}
#endif  //_C_SHOW_HPP_
