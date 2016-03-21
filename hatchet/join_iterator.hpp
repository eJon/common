// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Iteratively represent result of a selection
// Author: gejun@baidu.com
// Date: Dec.7 14:10:24 CST 2010
#pragma once
#ifndef _JOIN_ITERATOR_HPP_
#define _JOIN_ITERATOR_HPP_

namespace st {
template <class _IterMakerL, class _IterTup,
          class _VarTup, class _TablePtrTup,
          class _PreFPredL>
struct JoinIteratorData : public c_show_base {
    typedef _IterMakerL IterMakerL;
    typedef _IterTup IterTup;
    typedef _VarTup VarTup;
    typedef _TablePtrTup TablePtrTup;
    typedef _PreFPredL PreFPredL;

    static void c_to_string (std::ostream& os);
};

template <class _Pos, class _IterTup, class _VarTup>
struct instantiate_attr;

template <class _Pos, class _IterTup, class _VarTup>
struct instantiate_var;

template <class _Pos, class _IterTup, class _VarTup>
class instantiate_pos;

template <class _FPred, class _IterTup, class _VarTup>
struct check_filter {
    static bool call (const _IterTup&, const _VarTup&);
};

template <class _FPredL, class _IterTup, class _VarTup>
struct check_filter_list {
    static bool call (const _IterTup&, const _VarTup&);
};

template <class _TSL, class _JID> struct init_join_iterator;

template <class _TSL, class _JID> struct forward_join_iterator;

template <class _TSL, class _JID>
class JoinIterator : public c_show_base {
public:
    // Number of iterators
    static const int N_ITER = CAP(list_size, _TSL);
    C_ASSERT (N_ITER >= 1, at_least_one_iterator);
    
    typedef TCAP(list_reverse, _TSL) RevTSL;
    static const int LAST_TABLE_POS = CAP(list_head, RevTSL)::TABLE_POS;
    
    typedef class _JID::VarTup VarTup;
    typedef class _JID::IterTup IterTup;
    typedef class _JID::TablePtrTup TablePtrTup;

    // Constructors
    JoinIterator () {}
    JoinIterator (const VarTup& var_tup, const TablePtrTup* p_table_tup);

    // Default destructor
    ~JoinIterator () {}
    
    bool init (const VarTup& var_tup, const TablePtrTup* p_table_tup);
    
    // Test valid position or not
    inline operator bool () const;
    // Move to ending position directly
    inline void set_end ();

    // Get value at a position by at<TBLn, Attribute>
    template <template <typename> class _TBLn, class _Attr>
    inline const typename _Attr::Type& at () const;

    // Get value at a position by at<TBLn<Attribute> >
    template <class _TableRep>
    inline const typename _TableRep::Attr::Type& at () const;

    template <int _POS>
    TCAP(VarTup::template type_at_n, _POS) const& var () const
    { return var_tup_.template at_n<_POS>(); }

    // Make it->at<...> possible
    const JoinIterator* operator-> () const { return this; }
    const JoinIterator& operator* () const { return *this; }
    
    // Move one step forward
    inline void operator++ ();
    
    static void c_to_string (std::ostream& os);

    const IterTup& tuple_of_sub_iterators() const { return iter_tup_; }
    
    template <int _N, typename _Iterator>
    void set_sub_iterator (const _Iterator& it)
    { iter_tup_.template at_n<_N>() = it; }

//protected:
    const TablePtrTup* p_table_tup_;  // where iterators were created from
    IterTup iter_tup_;                // tuple of iterators
    VarTup var_tup_;                  // tuple of variables

};

}  // namespace st

#include "detail/join_iterator_inl.hpp"

#endif  //_JOIN_ITERATOR_HPP_

