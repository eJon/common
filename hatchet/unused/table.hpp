// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// (smalltable1) an implementation of table in C++
// Author: gejun@baidu.com
// Date: Fri Jul 30 13:18:10 2010
#pragma once
#ifndef _TABLE_HPP_
#define _TABLE_HPP_

#include "index_manager.hpp"
#include "base_table.h"
#include "observer.hpp"
#include <pthread.h>

namespace st {    
// regular way to use Table:
// ... somewhere columns and tables are defined ...
// DEFINE_COLUMN(colume1, type1);
// DEFINE_COLUMN(colume2, type2);
// DEFINE_COLUMN(colume3, type3);
// ...
// Table<tuple_t<column1,column2...>, Primary<...>, NonPrimary<...> > t;
// ... somewhere tables are used ...
// t.begin_update();
// t.insert(...);
// t.erase (...);
// t.end_update();
// <optionally flip table>
// t.begin_update();
// t.insert(...);
// t.erase (...);
// t.end_update();

template <typename _Tuple,
          typename _I0 = void, typename _I1 = void, typename _I2 = void>
struct Table : public BaseTable {
    // type of a tuple/record/row
    typedef _Tuple Tuple;

    // type makers
    typedef CheckPrimary<_I0> CP0;
    typedef CheckPrimary<_I1> CP1;
    typedef CheckPrimary<_I2> CP2;
        
    // constants
    enum { PRIMARY_NUM =
           CP0::PRIMARY + CP1::PRIMARY + CP2::PRIMARY

           , NON_PRIMARY_NUM =
           CP0::NON_PRIMARY + CP1::NON_PRIMARY + CP2::NON_PRIMARY
               
           , HAS_DEFAULT_INDEX =
           (0==PRIMARY_NUM && 1!=NON_PRIMARY_NUM)

           , HAS_PRIMARY =
           (0<PRIMARY_NUM || HAS_DEFAULT_INDEX)
    };

        
    typedef typename TypeMaker<_I0, Tuple, HAS_PRIMARY>::Type IndexCandi0;
    typedef typename TypeMaker<_I1, Tuple, HAS_PRIMARY>::Type IndexCandi1;
    typedef typename TypeMaker<_I2, Tuple, HAS_PRIMARY>::Type IndexCandi2;

    typedef PrimaryHashIndex <
        typename Tuple::template SubFunc<
            ST_SYMBOLL10(typename Tuple::Param)> > DefaultIndex;

    // type of index manager
    // FIXME: DefaultIndex makes IndexCandi2 absent
    typedef TCAP (if_, HAS_DEFAULT_INDEX
                  , IndexManager<DefaultIndex, IndexCandi0, IndexCandi1>
                  , IndexManager<IndexCandi0, IndexCandi1, IndexCandi2>) IM;
    // type of this class
    typedef Table<_Tuple,_I0,_I1> Self;

    // type of iterator that represents result of selection
    typedef typename IM::iterator iterator;

    // type of the base class of hash indices
    typedef BaseHashIndex<Tuple> Index;

    // the iterator to traverse modifications
    typedef typename IM::mod_iterator mod_iterator;        
        
    void mod (mod_iterator* p_it) const
    { im_.mod(p_it); }
        
    // create this table
    explicit Table (const SimpleVersion* p_ver=NULL)
        : disable_update_(false)
        , mutable_(false)
    {
        this->header_.template record<Tuple> ();
        im_.sort_indexes (this->header_);

        this->create (p_ver);
        for (size_t i=0; i<im_.a_index_.size(); ++i) {
            im_.a_index_[i]->create (this->p_ver_);
        }

        pthread_mutex_init(&_write_lock, NULL);
    }

    // destroy this table
    ~Table()
    {}

    // choose an index from this table
    int choose_index (const Header& header, bool* p_is_primary=NULL) const
    { return im_.choose_index (header, p_is_primary); }

