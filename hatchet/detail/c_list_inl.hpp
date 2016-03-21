// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Inline implementations of c_list.hpp
// Author: gejun@baidu.com
// Date: Dec.4 15:32:18 CST 2010

namespace st {

// list_concat
template <class _L2> struct list_concat<void, _L2> { typedef _L2 R; };

template <typename _H, class _T, class _L2>
struct list_concat<Cons<_H, _T>, _L2>
{ typedef Cons<_H, TCAP(list_concat, _T, _L2)> R; };


// list_size
template <typename _H, class _T> struct list_size<Cons<_H, _T> >
{ static const int R = 1 + CAP(list_size, _T); };

template <> struct list_size<void> { static const int R = 0; };

// list_head
template <typename _H, class _T> struct list_head<Cons<_H, _T> >
{ typedef _H R; };

template <> struct list_head<void> { typedef void R; };

// list_tail
template <typename _H, class _T> struct list_tail<Cons<_H, _T> >
{ typedef _T R; };

template <> struct list_tail<void> { typedef void R; };

// list_last
template <typename _H, class _T> struct list_last<Cons<_H, _T> >
{ typedef TCAP(if_void, _T, c_identity<_H>, list_last<_T>)::R R; };

template <> struct list_last<void> { typedef void R; };

// list_at    
template <int _POS, typename _H, class _T>
struct list_at <_POS, Cons<_H, _T> >
{ typedef TCAP(list_at, _POS-1, _T) R; };

template <int _POS> struct list_at<_POS, void>
{ typedef void R; };

template <typename _H, class _T>
struct list_at <0, Cons<_H, _T> >
{ typedef _H R; };

// implement list_seek_first
template <typename _Item, typename _H, class _T>
class list_seek_first<_Item, Cons<_H, _T> > {
    // if _idx is -1, returns -1; otherwise returns 1+_idx
    struct case_false
    { enum { R_ = CAP(list_seek_first, _Item, _T), 
             R = (R_ < 0) ? -1 : (1 + R_) }; };
            
public:
    const static int R =
        CAP(if_, CAP(c_same, _Item, _H), Int<0>, case_false)::R;
};

template <typename _Item> struct list_seek_first<_Item, void>
{ enum { R = -1 }; };

// list_find_first
template <typename _Item, typename _H, class _T>
struct list_find_first<_Item, Cons<_H, _T> > {
    typedef TCAP(if_, CAP(c_same, _Item, _H), 
                 c_identity<_H>, 
                 list_find_first<_Item, _T>)::R R;
};

template <typename _Item>
struct list_find_first<_Item, void> { typedef void R; };

// implement list_seek_first_true
template <template <typename> class _F, typename _H, class _T>
class list_seek_first_true<_F, Cons<_H, _T> > {
    // if _idx is -1, returns -1; otherwise returns 1+_idx
    struct case_false
    { enum { R_ = CAP(list_seek_first_true, _F, _T), 
             R = (R_ < 0) ? -1 : (1 + R_) }; };
            
public:
    const static int R = CAP(if_, CAP(_F, _H), Int<0>, case_false)::R;
};

template <template <typename> class _F> struct list_seek_first_true<_F, void>
{ enum { R = -1 }; };

// list_find_first_true
template <template <typename> class _F, typename _H, class _T>
struct list_find_first_true<_F, Cons<_H, _T> > {
    typedef TCAP(if_, CAP(_F, _H), 
                 c_identity<_H>, 
                 list_find_first_true<_F, _T>)::R R;
};

template <template <typename> class _F>
struct list_find_first_true<_F, void> { typedef void R; };

// list_dup    
template <typename _H, class _T>
struct list_dup<Cons<_H, _T> >
{ enum { R = CAP(if_, (CAP(list_seek_first, _H, _T) >= 0),
                 Int<true>,
                 list_dup<_T>)::R }; };

template <> struct list_dup<void> { enum { R = false }; };

// Implement list_erase_first    
template <typename _Item, typename _H, class _T>
class list_erase_first<_Item, Cons<_H, _T> > {
    struct go_on { typedef Cons<_H, TCAP(list_erase_first, _Item, _T)> R; };
    
public:
    typedef TCAP(if_, CAP(c_same, _Item, _H), c_identity<_T>, go_on)::R R;
};

template <typename _Item> struct list_erase_first<_Item, void>
{ typedef void R; };

// list_erase_first_true    
template <template <typename> class _F, typename _H, class _T>
class list_erase_first_true<_F, Cons<_H, _T> > {
    struct case_false {
        typedef TCAP(list_erase_first_true, _F, _T) P;
        typedef Pair<typename P::First, Cons<_H, typename P::Second> > R;
    };
    
public:
    typedef TCAP(if_, CAP(_F, _H),
                 c_identity<Pair<_H, _T> >,
                 case_false)::R R;
};

template <template <typename> class _F>
struct list_erase_first_true<_F, void>
{ typedef Pair<void, void> R; };


// list_filter    
template <template <typename> class _F, typename _H, class _T>
class list_filter<_F, Cons<_H, _T> > {
    typedef TCAP(list_filter, _F, _T) T2;
public:
    typedef TCAP(if_, CAP(_F, _H), Cons<_H, T2>, T2) R;
};

template <template <typename> class _F>
struct list_filter<_F, void> { typedef void R; };

// list_unfilter
template <template <typename> class _F, typename _H, class _T>
class list_unfilter<_F, Cons<_H, _T> > {
    typedef TCAP(list_unfilter, _F, _T) T2;
public:
    typedef TCAP(if_, CAP(_F, _H), T2, Cons<_H, T2>) R;
};

template <template <typename> class _F>
struct list_unfilter<_F, void> { typedef void R; };

// list_map    
template <template <typename> class _F, typename _H, class _T>
struct list_map<_F, Cons<_H, _T> >
{ typedef Cons<TCAP(_F, _H), TCAP(list_map, _F, _T)> R; };

template <template <typename> class _F>
struct list_map<_F, void> { typedef void R; };

// list_map_cat
template <template <typename> class _F, typename _H, class _T>
struct list_map_cat<_F, Cons<_H, _T> >
{ typedef TCAP(list_concat, TCAP(_F, _H), TCAP(list_map_cat, _F, _T)) R; };

template <template <typename> class _F>
struct list_map_cat<_F, void> { typedef void R; };

// list_filter_map
template <template <typename> class _F, typename _H, class _T>
class list_filter_map<_F, Cons<_H, _T> > {
    typedef TCAP(list_filter_map, _F, _T) T2;
    typedef TCAP(_F, _H) H2;
public:
    typedef TCAP(if_, !CAP(c_void, H2), Cons<H2, T2>, T2) R;
};

template <template <typename> class _F>
struct list_filter_map<_F, void> { typedef void R; };
    
// list_partition
template <template <typename> class _F, typename _H, class _T>
class list_partition<_F, Cons<_H, _T> > {
    typedef TCAP(list_partition, _F, _T) P;
public:
    typedef
    TCAP(if_, CAP(_F, _H)
         , Pair<Cons<_H, typename P::First>, typename P::Second>
         , Pair<typename P::First, Cons<_H, typename P::Second> >) R;

};

template <template <typename> class _F>
struct list_partition<_F, void> { typedef Pair<void, void> R; };

// make_list
template <typename _A1, typename _A2, typename _A3, 
          typename _A4, typename _A5, typename _A6, 
          typename _A7, typename _A8, typename _A9>
struct make_list
{ typedef Cons<_A1, Cons<_A2, Cons<_A3, Cons<_A4, Cons<_A5, 
          Cons<_A6, Cons<_A7, Cons<_A8, Cons<_A9, void> > > > > > > > > R; };

//! make a list with 0 item
template <> struct make_list<> { typedef void R; };
    
//! make a list with 1 item
template <typename _A1> struct make_list<_A1>
{ typedef Cons<_A1, void> R; };
    
//! make a list with 2 items
template <typename _A1, typename _A2>
struct make_list<_A1, _A2> { typedef Cons<_A1, Cons<_A2, void> > R; };

//! make a list with 3 items
template <typename _A1, typename _A2, typename _A3>
struct make_list<_A1, _A2, _A3>
{ typedef Cons<_A1, Cons<_A2, Cons<_A3, void> > > R; };
    
//! make a list with 4 items
template <typename _A1, typename _A2, typename _A3, typename _A4>
struct make_list<_A1, _A2 , _A3, _A4>
{ typedef Cons<_A1, Cons<_A2, Cons<_A3, Cons<_A4, void> > > > R; };
    
//! make a list with 5 items
template <typename _A1, typename _A2, typename _A3, typename _A4, 
          typename _A5>
struct make_list<_A1, _A2, _A3, _A4, _A5>
{ typedef Cons<_A1, Cons<_A2, Cons<_A3, Cons<_A4, Cons<_A5, 
                                                       void> > > > > R; };

//! make a list with 6 items
template <typename _A1, typename _A2, typename _A3, typename _A4, 
          typename _A5, typename _A6>
struct make_list<_A1, _A2, _A3, _A4, _A5, _A6>
{ typedef Cons<_A1, Cons<_A2, Cons<_A3, Cons<_A4, Cons<_A5, 
          Cons<_A6, void> > > > > > R; };

//! make a list with 7 items
template <typename _A1, typename _A2, typename _A3, typename _A4, 
          typename _A5, typename _A6, typename _A7>
struct make_list<_A1, _A2, _A3, _A4, _A5, _A6, _A7>
{ typedef Cons<_A1, Cons<_A2, Cons<_A3, Cons<_A4, Cons<_A5, 
          Cons<_A6, Cons<_A7, void> > > > > > > R; };
    
//! make a list with 8 items
template <typename _A1, typename _A2, typename _A3, typename _A4, 
          typename _A5, typename _A6, typename _A7, typename _A8>
struct make_list<_A1, _A2, _A3, _A4, _A5, _A6, _A7, _A8>
{ typedef Cons<_A1, Cons<_A2, Cons<_A3, Cons<_A4, Cons<_A5, 
          Cons<_A6, Cons<_A7, Cons<_A8, void> > > > > > > > R; };

// Implement make_non_void_list
template <typename _A0,  typename _A1,  typename _A2,
          typename _A3,  typename _A4,  typename _A5,
          typename _A6,  typename _A7,  typename _A8,
          typename _A9,  typename _A10, typename _A11,
          typename _A12, typename _A13, typename _A14,
          typename _A15, typename _A16, typename _A17>
struct make_non_void_list {
    typedef TCAP(list_filter, c_not_void,
                 Cons<_A0, Cons<_A1, Cons<_A2, Cons<_A3, Cons<_A4,
                 Cons<_A5, Cons<_A6, Cons<_A7, Cons<_A8, Cons<_A9,
                 Cons<_A10, Cons<_A11, Cons<_A12, Cons<_A13, Cons<_A14,
                 Cons<_A15, Cons<_A16, Cons<_A17,
                 void> > > > > > > > > > > > > > > > > >) R;
};

template <typename _A0,  typename _A1,  typename _A2,
          typename _A3,  typename _A4,  typename _A5,
          typename _A6,  typename _A7,  typename _A8,
          typename _A9,  typename _A10, typename _A11,
          typename _A12, typename _A13, typename _A14,
          typename _A15, typename _A16, typename _A17>
struct is_make_non_void_list<
    make_non_void_list<_A0, _A1, _A2, _A3, _A4, _A5, _A6, _A7, _A8,
                       _A9, _A10, _A11, _A12, _A13, _A14, _A15, _A16,
                       _A17> > {
    static const bool R = true;
};

template <typename>
struct is_make_non_void_list { static const bool R = false; };

// list_foldl
template <template <typename, typename> class _F, 
          typename _Init, typename _H, class _T>
struct list_foldl<_F, _Init, Cons<_H, _T> >
{ typedef TCAP(list_foldl, _F, TCAP(_F, _Init, _H), _T) R; };

template <template <typename, typename> class _F, typename _Init>
struct list_foldl<_F, _Init, void> { typedef _Init R; };

// list_foldl1
template <template <typename, typename> class _F, typename _H, class _T>
struct list_foldl1<_F, Cons<_H, _T> >
{ typedef TCAP(list_foldl, _F, _H, _T) R; };

template <template <typename, typename> class _F>
struct list_foldl1<_F, void> { typedef void R; };

// list_foldr
template <template <typename, typename> class _F,
          typename _Init,
          typename _H,
          class _T>
struct list_foldr<_F, _Init, Cons<_H, _T> >
{ typedef TCAP(_F, TCAP(list_foldr, _F, _Init, _T), _H) R; };

template <template <typename, typename> class _F, typename _Init>
struct list_foldr<_F, _Init, void>
{ typedef _Init R; };
    
// list_revserse
template <class _L, class _T> struct list_reverse_with;

template <typename _H, class _T, typename _NewT>
struct list_reverse_with <Cons<_H, _T>, _NewT>
{ typedef TCAP(list_reverse_with, _T, Cons<_H, _NewT>) R; };

template <class _T> struct list_reverse_with<void, _T>
{ typedef _T R; };

template <class _L> struct list_reverse
{ typedef TCAP(list_reverse_with, _L, void) R; };


// list_stable_sort
// implemented as stable quicksort, effective but not very efficient, since
// we have very limited list size at compile-time, this is currently not an
// issue
template <template <typename, typename> class _F, typename _H, class _T>
class list_stable_sort<_F, Cons<_H, _T> > {
    // make a partially applied comparator
    template <typename _A> struct less_than_head
    { static const int R = CAP(_F, _A, _H); };

