// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Provide compile-time templates to manipulate sets or maps which
// are base on list, many list operations are applicable for sets and maps
// Author: gejun@baidu.com
// Date: Dec 6 15:55:40 CST 2010
#pragma once
#ifndef _FUN_MAP_HPP_
#define _FUN_MAP_HPP_

#include "c_common.hpp"  // Cons

namespace st {

// Find the item equaling _D in _Set
// Params:
//   _cmp  report equivalence of two values
// Returns: void for not found, otherwise the equal value
template <typename _D, class _Set,
          template <typename, typename> class _eq = c_same>
struct set_find;

// Find the value mapped by _K in the _Map
// Returns:
//   void        not found
//   P::Second  where P::First equals the key
template <typename _K, class _Map> struct map_find;

// Find position of _D in _Set
// Returns:
//   -1   not found
//   >=0  the position counted from zero
template <typename _D, class _Set,
          template <typename, typename> class _eq = c_same>
struct set_find_pos;

// Find position of _K in _Map
// Returns:
//   -1   not found
//   >=0  the position counted from zero
template <typename _K, class _Map> struct map_find_pos;

// Erase the item equaling _D from _Set
// Returns: erased set
template <typename _D, class _Set,
          template <typename, typename> class _eq = c_same>
struct set_erase;

// Erase _K from _Map
// Returns: erased map
template <typename _K, class _Map> struct map_erase;
 
// Insert _D into _Set
// Note: If a pair with the key exists, its value gets updated
template <typename _D, class _Set,
          template <typename, typename> class _eq = c_same>
struct set_insert;

// Insert pair of _K and _V into _Map
// Note: If a pair with the key exists, its value gets updated
template <typename _K, typename _V, class _Map> struct map_insert;

// Merge _Set1 and _Set2 to be a new set
// Note: If _Set1 and _Set2 have same items, take the one from _Set1
template <class _Set1, class _Set2,
 template <typename, typename> class _eq = c_same>
struct set_concat;

// Test if every item of _Set2 is in _Set1
// Returns: yes or no
template <class _Set1, class _Set2> struct set_include;

// Remove items of _Set2 from _Set1
// Returns: changed _Set1
template <class _Set1, class _Set2> struct set_minus;

// Test if two sets are both included
// Returns: yes or no
template <class _Set1, class _Set2> struct set_equal;

// Get intersection of two sets
template <class _Set1, class _Set2> struct set_intersect;

// -----------------
//  Implementations
// -----------------

template <typename _K, typename _P> struct pair_first_same
{ enum { R = CAP(c_same, _K, typename _P::First) }; };

// set_find
template <typename _D, typename _H, class _T,
          template <typename, typename> class _eq>
struct set_find<_D, Cons<_H, _T>, _eq> {
    typedef TCAP(if_, CAP(_eq, _D, _H),
                 c_identity<_H>,
                 set_find<_D, _T, _eq>)::R R;
};

template <typename _D, template <typename, typename> class _eq>
struct set_find<_D, void, _eq> { typedef void R; };

// map_find
template <typename _K, typename _P, class _T>
struct map_find<_K, Cons<_P, _T> > {
    typedef TCAP(if_, CAP(c_same, _K, typename _P::First),
                 c_identity<typename _P::Second>,
                 map_find<_K, _T>)::R R;
};
    
template <typename _K>
struct map_find<_K, void> { typedef void R; };
    
// set_find_pos
template <typename _D, typename _H, class _T,
          template <typename, typename> class _eq>
class set_find_pos<_D, Cons<_H, _T>, _eq> {
    struct case_false
    { enum { R_ = CAP(set_find_pos, _D, _T, _eq), 
             R = (R_ == -1) ? -1 : (1 + R_) }; };
    
public:
    static const int R = CAP(if_, CAP(_eq, _D, _H),
                             Int<0>,
                             case_false)::R;
};

template <typename _D, template <typename, typename> class _eq>
struct set_find_pos<_D, void, _eq> { static const int R = -1; };

// map_find_pos
template <typename _K, class _Map> struct map_find_pos
    : public set_find_pos<_K, _Map, pair_first_same> {};

// set_erase
template <typename _D, typename _H, class _T,
          template <typename, typename> class _eq>
class set_erase<_D, Cons<_H, _T>, _eq> {
    struct on_D_exists
    { typedef Cons<_H, TCAP(set_erase, _D, _T, _eq)> R; };

public:
    typedef TCAP(if_, CAP(_eq, _D, _H), c_identity<_T>, on_D_exists)::R R;
};

template <typename _D, template <typename, typename> class _eq>
struct set_erase<_D, void, _eq> { typedef void R; };

// map_erase
template <typename _K, class _Map> struct map_erase
    : public set_erase<_K, _Map, pair_first_same> {};

// set_insert
template <typename _D, class _Set, template <typename, typename> class _eq>
struct set_insert
{ typedef Cons<_D, TCAP(set_erase, _D, _Set, _eq)> R; };

// map_insert
template <typename _K, typename _V, class _Map>
struct map_insert {
    // erase the pair (if it exists) and append the new one
    typedef Cons<Pair<_K, _V>, TCAP(map_erase, _K, _Map)> R;
};

// set_concat
template <typename _D, class _T, class _Set2,
          template <typename, typename> class _eq>
struct set_concat<Cons<_D, _T>, _Set2, _eq>
{ typedef TCAP(set_concat, _T, TCAP(set_insert, _D, _Set2, _eq)) R; };

template <class _Set2, template <typename, typename> class _eq>
struct set_concat<void, _Set2, _eq> { typedef _Set2 R; };

// set_include
template <class _Set1, typename _H2, class _T2>
struct set_include<_Set1, Cons<_H2, _T2> > {
    struct keep_on
    { static const bool R = CAP(set_include,
                                TCAP(set_erase, _H2, _Set1), _T2); };
    
    static const bool R = CAP(if_void, TCAP(set_find, _H2, _Set1),
                              Int<false>, keep_on)::R;
};

template <class _Set1> struct set_include<_Set1, void>
{ static const bool R = true; };

// set_minus
template <class _Set1, typename _H2, class _T2>
struct set_minus<_Set1, Cons<_H2, _T2> >
{ typedef TCAP(set_minus, TCAP(set_erase, _H2, _Set1), _T2) R; };
 
template <class _Set1> struct set_minus<_Set1, void> {
    typedef _Set1 R;
};

// Implement set_equal
template <class _Set1, typename _H2, class _T2>
class set_equal<_Set1, Cons<_H2, _T2> > {
    struct on_found {
        static const bool R = CAP(set_equal, TCAP(set_erase, _H2, _Set1), _T2);
    };
public:
    static const bool R = CAP(if_void, TCAP(set_find, _H2, _Set1),
                              Int<false>, on_found)::R;
};

template <class _Set1> struct set_equal<_Set1, void> {
    static const bool R = CAP(c_void, _Set1);
};

template <class _Set2> struct set_intersect<void, _Set2> {
    typedef void R;
};

template <typename _T, class _L, class _Set2>
struct set_intersect<Cons<_T, _L>, _Set2> {
    struct on_found {
        typedef Cons<_T, TCAP(set_intersect, _L, TCAP(set_erase, _T, _Set2))> R;
    };
    typedef TCAP(if_void, TCAP(set_find, _T, _Set2),
                 set_intersect<_L, _Set2>, on_found)::R R;
};

}  // namespace st

#endif  //_FUN_MAP_HPP_
