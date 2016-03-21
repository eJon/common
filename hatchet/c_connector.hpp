// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Watch change of multiple tables and select minimum joined tuples
// Author: gejun@baidu.com
// Date: Dec.7 14:10:24 CST 2010
#pragma once
#ifndef _C_CONNECTOR_HPP_
#define _C_CONNECTOR_HPP_

#include "c_selector.hpp"           // template helpers of Selector2
#include "observer.hpp"             // BaseObserver/Event

namespace st {

template <int _TABLE_POS, class _Table, class _TSL>
struct iterator_of_the_table_except {
    template <class _P> struct HR;
    
    template <int _TABLE_POS2, class _Table2>
    struct HR<Pair<Int<_TABLE_POS2>, _Table2> > {
        struct other_table {
            typedef TCAP(find_ts_of_table, _TABLE_POS2, _TSL) TS;
            typedef TCAP(getIteratorFromSeekInfo,
                         class TS::SeekInfo, _Table2) R;
        };
        typedef TCAP(if_, _TABLE_POS2 == _TABLE_POS,
                     c_identity<const class _Table::Tup*>,
                     other_table)::R R;
    };
};

template <int _TABLE_POS, class _TSL, class _IterTup, class _VarTup>
struct iter_maker_of_the_table_except {
    template <class _P> struct HR {
        static const int TABLE_POS2 = _P::First::R;
        struct other_table {
            typedef TCAP(find_ts_of_table, TABLE_POS2, _TSL) TS;
            typedef IteratorMaker<class TS::SeekInfo, TS,
                                  typename _P::Second, _IterTup,
                                  _VarTup> R;
        };
        typedef TCAP(if_, TABLE_POS2 != _TABLE_POS,
                     other_table,
                     c_identity<void>)::R R;
    };
};

template <int _TABLE_POS,
          class _Table,
          class _TableM,
          class _MaybeM,
          class _DomG,
          class _FPredL,
          class _VarTup,
          class _TablePtrTup,
          class _VPredL>
class TrigSelector : public c_show_base {
public:
    enum { TABLE_POS = _TABLE_POS };
    typedef _Table Table;

    typedef TCAP(map_find, Int<TABLE_POS>, _MaybeM) MaybePos;
     
    typedef TCAP(dominate_table, _TABLE_POS, _DomG) ModDomG;
    
    template <class _FPredL2, class _Dom> struct fold_unused_domer_as_filter {
        typedef typename _Dom::First Pos;
        
        // All domers fall back to filtering predicates, this is different 
        // from Selector
        struct on_match_table
        { typedef TCAP(accum_equi_filters, _FPredL2, Pos,
                       typename _Dom::Second::DomerL) R; };
    public:
        typedef TCAP(if_, Pos::TABLE_POS == _TABLE_POS/*1*/,
                     on_match_table, c_identity<_FPredL2>)::R R;
    };

    typedef TCAP(list_foldl, fold_unused_domer_as_filter, void, ModDomG) DomFPredL;

    typedef TCAP(separate_dominated_pred, ModDomG, _FPredL) P1;

    typedef TCAP(list_concat, _VPredL,
                 TCAP(list_concat, DomFPredL, typename P1::First)) PreFPredL;
        
    typedef typename P1::Second LeftFPredL;
        
    typedef TCAP(arrange_tables,
                 TCAP(map_erase, Int<_TABLE_POS>, _TableM),
                 _MaybeM,
                 ModDomG, LeftFPredL) TSL;

    typedef TCAP(list_map, THAP(iterator_of_the_table_except, _TABLE_POS,
                                _Table, TSL), _TableM) IterL;

    typedef BasicTuple<IterL> IterTup;

    typedef TCAP(list_map, THAP(iter_maker_of_the_table_except, _TABLE_POS,
                                TSL, IterTup, _VarTup), _TableM) IterMakerL;

    typedef JoinIterator<TSL, JoinIteratorData<IterMakerL,
                                               IterTup,
                                               _VarTup,
                                               _TablePtrTup,
                                               PreFPredL> > Iterator;

