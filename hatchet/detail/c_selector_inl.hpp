// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Inline implementation of c_selector.hpp
// Author: gejun@baidu.com
// Date: Dec.7 14:10:24 CST 2010

namespace st {

template <class _DomeeL, class _DomerL>
void DomStat<_DomeeL, _DomerL>::c_to_string (std::ostream& os)
{
    os << "domee=" << c_show(_DomeeL)
       << " domer=" << c_show(_DomerL);
}


template <class _DS> struct get_DomerL { typedef typename _DS::DomerL R; };
template <> struct get_DomerL<void> { typedef void R; };

template <int _TABLE_POS, int _INDEX_POS,
          class _DomerL, class _FPredL,
          class _SeekInfo>
void TableSelection<_TABLE_POS, _INDEX_POS, _DomerL, _FPredL, _SeekInfo>::
c_to_string (std::ostream& os)
{
    os << " \"";
    const int score = CAP(getSeekScore, _SeekInfo);
    switch (score) {
    case COW_TABLE_SCORE:
        os << "Traverse T" << _TABLE_POS;
        break;
    default:
        os << "Seek";
        switch (score) {
        case COW_HASH_SCORE:
            os << "Hash";
            break;
        case MAYBE_COW_HASH_SCORE:
            os << (INDEX_POS >=0 ? "MaybeHash" : "ExcludeHash");
            break;
        case COW_HASH_CLUSTER2_SCORE:
            os << "HashCluster2";
            break;
        case MAYBE_COW_HASH_CLUSTER2_SCORE:
            os << (INDEX_POS >= 0 ? "MaybeHashCluster2" : "ExcludeHashCluster2");
            break;
        case COW_HASH_CLUSTER1_SCORE:
            os << "HashCluster1";
            break;
        case MAYBE_COW_HASH_CLUSTER1_SCORE:
            os << (INDEX_POS >= 0 ? "MaybeHashCluster1" : "ExcludeHashCluster1");
            break;
        }
        os << " T" << _TABLE_POS
           << "/I" << _INDEX_POS
           << '/' << c_show(_SeekInfo)
           << " by " << c_show(_DomerL);
    }
        
    if (CAP(c_not_void, _FPredL)) {
        os << '+' << c_show(_FPredL);
    }
    os << '\"';
}

template <class _From, class _Where>
void Selector2<_From, _Where>::c_to_string (std::ostream& os)
{
    os << "Selector{Plan=" << c_show(TSL)
       << ", From=" << c_show(TableL)
       << ", Where=" << c_show(_PredL)
       << '}';
}

// insert_dom
template <class _Pos1, class _Pos2, class _DomerL, class _DomG>
struct insert_dom {
    C_ASSERT_NOT_VOID (_Pos1, _Pos1_must_be_non_void);

    typedef TCAP(map_find, _Pos1, _DomG) S;
    
    struct on_non_void_S
    { typedef TCAP(map_insert, _Pos1,
                   // duplications in S::DomeeL is rare and does not
                   // affect correctness
                   DomStat<TCAP(list_append_non_void, _Pos2,
                                typename S::DomeeL),
                           TCAP(set_concat, _DomerL, typename S::DomerL)>,
                   _DomG) R; };

    struct on_void_S {
        typedef Cons<Pair<_Pos1,
                          DomStat<TCAP(list_append_non_void, _Pos2, void),
                                  _DomerL> >, _DomG> R;
    };
    
    typedef TCAP(if_void, S, on_void_S, on_non_void_S)::R R;
};

// dominate_pos
template <class _Pos, class _Domer, class _DomG>
struct dominate_pos {
    typedef TCAP(map_find, _Pos, _DomG) S;

    struct on_non_void_S {
        // Update domee only if original DomerL is void
        struct on_void_domer {
            // update _Pos
            typedef TCAP(map_insert, _Pos,
                         DomStat<typename S::DomeeL, Cons<_Domer, void> >,
                         _DomG) DomG2;

            template <class _DomG2, class _Pos2>
            struct dominate_fanout
            { typedef TCAP(dominate_pos, _Pos2, _Domer, _DomG2) R; };

            typedef TCAP(list_foldl, dominate_fanout, DomG2,
                         typename S::DomeeL) R;
        };

        typedef TCAP (if_void, typename S::DomerL,
                      on_void_domer, c_identity<_DomG>)::R R;
    };

