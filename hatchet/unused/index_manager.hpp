// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// (smalltable1) Manage compile-time defined indexes
// Author: gejun@baidu.com
// Date: Wed Aug 11 14:57:47 2010
#pragma once
#ifndef _INDEX_MANAGER_HPP_
#define _INDEX_MANAGER_HPP_

#include "primary_hash_index.hpp"
#include "non_primary_hash_index.hpp"

namespace st {
template <typename _A0,      typename _A1=void, typename _A2=void,
          typename _A3=void, typename _A4=void, typename _A5=void,
          typename _A6=void, typename _A7=void, typename _A8=void,
          typename _A9=void>
struct Primary {};

template <typename _A0
          , typename _A1=void
          , typename _A2=void
          , typename _A3=void
          , typename _A4=void
          , typename _A5=void
          , typename _A6=void
          , typename _A7=void
          , typename _A8=void
          , typename _A9=void
          >
struct NonPrimary {};

template <typename _Anything>
struct CheckPrimary {
    enum { PRIMARY = 0, NON_PRIMARY = 0 };
};

template <ST_SYMBOLL10(typename _A)>
struct CheckPrimary<Primary<ST_SYMBOLL10(_A)> > {
    enum { PRIMARY = 1, NON_PRIMARY = 0 };
};

template <ST_SYMBOLL10(typename _A)>
struct CheckPrimary<NonPrimary<ST_SYMBOLL10(_A)> > {
    enum { PRIMARY = 0, NON_PRIMARY = 1 };
};
    
// ----
template <typename _Anything, typename _Tup, bool _Dummy>
struct TypeMaker {
    typedef void Type;
};

template <ST_SYMBOLL10(typename _A), typename _Tup, bool _Dummy>
struct TypeMaker<Primary<ST_SYMBOLL10(_A)>,_Tup,_Dummy> {
    typedef PrimaryHashIndex<
        typename _Tup::template SubFunc<ST_SYMBOLL10(_A)> > Type;
};

// FIXME
template <ST_SYMBOLL10(typename _A), typename _Tup, bool _StorePtr>
struct TypeMaker<NonPrimary<ST_SYMBOLL10(_A)>,_Tup,_StorePtr> {
    typedef NonPrimaryHashIndex<
        typename _Tup::template SubFunc<ST_SYMBOLL10(_A)>
        , (((sizeof(void*)*3/2)<sizeof(_Tup))?_StorePtr:false)> Type;
};



// base of IndexManagers
template <typename _Tup>
class BaseIndexManager {
public:
    typedef BaseHashIndex<_Tup> Index;
        
    const std::vector<Index*>& index_list () const
    { return a_index_; }

    // functor to order index
    struct CompareIndexFunc {
        bool operator() (const Index* p_index1, const Index* p_index2) const
        {
            if (p_index1->primary() != p_index2->primary()) {
                return p_index1->primary();
            }

            // both primary_p_index or non_primary_index
            return p_index1->header().contains (p_index2->header());
        }
    };


    // @brief create indices according to index creation requests,
    // generally this function should not be called.
    // @retval true/false ready/not-ready
    int sort_indexes (const Header& table_header)
    {
        int primary_cnt = 0;
        int non_primary_cnt = 0;

        for (size_t i=0; i<a_index_.size(); ++i) {
            Index* p_index = a_index_[i];
            if (p_index->primary()) {
                ++ primary_cnt;
                if (primary_cnt > 1) {
                    ST_FATAL ("more than one primary_key for table(%s),"
                              "content of the table is undefined"
                              , show(table_header).c_str());
                }
            } else {
                ++ non_primary_cnt;
            }
        }

        if (0 == primary_cnt && 0 == non_primary_cnt) {
            ST_FATAL ("No index for table(%s)", show(table_header).c_str());
        }

        // sort index topologically by their keys here
        std::vector<Index*> before_sort = a_index_;
        std::stable_sort
            (a_index_.begin(), a_index_.end(), CompareIndexFunc());
        // this is intended to point incorrect addressing of index
        // to the first one
        memset (m_idx_no_, 0, sizeof(m_idx_no_));
        for (size_t i=0; i<a_index_.size(); ++i) {
            for (size_t j=0; j<before_sort.size(); ++j) {
                if (a_index_[i] == before_sort[j]) {
                    m_idx_no_[i] = j;
                    break;
                }
            }
        }
        //LOG ("Sorted indices of table %s", show(table_header).c_str());
        return 0;
    }

