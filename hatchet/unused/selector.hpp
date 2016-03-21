// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// (smalltable1) Select tuples out from table
// Author: gejun@baidu.com
// Date: Fri Jul 30 13:18:38 2010
#pragma once
#ifndef _SELECTOR_HPP_
#define _SELECTOR_HPP_

#include "predicate.hpp"
#include "table.hpp"

namespace st {
    template <typename _Table> struct Maybe1 {
        typedef _Table Table;
    };

    template <typename _Table> struct IsMaybe1 {
        enum { YES = false };
        typedef _Table Table;
    };

    template <typename _Table> struct IsMaybe1<Maybe1<_Table> > {
        enum { YES = true };
        typedef _Table Table;
    };
        
    
    template <typename _Table0  // first table should not be Maybe1
              , typename _MTable1=void
              , typename _MTable2=void
              , typename _MTable3=void
              , typename _MTable4=void
              >
    struct Selector;
    
    // select data out from a single table, focus is performance
    template <typename _Table> struct Selector<_Table> {
        typedef typename _Table::Tuple Tuple;
        
        explicit Selector ()
            : p_table_(NULL), pa_filter_(&a_filter_)
            , index_no_(-1), p_conj_(NULL)
        { memset (&ref_tuple_, 0, sizeof(ref_tuple_)); }

        ~Selector ()
        {
            if (p_conj_) {
                delete p_conj_;
                p_conj_ = NULL;
            }
        }
        
        template <typename T>
        typename Tuple::template Field<T>::Value* var()
        { return &(var_tuple_.template at<T>()); }

        template <typename T>
        void set_var(const typename Tuple::template Field<T>::Value& v)
        { var_tuple_.template at<T>() = v; }

        int fg_select (typename _Table::iterator* __restrict p_it) const 
        { return select(p_it, p_table_->fg_no()); }
        
        int bg_select (typename _Table::iterator* __restrict p_it) const
        { return select(p_it, p_table_->bg_no()); }

        void set_false()
        {
            a_assign_.clear();
            a_filter_.clear();
            index_no_ = -1;
            a_filter_.push_back (&(FalsePredicate::create()));
        }
        
        int select (typename _Table::iterator* __restrict p_it
                    , int buf_no
                    ) const
        {
            if (likely( index_no_ >= 0 )) {
                p_table_->im_.seek_ref
                    (p_it, buf_no, index_no_, oh_, ref_tuple_, pa_filter_);
            } else {
                p_table_->im_.all(p_it, buf_no, 0, pa_filter_);
            }
            return 0;
        }

        const std::vector<const Tuple*>* erase ()
        {
            if (likely( index_no_ >= 0 )) {
                return p_table_->erase_ref_by
                    (index_no_, oh_, ref_tuple_, pa_filter_);
            } else {
                return p_table_->erase_all_by(pa_filter_);
            }
        }

        
        // for single predicate
        template <typename _Predicate>
        int set_predicate (_Table& table, _Predicate& a)
        {
#if _BullseyeCoverage
                #pragma BullseyeCoverage off
#endif
            return set_predicate (table, Conjunction::create() && a);
#if _BullseyeCoverage
                #pragma BullseyeCoverage on
#endif
        }

        
        // return -1 should mean selecting nothing out
        int set_predicate (_Table& table, Conjunction& conj)
        {
            if (p_conj_) {
                delete p_conj_;
            }
            p_conj_ = &conj;
            
            p_table_  = &table;
            const void* pp_table[] = { p_table_ };

            Header h;
            std::vector<Predicate*> a_assign_candi;
            a_assign_.clear();
            a_filter_.clear();
                
            std::vector<Predicate*>& a_bool = p_conj_->pred_list();
            for (size_t i=0; i<a_bool.size(); ++i) {
                if (NULL == a_bool[i]) {
                    ST_FATAL ("a_bool[%lu] is NULL", i);
                    set_false ();
                    return EIMPOSSIBLE;
                }
                switch (a_bool[i]->categorize(pp_table, 1)) {
                case PRED_ASSIGN:
                    a_assign_candi.push_back (a_bool[i]);
                    break;
                case PRED_FILTER:
                    a_filter_.push_back (a_bool[i]);
                    break;
                case PRED_INVALID:
                    ST_FATAL ("table=%p, predicate=%s is invalid"
                              , p_table_, show(a_bool[i]).c_str());
                    set_false();
                    return EGENERAL;
                }
            }

            // compose header
            for (size_t i=0; i<a_assign_candi.size(); ++i) {
                if (!h.insert (a_assign_candi[i]->attr_id(p_table_))) {
                    // same-named attribute exists, this predicate falls
                    // back to be a filter
                    a_filter_.push_back (a_assign_candi[i]);
                    a_assign_candi[i] = NULL;
                }
            }

            index_no_ = p_table_->choose_index (h);
            if (index_no_ < 0) {
                ST_WARN ("can't find a suitable index");
                // all predicates fall back to be filters
                for (size_t i=0; i<a_assign_candi.size(); ++i) {
                    if (a_assign_candi[i]) {
                        a_filter_.push_back (a_assign_candi[i]);
                    }
                }
            } else {
                // predicates that does not covered by the header of
                // selected index fall back to be filters
                const Header& h = p_table_->index_header(index_no_);
                for (size_t i=0; i<a_assign_candi.size(); ++i) {
                    if (a_assign_candi[i]) {
                        if (h.contains (a_assign_candi[i]->attr_id(p_table_))) {
                            a_assign_.push_back (a_assign_candi[i]);
                        } else {
                            a_filter_.push_back (a_assign_candi[i]);
                        }
                    }
                }
            }

            oh_.clear();
            for (size_t i=0; i<a_assign_.size(); ++i) {
                a_assign_[i]->set_ref_tuple
                    (p_table_, &ref_tuple_
                     , pp_table, 0/*don't need pp_table*/, oh_);
            }

            if (a_filter_.empty()) {
                pa_filter_ = NULL;
            } else {
                pa_filter_ = &a_filter_;
            }

            ST_DEBUG ("selector(%p)=%s", this, show(*this).c_str());
            return 0;
        }

        void to_string (StringWriter& sw) const
        {
            sw.append_format ("{table=%p", p_table_);
            sw << ", index_no=" << index_no_
               << ", a_assign=" << show_container(a_assign_)
               << ", a_filter=" << show_container(a_filter_)
               << ", ref_tuple=" << ref_tuple_
               << "}";
        }

        _Table* table_ptr() const
        { return p_table_; }

        bool satisfied (const Tuple& tup) const
        {
            for (size_t i=0; i<a_filter_.size(); ++i) {
                if (!a_filter_[i]->passed((const char*)&tup)) {
                    return false;
                }
            }
            for (size_t i=0; i<a_assign_.size(); ++i) {
                if (!a_assign_[i]->passed((const char*)&tup)) {
                    return false;
                }
            }
            return true;
        }

        //private:
        _Table* p_table_;
        typename Tuple::RefTuple ref_tuple_;
        Tuple var_tuple_;
        std::vector<Predicate*> a_filter_;
        std::vector<Predicate*>* pa_filter_;
        std::vector<Predicate*> a_assign_;
        int index_no_;
        ObjectHanger oh_;
        Conjunction* p_conj_;
    };
    

