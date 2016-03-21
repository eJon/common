// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Common stuff of meta-programming
// Author: gejun@baidu.com
// Date: Dec. 4 15:34:50 CST 2010
#pragma once
#ifndef _FUN_C_COMMON_HPP_
#define _FUN_C_COMMON_HPP_

#include <string>
#include <typeinfo>
#include "compile_time_assert.h"
#include "is_base_of.hpp"
#include "c_show.hpp"
#include "choose_variadic.h"

// Apply templates conveniently. Writing as "Func<Arg1, Arg2...>::R" is
// generally longer and may have compilation issues occasionally. with this
// macro, the previous invocation is written as "CAP(Func, Arg1, Arg2...)",
// more readably. 'CAP' is short for 'Compile-time APplication'.
// TCAP adds a 'typename' before CAP because 'typename' is required before
// a dependent type bound by template parameters
#define CAP(_name_, ...) _name_<__VA_ARGS__>::R
#define TCAP(_name_, ...) typename _name_<__VA_ARGS__>::R

// Get partial templates
#define HAP(_name_, ...) _name_<__VA_ARGS__>::HR
#define THAP(_name_, ...) _name_<__VA_ARGS__>::template HR

// Following macros help to write variadic template functions more easily
// Declare a list of symbol ended with consecutive integers from 0
#define ST_SYMBOLL1(s)  s##0
#define ST_SYMBOLL2(s)  s##0, s##1
#define ST_SYMBOLL3(s)  s##0, s##1, s##2
#define ST_SYMBOLL4(s)  s##0, s##1, s##2, s##3
#define ST_SYMBOLL5(s)  s##0, s##1, s##2, s##3, s##4
#define ST_SYMBOLL6(s)  s##0, s##1, s##2, s##3, s##4, s##5
#define ST_SYMBOLL7(s)  s##0, s##1, s##2, s##3, s##4, s##5, \
        s##6
#define ST_SYMBOLL8(s)  s##0, s##1, s##2, s##3, s##4, s##5, \
        s##6, s##7
#define ST_SYMBOLL9(s)  s##0, s##1, s##2, s##3, s##4, s##5, \
        s##6, s##7, s##8
#define ST_SYMBOLL10(s)  s##0, s##1, s##2, s##3, s##4, s##5, \
        s##6, s##7, s##8, s##9
#define ST_SYMBOLL11(s)  s##0, s##1, s##2, s##3, s##4, s##5, \
        s##6, s##7, s##8, s##9, s##10
#define ST_SYMBOLL12(s)  s##0, s##1, s##2, s##3, s##4, s##5, \
        s##6, s##7, s##8, s##9, s##10, s##11

#define ST_SYMBOLL1P(s)  s##0*
#define ST_SYMBOLL2P(s)  s##0 *, s##1*
#define ST_SYMBOLL3P(s)  s##0 *, s##1*, s##2*
#define ST_SYMBOLL4P(s)  s##0 *, s##1*, s##2*, s##3*
#define ST_SYMBOLL5P(s)  s##0 *, s##1*, s##2*, s##3*, s##4*
#define ST_SYMBOLL6P(s)  s##0 *, s##1*, s##2*, s##3*, s##4*, s##5*
#define ST_SYMBOLL7P(s)  s##0 *, s##1*, s##2*, s##3*, s##4*, s##5*, \
        s##6*
#define ST_SYMBOLL8P(s)  s##0 *, s##1*, s##2*, s##3*, s##4*, s##5*, \
        s##6 *, s##7*
#define ST_SYMBOLL9P(s)  s##0 *, s##1*, s##2*, s##3*, s##4*, s##5*, \
        s##6 *, s##7*, s##8*
#define ST_SYMBOLL10P(s)  s##0 *, s##1*, s##2*, s##3*, s##4*, s##5*, \
        s##6 *, s##7*, s##8*, s##9*
#define ST_SYMBOLL11P(s)  s##0 *, s##1*, s##2*, s##3*, s##4*, s##5*, \
        s##6 *, s##7*, s##8*, s##9*, s##10*
#define ST_SYMBOLL12P(s)  s##0 *, s##1*, s##2*, s##3*, s##4*, s##5*, \
        s##6 *, s##7*, s##8*, s##9*, s##10*, s##11*

#define ST_SYMBOLLD1(s,d)  s##0 = d
#define ST_SYMBOLLD2(s,d)  s##0 = d, s##1 = d
#define ST_SYMBOLLD3(s,d)  s##0 = d, s##1 = d, s##2 = d
#define ST_SYMBOLLD4(s,d)  s##0 = d, s##1 = d, s##2 = d, s##3 = d
#define ST_SYMBOLLD5(s,d)  s##0 = d, s##1 = d, s##2 = d, s##3 = d,      \
        s##4 = d
