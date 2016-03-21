// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Provide compile-time templates to manipulate inductive lists
// which are constructed with Cons and ended with void.
// Author: gejun@baidu.com
// Date: Dec.4 15:32:18 CST 2010

#pragma once
#ifndef _C_LIST_HPP_
#define _C_LIST_HPP_

#include "c_common.hpp"  // Cons

// Note: a list of Foo is often abbreviated as 'FooL'
namespace st {
// Append _L1 before _L2, validity of _L2 is not checked
// Returns: the new list
template <class _L1, class _L2> struct list_concat;
    
// Get number of items in a list, 
// Returns: 0 for void
template <class _L> struct list_size;

// Get first item in the list
// Returns: void for empty List, otherwise the first item
template <class _L> struct list_head;

// Get the sub list except the first item
// Returns: void for empty list or a list with only one item
template <class _L> struct list_tail;

// Get last item in the list
// Returns: void if _L is empty
template <class _L> struct list_last;

// Get the value at _POS in a list, _POS is counted from zero
template <int _POS, class _L> struct list_at;
        
// Find the first item same with _Item
// Returns: position of the item (from 0) or -1
template <typename _Item, class _L> struct list_seek_first;

// Find the first item same with _Item
// Returns: a same item or void
template <typename _Item, class _L> struct list_find_first;

// Find the first item that makes _F true
// Returns: position of the item (from 0) or -1
template <template <typename> class _F, class _L>
struct list_seek_first_true;

// Find the first item that makes _F true
// Returns: void for not found, otherwise the item
template <template <typename> class _F, class _L>
struct list_find_first_true;
    
// Check if there're same items in the list
// Returns: exist or not
template <class _L> struct list_dup;
    
// Erase first item same with _Item from the list
// Returns: erased list 
template <typename _Item, class _L> struct list_erase_first;

// Erase the first item that makes _F true
// Returns: the pair of erased item and erased list
template <template <typename> class _F, class _L>
struct list_erase_first_true;
    
// Collect items that make _F true
// Returns: a list of qualified items
template <template <typename> class _F, class _L> struct list_filter;

// Collect items that make _F _false_
template <template <typename> class _F, class _L> struct list_unfilter;

// Convert each item in a list
// Returns: a new list with converted items
template <template <typename> class _F, class _L> struct list_map;
    
// Convert each item to lists and concatenate them together
// Params:
//   _F  convert an item to a list
template <template <typename> class _F, class _L> struct list_map_cat;

// Convert each item by _F and collect results that're not void
// Returns: a new list with converted non-void items
template <template <typename> class _F, class _L> struct list_filter_map;

// Split a list into two lists by _F
// Returns: a pair of two lists, first of the pair contains all items making
//          _F true while the second contains all items making _F false
template <template <typename> class _F, class _L> struct list_partition;

// Pack parameters into a list, 12 parameters at most
template <typename _A1 = void, typename _A2 = void, typename _A3 = void, 
          typename _A4 = void, typename _A5 = void, typename _A6 = void, 
          typename _A7 = void, typename _A8 = void, typename _A9 = void>
struct make_list;

// Following macros can only be used on parameters without < and >
#define ST_MAKE_LIST(...) CHOOSE_VARIADIC_MACRO(ST_MAKE_LIST_, __VA_ARGS__)
#define ST_MAKE_LIST_0()  void
#define ST_MAKE_LIST_1(a1) Cons<a1, void>
#define ST_MAKE_LIST_2(a1, a2) Cons<a1, Cons<a2, void> >
#define ST_MAKE_LIST_3(a1, a2, a3) Cons<a1, Cons<a2, Cons<a3, void> > >
#define ST_MAKE_LIST_4(a1, a2, a3, a4)                  \
    Cons<a1, Cons<a2, Cons<a3, Cons<a4, void> > > >
#define ST_MAKE_LIST_5(a1, a2, a3, a4, a5)                      \
    Cons<a1, Cons<a2, Cons<a3, Cons<a4, Cons<a5, void> > > > >
#define ST_MAKE_LIST_6(a1, a2, a3, a4, a5, a6)                          \
    Cons<a1, Cons<a2, Cons<a3, Cons<a4, Cons<a5, Cons<a6, void> > > > > >
#define ST_MAKE_LIST_7(a1, a2, a3, a4, a5, a6, a7)                      \
    Cons<a1, Cons<a2, Cons<a3, Cons<a4, Cons<a5, Cons<a6,               \
    Cons<a7, void> > > > > > >
#define ST_MAKE_LIST_8(a1, a2, a3, a4, a5, a6, a7, a8)                  \
    Cons<a1, Cons<a2, Cons<a3, Cons<a4, Cons<a5, Cons<a6,               \
    Cons<a7, Cons<a8, void> > > > > > > >
#define ST_MAKE_LIST_9(a1, a2, a3, a4, a5, a6, a7, a8, a9)              \
    Cons<a1, Cons<a2, Cons<a3, Cons<a4, Cons<a5, Cons<a6,               \
    Cons<a7, Cons<a8, Cons<a9, void> > > > > > > > >
#define ST_MAKE_LIST_10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)        \
    Cons<a1, Cons<a2, Cons<a3, Cons<a4, Cons<a5, Cons<a6,               \
    Cons<a7, Cons<a8, Cons<a9, Cons<a10, void> > > > > > > > > >
#define ST_MAKE_LIST_11(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)   \
    Cons<a1, Cons<a2, Cons<a3, Cons<a4, Cons<a5, Cons<a6,               \
    Cons<a7, Cons<a8, Cons<a9, Cons<a10, Cons<a11, void> > > > > > > > > > >
#define ST_MAKE_LIST_12(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) \
    Cons<a1, Cons<a2, Cons<a3, Cons<a4, Cons<a5, Cons<a6,                  \
    Cons<a7, Cons<a8, Cons<a9, Cons<a10, Cons<a11, Cons<a12,               \
    void> > > > > > > > > > > >

// Pack non-void parameters into a list, 18 parameters at most
template <typename _A0,        typename _A1  = void, typename _A2  = void,
          typename _A3  = void, typename _A4  = void, typename _A5  = void,
          typename _A6  = void, typename _A7  = void, typename _A8  = void,
          typename _A9  = void, typename _A10 = void, typename _A11 = void,
          typename _A12 = void, typename _A13 = void, typename _A14 = void,
          typename _A15 = void, typename _A16 = void, typename _A17 = void>
struct make_non_void_list;

template <typename> struct is_make_non_void_list;
                
// Fold items from left side to right side, results are calculated as
// below: _F(_F(_F(_Init, Item0), Item1), Item2)....
template <template <typename, typename> class _F, typename _Init, class _L>
struct list_foldl;

template <template <typename, typename> class _F, class _L>
struct list_foldl1;

// Fold items from left side to right side, results are calculated as
// below: ... _F(_F(_F(_Init, Item_N), Item_N-1), Item_N-2)
template <template <typename, typename> class _F, typename _Init, class _L>
struct list_foldr;
    
// Reverse a list
// Returns: the reversed list
template <class _L> struct list_reverse;

// Stably sort a list
// Params:
//   _F  true for LHS less-than RHS, otherwise false
template <template <typename, typename> class _F, class _L>
struct list_stable_sort;

// Skip until the item makes _F true
// Returns: the list starting from the item
template <template <typename> class _F, class _L> struct list_skip;

// Split a list into two lists, the first list contains items from 0 to _N-1,
// while the second list contains items from N to last item
template <int _N, class _L> struct list_split;

// Remove consecutively repeated items in a list
template <class _L> struct list_uniq;
    
// Test if items all make _F true
template <template <typename> class _F, class _L> struct list_all;

// Zip two lists into one list
// Returns: a list of Pair<e1, e2> where e1 is from _L1 and e2 is from _L2
template <class _L1, class _L2> struct list_zip2;

// Generate a list of numbers from _Min to _Max
// Returns: void if _Min > _Max, otherwise a list of Int which begins with 
//          Int<_Min> and ends with Int<_Max>
template <int _Min, int _Max> class list_seq;

// Index items of a list with consecutive numbers
// Returns: a list of Pair<Int<N>, ItemN> where ItemN is the Nth item of _L
template <class _L, int _BEGIN_IDX = 0> struct list_index;

// Append _H to _L if _H is not void
template <typename _H, class _L> struct list_append_non_void;

}  // namespace st

#include "detail/c_list_inl.hpp"

#endif  //_C_LIST_HPP_