    struct BasicSelector {
        virtual ~BasicSelector() {}
        
        virtual void set_predicate
        (const std::vector<Predicate*>& a_assign_candi
         , const std::vector<Predicate*>& a_filter_candi
         , const int fixed_table_mask) = 0;

        virtual void set_ref_tuple
        (const void** pp_table
         , const int table_num
         , ObjectHanger& oh) = 0;
        
        virtual void to_string (StringWriter& sw) const = 0;
        
        int index_no() const
        { return index_no_; }

        int index_level() const
        { return index_level_; }

        int table_idx() const
        { return table_idx_; }

        bool is_maybe () const
        { return is_maybe_; }

        const void* p_table() const
        { return p_table_mirror_; }

    protected:
        // untyped version of p_table_ in TypedBasicSelector
        const void* p_table_mirror_; 
        int index_no_;
        int index_level_;
        int table_idx_;
        bool is_maybe_;
        std::vector<Predicate*> a_filter_;
        std::vector<Predicate*> a_assign_;
    };

    struct CompareBasicSelector {
        bool operator() (const BasicSelector* p_bs1
                         , const BasicSelector* p_bs2) const {
            return p_bs1->index_level() < p_bs2->index_level();
        }
    };

    template <typename _Table>
    struct TypedBasicSelector
        : public BasicSelector {
        explicit TypedBasicSelector(const _Table* p_table
                                    , const int table_idx
                                    , const bool is_maybe
                                    )
        {
            p_table_ = p_table;
            this->p_table_mirror_ = p_table;
            this->table_idx_ = table_idx;
            this->is_maybe_ = is_maybe;
            memset (&ref_tuple_, 0, sizeof(ref_tuple_));

            //  //FIXME
            // memset (&src_tuple_, table_idx, sizeof(src_tuple_));
        }

        void set_predicate (const std::vector<Predicate*>& a_assign_candi
                            , const std::vector<Predicate*>& a_filter_candi
                            , const int fixed_table_mask
                            )
        {
            std::cout << "a_assign_candi=" << show_container(a_assign_candi)
                      << ", a_filter_candi=" << show_container(a_filter_candi)
                      << ", fixed_table_mask=" << fixed_table_mask
                      << std::endl;
            
            std::vector<Predicate*> a_assign_tmp;
            this->a_assign_.clear();
            this->a_filter_.clear();

            // dispatch predicates
            for (size_t k=0; k<a_assign_candi.size(); ++k) {
                Predicate* p_pred = a_assign_candi[k];
                // check if all attributes are fixed
                if ((p_pred->table_mask() & fixed_table_mask)
                    == p_pred->table_mask()) {
                    a_assign_tmp.push_back(p_pred);
                }
            }
            
            for (size_t k=0; k<a_filter_candi.size(); ++k) {
                Predicate* p_pred = a_filter_candi[k];
                // check if all attributes are fixed
                if ((p_pred->table_mask() & fixed_table_mask)
                    == p_pred->table_mask()) {
                    this->a_filter_.push_back(p_pred);
                }
            }

            Header h;
            // compose header
            for (size_t i=0; i<a_assign_tmp.size(); ++i) {
                if (!h.insert (a_assign_tmp[i]->attr_id(p_table_))) {
                    this->a_filter_.push_back (a_assign_tmp[i]);
                    a_assign_tmp[i] = NULL;
                }
            }
            
            bool is_primary = false;
            index_no_ = p_table_->choose_index (h, &is_primary);
            // biggest goes most right
            index_level_ = (index_no_ < 0) ? 1 : (is_primary ? -1 : 0); 

            if (index_no_ < 0) {
                ST_WARN ("can't find a suitable index");
                for (size_t i=0; i<a_assign_tmp.size(); ++i) {
                    if (a_assign_tmp[i]) {
                        a_filter_.push_back (a_assign_tmp[i]);
                    }
                }
            } else {
                const Header& h = p_table_->index_header(index_no_);
                    
                for (size_t i=0; i<a_assign_tmp.size(); ++i) {
                    if (a_assign_tmp[i]) {
                        if (h.contains (a_assign_tmp[i]->attr_id(p_table_))) {
                            a_assign_.push_back (a_assign_tmp[i]);
                        } else {
                            a_filter_.push_back (a_assign_tmp[i]);
                        }
                    }
                }
            }
        }

        void set_ref_tuple(const void** pp_table
                           , const int table_num
                           , ObjectHanger& oh
                           )
        {
            // put references into ref_tuple
            for (size_t i=0; i<a_assign_.size(); ++i) {
                a_assign_[i]->set_ref_tuple
                    (p_table_, &ref_tuple_, pp_table, table_num, oh);
            }
        }
        
        void bg_seek (typename _Table::iterator* p_it
                      , const ObjectHanger& oh
                      )
        {
            typeof(a_filter_)* pa_filter = a_filter_.empty() ? NULL : & a_filter_;
            
            if (index_no_ < 0) {
                p_table_->im_.all(p_it, p_table_->bg_no(), 0, pa_filter);
            } else {
                p_table_->im_.seek_ref (p_it, p_table_->bg_no()
                                        , index_no_, oh, ref_tuple_, pa_filter);
            }
        }

        void to_string (StringWriter& sw) const
        {
            sw.append_format ("{table=%p", p_table_);
            sw << ",table_idx=" << table_idx_
               << ",is_maybe=" << is_maybe_
               << ",index_no=" << index_no_
               << ",a_assign=" << show_container(a_assign_)
               << ",a_filter=" << show_container(a_filter_)
               << ",ref_tuple=" << ref_tuple_
               << "}";            
        }
        
    private:
        const _Table* p_table_;
        typename _Table::Tuple::RefTuple ref_tuple_;
    };
    