#define ST_SYMBOLLD6(s,d)  s##0 = d, s##1 = d, s##2 = d, s##3 = d,      \
        s##4 = d, s##5 = d
#define ST_SYMBOLLD7(s,d)  s##0 = d, s##1 = d, s##2 = d, s##3 = d,      \
        s##4 = d, s##5 = d, s##6 = d
#define ST_SYMBOLLD8(s,d)  s##0 = d, s##1 = d, s##2 = d, s##3 = d,      \
        s##4 = d, s##5 = d, s##6 = d, s##7 = d
#define ST_SYMBOLLD9(s,d)  s##0 = d, s##1 = d, s##2 = d, s##3 = d,      \
        s##4 = d, s##5 = d, s##6 = d, s##7 = d, s##8 = d
#define ST_SYMBOLLD10(s,d)  s##0 = d, s##1 = d, s##2 = d, s##3 = d,     \
        s##4 = d, s##5 = d, s##6 = d, s##7 = d, s##8 = d, s##9 = d
#define ST_SYMBOLLD11(s,d)  s##0 = d, s##1 = d, s##2 = d, s##3 = d, \
        s##4 = d, s##5 = d, s##6 = d, s##7 = d, s##8 = d, s##9 = d, \
        s##10 = d
#define ST_SYMBOLLD12(s,d)  s##0 = d, s##1 = d, s##2 = d, s##3 = d, \
        s##4 = d, s##5 = d, s##6 = d, s##7 = d, s##8 = d, s##9 = d, \
        s##10 = d, s##11 = d

#define ST_APPL1(s)  s(0)
#define ST_APPL2(s)  s(0), s(1)
#define ST_APPL3(s)  s(0), s(1), s(2)
#define ST_APPL4(s)  s(0), s(1), s(2), s(3)
#define ST_APPL5(s)  s(0), s(1), s(2), s(3), s(4)
#define ST_APPL6(s)  s(0), s(1), s(2), s(3), s(4), s(5)
#define ST_APPL7(s)  s(0), s(1), s(2), s(3), s(4), s(5),        \
        s(6)
#define ST_APPL8(s)  s(0), s(1), s(2), s(3), s(4), s(5),        \
        s(6), s(7)
#define ST_APPL9(s)  s(0), s(1), s(2), s(3), s(4), s(5),        \
        s(6), s(7), s(8)
#define ST_APPL10(s)  s(0), s(1), s(2), s(3), s(4), s(5),       \
        s(6), s(7), s(8), s(9)
#define ST_APPL11(s)  s(0), s(1), s(2), s(3), s(4), s(5),       \
        s(6), s(7), s(8), s(9), s(10)
#define ST_APPL12(s)  s(0), s(1), s(2), s(3), s(4), s(5),       \
        s(6), s(7), s(8), s(9), s(10), s(11)

// Declare a list of parameters whose types and variables are both ended 
// with consecutive integers from 0
#define ST_PARAML1(_b_, _a_)  _b_##0 _a_##0
#define ST_PARAML2(_b_, _a_)  _b_##0 _a_##0, _b_##1 _a_##1
#define ST_PARAML3(_b_, _a_)  _b_##0 _a_##0, _b_##1 _a_##1, _b_##2 _a_##2
#define ST_PARAML4(_b_, _a_)  _b_##0 _a_##0, _b_##1 _a_##1, _b_##2 _a_##2, \
        _b_##3 _a_##3
#define ST_PARAML5(_b_, _a_)  _b_##0 _a_##0, _b_##1 _a_##1, _b_##2 _a_##2, \
        _b_##3 _a_##3, _b_##4 _a_##4
#define ST_PARAML6(_b_, _a_)  _b_##0 _a_##0, _b_##1 _a_##1, _b_##2 _a_##2, \
        _b_##3 _a_##3, _b_##4 _a_##4, _b_##5 _a_##5
#define ST_PARAML7(_b_, _a_)  _b_##0 _a_##0, _b_##1 _a_##1, _b_##2 _a_##2, \
        _b_##3 _a_##3, _b_##4 _a_##4, _b_##5 _a_##5,                    \
        _b_##6 _a_##6
