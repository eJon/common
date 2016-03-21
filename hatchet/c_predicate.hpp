// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Compose and normalize selection criteria
// Author: gejun@baidu.com
// Date: Jan 30 13:44:35 CST 2011
#pragma once
#ifndef _C_PREDICATE_HPP_
#define _C_PREDICATE_HPP_

#include "attribute.h"            // AttributeTag
#include "c_list.hpp"             // list_* templates
#include "default_value.h"        // DEFAULT_VALUE_OF

namespace st {
// Declare a variable in predicate
template <typename> class VAR {};

// Used in SelectResult
struct VAR1 { static const int TABLE_POS = -1; };
struct VAR2 { static const int TABLE_POS = -2; };
struct VAR3 { static const int TABLE_POS = -3; };
struct VAR4 { static const int TABLE_POS = -4; };
struct VAR5 { static const int TABLE_POS = -5; };
struct VAR6 { static const int TABLE_POS = -6; };
struct VAR7 { static const int TABLE_POS = -7; };

template <typename _T> struct c_show_impl<VAR<_T> >
{ static void c_to_string (std::ostream& os) { os << "VAR"; } };

struct TableRepTag {};

template <typename _Attr> struct TableRep {
    typedef TableRepTag Tag;
    typedef _Attr Attr;
};

// Table representations, at most 7 tables
#define ST_DEFINE_TBL(n)                                                \
    template <typename _Attr> struct TBL##n : public TableRep<_Attr>    \
    { static const int TABLE_POS = n-1; };

ST_DEFINE_TBL(1)
ST_DEFINE_TBL(2)
ST_DEFINE_TBL(3)
ST_DEFINE_TBL(4)
ST_DEFINE_TBL(5)
ST_DEFINE_TBL(6)
ST_DEFINE_TBL(7)
ST_DEFINE_TBL(8)
ST_DEFINE_TBL(9)

#define ST_DEFINE_C_SHOW_IMPL_OF_TBL(n)                                 \
    template <typename _Attr> struct c_show_impl<TBL##n<_Attr> > {      \
        static void c_to_string (std::ostream& os)                      \
        { os << c_show(_Attr) << "/" << (n-1); }                        \
    };

ST_DEFINE_C_SHOW_IMPL_OF_TBL(1)
ST_DEFINE_C_SHOW_IMPL_OF_TBL(2)
ST_DEFINE_C_SHOW_IMPL_OF_TBL(3)
ST_DEFINE_C_SHOW_IMPL_OF_TBL(4)
ST_DEFINE_C_SHOW_IMPL_OF_TBL(5)
ST_DEFINE_C_SHOW_IMPL_OF_TBL(6)
ST_DEFINE_C_SHOW_IMPL_OF_TBL(7)

// builtin function object as equi predicate
struct always_true {
    template <typename _Any> bool operator() (const _Any&) const
    { return true; }
};

struct always_false {
    template <typename _Any> bool operator() (const _Any&) const
    { return false; }
};

struct always_false0 {
    bool operator() () const { return false; }
};