    typedef TCAP(if_void, S, c_identity<_DomG>, on_non_void_S)::R R;

};


// collect_dom_and_fpred
template <class _DomG, class _FPredL, class _F, class _PosL>
struct collect_dom_and_fpred<Pair<_DomG, _FPredL>, NormalizedPred<_F, _PosL> > {
    template <class _DomG2, class _Pos> struct insert_trivial_dom
    { typedef TCAP(insert_dom, _Pos, void, void, _DomG2) R; };
        
    typedef Pair<TCAP(list_foldl, insert_trivial_dom, _DomG, _PosL),
                 Cons<NormalizedPred<_F, _PosL>, _FPredL> > R;
};


template <class _DomG, class _FPredL, class _Pos1, class _Pos2>
class collect_dom_and_fpred<Pair<_DomG, _FPredL>,
                    NormalizedPred<eq,
                                   Cons<_Pos1, Cons<_Pos2, void> > > > {
    struct add_bidirectional_doms
    { typedef Pair<TCAP(insert_dom, _Pos2, _Pos1, void,
                        TCAP(insert_dom, _Pos1, _Pos2, void, _DomG)),
                   _FPredL> R; };

    struct add_var_to_p1
    { typedef Pair<TCAP(insert_dom, _Pos1, void, Cons<_Pos2, void>, _DomG),
                   _FPredL> R; };

    struct add_var_to_p2
    { typedef Pair<TCAP(insert_dom, _Pos2, void, Cons<_Pos1, void>, _DomG),
                   _FPredL> R; };

    struct add_trivial_doms {
        typedef NormalizedPred<eq, Cons<_Pos1, Cons<_Pos2, void> > > FPred;
        
        typedef Pair<TCAP(insert_dom, _Pos2, void, void,
                        TCAP(insert_dom, _Pos1, void, void, _DomG)),
                     Cons<FPred, _FPredL> > R;
    };

public:
    typedef TCAP(if_, _Pos1::TABLE_POS >= 0,
                 TCAP(if_, _Pos2::TABLE_POS >= 0,
                      TCAP(if_, _Pos1::TABLE_POS == _Pos2::TABLE_POS,
                           add_trivial_doms,
                           add_bidirectional_doms),
                      add_var_to_p1),
                 // _Pos1 is varaible
                 TCAP(if_, _Pos2::TABLE_POS >= 0,
                      add_var_to_p2,
                      add_trivial_doms))::R R;
};

//  propagate_domination
template <class _DomG> class propagate_domination {
    template <class _DomG2, class _Dom>
    struct keep_domination_if_dominated {
        typedef class _Dom::Second DS;
    
        struct on_domer_exists {
            typedef TCAP(list_head, class DS::DomerL) FirstDomer;
            
            template <class _DomG3, class _Pos2> struct dominate_domee
            { typedef TCAP(dominate_pos, _Pos2, FirstDomer, _DomG3) R; };
        
            typedef TCAP(list_foldl, dominate_domee, _DomG2,
                         class DS::DomeeL) R;
        };

        typedef TCAP(if_void, class DS::DomerL,
                     c_identity<_DomG2>, on_domer_exists)::R R;
    };
public:
    typedef TCAP(list_foldl, keep_domination_if_dominated, _DomG, _DomG) R;
};

// build_dom_graph
template <class _NPredL> struct build_dom_graph {
    typedef TCAP(list_foldl, collect_dom_and_fpred, Pair<void, void>, _NPredL) P;

    typedef class P::First DomG;
    
    // Make sure domination relations are consistent
    typedef TCAP(propagate_domination, DomG) DomG2;
    
    typedef Pair<DomG2, class P::Second> R;
};

// dominate_table
template <int _TABLE_POS, class _DomG> struct dominate_table {
    template <class _Dom> struct get_unfixed_domer;
    
    template <class _Pos, class _DS>
    struct get_unfixed_domer <Pair<_Pos, _DS> > {
        typedef TCAP(if_, _TABLE_POS == _Pos::TABLE_POS
                          && CAP(c_void, class _DS::DomerL),
                     _Pos, void) R;
    };

    template <class _DomG2, class _Pos> struct dominate_pos0 {
        typedef TCAP(dominate_pos, _Pos, _Pos, _DomG2) R;
    };
    
    typedef TCAP(list_filter_map, get_unfixed_domer, _DomG) DomerL;
    typedef TCAP(list_foldl, dominate_pos0, _DomG, DomerL) DomG2;

    typedef DomG2 R;
};

// is_pos_dominated
template <class _Pos, class _DomG> struct is_pos_dominated {
    struct on_Pos_is_attr {
        typedef TCAP(map_find, _Pos, _DomG) S;

        struct on_non_void_S { enum { R = !CAP(c_void, class S::DomerL) }; };
    
        enum { R = CAP(if_void, S, Int<false>, on_non_void_S)::R };
    };

    static const bool R = CAP(if_, (_Pos::TABLE_POS < 0),
                              Int<true>, on_Pos_is_attr)::R;
};

// is_pred_dominated
template <class _NPred, class _DomG> struct is_pred_dominated {
    template <class _Pos2> struct is_pos_dominated0
    { enum { R = CAP(is_pos_dominated, _Pos2, _DomG) }; };
        
    static const bool R = CAP(list_all, is_pos_dominated0,
                              typename _NPred::ArgL);
};

// separate_dominated_pred
template <class _DomG, class _NPredL> struct separate_dominated_pred {
    template <class _NPred2> struct is_pred_dominated_
    { enum { R = CAP(is_pred_dominated, _NPred2, _DomG) }; };
    
    typedef TCAP(list_partition, is_pred_dominated_, _NPredL) R;
};

// is_var_pos
template <class _Pos> struct is_var_pos
{ static const bool R = (_Pos::TABLE_POS < 0); };

// is_var_pred
template <class _NPred> struct is_var_pred
{ static const bool R = CAP(list_all, is_var_pos, typename _NPred::ArgL); };

// try_table
template <int _TABLE_POS, class _Table, bool _IS_MAYBE, class _DomG>
struct try_table {
    template <class _Attr> struct collect_dominated_attr {
        typedef SelPos<_TABLE_POS, _Attr> Pos;
        typedef TCAP(if_, CAP(is_pos_dominated, Pos, _DomG), _Attr, void) R;
    };

