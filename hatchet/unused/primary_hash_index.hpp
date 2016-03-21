// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// (smalltable1) Tuples in PrimaryHashIndex do not have same keys, if
// that happens later tuple overwrites previous tuple.
// Author: gejun@baidu.com
// Date: Wed Aug 11 14:57:47 2010
#pragma once
#ifndef _PRIMARY_HASH_INDEX_HPP_
#define _PRIMARY_HASH_INDEX_HPP_

#include "base_hash_index.hpp"
#include <vector>
#include "functional.hpp"
#include "linear_hash_map.hpp"
#include "ext_hash_set.hpp"


namespace st {

template <typename _SubF>
class PrimaryHashIndex : public BaseHashIndex<typename _SubF::FullTuple> {
public:
    enum { PRIMARY = true };
    
    typedef typename _SubF::FullTuple Tup;

    typedef typename Tup::RefTuple RefTup;

    typedef typename _SubF::Tuple Key;

    typedef FunCon<Hash<Key>, _SubF> Hash2;

    struct Equal {
        _SubF sub_conv_;
        bool operator() (const Tup& t1, const Tup& t2) const
        { return sub_conv_(t1) == sub_conv_(t2); }
    };

    struct RefEqual {
        _SubF sub_conv_;
        bool operator() (const Tup& t1, const ObjectHanger& oh,
                         const RefTup& t2) const
        { return sub_conv_(t1) == sub_conv_(oh, t2); }
    };

    typedef ExtHashSet< Tup, Hash2, Equal > Container;
    
    typedef LinearHashMap<const Tup*, ModType> ModSet;
        
    struct seek_iterator {
        //enum { TYPE = HI_SEEK };
            
        void setup (const Tup* p_cur,
                    const std::vector<Predicate*>* pa_filter)
        {
            if (NULL != pa_filter) {
                for (size_t i=0; i<pa_filter->size(); ++i) {
                    if (! (*pa_filter)[i]->passed ((const char*)p_cur_)) {
                        p_cur_ = NULL;
                        return;
                    }
                }
            }
            p_cur_ = p_cur;
        }

        void operator++ () { p_cur_ = NULL; }

        operator bool () const { return p_cur_ != NULL; }

        const Tup& operator* () const { return *p_cur_; }

        const Tup* operator-> () const { return p_cur_; }
            
        const Tup* p_cur_;  //MUST BE FIRST
    };


    struct iterator {
        void operator++ ()
        {
            if (likely (it_ != ite_)) {
                p_cur_ = &(*it_);
                if (NULL != pa_filter_) {
                    for (size_t i=0; i<pa_filter_->size(); ) {
                        if (!(*pa_filter_)[i]->passed((const char*)p_cur_)) {
                            ++ it_;
                            if (it_ != ite_) {
                                p_cur_ = &(*it_);
                                i = 0;  // go back to first
                            } else {
                                p_cur_ = NULL;
                                return;
                            }
                        } else {
                            ++i;
                        }
                    }
                }
                ++ it_;
            } else {
                p_cur_ = NULL;
            }
        }

        void setup (const typename Container::iterator& it
                    , const typename Container::iterator& ite
                    , const std::vector<Predicate*>* pa_filter)
        {
            it_ = it;
            ite_ = ite;
            pa_filter_ = pa_filter;
            operator++();
        }

        operator bool () const { return p_cur_ != NULL; }

        const Tup& operator* () const { return *p_cur_; }

        const Tup* operator-> () const { return p_cur_; }
            
        const Tup* p_cur_;  //MUST BE FIRST
        const std::vector<Predicate*>* pa_filter_;
        typename Container::iterator it_;
        typename Container::iterator ite_; 
    };

    union dummy_ {
        char d0_[sizeof(iterator)];
        char d1_[sizeof(seek_iterator)];
    };
    enum { ITERATOR_SIZE = sizeof(dummy_) };


    struct mod_iterator {
        void operator++ ()
        {
            if (likely(it_ != ite_)) {
                p_cur_ = it_->key;
                type_ = it_->value;
                ++ it_;
            } else {
                p_cur_ = NULL;
            }
        }

        void setup (const typename ModSet::iterator& it
                    , const typename ModSet::iterator& ite)
        {
            it_ = it;
            ite_ = ite;
            operator++();
        }

        operator bool () const { return p_cur_ != NULL; }

        ModType type() const { return type_; }

        const Tup& operator* () const { return *p_cur_; }

        const Tup* operator-> () const { return p_cur_; }
            
        const Tup* p_cur_;  //MUST BE FIRST
        ModType type_;
        typename ModSet::iterator it_;
        typename ModSet::iterator ite_;
    };