    // @brief pick an index by header, generally the index should be
    // fastest at querying an instance of the header
    // @retval >=0 position of the index
    // @retval -1 the table is unready or no index is available
    int choose_index (const Header& header, bool* p_is_primary=NULL) const
    {
        for (size_t i=0; i<a_index_.size(); ++i) {
            //cout << "header[" << i << "]=" << show(a_index[i]->header())
            //     << "iheader=" << show(header) << endl;
            if (header.contains(a_index_[i]->header())) {
                if (p_is_primary) {
                    *p_is_primary = a_index_[i]->primary();
                }
                return i;
            }
        }
        return -1;
    }
        
    std::vector<BaseHashIndex<_Tup>*> a_index_;
    int m_idx_no_[8];
};


// -----------------------------------
template <typename _I0=void, typename _I1=void, typename _I2=void>
struct IndexManager;

// IndexManager with one table, this is most common and we want performance here
template <typename _I0>
class IndexManager<_I0> : public BaseIndexManager<typename _I0::Tup>
{
public:
    typedef typename _I0::Tup Tup;
    typedef typename Tup::RefTuple RefTup;
        
    IndexManager ()
    {
        this->a_index_.push_back (&index0_);
    }
        
    struct iterator {
        void operator++()
        {
            switch (HI_TYPE(it_type_)) {
            case HI_SEEK:
                evil_.p_cur_ = NULL;
                return;
            case HI_COWBASS:
                ++ (CowbassIterator<Tup,false>&)evil_;
                return;
            case HI_COWBASS_PTR:
                ++ (CowbassIterator<Tup,true>&)evil_;
                return;
            case HI_SLOWER:
                p_index_->forward_slower_iterator
                    (&evil_, HI_SUB_TYPE(it_type_));
                return;
            }
        }

        void set_end () { evil_.p_cur_ = NULL; }
        operator bool () const { return NULL != evil_.p_cur_; }

        bool operator== (const iterator& rhs) const
        { return evil_.p_cur_ == rhs.evil_.p_cur_; }

        bool operator!= (const iterator& rhs) const
        { return evil_.p_cur_ != rhs.evil_.p_cur_; }
        
        Tup& operator* () const { return *evil_.p_cur_; }
        Tup* operator-> () const { return evil_.p_cur_; }

        int it_type_;
        BaseHashIndex<Tup>* p_index_;
        struct {
            Tup* p_cur_;
            char d_[_I0::ITERATOR_SIZE - sizeof(Tup*)];
        } evil_;
    };

    void seek_ref (iterator* p_it, int buf_no, int index_no /*must be zero*/,
                   const ObjectHanger& oh, const RefTup& tuple,
                   const std::vector<Predicate*>* pa_filter) const
    {
        p_it->it_type_ = index0_.seek_ref
            (&(p_it->evil_), buf_no, oh, tuple, pa_filter);
        if (HI_SLOWER == HI_TYPE(p_it->it_type_)) {
            p_it->p_index_ = this->a_index_[index_no];
        }
    }

    void all (iterator* p_it, int buf_no,
              int /*index_no, must be zero*/,
              const std::vector<Predicate*>* pa_filter) const
    {
        p_it->it_type_ = index0_.all (&(p_it->evil_), buf_no, pa_filter);
        p_it->p_index_ = this->a_index_[0];
    }

    // notice following checking sequence, _I1->_I0 which guarantees
    // the primary index is selected if there's any otherwise use _I0
    typedef typename _I0::mod_iterator mod_iterator;
        