    // Add a tuple into this table, if the tuple exists, it overwrites the old one.
    // Returns: currently always return 0
    int insert_tuple (const Tuple& t)
    {
        if (unlikely (disable_update_)) {
            return 0;
        }
        if (unlikely (im_.a_index_.empty())) {
            ST_FATAL ("im_.a_index_ is empty");
            return EIMPOSSIBLE;
        }
        Index* p_index = im_.a_index_[0];
        std::pair<const Tuple*, const Tuple*> chg = p_index->insert(t);

        if (chg.first) {
            for (size_t i=1; i<im_.a_index_.size(); ++i) {
                im_.a_index_[i]->erase (*chg.first);
            }

            erase_event.notify(chg.first);
        }
        if (chg.second) {
            for (size_t i=1; i<im_.a_index_.size(); ++i) {
                im_.a_index_[i]->insert (*chg.second);
            }

            insert_event.notify(chg.second);
        }

        return 0;
    }
    
    int insert_tuple_with_lock (const Tuple& t)
    {
        pthread_mutex_lock(&_write_lock);
        int ret = insert_tuple(t);
        pthread_mutex_unlock(&_write_lock);
        return ret;
    }

    template <ST_SYMBOLL1(typename _T)>
    int insert (const _T0 &t0)
    { return insert_tuple(Tuple(t0)); }
        
    template <ST_SYMBOLL2(typename _T)>
    int insert (const _T0 &t0, const _T1 &t1)
    { return insert_tuple(Tuple(t0, t1)); }

    template <ST_SYMBOLL3(typename _T)>
    int insert (const _T0 &t0, const _T1 &t1, const _T2 &t2)
    { return insert_tuple(Tuple(t0, t1, t2)); }
        
    template <ST_SYMBOLL4(typename _T)>
    int insert (const _T0 &t0, const _T1 &t1, const _T2 &t2
                , const _T3 &t3)
    { return insert_tuple(Tuple(t0, t1, t2, t3)); }
        
    template <ST_SYMBOLL5(typename _T)>
    int insert (const _T0 &t0, const _T1 &t1, const _T2 &t2
                , const _T3 &t3, const _T4 &t4)
    { return insert_tuple(Tuple(t0, t1, t2, t3, t4)); }
        
    template <ST_SYMBOLL6(typename _T)>
    int insert (const _T0 &t0, const _T1 &t1, const _T2 &t2
                , const _T3 &t3, const _T4 &t4, const _T5 &t5)
    { return insert_tuple(Tuple(t0, t1, t2, t3, t4, t5)); }
        
    template <ST_SYMBOLL7(typename _T)>
    int insert (const _T0 &t0, const _T1 &t1, const _T2 &t2
                , const _T3 &t3, const _T4 &t4, const _T5 &t5
                , const _T6 &t6)
    { return insert_tuple(Tuple(t0, t1, t2, t3, t4, t5, t6)); }
        
    template <ST_SYMBOLL8(typename _T)>
    int insert (const _T0 &t0, const _T1 &t1, const _T2 &t2
                , const _T3 &t3, const _T4 &t4, const _T5 &t5
                , const _T6 &t6, const _T7 &t7)
    { return insert_tuple(Tuple(t0, t1, t2, t3, t4, t5, t6, t7)); }

    template <ST_SYMBOLL9(typename _T)>
    int insert (const _T0 &t0, const _T1 &t1, const _T2 &t2
                , const _T3 &t3, const _T4 &t4, const _T5 &t5
                , const _T6 &t6, const _T7 &t7, const _T8 &t8)
    { return insert_tuple(Tuple(t0, t1, t2, t3, t4, t5, t6, t7, t8)); }

    template <ST_SYMBOLL10(typename _T)>
    int insert (const _T0 &t0, const _T1 &t1, const _T2 &t2
                , const _T3 &t3, const _T4 &t4, const _T5 &t5
                , const _T6 &t6, const _T7 &t7, const _T8 &t8
                , const _T9 &t9)
    { return insert_tuple(Tuple(t0, t1, t2, t3, t4, t5, t6, t7, t8, t9)); }

    // Insert with locks
    template <ST_SYMBOLL1(typename _T)>
    int insert_with_lock (const _T0 &t0)
    { return insert_tuple_with_lock(Tuple(t0)); }
        
    template <ST_SYMBOLL2(typename _T)>
    int insert_with_lock (const _T0 &t0, const _T1 &t1)
    { return insert_tuple_with_lock(Tuple(t0, t1)); }

