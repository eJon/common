// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// (smalltable1) Implement materialized views which select data from other
// tables under constraints and watch updates incrementally
// Author: gejun@baidu.com
// Date: Sun Nov  7 18:07:24 CST 2010

#pragma once
#ifndef _MAT_VIEW_HPP_
#define _MAX_VIEW_HPP_

#include "table.hpp"
#include "selector.hpp"

namespace st {
template <typename _Table, typename _MapFunc,
          typename _SrcTable0,  // first table should not be Maybe
          typename _SrcMTable1 = void,
          typename _SrcMTable2 = void,
          typename _SrcMTable3 = void,
          typename _SrcMTable4 = void>
class MatView;

template <typename _Table, typename _MapFunc, typename _SrcTable>
class MatView<_Table, _MapFunc, _SrcTable> {
public:
    typedef typename _Table::Tuple Tup;
    typedef typename _SrcTable::Tuple SrcTup;

    // static SrcTup dummy_typing_tuple_;
    // static _MapFunc dummy_typing_mapf_;
    // typedef typeof(dummy_typing_mapf_(dummy_typing_tuple_)) Tup;

    class BeginUpdateObserver : public BaseObserver<> {
    public:
        explicit BeginUpdateObserver (MatView* t) : this_(t) {}

        void on_event ()
        {
            if (this_->selector_.table_ptr()->mutability()
                && !this_->table_.mutability()) {
                this_->table_.begin_update ();
            }
        }
    private:
        MatView* this_;
    };

    class EndUpdateObserver : public BaseObserver<> {
    public:
        explicit EndUpdateObserver (MatView* t) : this_(t) {}

        void on_event ()
        {
            if (!this_->selector_.table_ptr()->mutability()
                && this_->table_.mutability()) {
                this_->table_.end_update ();
            }
        }
    private:
        MatView* this_;
    };

    class InsertObserver : public BaseObserver<const SrcTup*> {
    public:
        explicit InsertObserver (MatView* t) : this_(t) {}
        
        void on_event (const SrcTup* p_tuple)
        {
            if (this_->selector_.satisfied (*p_tuple)) {
                this_->table_.insert_tuple (f_(*p_tuple));
            }
        }
    private:
        MatView* this_;
        _MapFunc f_;
    };

    class EraseObserver : public BaseObserver<const SrcTup*> {
    public:
        explicit EraseObserver (MatView* t) : this_(t) {}
    
        void on_event (const SrcTup* p_tuple)
        {
            if (this_->selector_.satisfied (*p_tuple)) {
                this_->table_.erase_tuple (f_(*p_tuple));
            }
        }
    private:
        MatView* this_;
        _MapFunc f_;
    };

    class ClearObserver : public BaseObserver<> {
    public:
        explicit ClearObserver (MatView* t) : this_(t) {}

        void on_event ()
        {
            this_->table_.clear();
        }

    private:
        MatView* this_;
    };

    MatView ()
        : insert_ob_(this)
        , erase_ob_(this)
        , clear_ob_(this)
        , begin_update_ob_(this)
        , end_update_ob_(this)
    {}
    
    ~MatView ()
    {
        if (selector_.table_ptr()) {
            selector_.table_ptr()->insert_event.unsubscribe(&insert_ob_);
            selector_.table_ptr()->erase_event.unsubscribe(&erase_ob_);
            selector_.table_ptr()->clear_event.unsubscribe(&clear_ob_);
            selector_.table_ptr()->begin_update_event
                .unsubscribe(&begin_update_ob_);
            selector_.table_ptr()->end_update_event.unsubscribe(&end_update_ob_);
        }
    }

    template <typename _Predicate>
    int set_predicate (_SrcTable& table, _Predicate& a)
    {
        return set_predicate (table, Conjunction::create() && a);
    }
        
    int set_predicate (_SrcTable& src_table, Conjunction& conj)
    {
        int ret = 0;
            
        ret = selector_.set_predicate (src_table, conj);
        if (0 != ret) {
            ST_FATAL ("Fail to set_predicate");
            return ret;
        }
                
        ret = refresh ();
        if (0 != ret) {
            ST_FATAL ("Fail to refresh");
            return ret;
        }

        src_table.insert_event.subscribe(&insert_ob_);
        src_table.erase_event.subscribe(&erase_ob_);
        src_table.clear_event.subscribe(&clear_ob_);
        src_table.begin_update_event.subscribe(&begin_update_ob_);
        src_table.end_update_event.subscribe(&end_update_ob_);
            
        return 0;
    }

    int refresh ()
    {
        if (NULL == selector_.table_ptr()) {
            ST_FATAL ("missing set_predicate");
            return ENOTINIT;
        }

        bool previously_mutable = table_.mutability ();

        if (!previously_mutable) {
            table_.begin_update ();
        }
        table_.clear ();
        _MapFunc f;
        typename _SrcTable::iterator it;
        for (selector_.bg_select (&it); it.not_end(); ++it) {
            table_.insert_tuple (f(*it));
        }
        if (!previously_mutable) {
            table_.end_update ();
        }

        return 0;
    }

    const _Table& table() const { return table_; }

    const Selector<_SrcTable>& selector() const { return selector_; }

    void to_string (StringWriter& sw) const
    {
        sw << "{selector=" << selector_
           << " table=" << table_
           << "}";
    }
        
private:
    Selector<_SrcTable> selector_;
    _Table table_;
    InsertObserver insert_ob_;
    EraseObserver erase_ob_;
    ClearObserver clear_ob_;
    BeginUpdateObserver begin_update_ob_;
    EndUpdateObserver end_update_ob_;
};
    
}  // namespace st
#endif  // _MAX_VIEW_HPP_