    // select data out from multiple tables, focus is functionality
    // and expressivity
    template <typename _Table0  // first table should not be Maybe1
              , typename _MTable1
              , typename _MTable2
              >
    struct Selector<_Table0,_MTable1,_MTable2> {
        typedef Selector<_Table0,_MTable1,_MTable2> Self;
        static const int TABLE_NUM = 3;
        typedef _Table0 Table0;
        typedef typename IsMaybe1<_MTable1>::Table Table1;
        typedef typename IsMaybe1<_MTable2>::Table Table2;
        
        struct iterator {
            iterator ()
                : not_end_(false)
                , pp_bs_(NULL)
            {
            }
            
            void bs_seek (const int sel_idx)
            {
                const BasicSelector* p_bs = pp_bs_[sel_idx];
                switch (p_bs->table_idx()) {
                case 0:
                    ((TypedBasicSelector<Table0>*)p_bs)->bg_seek(&it0, oh_);
                    oh_.set_object (0, it0.operator->());
                    break;
                case 1:
                    ((TypedBasicSelector<Table1>*)p_bs)->bg_seek(&it1, oh_);
                    oh_.set_object (1, it1.operator->());
                    break;
                case 2:
                    ((TypedBasicSelector<Table2>*)p_bs)->bg_seek(&it2, oh_);
                    oh_.set_object (2, it2.operator->());
                    break;
                }
            }