    void setup_mod_iterator(void* p_it) const
    { ((mod_iterator*)p_it)->setup (mod_tup_.begin(), mod_tup_.end()); }
        
    
    PrimaryHashIndex (unsigned char load_factor=80)
        : immutable_(true)
    {
        if (load_factor > 100) {
            ST_FATAL ("load_factor=%d is too big", (int)load_factor);
        }
            
        this->primary_ = true;
        this->header_.template record<Key>();
        this->table_header_.template record<Tup>();

        for (int i=0; i<2; ++i) {
            ap_cont_[i] = ST_NEW (Container, DEFAULT_HASH_SIZE, 64, load_factor);
            if (NULL == ap_cont_[i]) {
                ST_FATAL ("Fail to new ap_cont_[%d]", i);
            }
        }
        for (int i=0; i<2; ++i) {
            a_ver_[i] = 0;
        }
        mod_tup_.create (DEFAULT_HASH_SIZE, 80, true);
    }

    ~PrimaryHashIndex ()
    {
        for (int i=0; i<2; ++i) {
            if (ap_cont_[i]) {
                delete ap_cont_[i];
                ap_cont_[i] = NULL;
            }
        }
    }

    void forward_slower_iterator (void* p_it, int /*slower_type*/) const
    { ++ *(iterator*)p_it; }
        
    int all (void* p_it, int buf_no,
             const std::vector<Predicate*>* pa_filter) const
    {
        Container& cont = *ap_cont_[buf_no];
        ((iterator*)p_it)->setup (cont.begin(), cont.end(), pa_filter);
        return HI_MARK_SLOWER(HI_TRAVERSE);
    }
                
    void seek (seek_iterator* p_it, int buf_no, const Tup& tuple,
               const std::vector<Predicate*>* pa_filter) const
    { p_it->setup (ap_cont_[buf_no]->template seek(tuple), pa_filter); }

    inline int seek_ref (void* p_it, int buf_no, const ObjectHanger& oh,
                         const RefTup& tuple,
                         const std::vector<Predicate*>* pa_filter) const
    {
        ((seek_iterator*)p_it)->setup
            (ap_cont_[buf_no]->template seek_ref<RefTup,Hash2,RefEqual>(oh,
                                                                        tuple)
             , pa_filter);
        return HI_SEEK; 
    }

    // Insert an element
    // Returns: pointer to inserted value
    std::pair<const Tup*, const Tup*> insert (const Tup& tuple)
    {
        if (unlikely (immutable_)) {
            ST_FATAL ("Fail to modify immutable index%s of table%s"
                      , show(this->header_).c_str()
                      , show(this->table_header_).c_str());
            return std::pair<const Tup*, const Tup*>(NULL,NULL);
        }
        std::pair<const Tup*, const Tup*> chg =
            ap_cont_[this->bg_no()]->insert2 (tuple);

        if (chg.first)
        { mod_tup_.insert (chg.first, MOD_ERASE); }

        if (chg.second)
        { mod_tup_.insert (chg.second, MOD_INSERT); }

        return chg;
    }

    // Erase an element,
    // Returns: pointer to erased value, the pointer is invalid
    //          after recycle_delayed_memory is called
    const Tup* erase (const Tup& tuple)
    {
        if (unlikely (immutable_)) {
            ST_FATAL ("Fail to modify immutable index%s of table%s"
                      , show(this->header_).c_str()
                      , show(this->table_header_).c_str());
            return NULL;
        }
        Tup* p_del_tup = ap_cont_[this->bg_no()]->delayed_erase(tuple);
        if (p_del_tup) {
            mod_tup_.insert (p_del_tup, MOD_ERASE);
        }
        return p_del_tup;
    }

    const std::vector<const Tup*>*
    erase_by (const Tup& tuple, const std::vector<Predicate*>* pa_filter)
    {
        a_tmp_erased_.clear();
        if (unlikely (immutable_)) {
            ST_FATAL ("Fail to modify immutable index%s of table%s"
                      , show(this->header_).c_str()
                      , show(this->table_header_).c_str());
            return &a_tmp_erased_;
        }
        seek_iterator sit;
        seek (&sit, this->bg_no(), tuple, pa_filter);
        if (sit) {
            const Tup* p_tuple = erase (*sit);
            if (NULL == p_tuple) {
                ST_FATAL ("p_tuple is NULL");
            } else {
                a_tmp_erased_.push_back (p_tuple);
            }
        }
        return &a_tmp_erased_;
    }

    const std::vector<const Tup*>*
    erase_ref_by (const ObjectHanger& oh, const RefTup& tuple,
                  const std::vector<Predicate*>* pa_filter)
    {
        a_tmp_erased_.clear();

        if (unlikely (immutable_)) {
            ST_FATAL ("Fail to modify immutable index%s of table%s"
                      , show(this->header_).c_str()
                      , show(this->table_header_).c_str());
            return &a_tmp_erased_;
        }
            
        seek_iterator sit;
        seek_ref (&sit, this->bg_no(), oh, tuple, pa_filter);
        if (sit) {
            //cout << "erasing " << show(*sit) << endl;
            const Tup* p_tuple = erase (*sit);
            if (p_tuple != &(*sit)) {
                ST_FATAL ("p_tuple is invalid");
            } else {
                a_tmp_erased_.push_back (p_tuple);
            }
        }
        return &a_tmp_erased_;
    }