    template <ST_SYMBOLL3(typename _T)>
    int insert_with_lock (const _T0 &t0, const _T1 &t1, const _T2 &t2)
    { return insert_tuple_with_lock(Tuple(t0, t1, t2)); }
        
    template <ST_SYMBOLL4(typename _T)>
    int insert_with_lock (const _T0 &t0, const _T1 &t1, const _T2 &t2
                , const _T3 &t3)
    { return insert_tuple_with_lock(Tuple(t0, t1, t2, t3)); }
        
    template <ST_SYMBOLL5(typename _T)>
    int insert_with_lock (const _T0 &t0, const _T1 &t1, const _T2 &t2
                , const _T3 &t3, const _T4 &t4)
    { return insert_tuple_with_lock(Tuple(t0, t1, t2, t3, t4)); }
        
    // Erase a tuple from table, if the tuple does not exist, nothing happens
    // Returns: currently always return 0
    int erase_tuple (const Tuple& t)
    {
        if (unlikely (disable_update_)) {
            return 0;
        }
            
        if (unlikely (im_.a_index_.empty())) {
            ST_FATAL ("im_.a_index_ is empty");
            return 0;
        }
            
        Index* p_index = im_.a_index_[0];
        const Tuple* p_erased = p_index->erase(t);
        if (p_erased) {
            for (size_t i=1; i<im_.a_index_.size(); ++i) {
                im_.a_index_[i]->erase (*p_erased);
            }
            erase_event.notify(p_erased);
        }
        return 0;
    }
        
    template <ST_SYMBOLL1(typename _T)>
    int erase (const _T0 &t0)
    { return erase_tuple(Tuple(t0)); }
        
    template <ST_SYMBOLL2(typename _T)>
    int erase (const _T0 &t0, const _T1 &t1)
    { return erase_tuple(Tuple(t0, t1)); }
        
    template <ST_SYMBOLL3(typename _T)>
    int erase (const _T0 &t0, const _T1 &t1, const _T2 &t2)
    { return erase_tuple(Tuple(t0, t1, t2)); }
        
    template <ST_SYMBOLL4(typename _T)>
    int erase (const _T0 &t0, const _T1 &t1, const _T2 &t2
               , const _T3 &t3)
    { return erase_tuple(Tuple(t0, t1, t2, t3)); }
        
    template <ST_SYMBOLL5(typename _T)>
    int erase (const _T0 &t0, const _T1 &t1, const _T2 &t2
               , const _T3 &t3, const _T4 &t4)
    { return erase_tuple(Tuple(t0, t1, t2, t3, t4)); }
        
    template <ST_SYMBOLL6(typename _T)>
    int erase (const _T0 &t0, const _T1 &t1, const _T2 &t2
               , const _T3 &t3, const _T4 &t4, const _T5 &t5)
    { return erase_tuple(Tuple(t0, t1, t2, t3, t4, t5)); }

    template <ST_SYMBOLL7(typename _T)>
    int erase (const _T0 &t0, const _T1 &t1, const _T2 &t2
               , const _T3 &t3, const _T4 &t4, const _T5 &t5
               , const _T6 &t6)
    { return erase_tuple(Tuple(t0, t1, t2, t3, t4, t5, t6)); }

    template <ST_SYMBOLL8(typename _T)>
    int erase (const _T0 &t0, const _T1 &t1, const _T2 &t2
               , const _T3 &t3, const _T4 &t4, const _T5 &t5
               , const _T6 &t6, const _T7 &t7)
    { return erase_tuple(Tuple(t0, t1, t2, t3, t4, t5, t6, t7)); }

    template <ST_SYMBOLL9(typename _T)>
    int erase (const _T0 &t0, const _T1 &t1, const _T2 &t2
               , const _T3 &t3, const _T4 &t4, const _T5 &t5
               , const _T6 &t6, const _T7 &t7, const _T8 &t8)
    { return erase_tuple(Tuple(t0, t1, t2, t3, t4, t5, t6, t7, t8)); }