#define ST_PARAML8(_b_, _a_)  _b_##0 _a_##0, _b_##1 _a_##1, _b_##2 _a_##2, \
        _b_##3 _a_##3, _b_##4 _a_##4, _b_##5 _a_##5,                    \
        _b_##6 _a_##6, _b_##7 _a_##7
#define ST_PARAML9(_b_, _a_)  _b_##0 _a_##0, _b_##1 _a_##1, _b_##2 _a_##2, \
        _b_##3 _a_##3, _b_##4 _a_##4, _b_##5 _a_##5,                    \
        _b_##6 _a_##6, _b_##7 _a_##7, _b_##8 _a_##8
#define ST_PARAML10(_b_, _a_)  _b_##0 _a_##0, _b_##1 _a_##1, _b_##2 _a_##2, \
        _b_##3 _a_##3, _b_##4 _a_##4, _b_##5 _a_##5,                    \
        _b_##6 _a_##6, _b_##7 _a_##7, _b_##8 _a_##8,                    \
        _b_##9 _a_##9
#define ST_PARAML11(_b_, _a_)  _b_##0 _a_##0, _b_##1 _a_##1, _b_##2 _a_##2, \
        _b_##3 _a_##3, _b_##4 _a_##4, _b_##5 _a_##5,                    \
        _b_##6 _a_##6, _b_##7 _a_##7, _b_##8 _a_##8,                    \
        _b_##9 _a_##9, _b_##10 _a_##10
#define ST_PARAML12(_b_, _a_)  _b_##0 _a_##0, _b_##1 _a_##1, _b_##2 _a_##2, \
        _b_##3 _a_##3, _b_##4 _a_##4, _b_##5 _a_##5,                    \
        _b_##6 _a_##6, _b_##7 _a_##7, _b_##8 _a_##8,                    \
        _b_##9 _a_##9, _b_##10 _a_##10, _b_##11 _a_##11


// Cons are outside namespace of smalltable, because it frequently appear
// in compilation log where the prefixes "st::" overwhelm screen.
// 
// Basic joint to form an inductive list. A valid list is formed by Cons 
// and ended by void
// Example: Cons<T1, Cons<T2, void> > creates a list with two elements(T1 
//          and T2). 
// Note: There's no typedef of _Head and _Tail inside the class because 
//       pattern matching is usually more convenient.
template <typename _Head, typename _Tail> struct Cons {};