    const std::vector<const Tup*>*
    erase_all_by (const std::vector<Predicate*>* pa_filter)
    {
        a_tmp_erased_.clear();

        if (unlikely (immutable_)) {
            ST_FATAL ("Fail to modify immutable index%s of table%s"
                      , show(this->header_).c_str()
                      , show(this->table_header_).c_str());
            return &a_tmp_erased_;
        }

        // collect to-be-erased tuples
        Container& bg_cont = *ap_cont_[this->bg_no()];
        for (typename Container::iterator it=bg_cont.begin()
                 ; it != bg_cont.end()
                 ; ++it) {
            bool passed = true;
            if (pa_filter) {
                for (size_t i=0; i<pa_filter->size(); ++i) {
                    if (!(*pa_filter)[i]->passed((const char*)&(*it))) {
                        passed = false;
                        break;
                    }
                }
            }
            if (passed) {
                a_tmp_erased_.push_back (&(*it));
            }
        }
        // delete them
        for (size_t i=0; i<a_tmp_erased_.size(); ++i) {
            erase (*a_tmp_erased_[i]);
        }
        return &a_tmp_erased_;
    }

    void clear ()
    { erase_all_by(NULL); }
        
    int begin_update ()
    {
        if (!immutable_) {
            ST_FATAL ("miss end_update in index%s of table%s"
                      , show(this->header_).c_str()
                      , show(this->table_header_).c_str());
            return EGENERAL;
        }
        int ret = 0;
        // if there's no modifications, sync does nothing
        if (a_ver_[this->fg_no()] > a_ver_[this->bg_no()]) {
            ret = sync ();
            if (0 != ret) {
                ST_FATAL ("Fail to sync");
                return ret;
            }
            recycle_delayed_memory ();
            clear_mods ();
        }
            
        immutable_ = false;
        return ret;
    }

    int end_update ()
    {
        if (immutable_) {
            ST_FATAL ("miss begin_update in index%s of table%s"
                      , show(this->header_).c_str()
                      , show(this->table_header_).c_str());
            return EGENERAL;
        }

        a_ver_[this->bg_no()] = a_ver_[this->fg_no()] + 1;
        // primary indexes are already modified in-place,
        // simply swap foreground and background
        //this->swap ();

        immutable_ = true;
        return 0;
    }
        
private:
    void clear_mods ()
    { mod_tup_.clear(); }
        
    int sync ()
    {
        Container& fg_cont = *ap_cont_[this->fg_no()];
        Container& bg_cont = *ap_cont_[this->bg_no()];
            
        for (typename ModSet::iterator it=mod_tup_.begin(), it_e=mod_tup_.end();
             it!=it_e; ++it) {
            // Tup* p_tup2 = *it;
            // if (NULL == p_tup2) {
            //     ST_FATAL ("p_tup2 is NULL");
            // }
            Tup* p_tup = fg_cont.seek(*(it->key));
            if (NULL == p_tup) {
                bg_cont.delayed_erase (*(it->key));
            } else {
                bg_cont.insert (*p_tup);
            }
        }
        return 0;
    }

    void recycle_delayed_memory ()
    {
        ap_cont_[0]->recycle_delayed();
        ap_cont_[1]->recycle_delayed();
    }

public:
    size_t size (int buf_no) const
    { return ap_cont_[buf_no]->size(); }

    bool empty (int buf_no) const
    { return ap_cont_[buf_no]->empty(); }

    size_t mem () const
    {
        return sizeof(*this)
            + ap_cont_[0]->mem()
            + ap_cont_[1]->mem()
            + mod_tup_.mem()
            + a_tmp_erased_.capacity() * sizeof(const Tup*)
            ;
    }
        
    void to_string (StringWriter& sb) const
    {
        sb << "PrimaryHashIndex" << this->header() << "==>";
        sb.append_format ("{address=%p", this);
        sb << " mem=" << mem()
           << " cont0_sz=" << ap_cont_[0]->mem()
           << " cont1_sz=" << ap_cont_[1]->mem()
           << " mod_tup_sz=" << mod_tup_.mem()
           << " tmp_erased_sz="
           << a_tmp_erased_.capacity() * sizeof(const Tup*)
            ;

        for (int i=0; i<2; ++i) {
            sb << " cont[" << i << "]=>"
               << "{version=" << a_ver_[i]
               << " changed_num=" << mod_tup_.size()
               << " " << ap_cont_[i]
               << "}"
                ;
        }
        sb << "}";
    }

private:
    ModSet mod_tup_;
    Container* ap_cont_[2];
    std::vector<const Tup*> a_tmp_erased_;
    int a_ver_[2];
    bool immutable_;
    _SubF sub_conv_;
};

}

#endif  //_PRIMARY_HASH_INDEX_HPP_