            void bs_forward (const int sel_idx)
            {
                switch (pp_bs_[sel_idx]->table_idx()) {
                case 0:
                    ++ it0;
                    oh_.set_object (0, it0.operator->());
                    break;
                case 1:
                    ++ it1;
                    oh_.set_object (1, it1.operator->());
                    break;
                case 2:
                    ++ it2;
                    oh_.set_object (2, it2.operator->());
                    break;
                }
            }

            bool bs_not_end (const int sel_idx) const
            {
                return NULL != oh_.object (pp_bs_[sel_idx]->table_idx());
            }

            bool bs_end (const int sel_idx) const
            {
                return NULL == oh_.object (pp_bs_[sel_idx]->table_idx());
            }
            
            void setup (const Self* p_sel, const int start_table_idx)
            {
                pp_bs_ = p_sel->pp_selector(start_table_idx, 0);

                const ObjectHanger& cv = p_sel->a_oh_[start_table_idx];
                for (int i=0; i<TABLE_NUM; ++i) {
                    oh_.set_object(i, NULL);
                }
                for (size_t i=0; i<cv.size(); ++i) {
                    oh_.set_object (i+TABLE_NUM, cv.object(i));
                }

                std::cout << "idx0=" << pp_bs_[0]->table_idx()
                          << ", idx1=" << pp_bs_[1]->table_idx()
                          << ", idx2=" << pp_bs_[2]->table_idx()
                          << std::endl;

                std::cout << "oh_=" << show (oh_.begin(), oh_.end())
                          << std::endl;

                std::cout << "begin setup" << std::endl;
                bs_seek (0);
                std::cout << "oh0_=" << show (oh_.begin(), oh_.end())
                          << std::endl;
                while (bs_not_end(0)) {
                    bs_seek (1);
                    std::cout << "oh1_=" << show (oh_.begin(), oh_.end())
                              << std::endl;
                    while (bs_not_end(1)) {
                        bs_seek (2);
                        std::cout << "oh2_=" << show (oh_.begin(), oh_.end())
                                  << std::endl;
                        if (bs_not_end(2)) {
                            not_end_ = true;
                            std::cout << "end setup, has something" << std::endl;
                            return;
                        }
                        bs_forward (1);
                    } 
                    bs_forward (0);
                }
                not_end_ = false;
                std::cout << "end setup, nothing" << std::endl;
            }