    template <ST_SYMBOLL10(typename _T)>
    int erase (const _T0 &t0, const _T1 &t1, const _T2 &t2
               , const _T3 &t3, const _T4 &t4, const _T5 &t5
               , const _T6 &t6, const _T7 &t7, const _T8 &t8
               , const _T9 &t9)
    { return erase_tuple(Tuple(t0, t1, t2, t3, t4, t5, t6, t7, t8, t9)); }


    const std::vector<const Tuple*>*
    erase_ref_by (int index_no
                  , const ObjectHanger& oh
                  , const typename Tuple::RefTuple& __restrict tuple
                  , const std::vector<Predicate*>* __restrict pa_filtering)
    {
        static std::vector<const Tuple*> dummy_empty;

        if (unlikely (disable_update_)) {
            return &dummy_empty;
        }
            
        if (index_no < 0 || index_no >= (int)index_num()) {
            ST_FATAL ("index_no=%d is invalid", index_no);
            return &dummy_empty;
        }

        const std::vector<const Tuple*>* pa_erased =
            im_.a_index_[index_no]->erase_ref_by (oh, tuple, pa_filtering);
        for (size_t i=0; i<im_.a_index_.size(); ++i) {
            if (index_no != (int)i) {
                Index* p_index = im_.a_index_[i];
                for (size_t j=0; j<pa_erased->size(); ++j) {
                    p_index->erase (*(*pa_erased)[j]);
                }
            }
        }

        // notify observers
        for (size_t j=0; j<pa_erased->size(); ++j) {
            erase_event.notify((*pa_erased)[j]);
        }
        
        return pa_erased;
    }
        
    const std::vector<const Tuple*>*
    erase_all_by (const std::vector<Predicate*>* __restrict pa_filtering)
    {
        static std::vector<const Tuple*> dummy_empty;

        if (unlikely (disable_update_)) {
            return &dummy_empty;
        }

        if (index_num() <= 0) {
            ST_FATAL ("erase_all_by from empty table%s"
                      , show(this->header_).c_str());
            return &dummy_empty;
        }

        const std::vector<const Tuple*>* pa_erased =
            im_.a_index_[0]->erase_all_by (pa_filtering);
        for (size_t i=1; i<im_.a_index_.size(); ++i) {
            Index* p_index = im_.a_index_[i];
            for (size_t j=0; j<pa_erased->size(); ++j) {
                p_index->erase (*(*pa_erased)[j]);
            }
        }

        // notify observers
        for (size_t j=0; j<pa_erased->size(); ++j) {
            erase_event.notify((*pa_erased)[j]);
        }

        return pa_erased;
    }

        
    // clear all background data
    void clear()
    {
        if (unlikely (disable_update_)) {
            return;
        }

        // clear one by one in reverse order
        // "reverse order" is not a must here
        for (int i=(int)im_.a_index_.size()-1; i>=0; --i) {
            im_.a_index_[i]->clear();
        }

        // notify observers
        clear_event.notify();
    }

    // get all elements of foreground buffer
    // @param [p_it] pointer to output iterator
    iterator bg_all () const
    {
        iterator it;
        all (&it, this->bg_no(), 0);
        return it;
    }

    iterator fg_all () const
    {
        iterator it;
        all (&it, this->fg_no(), 0);
        return it;
    }

    inline void bg_all (iterator* p_it) const
    { all (p_it, this->bg_no(), 0); }

    inline void fg_all (iterator* p_it) const
    { all (p_it, this->fg_no(), 0); }
        
    inline void all (iterator* p_it, int buf_no) const
    { all (p_it, buf_no, 0); }

    inline void all (iterator* p_it, int buf_no, int index_no) const
    {
        if (unlikely(index_no < 0 || index_no >= (int)im_.a_index_.size())) {
            p_it->set_end();
        } else {
            return im_.all (p_it, buf_no, index_no, NULL);
        }
    }
        
    // synchronize changes to background buffer and make it
    // identical with foreground buffer, this function is generally
    // called after "swap"
    // Returns: 0/-1: good or fail
    int begin_update ()
    {
        // in reverse order
        for (int i=(int)im_.a_index_.size()-1; i>=0; --i) {
            //for (size_t i=0; i<im_.a_index_.size(); ++i) {
            int ret = im_.a_index_[i]->begin_update();
            if (0 != ret) {
                ST_FATAL ("Fail to begin_update index[%d]", i);
                return ret;
            }
        }

        // this should happen before notifications
        mutable_ = true;

        // notify observers
        begin_update_event.notify();

        return 0;
    }