#define ST_DEFINE_BIN_OP(_name_, _op_)                                  \
    struct _name_ {                                                     \
        template <typename _T>                                          \
        bool operator() (const _T& t1, const _T& t2) const              \
        { return t1 _op_ t2; }                                          \
    };                                                                  \
    template <> struct c_show_impl<_name_> {                            \
        static void c_to_string (std::ostream& os) { os << #_name_; }   \
    };

ST_DEFINE_BIN_OP(eq, ==)
ST_DEFINE_BIN_OP(ne, !=)
ST_DEFINE_BIN_OP(gt, >)
ST_DEFINE_BIN_OP(ge, >=)
ST_DEFINE_BIN_OP(lt, <)
ST_DEFINE_BIN_OP(le, <=)

// ST_WHERE(...)
template <class _PredL> struct WhereClause { typedef _PredL PredL; };

template <ST_SYMBOLLD12(class _P, void)> struct make_where
{ typedef WhereClause<TCAP(list_filter, c_not_void,
                           ST_MAKE_LIST(ST_SYMBOLL12(_P)))> R; };
#define ST_WHERE(...) st::make_where<__VA_ARGS__>::R

// ST_FROM(...)
template <class _ArgL> struct FromClause { typedef _ArgL ArgL; };

template <ST_SYMBOLLD9(class _P, void)> struct make_from
{ typedef FromClause<TCAP(list_filter, c_not_void,
                          ST_MAKE_LIST(ST_SYMBOLL9(_P)))> R; };
#define ST_FROM(...) st::make_from<__VA_ARGS__>::R

#define ST_PICK(...) st::make_non_void_list<__VA_ARGS__>

// Maybe marker
template <class _Table, int _INDEX_POS = 0> class Maybe {};
template <class _Table, int _INDEX_POS = 0> class Exclude {};

const int INVALID_MAYBE_POS = 0x7FFFFFFF;

// Test if input is an instance of Maybe
template <typename _Any> struct get_maybe_index_pos
{ typedef Int<INVALID_MAYBE_POS> R; };

template <class _Table, int _INDEX_POS> 
struct get_maybe_index_pos<Maybe<_Table, _INDEX_POS> >
{ typedef Int<_INDEX_POS> R; };

template <class _Table, int _INDEX_POS> 
struct get_maybe_index_pos<Exclude<_Table, _INDEX_POS> >
{ typedef Int<-_INDEX_POS-1> R; };

template <class _Table> struct unmark_maybe { typedef _Table R; };

template <class _Table, int _INDEX_POS> 
struct unmark_maybe<Maybe<_Table, _INDEX_POS> > { typedef _Table R; };

template <class _Table, int _INDEX_POS> 
struct unmark_maybe<Exclude<_Table, _INDEX_POS> > { typedef _Table R; };



// Silent marker
template <typename> class Silent {};

template <typename> struct is_marked_silent
{ enum { R = false }; };
template <class _MTable> struct is_marked_silent<Silent<_MTable> >
{ enum { R = true }; };

template <typename _A> struct unmark_silent { typedef _A R; };
template <typename _A> struct unmark_silent<Silent<_A> > { typedef _A R; };

// A SelPos marks a unique place in a selection.
// It's probably:
//   - <sequence number of a table, an attribute>
//   - <type of a variable, its sequence in variables>
// Note: SelPos has direct correspondences to parameters of predicates. 
template <int _TABLE_POS, typename _Attr> struct SelPos {
    static const int TABLE_POS = _TABLE_POS;
    typedef _Attr Attr;
};

template <int _TABLE_POS, typename _Attr>
struct c_show_impl<SelPos<_TABLE_POS, _Attr> > {
    static void c_to_string (std::ostream& os)
    {
        if (_TABLE_POS < 0) {
            os << "VAR" << (-_TABLE_POS) << '/' << c_show(_Attr);
        } else {
            os << c_show(_Attr) << '/' << _TABLE_POS;
        }
    }
};

template <typename _Pred, typename _ArgL> struct NormalizedPred
    : public c_show_as<_Pred, _ArgL> {
    typedef _Pred Pred;
    typedef _ArgL ArgL;
};

template <int _N_VAR, typename _VarTypeL, typename _PosL> struct NormArgState {
    static const int N_VAR = _N_VAR;
    typedef _VarTypeL VarTypeL;
    typedef _PosL PosL;
};

template <int _N_VAR, typename _VarTypeL, typename _PredL>
struct NormPredState : public c_show_base {
    static const int N_VAR = _N_VAR;
    typedef _PredL PredL;
    typedef _VarTypeL VarTypeL;

    static void c_to_string (std::ostream& os)
    { os << "(var=" << c_show(_VarTypeL)
         << " pred=" << c_show(_PredL) << ")"; }
};


// Convert an attribute to a SelPos
// TODO: this name is improper
template <typename _Arg> struct attr_to_pos;

// Unify a predicate as a function object + a list of arguments
// Returns: a NormalizedPred where args are still not normalized
template <typename _Pred> class unify_pred;

// A template folder to normalize VAR and attributes
template <typename _NormArgState, typename _Attr> class normalize_arg;

// A template folder to normalize predicates
template <typename _State, typename _NPredL> struct normalize_pred;

// Normalize a list of predicates
template <typename _PredL> class normalize_pred_list;


// -------------------
//   Implementations
// -------------------
template <typename _Arg> class attr_to_pos {
    // Treat as a TableRep
    struct not_an_attribute
    { typedef SelPos<_Arg::TABLE_POS, typename _Arg::Attr> R; };
public:
    typedef TCAP(if_, CAP(c_same, typename _Arg::Tag, AttributeTag),
                 c_identity<SelPos<0, _Arg> >,
                 not_an_attribute)::R R;
};

// unify_pred
template <typename _F> struct unify_pred
{ typedef NormalizedPred<_F, void> R; };

#define ST_DEFINE_UNIFY_PRED_WITH_FN(n)                                 \
    template <typename _F, ST_SYMBOLL##n(typename _A)>                  \
    struct unify_pred<_F(ST_SYMBOLL##n(_A))> {                          \
        typedef NormalizedPred<_F, ST_MAKE_LIST(ST_SYMBOLL##n(_A))> R;  \
    };

ST_DEFINE_UNIFY_PRED_WITH_FN(1)
ST_DEFINE_UNIFY_PRED_WITH_FN(2)
ST_DEFINE_UNIFY_PRED_WITH_FN(3)

template <typename _F, typename _A1>
struct c_show_impl<_F(_A1)> {
    static void c_to_string (std::ostream& os)
    { os << c_show(_F) << '(' << c_show(_A1) << ')'; }
};

template <typename _F, typename _A1, typename _A2>
struct c_show_impl<_F(_A1, _A2)> {
    static void c_to_string (std::ostream& os)
    { os << c_show(_F) << '(' << c_show(_A1) << ',' << c_show(_A2) << ')'; }
};

template <typename _F, typename _A1, typename _A2, typename _A3>
struct c_show_impl<_F(_A1, _A2, _A3)> {
    static void c_to_string (std::ostream& os)
    { os << c_show(_F) << '(' << c_show(_A1) << ',' << c_show(_A2)
         << ',' << c_show(_A3) << ')'; }
};

// normalize_arg
template <int _N_VAR, typename _VarTypeL, typename _PosL, typename _Attr>
struct normalize_arg<NormArgState<_N_VAR, _VarTypeL, _PosL>, _Attr>
{ typedef NormArgState<_N_VAR, _VarTypeL,
                       Cons<TCAP(attr_to_pos, _Attr), _PosL> > R; };

// Specialize for VAR, variables are numbered from -1 (to negative infinity)
template <int _N_VAR, typename _VarTypeL, typename _PosL, typename _Type>
struct normalize_arg<NormArgState<_N_VAR, _VarTypeL, _PosL>, VAR<_Type> >
{ typedef NormArgState<_N_VAR+1, Cons<_Type, _VarTypeL>,
                       Cons<SelPos<-_N_VAR-1, _Type>, _PosL> > R; };

// normalize_pred
template <int _N_VAR, typename _VarTypeL,
          typename _PredL, typename _Pred, typename _ArgL>
class normalize_pred<NormPredState<_N_VAR, _VarTypeL, _PredL>,
                     NormalizedPred<_Pred, _ArgL> > {
    typedef TCAP(list_foldr, normalize_arg,
                 NormArgState<_N_VAR, _VarTypeL, void>,  _ArgL) S;
public:
    typedef NormPredState<S::N_VAR,
                          typename S::VarTypeL,
                          Cons<NormalizedPred<_Pred, typename S::PosL>,
                               _PredL> > R;
};

// normalize_pred_list
template <typename _PredL> class normalize_pred_list {
    typedef TCAP(list_foldl, normalize_pred, NormPredState<0, void, void>,
                 TCAP(list_map, unify_pred, _PredL)) S;
public:
    typedef NormPredState<S::N_VAR,
                          TCAP(list_reverse, typename S::VarTypeL),
                          TCAP(list_reverse, typename S::PredL)> R;
};

}  // namespace st

// These types are exposed because they're frequently used in Connector
using st::TBL1;
using st::TBL2;
using st::TBL3;
using st::TBL4;
using st::TBL5;
using st::TBL6;
using st::TBL7;
using st::TBL8;
using st::TBL9;

#endif  // _C_PREDICATE_HPP_