            void operator++ ()
            {
                bs_forward (2);
                while (bs_end(2)) {
                    bs_forward (1);
                    while (bs_end(1)) {
                        bs_forward (0);
                        if (bs_end(0)) {
                            // it0 is end now, quit
                            not_end_ = false;
                            return;
                        }
                        bs_seek (1);
                    }
                    bs_seek (2);
                }
                not_end_ = true;
            }

            bool not_end() const
            {
                return not_end_;
            }

            void to_string (StringWriter& sw) const
            {
                sw << "(" << *it0 << "," << *it1 << "," << *it2 << ")";
            }

        public:
            // these fields are accessed by users as well
            typename Table0::iterator it0;
            typename Table1::iterator it1;
            typename Table2::iterator it2;
            //private:
            bool not_end_;
            const BasicSelector* const* pp_bs_;
            int buf_no_;
            ObjectHanger oh_;
        };

        void bg_select (iterator* __restrict p_it) const
        {
            p_it->setup (this, start_table_idx_);
        }
        
        explicit Selector ()
            : p_conj_(NULL)
        {
            for (int i=0; i<TABLE_NUM*TABLE_NUM; ++i) {
                ap_selector_[i] = NULL;
            }
        }

        ~Selector()
        {
            for (int i=0; i<TABLE_NUM*TABLE_NUM; ++i) {
                if (ap_selector_[i]) {
                    delete ap_selector_[i];
                    ap_selector_[i] = NULL;
                }
            }
            if (p_conj_) {
                delete p_conj_;
                p_conj_ = NULL;
            }
        }
        
        // for single predicate
        int set_predicate (const Table0& table0
                           , const Table1& table1
                           , const Table2& table2
                           , Predicate& pred)
        { return set_predicate
                (table0, table1, table2, Conjunction::create() && pred); }
        