    // Write changs to background buffer,
    // add/del/clear do not take effects until this function is called.
    // Returns: 0/-1: good or fail
    int end_update ()
    {
        // in reverse order
        for (int i=(int)im_.a_index_.size()-1; i>=0; --i) {
            int ret = im_.a_index_[i]->end_update();
            if (0 != ret) {
                ST_FATAL ("Fail to end_update index[%d]", i);
                return ret;
            }
        }

        // this should happen before notifications
        mutable_ = false;
            
        // notify observers
        end_update_event.notify();

        return 0;
    }

    // mutable or not
    bool mutability () const
    { return mutable_; }

    // number of elements in given buffer
    size_t bg_size () const
    { return size (this->bg_no()); }

    size_t fg_size () const
    { return size (this->fg_no()); }

    size_t size (int buf_no) const
    { return im_.a_index_.empty() ? 0 : im_.a_index_[0]->size(buf_no); }

    // test if given buffer has any element
    bool empty (int buf_no) const
    { return im_.a_index_.empty() ? true : im_.a_index_[0]->empty(buf_no); }
        
    // show important infomation of this table, content is not printed
    void to_string (StringWriter& sb) const
    {
        sb << "Table" << this->header_ << "==>"
           << "{tuple_size=" << sizeof(Tuple)
           << " index_cnt=" << im_.a_index_.size()
            ;
        for (size_t i=0; i<im_.a_index_.size(); ++i) {
            // notice im_.a_index_[i] is different from index(i)
            sb << " index" << i << ":" << im_.a_index_[i]; 
        }
        sb << "}";
    }
        
        
    // get foreground index by position
    const Index* index(int idx) const
    { return im_.a_index_[idx]; }

    size_t index_num () const
    { return im_.a_index_.size(); }

    // get header of index by position, non-const version
    const Header& index_header(int idx) const
    { return im_.a_index_[idx]->header(); }

    // enable modifications to this table
    void enable_update ()
    { disable_update_ = false; }

    // ignore all modifications to this table.
    // begin_update/end_update are not affected by this function
    // insert_tuple/erase_tuple/insert/erase/erase_ref_by/erase_all_by/clear
    // are affected
    void disable_update ()
    { disable_update_ = true; }
        
    // create an attribute
    template <typename _I>
    Attribute<typename Tuple::template Field<_I>::Value>&
    a()
    {
        typedef typename Tuple::template Field<_I> F;
        typedef typename Tuple::template Field<_I>::Value T;
            
        return Attribute<T>::create (this, _I().name(), _I().id()
                                     , (int)F::IDX, (int)F::OFFSET);
    }

    size_t mem () const
    {
        size_t m = sizeof(*this);
        for (size_t i=0; i<im_.index_list().size(); ++i) {
            m += im_.index_list()[i]->mem();
        }
        return m;
    }


    void seek_ref_v (void* p_iterator
                     , int buf_no
                     , int index_no
                     , const ObjectHanger& oh
                     , const void* p_ref_tuple
                     , const std::vector<Predicate*>* __restrict pa_filter
        ) const
    {
        im_.seek_ref ((iterator*)p_iterator
                      , buf_no
                      , index_no
                      , oh
                      , *(const typename Tuple::RefTuple*)p_ref_tuple
                      , pa_filter
            );
    }
        
    void all_v (void* p_iterator
                , int buf_no
                , int index_no
                , const std::vector<Predicate*>* __restrict pa_filter
        ) const
    { im_.all ((iterator*)p_iterator, buf_no, index_no, pa_filter); }
        
        
//private:
    bool disable_update_;
    bool mutable_;
    IM im_;
    pthread_mutex_t _write_lock;

    // observers
public:
    Event<const Tuple*> insert_event;
    Event<const Tuple*> erase_event;
    Event<> clear_event;
    Event<> begin_update_event;
    Event<> end_update_event;
};

}

#endif  // _TABLE_HPP_