    void mod (mod_iterator* p_it) const
    { index0_.setup_mod_iterator(p_it); }
        
private:
    _I0 index0_;
};
    

template <typename _I0, typename _I1>
struct IndexManager<_I0,_I1> : public BaseIndexManager<typename _I0::Tup>
{
public:
    typedef typename _I0::Tup Tup;
    typedef typename Tup::RefTuple RefTup;

    IndexManager ()
    {
        this->a_index_.push_back (&index0_);
        this->a_index_.push_back (&index1_);
    }
        
    struct iterator {
        void operator++()
        {
            switch (HI_TYPE(it_type_)) {
            case HI_SEEK:
                evil_.p_cur_ = NULL;
                return;
            case HI_COWBASS:
                ++ (CowbassIterator<Tup,false>&)evil_;
                return;
            case HI_COWBASS_PTR:
                ++ (CowbassIterator<Tup,true>&)evil_;
                return;
            case HI_SLOWER:
                p_index_->forward_slower_iterator
                    (&evil_, HI_SUB_TYPE(it_type_));
                return;
            }
        }

        void set_end () { evil_.p_cur_ = NULL; }
        operator bool () const { return NULL != evil_.p_cur_; }

        bool operator== (const iterator& rhs) const
        { return evil_.p_cur_ == rhs.evil_.p_cur_; }

        bool operator!= (const iterator& rhs) const
        { return evil_.p_cur_ != rhs.evil_.p_cur_; }

        Tup& operator* () const { return *evil_.p_cur_; }
        Tup* operator-> () const { return evil_.p_cur_; }

        int it_type_;
        BaseHashIndex<Tup>* p_index_;
        union CountIteratorSize {
            char dummy0_[_I0::ITERATOR_SIZE];
            char dummy1_[_I1::ITERATOR_SIZE];
        };
        struct {
            Tup* p_cur_;
            char d_[sizeof(CountIteratorSize) - sizeof(Tup*)];
        } evil_;
    };

    void seek_ref (iterator* p_it, int buf_no, int index_no,
                   const ObjectHanger& oh, const RefTup& tuple,
                   const std::vector<Predicate*>* pa_filter) const
    {
        switch (this->m_idx_no_[index_no]) {
        case 0:
            p_it->it_type_ = index0_.seek_ref
                (&(p_it->evil_), buf_no, oh, tuple, pa_filter);
            break;
        case 1:
            p_it->it_type_ = index1_.seek_ref
                (&(p_it->evil_), buf_no, oh, tuple, pa_filter);
            break;
        }
        if (HI_SLOWER == HI_TYPE(p_it->it_type_)) {
            p_it->p_index_ = this->a_index_[index_no];
        }
    }
    
    void all (iterator* p_it, int buf_no, int index_no
              , const std::vector<Predicate*>* pa_filter) const
    {
        switch (this->m_idx_no_[index_no]) {
        case 0:
            p_it->it_type_ = index0_.all (&(p_it->evil_), buf_no, pa_filter);
            break;
        case 1:
            p_it->it_type_ = index1_.all (&(p_it->evil_), buf_no, pa_filter);
            break;
        }
        p_it->p_index_ = this->a_index_[index_no];
    }

    // notice following checking sequence, _I1->_I0 which guarantees
    // the primary index is selected if there's any otherwise use _I0
    typedef TCAP(if_, _I1::PRIMARY, _I1, _I0) MaybePrimary;
        
    typedef typename MaybePrimary::mod_iterator mod_iterator;
        