    typedef TCAP(list_filter_map, collect_dominated_attr,
                 class _Table::Tup::AttrS) DomAttrS;

    template <class _IndexInfo> struct is_key_dominated
    { enum { R = CAP(set_include, DomAttrS, class _IndexInfo::SeekInfo::KeyAttrS) }; };

    typedef TCAP(list_find_first_true, is_key_dominated,
                 class _Table::IndexInfoL) II;
 
    
    struct on_index_available {
        typedef TableSelection<_TABLE_POS, II::POS, void, void,
                               typename II::SeekInfo> R;
    };

    typedef TCAP(if_void, II,
                 c_identity<TableSelection<_TABLE_POS, -1, void, void, void> >,
                 on_index_available)::R R;
};

template <class _FPredL, class _Pos, class _DomerL>
class accum_equi_filters {
    template <class _FPredL2, class _Domer> struct fold_domer_as_filter
    { typedef Cons<NormalizedPred<eq, Cons<_Pos, Cons<_Domer, void> > >,
                   _FPredL2> R; };
public:
    typedef TCAP(list_foldl, fold_domer_as_filter, _FPredL, _DomerL) R;
};

// Seperate domers of _KeyPos in _DomG into _DomerL and _FPredL
// first domer of _Pos is appended to _DomerL, other domers fall back 
// forming equi filters with _Pos
template <class _KeyPos, class _DomG, class _DomerL, class _FPredL>
class separate_key_domers {
    typedef TCAP(get_DomerL, TCAP(map_find, _KeyPos, _DomG)) PosDomerL;
    C_ASSERT_NOT_VOID (PosDomerL, PosDomerL_is_void);

public:
    typedef Pair<Cons<TCAP(list_head, PosDomerL), _DomerL>,
                 TCAP(accum_equi_filters, _FPredL, _KeyPos,
                      TCAP(list_tail, PosDomerL))> R;
};

// Convert domers of _Pos in _DomG to equi filters and append them to _FPredL
template <class _Pos, class _DomG, class _FPredL>
class separate_non_key_domers {
    typedef TCAP(get_DomerL, TCAP(map_find, _Pos, _DomG)) PosDomerL;
public:
    typedef TCAP(accum_equi_filters, _FPredL, _Pos, PosDomerL) R;
};

template <class _SeekInfo>
struct getKeyFromSeekInfo {
    typedef typename _SeekInfo::KeyAttrS R;
};

template <class _SeekInfo, int _>
struct getKeyFromSeekInfo<Maybe<_SeekInfo, _> > {
    typedef typename _SeekInfo::KeyAttrS R;
};

template <>
struct getKeyFromSeekInfo<void> {
    typedef void R;
};

// arrange_tables
template <class _TableM, class _MaybeM, class _DomG, class _FPredL>
class arrange_tables {
    template <class _P> struct try_table_with_DomG {
        enum { TABLE_POS = _P::First::R };
        typedef TCAP(
            try_table,
            TABLE_POS,
            typename _P::Second,
            CAP(c_not_void, TCAP(map_find, Int<TABLE_POS>, _MaybeM)),
            _DomG) R;
    };

