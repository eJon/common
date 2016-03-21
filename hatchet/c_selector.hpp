// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Select joined tuples from multiple tables
// Author: gejun@baidu.com
// Date: Dec.7 14:10:24 CST 2010
#pragma once
#ifndef _C_SELECTOR_HPP_
#define _C_SELECTOR_HPP_

#include "c_predicate.hpp"              // predicate stuff
#include "cow_table.hpp"                // CowTable
#include "join_iterator.hpp"            // JoinIterator

namespace st {

// Status of a position in a dominating graph  which is a list of
// Pair<SelPos, DomStat>
template <class _DomeeL, class _DomerL>
struct DomStat : public c_show_base {
    typedef _DomerL DomerL;
    typedef _DomeeL DomeeL;

    static void c_to_string (std::ostream& os);
};

// Information of selection of a table
// instances of this class is often abbreviated as 'TS'
template <int _TABLE_POS, int _INDEX_POS,
          class _DomerL, class _FPredL,
          class _SeekInfo>
struct TableSelection : public c_show_base {
    static const int TABLE_POS = _TABLE_POS;
    static const int INDEX_POS = _INDEX_POS;
    typedef _SeekInfo SeekInfo;
    typedef _DomerL DomerL;
    typedef _FPredL FPredL;

    static void c_to_string (std::ostream& os);
};

template <class _DS> struct get_DomerL;

// Insert the dominating relation into _DomG that _Pos1 is dominated
// by _DomerL and potentially dominates _Pos2
// Note: _Pos1 can't be void, while _Pos2 and _DomerL can.
// Note: this template only inserts a single relation, it does not guarantee
//       consistency of _DomG after insertion. To make _DomG consistent, call
//       propagate_domination
template <class _Pos1, class _Pos2, class _DomerL, class _DomG>
struct insert_dom;

// Add the dominating relation implied by _NPred to _DomG
// Or treat the predicate as a filtering predicate
// Returns: a Pair of DomG and FPredL
template <class _DomG_and_FPredL, class _NPred>
struct collect_dom_and_fpred;

// Dominate _Pos in _DomG with _Domer. _DomG is unchanged if _Pos does not
// exist in the graph or is already dominated. This template recursively
// dominates domees of the position until all downstream positions are
// dominated
// Note: _Domer is a single position, not a list of positions as in insert_dom
// Returns: changed or unchanged dominating graph
template <class _Pos, class _Domer, class _DomG>
struct dominate_pos;

// Dominate domees if the dom is dominated so that any position in the
// graph is dominated if its potential domer is dominated.
// Returns: consistent dominating graph
template <class _DomG> struct propagate_domination;

// Build a dominating graph from a conjunction of normalized predicates
// Returns: a Pair of DomG and FPredL
template <class _NPredL> struct build_dom_graph;

// Dominate all positions belonging to the table in _DomG
template <int _TABLE_POS, class _DomG> struct dominate_table;

// Test if _Pos is dominated in _DomG
template <class _Pos, class _DomG> struct is_pos_dominated;

// Test if the normalized pred is dominated in _DomG
template <class _NPred, class _DomG> struct is_pred_dominated;

template <class _DomG, class _NPredL> struct separate_dominated_pred;

// Test if _Pos is a variable
template <class _Pos> struct is_var_pos;

// Test if all arguments of the normalized predicate are variables
template <class _NPred> struct is_var_pred;

template <class _TS> struct get_table_pos_from_ts
{ typedef Int<_TS::TABLE_POS> R; };

// Try to select index from the table with dominating relations in _DomG
// Returns: a TableSelection
template <int _TABLE_POS, class _Table, bool _IS_MAYBE, class _DomG>
struct try_table;

// Compare two table selections
template <class _SeekInfo> struct getSeekScore {
    static const int R = _SeekInfo::SEEK_SCORE;
};

template <class _SeekInfo, int _> struct getSeekScore<Maybe<_SeekInfo, _> > {
    static const int R = _SeekInfo::PARTIAL_SEEK_SCORE;
};

template <> struct getSeekScore<void> {
    static const int R = COW_TABLE_SCORE;
};

template <class _TS1, class _TS2> struct ts_less_than {
    static const bool R =
        (CAP(getSeekScore, class _TS1::SeekInfo) >
         CAP(getSeekScore, class _TS2::SeekInfo));
};

template <class _Pos, class _Dom> struct match_pos_of_dom
{ static const bool R = CAP(c_same, _Pos, typename _Dom::First); };

template <class _TableM, class _MaybeM, class _DomG, class _FPredL>
struct arrange_tables;

// Assign _AttrL of _Tup with _PosL in _IterTup or _VarTup
// (according to SelPos::TABLE_POS)
template <class _AttrL, class _Tup, class _PosL, class _IterTup, class _VarTup>
struct instantiate_tuple;

template <class _SeekInfo, class _TS, class _Table, class _IterTup, class _VarTup>
struct IteratorMaker;

template <class _TSL> struct iterator_of_the_table;

template <class _TSL, class _IterTup, class _VarTup>
struct iter_maker_of_the_table;

template <class _P> struct is_second_valid_pos {
    static const bool R = (_P::Second::R != INVALID_MAYBE_POS);
};

// Select tuples joined from _TableL and constrained by _PredL
#define ST_SELECTOR(...) st::Selector2<__VA_ARGS__>

template <class _From, class _Where> class Selector2 : public c_show_base {
    typedef class _From::ArgL _MTableL;
    typedef class _Where::PredL _PredL;
    typedef TCAP(list_head, _MTableL) FirstMTable_;
    C_ASSERT_NOT_VOID(FirstMTable_, selector_needs_at_least_one_table);
    C_ASSERT(CAP(get_maybe_index_pos, FirstMTable_)::R == INVALID_MAYBE_POS,
             first_table_cannot_be_marked_maybe);

public:    
    typedef TCAP(list_map, unmark_maybe, _MTableL) TableL;
    typedef TCAP(list_map, get_maybe_index_pos, _MTableL) MaybeL;
    
    static const int N_TABLE = CAP(list_size, TableL);  // Number of tables
    
    // tables indexed with their sequence numbers
    typedef TCAP(list_index, TableL) TableM;
    typedef TCAP(list_filter, is_second_valid_pos,
                 TCAP(list_index, MaybeL)) MaybeM;

    // A list of const pointers to tables
    typedef BasicTuple<TCAP(list_map, ConstPointerOf, TableL)> TablePtrTup;
    
    typedef TCAP(normalize_pred_list, _PredL) NPS1;
    // A list of normalized predicates from _Conj
    typedef typename NPS1::PredL NPredL;
    // A list of types of variables
    typedef typename NPS1::VarTypeL VarTypeL;
    
    typedef TCAP(list_partition, is_var_pred, NPredL) P1;
    // A list of predicates that all arguments are variables
    typedef typename P1::First VPredL;
    typedef typename P1::Second LeftNPredL;

    // Build dominating graph from predicates whose arguments reference
    // at least one attribute
    typedef TCAP(build_dom_graph, LeftNPredL) P2;
    // The initial dominating graph
    typedef typename P2::First DomG;
    // A list of filtering predicates which do not imply dominating relation
    typedef typename P2::Second FPredL;
    
    // Tuple of types of variables
    typedef BasicTuple<VarTypeL> VarTup;

    // Arrange order of selections on tables
    typedef TCAP(arrange_tables, TableM, MaybeM, DomG, FPredL) TSL;
    
    typedef TCAP(list_map, THAP(iterator_of_the_table, TSL), TableM) IterL;
    typedef BasicTuple<IterL> IterTup;

    typedef TCAP(list_map, THAP(iter_maker_of_the_table, TSL, IterTup, VarTup),
                 TableM) IterMakerL;

    typedef JoinIterator<TSL, JoinIteratorData<IterMakerL, IterTup, VarTup,
                                               TablePtrTup, VPredL> > Iterator;

    // Constructors, at most 6 parameters
#define DEFINE_SELECTOR2_VARIADIC_CONSTRUCTOR(_n_)      \
    template <ST_SYMBOLL##_n_(typename _T)>             \
    explicit Selector2 (ST_PARAML##_n_(const _T, *a))   \
    { init_tuple(&table_tup_, ST_SYMBOLL##_n_(a)); }

    DEFINE_SELECTOR2_VARIADIC_CONSTRUCTOR(1)
    DEFINE_SELECTOR2_VARIADIC_CONSTRUCTOR(2)
    DEFINE_SELECTOR2_VARIADIC_CONSTRUCTOR(3)
    DEFINE_SELECTOR2_VARIADIC_CONSTRUCTOR(4)
    DEFINE_SELECTOR2_VARIADIC_CONSTRUCTOR(5)
    DEFINE_SELECTOR2_VARIADIC_CONSTRUCTOR(6)
    DEFINE_SELECTOR2_VARIADIC_CONSTRUCTOR(7)
    DEFINE_SELECTOR2_VARIADIC_CONSTRUCTOR(8)

    
    // Do selection, at most 4 parameters
    // Use this if there're no variables in predicates
    Iterator select () const
    { return Iterator(BasicTuple<void>(), &table_tup_); }

    // Prototype of select(...)
#define DEFINE_SELECTOR2_VARIADIC_SELECT(_n_)                   \
    template <ST_SYMBOLL##_n_(typename _V)>                     \
    Iterator select (ST_PARAML##_n_(const _V, & a)) const       \
    {                                                           \
        VarTup vt;                                              \
        init_tuple (&vt, ST_SYMBOLL##_n_(a));                   \
        return Iterator(vt, &table_tup_);                       \
    }

    DEFINE_SELECTOR2_VARIADIC_SELECT(1)
    DEFINE_SELECTOR2_VARIADIC_SELECT(2)
    DEFINE_SELECTOR2_VARIADIC_SELECT(3)
    DEFINE_SELECTOR2_VARIADIC_SELECT(4)
    DEFINE_SELECTOR2_VARIADIC_SELECT(5)
    DEFINE_SELECTOR2_VARIADIC_SELECT(6)
    DEFINE_SELECTOR2_VARIADIC_SELECT(7)
    DEFINE_SELECTOR2_VARIADIC_SELECT(8)
    
    static void c_to_string (std::ostream& os);
    
protected:
    TablePtrTup table_tup_;
};

}  // namespace st

#include "detail/c_selector_inl.hpp"

#endif  //_C_SELECTOR_HPP_