    // partition tail with less_than_head, after that, all items
    // less-than head should be in left side, and they have nothing
    // to with "stable", all items greater-equal-than head are on the
    // right side, if there're several items same with head, head is
    // still the first of the group.
    typedef TCAP(list_partition, less_than_head, _T) P;
        
public:
    // recursively sort and concatenate lists
    typedef
    TCAP(list_concat, 
         TCAP(list_stable_sort, _F, typename P::First), 
         Cons<_H, TCAP(list_stable_sort, _F, typename P::Second)>) R;
};

template <template <typename, typename> class _F>
struct list_stable_sort<_F, void> { typedef void R; };

// list_skip
template <template <typename> class _F, typename _H, class _L>
struct list_skip<_F, Cons<_H, _L> > {
    typedef TCAP(if_, CAP(_F, _H),
                 c_identity<Cons<_H, _L> >, 
                 list_skip<_F, _L>)::R R;
};

template <template <typename> class _F>
struct list_skip<_F, void>
{ typedef void R; };

// list_split
template <int _N, typename _H, class _T>
class list_split<_N, Cons<_H, _T> > {
    struct on_positive_N {
        typedef TCAP(list_split, _N-1, _T) P;
        typedef Pair<Cons<_H, typename P::First>, typename P::Second> R;
    };
public:
    typedef TCAP(if_, _N <= 0,
                 c_identity<Pair<void, Cons<_H, _T> > >,
                 on_positive_N)::R R;
};

template <int _N> struct list_split<_N, void>
{ typedef Pair<void, void> R; };

// list_uniq
template <typename _H, class _L> class list_uniq<Cons<_H, _L> > {
    template <typename _Any> struct different_from_H
    { enum { R = !CAP(c_same, _H, _Any) }; };
public:
    typedef Cons<_H, TCAP(list_uniq,
                          TCAP(list_skip, different_from_H, _L))> R;
};

template <> struct list_uniq<void> { typedef void R; };

// list_all
template <template <typename> class _F, typename _H, class _T>
struct list_all<_F, Cons<_H, _T> >
{ enum { R = CAP(if_, CAP(_F, _H), list_all<_F, _T>, Int<false>)::R }; };

template <template <typename> class _F>
struct list_all<_F, void> { enum { R = true }; };

// list_zip2
template <typename _H1, class _T1, typename _H2, class _T2>
struct list_zip2<Cons<_H1, _T1>, Cons<_H2, _T2> >
{ typedef Cons<Pair<_H1, _H2>, TCAP(list_zip2, _T1, _T2)> R; };

template <class _L1, class _L2> struct list_zip2 { typedef void R; };

// list_seq
template <int _Min, int _Max> class list_seq {
    struct case_false
    { typedef Cons<Int<_Min>, TCAP(list_seq, _Min+1, _Max)> R; };
        
public:
    typedef TCAP(if_, (_Min > _Max),
                 c_identity<void>,
                 case_false)::R R;
};

// list_index
template <int _BEGIN_IDX> struct list_index<void, _BEGIN_IDX>
{ typedef void R; };

template <typename _H, class _T, int _BEGIN_IDX>
struct list_index<Cons<_H, _T>, _BEGIN_IDX> {
    typedef Cons<Pair<Int<_BEGIN_IDX>, _H>,
        TCAP(list_index, _T, _BEGIN_IDX+1)> R;
};



// list_append_non_void
template <typename _H, class _L> struct list_append_non_void
{ typedef Cons<_H, _L> R; };

template <class _L> struct list_append_non_void<void, _L>
{ typedef _L R; };

}  // namespace st