    typedef TCAP(list_map, try_table_with_DomG, _TableM) TSL;
    typedef TCAP(list_stable_sort, ts_less_than, TSL) SortedTSL;
    typedef TCAP(list_head, SortedTSL) TS;

    // Check INDEX_POS of Maybe<>
    typedef TCAP(map_find, Int<TS::TABLE_POS>, _MaybeM) MaybePos;
    enum {
        IS_MAYBE = CAP(c_not_void, MaybePos),
        MAYBE_POS = CAP(if_, IS_MAYBE, MaybePos, Int<INVALID_MAYBE_POS>)::R
    };
    C_ASSERT(!IS_MAYBE ||
             (MAYBE_POS < 0 && TS::INDEX_POS == (-MAYBE_POS-1)) ||
             (MAYBE_POS >= 0 && TS::INDEX_POS == MAYBE_POS),
             chosen_index_pos_does_not_match_the_one_in_Maybe);

    typedef TCAP(map_find, Int<TS::TABLE_POS>, _TableM) ChosenTable;
    
    template <class _P,  // (DomerL, FPredL)
              class _KeyAttr> struct separate_key_domers_with_TS_DomG
    { typedef TCAP(separate_key_domers, SelPos<TS::TABLE_POS, _KeyAttr>,
                   _DomG, typename _P::First, typename _P::Second) R; };
    
    typedef TCAP(list_foldr, separate_key_domers_with_TS_DomG,
                 Pair<void, void>,
                 TCAP(getKeyFromSeekInfo, class TS::SeekInfo)) P1;
    typedef typename P1::First KeyDomerL;  // order is consistent with TS::Key
    typedef typename P1::Second KeyFPredL;  // fall-back equis on keys

    typedef TCAP(set_minus,
                 class ChosenTable::Tup::AttrS,
                 TCAP(getKeyFromSeekInfo, class TS::SeekInfo)) NonKeyAttrS;
    
    template <class _FPredL2, class _Attr>
    struct separate_non_key_domers_with_TS_DomG
    { typedef TCAP(separate_non_key_domers, SelPos<TS::TABLE_POS, _Attr>,
                   _DomG, _FPredL2) R; };

    typedef TCAP(list_foldl, separate_non_key_domers_with_TS_DomG,
                 KeyFPredL, NonKeyAttrS) EquiFPredL;
    
    typedef TCAP(dominate_table, TS::TABLE_POS, _DomG) ModDomG;
    