        int set_predicate (const Table0& table0
                           , const Table1& table1
                           , const Table2& table2
                           , Conjunction& conj)
        {
            if (p_conj_) {
                delete p_conj_;
            }
            p_conj_ = &conj;
            
            // new BasicSelectors and put them in ap_selector_ in following
            // order represented by corresponding table_idx
            //    0 -> 1 -> 2
            // -> 1 -> 0 -> 0      
            // -> 2 -> 2 -> 1
            // selectors in each column except the first one will be
            // re-arranged by index_level which is got during choose_index
            selector(0,0) =
                ST_NEW (TypedBasicSelector<Table0>, &table0, 0, false);
            selector(1,1) =
                ST_NEW (TypedBasicSelector<Table0>, &table0, 0, false);
            selector(2,1) =
                ST_NEW (TypedBasicSelector<Table0>, &table0, 0, false);
            selector(0,1) =
                ST_NEW (TypedBasicSelector<Table1>, &table1, 1, IsMaybe1<_MTable1>::YES);
            selector(1,0) =
                ST_NEW (TypedBasicSelector<Table1>, &table1, 1, IsMaybe1<_MTable1>::YES);
            selector(2,2) =
                ST_NEW (TypedBasicSelector<Table1>, &table1, 1, IsMaybe1<_MTable1>::YES);
            selector(0,2) =
                ST_NEW (TypedBasicSelector<Table2>, &table2, 2, IsMaybe1<_MTable2>::YES);
            selector(1,2) =
                ST_NEW (TypedBasicSelector<Table2>, &table2, 2, IsMaybe1<_MTable2>::YES);
            selector(2,0) =
                ST_NEW (TypedBasicSelector<Table2>, &table2, 2, IsMaybe1<_MTable2>::YES);

            const void* pp_table[] = {&table0, &table1, &table2};
            std::vector<Predicate*> a_assign_candi[TABLE_NUM];
            std::vector<Predicate*> a_filter_candi[TABLE_NUM];
                
            std::vector<Predicate*>& a_bool = p_conj_->pred_list();
            for (size_t i=0; i<a_bool.size(); ++i) {
                if (NULL == a_bool[i]) {
                    ST_FATAL ("a_bool[%lu] is NULL", i);
                    bad_ = true;
                    return EIMPOSSIBLE;
                }
                switch (a_bool[i]->categorize(pp_table, TABLE_NUM)) {
                case PRED_ASSIGN:
                    for (int j=0; j<TABLE_NUM; ++j) {
                        if (a_bool[i]->table_mask() & PRED_TABLE(j)) {
                            a_assign_candi[j].push_back (a_bool[i]);
                        }
                    }
                    break;
                case PRED_FILTER:
                    for (int j=0; j<TABLE_NUM; ++j) {
                        if (a_bool[i]->table_mask() & PRED_TABLE(j)) {
                            a_filter_candi[j].push_back (a_bool[i]);
                        }
                    }
                    break;
                case PRED_INVALID:
                    ST_FATAL ("predicate=%s is invalid", show(a_bool[i]).c_str());
                    bad_ = true;
                    return EGENERAL;
                }
            }

            // horizontally
            for (int i=0; i<TABLE_NUM; ++i) {
                int idx0 = selector(i,0)->table_idx();
                int fixed_m = PRED_TABLE(idx0);
                selector(i,0)->set_predicate
                    (a_assign_candi[idx0], a_filter_candi[idx0], fixed_m);
                
                // vertically
                // notice "j<TABLE_NUM-1" because single last selector does
                // not need to be re-arranged
                for (int j=1; j<TABLE_NUM; ++j) {
                    // select index for selectors between [j,TABLE_NUM-1] with
                    // same other fixed tables, and swap the best with the one
                    // at position j
                    for (int k=j; k<TABLE_NUM; ++k) {
                        // i==j -> PRED_TABLE(i)
                        // i!=j -> PRED_TABLE(i) | PRED_TABLE()
                        int idxk = selector(i,k)->table_idx();
                        selector(i,k)->set_predicate
                            (a_assign_candi[idxk]
                             , a_filter_candi[idxk]
                             , fixed_m | PRED_TABLE(idxk));
                    }

                    // stably sort BasicSelectors(i, 1..(TABLE_NUM-1))by their
                    // index_level. primany indexes are left-most, non primary
                    // indexes are in middle those did not get indexes are
                    // right-most. this is a very simple approach comparing to
                    // original design, but I think it's effective in our real
                    // application, because: 1. changed sequences are guaranteed
                    // to be better; 2. for unsure conditions, respect user's
                    // input.
                    if (j != (TABLE_NUM-1)) {
                        std::stable_sort (&selector(i,j)
                                          , &selector(i,TABLE_NUM-1)
                                          , CompareBasicSelector());
                        fixed_m |= PRED_TABLE(selector(i,j)->table_idx());
                    }
                }

                const void* pp_table2[] = { &table0, &table1, &table2 };
                a_oh_[i].clear();
                for (int j=0; j<TABLE_NUM; ++j) {
                    selector(i,j)->set_ref_tuple(pp_table2, TABLE_NUM, a_oh_[i]);
                }
            }

            // choose best sequence for starting a join, non-maybe table is
            // better and lower index_level is better
            int least_value = 10000;  // this is big enough
            for (int i=0; i<TABLE_NUM; ++i) {
                int v = selector(i,0)->index_level()
                    + (selector(i,0)->is_maybe() ? 20 : 0);
                if (v < least_value) {
                    least_value = v;
                    start_table_idx_ = i;
                }
            }
            
            return 0;
        }

        void to_string (StringWriter& sw) const
        {
            sw << "{start_idx=" << start_table_idx_ << "\n";
            for (int i=0; i<TABLE_NUM * TABLE_NUM; ++i) {
                sw << "selector[" << i << "]=" << ap_selector_[i] << "\n";
            }
            for (int i=0; i<TABLE_NUM; ++i) {
                sw << ", oh[" << i << "]=";
                shows_range (sw, a_oh_[i].begin(), a_oh_[i].end());
            }
            sw << "\n}";
        }
            
    private:
        BasicSelector*& selector(const int table_start, const int table_seq)
        { return ap_selector_[table_start*TABLE_NUM + table_seq]; }

        const BasicSelector* const* pp_selector
        (const int table_start, const int table_seq) const
        { return ap_selector_ + table_start*TABLE_NUM + table_seq; }
        
        ObjectHanger a_oh_[TABLE_NUM];
        bool bad_;
        BasicSelector* ap_selector_[TABLE_NUM * TABLE_NUM];
        int start_table_idx_;
        Conjunction* p_conj_;
    };
}

#endif  // _SELECTOR_HPP_