    static void c_to_string (std::ostream& os);
};

// Represent result of selection in a Connector so that
// Users are able to access values by:
//   - .at<TBLn, Attribute>
//   - .at<TBLn<Attribute> >
//   - .at<VARn>
template <class _IterTup, class _VarTup> class SelectResult {
public:
    // Construct from iter_tup and var_tup
    explicit SelectResult (const _IterTup* p_iter_tup, 
                           const _VarTup* p_var_tup)
        : p_iter_tup_(p_iter_tup), p_var_tup_(p_var_tup) {}

    // Get value at a position by .at<TBLn, Attribute >
    template <template <typename> class _TBLn, class _Attr>
    const typename _Attr::Type& at () const
    { return p_iter_tup_->template at_n<_TBLn<_Attr>::TABLE_POS>()
                        ->at<_Attr>(); }

    template <int _POS>
    TCAP(_VarTup::template type_at_n, _POS) const& var () const
    { return p_var_tup_->template at_n<_POS>(); }
    
protected:
    const _IterTup* p_iter_tup_;
    const _VarTup* p_var_tup_;
};

// Select joined tuples triggered by any table in _SrcTableL and implied by _PredL
#define ST_CONNECTOR(...) st::Connector<__VA_ARGS__>

template <class _DstTable, class _Pick, class _From, class _Where>
class Connector : public c_show_base {
    // internal types
    typedef TCAP(list_map, unmark_silent, typename _From::ArgL) SrcMTableL_;
    typedef TCAP(list_head, SrcMTableL_) FirstMTable_;
    C_ASSERT_NOT_VOID(FirstMTable_, connector_needs_at_least_one_table);
    C_ASSERT(CAP(get_maybe_index_pos, FirstMTable_)::R == INVALID_MAYBE_POS,
             first_table_cannot_be_marked_maybe);
    
    typedef TCAP(list_map, unmark_maybe, SrcMTableL_) SrcTableL;
    typedef TCAP(list_map, get_maybe_index_pos, SrcMTableL_) MaybeL;

    // source tables indexed with their sequence numbers
    typedef TCAP(list_index, SrcTableL) SrcTableM;
    typedef TCAP(list_filter, is_second_valid_pos,
                 TCAP(list_index, MaybeL)) MaybeM;
    
    // List of const pointers to source tables

    typedef TCAP(normalize_pred_list, typename _Where::PredL) NPS1;
    typedef typename NPS1::PredL NPredL;
    typedef typename NPS1::VarTypeL VarTypeL;

    typedef TCAP(list_partition, is_var_pred, NPredL) P1;
    typedef typename P1::Second LeftNPredL;

    typedef TCAP(build_dom_graph, LeftNPredL) P2;

public:  // public types
    typedef _DstTable DstTableType;
    typedef BasicTuple<TCAP(list_map, ConstPointerOf, SrcTableL)> TablePtrTup;

    typedef typename P1::First VPredL;   // Filtering predicate list solely by VARs
    typedef typename P2::Second FPredL;  // Initial filtering predicate list not
                                      // solely by VARs
    typedef typename P2::First DomG;     // Initial dominating graph
    // Order of selections on tables
    typedef TCAP(arrange_tables, SrcTableM, MaybeM, DomG, FPredL) TSL;
    
    // Number of tables
    static const int N_TABLE = CAP(list_size, SrcTableL);

    typedef typename _DstTable::Tup DstTup;   // Tuple type of destination table
    // List of Iterators
    typedef TCAP(list_map, THAP(iterator_of_the_table, TSL), SrcTableM) IterL;
    typedef BasicTuple<IterL>    IterTup;  // Tuple of Iterators
    typedef BasicTuple<VarTypeL> VarTup;   // Tuple of types of variables
    
    typedef TCAP(list_map, THAP(iter_maker_of_the_table, TSL, IterTup, VarTup),
                 SrcTableM) IterMakerL;

    typedef JoinIterator<TSL,
                         JoinIteratorData<
                             IterMakerL,
                             IterTup,
                             VarTup,
                             TablePtrTup,
                             VPredL> > Iterator;
    
protected:
    template <class _P> struct make_trig_selector {
        typedef TCAP(if_, N_TABLE == 1,
                     void, 
                     TrigSelector<_P::First::R,
                                  typename _P::Second,
                                  SrcTableM,
                                  MaybeM,
                                  DomG,
                                  FPredL,
                                  VarTup,
                                  TablePtrTup,
                                  VPredL>) R;
    };
    
    typedef TCAP(list_index,
                 TCAP(list_map, unmark_maybe,
                      TCAP(list_unfilter, is_marked_silent,
                           typename _From::ArgL))) TrigTableM_;
public:
    // A list of selectors corresponding to each table
    typedef TCAP(list_map, make_trig_selector, TrigTableM_) TrigSelL;
    