namespace st {
// compile-time pair
template <typename _C1, typename _C2> struct Pair {
    typedef _C1 First;
    typedef _C2 Second;
};

// compile-time vector with 3 elements, a cousin of Pair
template <typename _C1, typename _C2, typename _C3>
struct Vector3 {
    typedef _C1 First;
    typedef _C2 Second;
    typedef _C3 Third;
};

// Wrap characters into types
template <char _C> struct Char { static const char R = _C; };

// Wrap integers into types
template <int _N> struct Int { static const int R = _N; };

// Wrap u_int into types
template <u_int _N> struct UInt { static const u_int R = _N; };

template <class _Any> struct value_of { enum { R = _Any::R }; };

// test if _Any is void
template <typename _Any> struct c_void;

// test if input is not void
template <typename _Any> struct c_not_void;

// // Returns _Rep if _T is void, otherwise _T
// template <typename _T, typename _Rep> struct void_replace;

// Returns _Then if _Condition is true, otherwise _Else
template <bool _Condition, typename _Then, typename _Else> struct if_;

// Returns _Then if _Type is void, otherwise _Else
template <typename _Type, typename _Then, typename _Else> struct if_void;

// Returns equality of inputs
template <typename _T1, typename _T2> struct c_same;

// Returns inequality of inputs
template <typename _T1, typename _T2> struct c_not_same;

// Compile-time identity, mainly used for making evaluation lazy.
// Note: in if_<condition, then, else>::R, where then or else has really 
//       high probability having a "::R" as their endings because evaluation
//       of compile-time functions in smalltable takes form of ct_fun<...>::R.
//       "::R" forces G++ to evaluate then/else before if_ takes place which
//       may bring heavy workload for compilation, but G++ guarantees that
//       ct_fun<...> is not evaluated before accessing to its internal stuff.
// Example: We may rewrite branches like if_<condition, then, else::R>::R to
//          if_<condition, c_identity<then>, else>::R::R to make it lazy
template <typename _Any> struct c_identity { typedef _Any R; };

// extract first part, generally used in higher-order templates
template <typename _P> struct pair_first { typedef typename _P::First R; };

// extract first part, generally used in higher-order templates
template <typename _P> struct pair_second { typedef typename _P::Second R; };

// extract first part, generally used in higher-order templates
template <typename _P> struct pair_third { typedef typename _P::Third R; };

// Get const pointer to input type
template <typename _Any> struct ConstPointerOf { typedef const _Any* R; };

// Get pointer of input type
template <typename _Any> struct PointerOf { typedef _Any* R; };

// Get dereferenced type of input type
template <typename _Any> struct content_type_of;
template <typename _Any> struct content_type_of<_Any*> { typedef _Any R; };
template <typename _Any> struct content_type_of<const _Any*> { typedef _Any R; };

// Get reference of dereferenced type of input type
template <typename _Any> struct content_type_ref_of;
template <typename _Any> struct content_type_ref_of<_Any*> { typedef _Any& R; };


// Produce a template checking if the parameter type has _Tag internally
// Note: _Host must have ::Tag
template <class _Tag> struct is_tagged_with {
    template <class _Host> struct HR {
        static const bool R = CAP(c_same, class _Host::Tag, _Tag);
    };
};

// -------------------
//   Implementations
// -------------------

// c_void
template <> struct c_void<void> { enum { R = true }; };
template <typename _Any> struct c_void { enum { R = false }; };

// c_not_void
template <> struct c_not_void<void> { enum { R = false }; };
template <typename _Any> struct c_not_void { enum { R = true }; };

// // void_replace
// template <typename _T, typename _Rep> struct void_replace { typedef _T R; };
// template <typename _Rep> struct void_replace<void, _Rep> { typedef _Rep R; };

// show void
template <> struct c_show_impl<void>
{ static void c_to_string (std::ostream& os) { os << "void"; } };

// if_
template <typename _Then, typename _Else> struct if_<false, _Then, _Else>
{ typedef _Else R; };

template <bool _Cond, typename _Then, typename _Else> struct if_
{ typedef _Then R; };

// if_void
template <typename _Then, typename _Else> struct if_void<void, _Then, _Else>
{ typedef _Then R; };

template <typename _Type, typename _Then, typename _Else> struct if_void
{ typedef _Else R; };

// c_same
template <typename _T> struct c_same <_T, _T>
{ static const bool R = true; };

template <typename _T1, typename _T2> struct c_same
{ static const bool R = false; };

// c_not_same
template <typename _T> struct c_not_same <_T, _T>
{ static const bool R = false; };

template <typename _T1, typename _T2> struct c_not_same
{ static const bool R = true; };
    
// show Cons
template <typename _T> struct NoBrace;
const char* const SHOW_LIST_SEP = ",";

template <typename _Head, typename _Tail>
struct c_show_impl<Cons<_Head, _Tail> > {
    static void c_to_string (std::ostream& os)
    { os << '[' << c_show(_Head) << SHOW_LIST_SEP
         << c_show(NoBrace<_Tail>) << ']'; }
};

template <typename _Head>
struct c_show_impl<Cons<_Head, void> > {
    static void c_to_string (std::ostream& os)
    { os << '[' << c_show(_Head) << ']'; }
};

template <typename _Head, typename _Tail>
struct c_show_impl<NoBrace<Cons<_Head, _Tail> > > {
    static void c_to_string (std::ostream& os)
    { os << c_show(_Head) << SHOW_LIST_SEP << c_show(NoBrace<_Tail>); }
};

template <typename _Head>
struct c_show_impl<NoBrace<Cons<_Head, void> > > {
    static void c_to_string (std::ostream& os)
    { os << c_show(_Head); }
};


template <char _C> struct c_show_impl<Char<_C> >
{ static void c_to_string (std::ostream& os) { os << _C; } };

template <int _N> struct c_show_impl<Int<_N> >
{ static void c_to_string (std::ostream& os) { os << _N; } };

    
// convert Pair to std::string
template <typename _C1, typename _C2>
struct c_show_impl<Pair<_C1, _C2> > {
    static void c_to_string (std::ostream& os)
    { os << '(' << c_show(_C1) << ' ' << c_show(_C2) << ')'; }
};

// template <template <typename, typename> class _F, class _L, typename _S1>
// struct list_call1;

// template <template <typename> class _F, typename _S1>
// struct list_call1<_F, void, _S1> {
//     static void call (const _S1&) {}
// };

// template <template <typename> class _F, typename _H, typename _T, typename _S1>
// struct list_call1<_F, Cons<_H, _T>, _S1> {
//     static void call (const _S1& s) {
//         _F<_H, _S1>::call(s);
//         list_call1<_F, _T, _S1>::call(s);
//     }
// };
}

#endif  // _FUN_C_COMMON_HPP_