    void mod (mod_iterator* p_it) const
    {
        // must be consistent with definition of MaybePrimary
        if (index1_.primary()) {
            index1_.setup_mod_iterator(p_it);
        } else {
            index0_.setup_mod_iterator(p_it);
        }
    }
        
private:
    _I0 index0_;
    _I1 index1_;
};
    
    
template <typename _I0, typename _I1, typename _I2>
struct IndexManager
    : public BaseIndexManager<typename _I0::Tup>
{
public:
    typedef typename _I0::Tup Tup;
    typedef typename Tup::RefTuple RefTup;

    IndexManager ()
    {
        this->a_index_.push_back (&index0_);
        this->a_index_.push_back (&index1_);
        this->a_index_.push_back (&index2_);
    }
        
    struct iterator {
        void operator++()
        {
            switch (HI_TYPE(it_type_)) {
            case HI_SEEK:
                evil_.p_cur_ = NULL;
                return;
            case HI_COWBASS:
                ++ (CowbassIterator<Tup,false>&)evil_;
                return;
            case HI_COWBASS_PTR:
                ++ (CowbassIterator<Tup,true>&)evil_;
                return;
            case HI_SLOWER:
                p_index_->forward_slower_iterator(&evil_, HI_SUB_TYPE(it_type_));
                return;
            }
        }

        void set_end () { evil_.p_cur_ = NULL; }
        operator bool () const { return NULL != evil_.p_cur_; }

        bool operator== (const iterator& rhs) const
        { return evil_.p_cur_ == rhs.evil_.p_cur_; }

        bool operator!= (const iterator& rhs) const
        { return evil_.p_cur_ != rhs.evil_.p_cur_; }
        
        Tup& operator* () const { return *evil_.p_cur_; }
        Tup* operator-> () const { return evil_.p_cur_; }
            
        int it_type_;
        BaseHashIndex<Tup>* p_index_;
        union CountIteratorSize {
            char dummy0_[_I0::ITERATOR_SIZE];
            char dummy1_[_I1::ITERATOR_SIZE];
            char dummy2_[_I2::ITERATOR_SIZE];
        };
        struct {
            Tup* p_cur_;
            char placeholder_[sizeof(CountIteratorSize)-sizeof(Tup*)];
        } evil_;
    };

    void seek_ref (iterator* p_it, int buf_no, int index_no,
                   const ObjectHanger& oh, const RefTup& tuple,
                   const std::vector<Predicate*>* pa_filter) const
    {
        //switch (__builtin_expect(index_no,0)) {
        switch (this->m_idx_no_[index_no]) {
        case 0:
            p_it->it_type_ = index0_.seek_ref
                (&(p_it->evil_), buf_no, oh, tuple, pa_filter);
            break;
        case 1:
            p_it->it_type_ = index1_.seek_ref
                (&(p_it->evil_), buf_no, oh, tuple, pa_filter);
            break;
        case 2:
            p_it->it_type_ = index2_.seek_ref
                (&(p_it->evil_), buf_no, oh, tuple, pa_filter);
            break;
        }
        if (HI_SLOWER == HI_TYPE(p_it->it_type_)) {
            p_it->p_index_ = this->a_index_[index_no];
        }
    }

    void all (iterator* p_it, int buf_no, int index_no,
              const std::vector<Predicate*>* pa_filter) const
    {
        switch (this->m_idx_no_[index_no]) {
        case 0:
            p_it->it_type_ = index0_.all (&(p_it->evil_), buf_no, pa_filter);
            break;
        case 1:
            p_it->it_type_ = index1_.all (&(p_it->evil_), buf_no, pa_filter);
            break;
        case 2:
            p_it->it_type_ = index2_.all (&(p_it->evil_), buf_no, pa_filter);
            break;
        }
        p_it->p_index_ = this->a_index_[index_no];
    }
        
    // notice following checking sequence, _I2->_I1->_I0 which guarantees
    // the primary index is selected if there's any otherwise use _I0
    typedef TCAP(if_, _I2::PRIMARY,
                 _I2,
                 TCAP(if_, _I1::PRIMARY, _I1, _I0)) MaybePrimary;
        
    typedef typename MaybePrimary::mod_iterator mod_iterator;
        
    void mod (mod_iterator* p_it) const
    {
        // must be consistent with definition of MaybePrimary
        if (index2_.primary()) {
            index2_.setup_mod_iterator(p_it);
        } else if (index1_.primary()) {
            index1_.setup_mod_iterator(p_it);
        } else {
            index0_.setup_mod_iterator(p_it);
        }
    }

private:
    _I0 index0_;
    _I1 index1_;
    _I2 index2_;
};
}

#endif  // _INDEX_MANAGER_HPP_