    // Constructors, at most 6 parameters
#define DEFINE_CONNECTOR_VARIADIC_CONSTRUCTOR(_n_)                      \
    template <ST_SYMBOLL##_n_(typename _T)>                             \
    Connector (_DstTable* p_dst_table, ST_PARAML##_n_(const _T, *a))    \
        : p_dst_table_(p_dst_table)                                     \
    {                                                                   \
        init_tuple (&table_tup_, ST_SYMBOLL##_n_(a));                   \
        init_observers();                                               \
    }

    Connector (_DstTable* p_dst_table, TablePtrTup * table_tup)
        : table_tup_(*table_tup), p_dst_table_(p_dst_table) 
    {
        init_observers();
    }

    //DEFINE_CONNECTOR_VARIADIC_CONSTRUCTOR(1)
    DEFINE_CONNECTOR_VARIADIC_CONSTRUCTOR(2)
    DEFINE_CONNECTOR_VARIADIC_CONSTRUCTOR(3)
    DEFINE_CONNECTOR_VARIADIC_CONSTRUCTOR(4)
    DEFINE_CONNECTOR_VARIADIC_CONSTRUCTOR(5)
    DEFINE_CONNECTOR_VARIADIC_CONSTRUCTOR(6)
    DEFINE_CONNECTOR_VARIADIC_CONSTRUCTOR(7)
    DEFINE_CONNECTOR_VARIADIC_CONSTRUCTOR(8)
    
    // Make destination table consitent with source tables and watch
    // changes on source tables
    void connect ()
    {
        C_ASSERT (CAP(c_void, VarTypeL), there_is_VAR_inside_ST_WHERE);
        refresh();
        enable_observers();
    }
    
#define DEFINE_CONNECTOR_VARIADIC_CONNECT(_n_)                        \
    template <ST_SYMBOLL##_n_(typename _T)>                           \
    void connect (ST_PARAML##_n_(const _T, & a))                      \
    {                                                                 \
        init_tuple(&var_tup_, ST_SYMBOLL##_n_(a));                    \
        refresh();                                                    \
        enable_observers();                                           \
    }                                                                 \
    
    DEFINE_CONNECTOR_VARIADIC_CONNECT(1)
    DEFINE_CONNECTOR_VARIADIC_CONNECT(2)
    DEFINE_CONNECTOR_VARIADIC_CONNECT(3)
    DEFINE_CONNECTOR_VARIADIC_CONNECT(4)

    template <int _N, class _Table>
    void change_source_table (const _Table* p_table)
    { table_tup_.template at_n<_N>() = p_table; }

    // Re-fill destination table with joined tuples from source tables
    void refresh ();
    
    // Stop listening to source tables
    void disable_observers ();

    // Start listening to source tables
    void enable_observers ();
    
    static void c_to_string (std::ostream& os);
    
    // Pass in the function object for picking attributes to form tuples
    // into destination table
    // We need this method because _UserPick may have states, say local id
    // translation
    template <class _UserPick> void set_user_pick (const _UserPick& up)
    { tup_maker_.user_pick_ = up; }
    
protected:
    void init_observers ();

    class DefaultDstTupMaker;    // _Pick a list of attributes
    class UserDstTupMaker;       // _Pick is a function object
    class DstTupMaker : public CAP(if_, CAP(is_make_non_void_list, _Pick),
                                   DefaultDstTupMaker, UserDstTupMaker) {};

    // Get joined tuples triggered by p_tup    
    template <class _TrigSel> class InsertObserver;
    template <class _TrigSel> class EraseObserver;
    template <class _TrigSel> class InsertMaybeObserver;
    template <class _TrigSel> class EraseMaybeObserver;
    class ClearObserver;

    template <class _TrigSel> class make_observer_group;
    typedef BasicTuple<TCAP(list_map, make_observer_group, TrigSelL)> OGTup;

    
    TablePtrTup table_tup_;           // pointer tuple of source tables
    _DstTable* p_dst_table_;          // pointer to destination table
    VarTup var_tup_;                  // tuple of variables
    OGTup og_tup_;                    // tuple of observer groups
    DstTupMaker tup_maker_;           // may contain instance of _Pick if
                                      // if it's an user's function object
};

}  // namespace st

#include "detail/c_connector_inl.hpp"

#endif  //_C_CONNECTOR_HPP_