    typedef TCAP(separate_dominated_pred, ModDomG, _FPredL) P2;
    typedef typename P2::First DomFPredL;
    typedef typename P2::Second FreeFPredL;
    typedef TableSelection<TS::TABLE_POS, TS::INDEX_POS,
                           KeyDomerL, TCAP(list_concat, EquiFPredL, DomFPredL),
                           TCAP(if_, IS_MAYBE,
                                Maybe<typename TS::SeekInfo, MAYBE_POS>,
                                typename TS::SeekInfo)> TS2;
public:
    typedef Cons<TS2,
                 TCAP(arrange_tables, 
                      TCAP(map_erase, Int<TS::TABLE_POS>, _TableM),
                      _MaybeM,
                      ModDomG,
                      FreeFPredL)> R;
};

template <class _MaybeM, class _DomG, class _FPredL>
struct arrange_tables<void, _MaybeM, _DomG, _FPredL> {
    C_ASSERT_VOID(_FPredL,
                  An_attribute_does_not_belong_to_any_table_in_predicates);
    typedef void R;
};

// instantiate_tuple
template <class _AttrL, class _Tup, class _PosL, class _IterTup, class _VarTup>
struct instantiate_tuple {
#ifndef NDEBUG
    C_ASSERT_VOID(_AttrL, _AttrL_should_be_void);
    C_ASSERT_VOID(_PosL, _PosL_should_be_void);
#endif
    static void call (_Tup*, const _IterTup&, const _VarTup&) {}
};

template <class _Attr, class _AttrL, class _Tup,
          class _Pos, class _PosL, class _IterTup, class _VarTup>
struct instantiate_tuple<Cons<_Attr, _AttrL>, _Tup,
                       Cons<_Pos, _PosL>, _IterTup, _VarTup> {
    static void call (_Tup* p_tup,
                      const _IterTup& iter_tup, const _VarTup& var_tup)
    {
        p_tup->template at<_Attr>() =
            instantiate_pos<_Pos, _IterTup, _VarTup>::call(iter_tup, var_tup);
        
        instantiate_tuple<_AttrL, _Tup, _PosL, _IterTup, _VarTup>
            ::call(p_tup, iter_tup, var_tup);
    }
};

// Specialize IteratorMaker for CowHashSet
template <class _SeekInfo, class _TS, class _Table, class _IterTup, class _VarTup>
class IteratorMaker {
    typedef NamedTuple<typename _SeekInfo::KeyAttrS> Key;
    typedef typename _SeekInfo::SeekIterator Iterator;
    
public:
    static void make (
        Iterator* p_it, const _Table* p_table,
        const _IterTup& iter_tup, const _VarTup& var_tup)
    {
        Key key;
        instantiate_tuple<class Key::AttrS, Key, class _TS::DomerL,
            _IterTup, _VarTup>::call(&key, iter_tup, var_tup);
        *p_it = p_table->template index<_TS::INDEX_POS>().seek(key);
    }
};

template <class _SeekInfo, class _TS, int _MPOS, class _Table, class _IterTup, class _VarTup>
class IteratorMaker<Maybe<_SeekInfo, _MPOS>, _TS, _Table, _IterTup, _VarTup> {
    typedef NamedTuple<typename _SeekInfo::KeyAttrS> Key;
    typedef typename _SeekInfo::PartialSeekIterator Iterator;
    
public:
    static void make (Iterator* p_it, const _Table* p_table,
                      const _IterTup& iter_tup, const _VarTup& var_tup)
    {
        Key key;
        instantiate_tuple<class Key::AttrS, Key, class _TS::DomerL,
            _IterTup, _VarTup>::call(&key, iter_tup, var_tup);
        if (_MPOS >= 0) {
            p_table->template index<_TS::INDEX_POS>().maybe_seek(p_it, key);
        } else {
            p_table->template index<_TS::INDEX_POS>().exclude_seek(p_it, key);
        }
    }
};

// Specialize IteratorMaker for traversing CowTable
template <class _TS, class _Table, class _IterTup, class _VarTup>
class IteratorMaker<void, _TS, _Table, _IterTup, _VarTup> {
    typedef class _Table::Iterator Iterator;
    
public:
    static void make (Iterator* p_it, const _Table* p_table,
                      const _IterTup&, const _VarTup&)
    { *p_it = p_table->begin(); }
};

// Get TS of certain table
template <int _TABLE_POS, class _TSL> struct find_ts_of_table {
    template <class _TS> struct match_pos
    { static const bool R = (_TS::TABLE_POS == _TABLE_POS); };

    typedef TCAP(list_find_first_true, match_pos, _TSL) R;
};

template <class _SeekInfo, class _Table>
struct getIteratorFromSeekInfo {
    typedef typename _SeekInfo::SeekIterator R;
};

template <class _SeekInfo, int _MAYBE_POS, class _Table>
struct getIteratorFromSeekInfo<Maybe<_SeekInfo, _MAYBE_POS>, _Table> {
    typedef typename _SeekInfo::PartialSeekIterator R;
};

template <class _Table>
struct getIteratorFromSeekInfo<void, _Table> {
    typedef typename _Table::Iterator R;
};


template <class _TSL> struct iterator_of_the_table {
    template <class _P> struct HR {
        typedef TCAP(find_ts_of_table, _P::First::R, _TSL) TS;
        typedef TCAP(getIteratorFromSeekInfo,
                     class TS::SeekInfo, class _P::Second) R;
    };
};

template <class _TSL, class _IterTup, class _VarTup>
struct iter_maker_of_the_table {
    template <class _P> struct HR {
        typedef TCAP(find_ts_of_table, _P::First::R, _TSL) TS;
        typedef IteratorMaker<typename TS::SeekInfo, TS,
                              typename _P::Second,
                              _IterTup, _VarTup> R;
    };
};

}  // namespace st
