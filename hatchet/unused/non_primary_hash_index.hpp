// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// (smalltable1) Tuples in NonPrimaryHashIndex may have same keys
// and clustered together by their keys. Iterating same-key tuples
// is as fast as vector
// Author: gejun@baidu.com
// Date: Wed Aug 11 14:57:47 2010
#pragma once
#ifndef _NON_PRIMARY_HASH_INDEX_HPP_
#define _NON_PRIMARY_HASH_INDEX_HPP_

#include "base_hash_index.hpp"
#include <vector>
#include "ext_hash_map.hpp"

#include "functional.hpp"
#include "cowbass3.hpp"
#include "linear_hash_map.hpp"

namespace st {    

template <typename V> struct PtrUnifier {
    const V* to_ptr (const V& t) const { return &t; }
    const V& to_ref (const V& t) const { return t; }
    const V& from_ptr (const V* t) const { return *t; }
    const V& from_ref (const V& t) const { return t; }
};

template <typename V> struct PtrUnifier<const V*> {
    const V* to_ptr (const V* t) const { return t; }
    const V& to_ref (const V* t) const { return *t; }
    const V* from_ptr (const V* t) const { return t; }
    const V* from_ref (const V& t) const { return &t; }
};

// ----------------

template <typename _Compare> struct ComparePtrF {
    int operator() (const typename _Compare::Arg1* p1,
                    const typename _Compare::Arg2* p2) const
    { return _Compare()(*p1, *p2); }
};

// --------------------
template <typename _Tup, bool _StorePtr> struct CowbassIterator {
    enum { TYPE = (_StorePtr ? HI_COWBASS_PTR : HI_COWBASS) };

    typedef TCAP(if_, _StorePtr, const _Tup*, _Tup) StoredTup;

    typedef Compare<_Tup> _Compare;

    typedef Cowbass3<StoredTup, TCAP(if_, _StorePtr,
                                     ComparePtrF<_Compare>,
                                     _Compare)> Cowbass;

    void operator++ ()
    {
        PtrUnifier<StoredTup> uni_;
        if (likely (it_ != ite_)) {
            p_cur_ = uni_.to_ptr(*it_);
            if (NULL != pa_filter_) {
                for (size_t i=0; i<pa_filter_->size(); ) {
                    if (! (*pa_filter_)[i]->passed ((const char*)p_cur_)) {
                        ++ it_;
                        if (likely(it_!= ite_)) {
                            p_cur_ = uni_.to_ptr(*it_);
                            i = 0;  // go back to first
                        } else {
                            p_cur_ = NULL;
                            return;
                        }
                    } else {
                        ++ i;
                    }
                }
            }
            ++ it_;
        } else {
            p_cur_ = NULL;
        }
    }

    void setup (const typename Cowbass::iterator& it,
                const typename Cowbass::iterator& ite,
                const std::vector<Predicate*>* pa_filter)
    {
        it_ = it;
        ite_ = ite;
        pa_filter_ = pa_filter;
        operator++ ();
    }

    void set_end () { p_cur_ = NULL; }
        
    operator bool () const { return p_cur_ != NULL; }

    const _Tup& operator* () const { return *p_cur_; }

    const _Tup* operator-> () const { return p_cur_; }
        
    const _Tup* p_cur_;  //MUST BE FIRST
    const std::vector<Predicate*>* pa_filter_;
    typename Cowbass::iterator it_;
    typename Cowbass::iterator ite_;
};


template <typename _Tup, bool _StorePtr> struct CowbassBgIterator {
    enum { TYPE = (_StorePtr ? HI_BG_COWBASS_PTR : HI_BG_COWBASS) };

    typedef TCAP(if_, _StorePtr, const _Tup*, _Tup) StoredTup;

    typedef Compare<_Tup> _Compare;

    typedef Cowbass3<StoredTup, TCAP(if_, _StorePtr,
                                     ComparePtrF<_Compare>,
                                     _Compare)> Cowbass;

    void operator++ ()
    {
        PtrUnifier<StoredTup> uni_;
        if (likely(it_)) {
            p_cur_ = uni_.to_ptr(*it_);
            if (NULL != pa_filter_) {
                for (size_t i=0; i<pa_filter_->size(); ) {
                    if (! (*pa_filter_)[i]->passed ((const char*)p_cur_)) {
                        ++ it_;
                        if (likely(it_)) {
                            p_cur_ = uni_.to_ptr(*it_);
                            i = 0;  // go back to first
                        } else {
                            p_cur_ = NULL;
                            return;
                        }
                    } else {
                        ++ i;
                    }
                }
            }
            ++ it_;
        } else {
            p_cur_ = NULL;
        }
    }

    void setup (const typename Cowbass::bg_iterator& it,
                       const std::vector<Predicate*>* pa_filter)
    {
        it_ = it;
        pa_filter_ = pa_filter;
        operator++ ();
    }

    void set_end () { p_cur_ = NULL; }
        
    operator bool () const { return p_cur_ != NULL; }

    const _Tup& operator* () const { return *p_cur_; }

    const _Tup* operator-> () const { return p_cur_; }
        
    const _Tup* p_cur_;  //MUST BE FIRST
    const std::vector<Predicate*>* pa_filter_;
    typename Cowbass::bg_iterator it_;
};

    
// ----------------------------
enum { STORE_POINTER_OF_TUPLE = true, STORE_VALUE_OF_TUPLE = false };
    
template <typename _SubF, bool _StorePtr> class NonPrimaryHashIndex
    : public BaseHashIndex<typename _SubF::FullTuple> {
public:
    enum { PRIMARY = false };

    typedef typename _SubF::FullTuple Tup;

    typedef typename Tup::RefTuple RefTup;

    typedef typename _SubF::Tuple Key;

    typedef TCAP(if_, _StorePtr, const Tup*, Tup) StoredTup;

    typedef PtrUnifier<StoredTup> Uni;

    typedef Compare<typename _SubF::FullTuple> _Compare;
    
    typedef Cowbass3<StoredTup, TCAP(if_, _StorePtr,
                                     ComparePtrF<_Compare>,
                                     _Compare)> Cowbass;

    typedef ExtHashMap< Key, Cowbass*, Hash<typename _SubF::Tuple> > Map;

    typedef LinearHashMap<Key,u_int> ModSet;

    typedef CowbassIterator<Tup,_StorePtr> seek_iterator;
    
    typedef CowbassBgIterator<Tup,_StorePtr> bg_seek_iterator;
        
    struct iterator {
        enum { TYPE = HI_TRAVERSE };
            
        void operator++ ()
        {
            static PtrUnifier<StoredTup> uni_;
        REFETCH:
            if (it_ != ite_) {
                p_cur_ = uni_.to_ptr(*it_);
                if (NULL != pa_filter_) {
                    for (size_t i=0; i<pa_filter_->size(); ++i) {
                        if (! (*pa_filter_)[i]->passed ((const char*)p_cur_)) {
                            ++ it_;
                            goto REFETCH;
                        }
                    }
                }
                ++ it_;
            } else {  // it_ == ite_
                ++ hit_;
                if (next_pair_of_iterators()) {
                    goto REFETCH;
                }
            }
        }

        bool next_pair_of_iterators ()
        {
            while (1) {
                if (hit_ != hite_) {
                    Cowbass* p_cb = hit_->second;
                    if (NULL == p_cb) {
                        ST_FATAL ("p_cb is NULL");
                        ++ hit_;
                    } else {
                        it_ = p_cb->fg_begin();
                        ite_ = p_cb->fg_end();
                        return true;
                    }
                } else {
                    p_cur_ = NULL;
                    return false;
                }
            }
        }

        void setup (const typename Map::iterator& hit,
                           const typename Map::iterator& hite,
                           const std::vector<Predicate*>* pa_filter)
        {
            hit_ = hit;
            hite_ = hite;
            pa_filter_ = pa_filter;
            if (next_pair_of_iterators()) {
                operator++ ();
            }
        }
        
        operator bool () const { return p_cur_ != NULL; }

        void set_end () { p_cur_ = NULL; }

        const Tup& operator* () const { return *p_cur_; }

        const Tup* operator-> () const { return p_cur_; }
            
        const Tup* p_cur_;  //MUST BE FIRST
        const std::vector<Predicate*>* pa_filter_;
        typename Cowbass::iterator it_;
        typename Cowbass::iterator ite_;
        typename Map::iterator hit_;
        typename Map::iterator hite_;
    };


    struct bg_iterator {
        enum { TYPE = HI_BG_TRAVERSE };
            
        void operator++ ()
        {
            static PtrUnifier<StoredTup> uni_;
        REFETCH:
            if (it_) {
                p_cur_ = uni_.to_ptr(*it_);
                if (pa_filter_) {
                    for (size_t i=0; i<pa_filter_->size(); ++i) {
                        if (! (*pa_filter_)[i]->passed ((const char*)p_cur_)) {
                            ++ it_;
                            goto REFETCH;
                        }
                    }
                }
                ++ it_;
            } else {  // it_ ends
                ++ hit_;
                if (next_pair_of_iterators()) {
                    goto REFETCH;
                }
            }
        }

        bool next_pair_of_iterators ()
        {
            while (1) {
                if (hit_ != hite_) {
                    Cowbass* p_cb = hit_->second;
                    if (NULL == p_cb) {
                        ST_FATAL ("p_cb is NULL");
                        ++ hit_;
                    } else {
                        it_ = p_cb->bg_begin();
                        return true;
                    }
                } else {
                    p_cur_ = NULL;
                    return false;
                }
            }
        }
        
        void setup (const typename Map::iterator& hit,
                           const typename Map::iterator& hite,
                           const std::vector<Predicate*>* pa_filter
            )
        {
            hit_ = hit;
            hite_ = hite;
            pa_filter_ = pa_filter;

            if (next_pair_of_iterators()) {
                operator++ ();
            }
        }
        
        operator bool () const { return p_cur_ != NULL; }

        void set_end () { p_cur_ = NULL; }

        const Tup& operator* () const { return *p_cur_; }

        const Tup* operator-> () const { return p_cur_; }
            
        const Tup* p_cur_;  //MUST BE FIRST
        const std::vector<Predicate*>* pa_filter_;
        typename Cowbass::bg_iterator it_;
        typename Map::iterator hit_;
        typename Map::iterator hite_;
    };

    union dummy_ {
        char _d0[sizeof(seek_iterator)];
        char _d1[sizeof(bg_seek_iterator)];
        char _d2[sizeof(iterator)];
        char _d3[sizeof(bg_iterator)];
    };
    enum { ITERATOR_SIZE = sizeof(dummy_) };

    struct mod_iterator {
        void setup(const typename ModSet::iterator& it,
                          const typename ModSet::iterator& it_e)
        {
            it_ = it;
            it_e_ = it_e;
        }

        void operator++ () { ++ it_; }
        
        operator bool () const { return it_ != it_e_; }

        const Key& operator* () const { return it_->key; }

        const Key* operator-> () const { return &(it_->key); }
            
        typename ModSet::iterator it_;
        typename ModSet::iterator it_e_;
    };

    void setup_mod_iterator(void* p_it) const
    { ((mod_iterator*)p_it)->setup (s_mod_key_.begin(), s_mod_key_.end()); }

    struct Info {
        size_t cb_new_cnt;
        size_t cb_del_cnt;
            
        Info() : cb_new_cnt(0), cb_del_cnt(0)
        {}
    };
    
    NonPrimaryHashIndex (unsigned char load_factor=80)
        : mp_(sizeof(typename Cowbass::Node))
#ifdef USE_NAIVE_SET
        , mmp_(sizeof(typename Cowbass::ModNode))
#endif
        , immutable_(true)
    {
        if (load_factor > 100) {
            ST_FATAL ("load_factor=%d is too big", (int)load_factor);
        }
 
        this->primary_ = false;
        this->header_.template record<Key>();
        this->table_header_.template record<Tup>();

        for (int i=0; i<2; ++i) {
            ap_cont_[i] = ST_NEW (Map, DEFAULT_HASH_SIZE, 32, load_factor);
            if (NULL == ap_cont_[i]) {
                ST_FATAL ("Fail to new ap_cont_[%d]", i);
            }
        }
        for (int i=0; i<2; ++i) {
            a_ver_[i] = 0;
        }

        s_mod_key_.create (512, 50, true);
    }
    
    ~NonPrimaryHashIndex ()
    {
        destroy_shared ();
            
        for (int i=0; i<2; ++i) {
            if (ap_cont_[i]) {
                delete ap_cont_[i];
                ap_cont_[i] = NULL;
            }
        }
    }

public:
    void forward_slower_iterator (void* p_it, int slower_type) const
    {
        switch (slower_type) {
        case HI_BG_COWBASS:
            ++ *(CowbassBgIterator<Tup,false>*)p_it;
            return;
        case HI_BG_COWBASS_PTR:
            ++ *(CowbassBgIterator<Tup,true>*)p_it;
            return;
        case HI_TRAVERSE:
            ++ *(iterator*)p_it;
            return;
        case HI_BG_TRAVERSE:
            ++ *(bg_iterator*)p_it;
            return;
        }
    }

    int all (void* p_it, int buf_no,
             const std::vector<Predicate*>* pa_filter) const
    {
        Map& cont = *ap_cont_[buf_no];
        if (buf_no == this->fg_no()) {
            ((iterator*)p_it)->setup (cont.begin(), cont.end(), pa_filter);
            return HI_MARK_SLOWER (HI_TRAVERSE);
        } else {
            ((bg_iterator*)p_it)->setup (cont.begin(), cont.end(), pa_filter);
            return HI_MARK_SLOWER (HI_BG_TRAVERSE);
        }
    }
    
    // @retval 0: not found
    // @retval 1->+inf: storing position
    int seek_sub (void* p_it, int buf_no, const Key& sub_tuple,
                  const std::vector<Predicate*>* pa_filter) const
    {
        Map& cont = *ap_cont_[buf_no];
        Cowbass** pp_cb = cont.seek (sub_tuple);
        if (NULL == pp_cb || NULL == *pp_cb) {
            // no cowbass to the key, just end the iterator
            ((const Tup**)p_it)[0] = NULL;
            return HI_NONE;
        } else {
            Cowbass* p_cb = *pp_cb;
            // forground buffer -> want foreground info
            // the cowbass wasn't changed -> fg is same with bg
            if (buf_no == this->fg_no() || 0 == p_cb->mod_size()) {
                ((seek_iterator*)p_it)->setup
                    (p_cb->fg_begin(), p_cb->fg_end(), pa_filter);
                return _StorePtr ? HI_COWBASS_PTR : HI_COWBASS;
            } else {
                // want background info and background is different from foreground
                ((bg_seek_iterator*)p_it)->setup (p_cb->bg_begin(), pa_filter);
                return HI_MARK_SLOWER
                    (_StorePtr ? HI_BG_COWBASS_PTR : HI_BG_COWBASS);
            }
        }
    }

    int seek (void* p_it, int buf_no, const Tup& tuple,
              const std::vector<Predicate*>* pa_filter) const
    { return seek_sub (p_it, buf_no, sub_conv_(tuple), pa_filter); }

    int seek_ref (void* p_it, int buf_no, const ObjectHanger& oh,
                  const RefTup& tuple,
                  const std::vector<Predicate*>* pa_filter) const
    { return seek_sub (p_it, buf_no, sub_conv_(oh,tuple), pa_filter); }

    std::pair<const Tup*, const Tup*> insert (const Tup& tuple)
    {
        if (unlikely (immutable_)) {
            ST_FATAL ("Fail to modify immutable index%s of table%s"
                      , show(this->header_).c_str()
                      , show(this->table_header_).c_str());
            return std::pair<const Tup*, const Tup*>(NULL,NULL);
        }

        Map& bg_cont = *ap_cont_[this->bg_no()];
        Key key = sub_conv_(tuple);
        Cowbass** pp_cb = bg_cont.seek (key);
        Cowbass* p_cb = NULL;

        if (NULL == pp_cb || NULL == *pp_cb) {
            p_cb = borrow_cowbass();
            if (NULL == p_cb) {
                goto END;
            }
            bg_cont.insert (key, p_cb);
        } else {
            p_cb = *pp_cb;
        }

        p_cb->insert (uni_.from_ref(tuple));
        s_mod_key_.insert (key, 1);

    END:
        // don't know the exact storage position now
        return std::pair<const Tup*, const Tup*>(NULL,NULL); 
    }

    const Tup* erase (const Tup& tuple)
    {
        if (unlikely (immutable_)) {
            ST_FATAL ("Fail to modify immutable index%s of table%s"
                      , show(this->header_).c_str()
                      , show(this->table_header_).c_str());
            return NULL;
        }

        Map& bg_cont = *ap_cont_[this->bg_no()];
        Key key = sub_conv_(tuple);
        Cowbass** pp_cb = bg_cont.seek (key);

        if (pp_cb && *pp_cb) {
            Cowbass* p_cb = *pp_cb;
            p_cb->erase (uni_.from_ref(tuple));
            s_mod_key_.insert (key, 1);
        }  // else if there's no cowbass, ignore the erasure

        return NULL;  // don't know the exact storage position now
    }

    // seek_sub (p_it, sub_conv_(oh,tuple), pa_filter);

    struct TrueToErase {
        TrueToErase (const std::vector<Predicate*>* pa_filter)
            : pa_filter_(pa_filter)
        {}
            
        bool operator() (const Tup& tuple) const
        {
            if (NULL != pa_filter_) {
                for (size_t i=0; i<pa_filter_->size(); ++i) {
                    if (! (*pa_filter_)[i]->passed ((const char*)&tuple)) {
                        return false;
                    }
                }
            }
            return true;
        }

        bool operator() (const Tup* p_tuple) const
        {
            if (NULL != pa_filter_) {
                for (size_t i=0; i<pa_filter_->size(); ++i) {
                    if (! (*pa_filter_)[i]->passed ((const char*)p_tuple)) {
                        return false;
                    }
                }
            }
            return true;
        }
        
        const std::vector<Predicate*>* pa_filter_;
    };

    // template <typename _Mapping>
    // struct MappingWrapper {
    //     static Tup dummy_;

    //     MappingWrapper(const _Mapping& f)
    //         : f_(f)
    //     {}
        
    //     typeof(f_(dummy_)) operator() (const Tup& tuple) const
    //     { return f(tuple); }

    //     typeof(f_(dummy_)) operator() (const Tup* p_tuple) const
    //     { return f(*p_tuple); }

    // private:
    //     const _Mapping& f_;
    // };

private:
    typedef TCAP (if_, _StorePtr
                  , std::vector<const Tup*>
                  , std::vector<Tup>) ErasedList;

    // Removes elements with the key and passing all [pa_filter]
    // To be wrapped by erase_by and erase_ref_by
    // Returns: a list of erased values
    const std::vector<const Tup*>*
    erase_sub_by (const Key& key, const std::vector<Predicate*>* pa_filter)
    {
        a_tmp_erased_.clear();

        if (unlikely (immutable_)) {
            ST_FATAL ("Fail to modify immutable index%s of table%s"
                      , show(this->header_).c_str()
                      , show(this->table_header_).c_str());
            return &a_tmp_erased_;
        }

        Map& bg_cont = *ap_cont_[this->bg_no()];
        Cowbass** pp_cb = bg_cont.seek (key);
        const ErasedList* p_erased_list = NULL;
        if (pp_cb && *pp_cb) {
            Cowbass* p_cb = *pp_cb;
            TrueToErase tte(pa_filter);
            p_erased_list = p_cb->erase_by(tte);
        }  // else if there's no cowbass, ignore the erasure

        if (p_erased_list && !p_erased_list->empty()) {
            s_mod_key_.insert (key, 1);
            for (typename ErasedList::const_iterator it=p_erased_list->begin()
                     ; it!=p_erased_list->end()
                     ; ++it) {
                a_tmp_erased_.push_back (uni_.to_ptr(*it));
            }
        }
        return &a_tmp_erased_;
    }
        
public:
    // Remove tuples with the same key of tuple and passing all pa_filter
    // Returns: a vector of erased tuples, the vector will be re-used after
    //          next erasure, caller of this function should not keep the
    //          pointer or do another erasure before dropping the vector
    const std::vector<const Tup*>*
    erase_by (const Tup& tuple, const std::vector<Predicate*>* pa_filter)
    { return erase_sub_by (sub_conv_(tuple), pa_filter); }

    // Remove tuples with the same key of ref_tuple and passing all pa_filter
    // Returns: a vector of erased tuples, the vector will be re-used after
    //          next erasure, caller of this function should not keep the
    //          pointer or do another erasure before dropping the vector
    const std::vector<const Tup*>*
    erase_ref_by (const ObjectHanger& oh, const RefTup& ref_tuple,
                  const std::vector<Predicate*>* pa_filter)
    { return erase_sub_by (sub_conv_(oh, ref_tuple), pa_filter); }

    // Erase tuples passing all pa_filter
    // Returns: a vector of erased tuples, the vector will be re-used after
    //          next erasure, caller of this function should not keep the
    //          pointer or do another erasure before dropping the vector
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

        Map& bg_cont = *ap_cont_[this->bg_no()];
        TrueToErase tte(pa_filter);
        for (typename Map::iterator it=bg_cont.begin()
                 , it_e=bg_cont.end()
                 ; it!=it_e; ++it) {
            const ErasedList* p_erased_list = NULL;
            if (it->second) {
                Cowbass* p_cb = it->second;
                p_erased_list = p_cb->erase_by(tte);
            }  //else if there's no cowbass, ignore the erasure
            
            if (p_erased_list && !p_erased_list->empty()) {
                s_mod_key_.insert (it->first, 1);

                for (typename ErasedList::const_iterator
                         it2 = p_erased_list->begin();
                     it2!=p_erased_list->end(); ++it2) {
                    a_tmp_erased_.push_back (uni_.to_ptr(*it2));
                }
            }
        }
            
        return &a_tmp_erased_;
    }

    // Erase all elements in background
    // Note: this function takes exactly same effect with erase_all_by(NULL)
    void clear()
    {
        if (unlikely (immutable_)) {
            ST_FATAL ("Fail to modify immutable index%s of table%s"
                      , show(this->header_).c_str()
                      , show(this->table_header_).c_str());
            return;
        }

        Map& bg_cont = *ap_cont_[this->bg_no()];
        for (typename Map::iterator it=bg_cont.begin()
                 , it_e=bg_cont.end()
                 ; it!=it_e; ++it) {
            if (it->second) {
                s_mod_key_.insert (it->first, 1);
                it->second->clear();
            }
        }       
    }        
    
    // Begin a round of update, call this before any modifications on this index
    // Returns: 0/errnos
    int begin_update ()
    {
        // disallow calling more than once
        if (!immutable_) {
            ST_FATAL ("miss end_update in index%s of table%s"
                      , show(this->header_).c_str()
                      , show(this->table_header_).c_str());
            return EGENERAL;
        }
            
        int ret = 0;
        // if the version of foreground is strictly greater than background,
        // buffers were not swapped and we'd better leave the change in
        // background and don't do any synchronization or recycling
        if (a_ver_[this->fg_no()] > a_ver_[this->bg_no()]) {
            // if there's no modifications, sync does nothing
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

    // End a round of update
    // With current implementation, you're able to get in-place data
    // before end_update, with a relatively slower speed, so this
    // function just works as a "flush" and may significantly improve
    // seeking and traversing speed.
    int end_update ()
    {
        // disallow calling more than once
        if (immutable_) {
            ST_FATAL ("miss begin_update in index%s of table%s"
                      , show(this->header_).c_str()
                      , show(this->table_header_).c_str());
            return EGENERAL;
        }

        // before spawn don't change versions of buffers because we
        // need that info to decide buffers were swapped or not
        int ret = spawn ();
        if (0 != ret) {
            ST_FATAL ("Fail to spawn");
            return ret;
        }
        // make version of background strictly greater than foreground
        // comparison between versions of buffers indicates if the
        // buffers are swapped
        a_ver_[this->bg_no()] = a_ver_[this->fg_no()] + 1;

        immutable_ = true;
        return ret;
    }

        
private:
    // Freeze changes in changed background cowbasses
    // Returns: 0 success or not
    int spawn ()
    {
        int ret = 0;
        int bg_no = this->bg_no();
        Map& bg_cont = *ap_cont_[bg_no];
        for (typename ModSet::iterator it=s_mod_key_.begin();
             it!=s_mod_key_.end(); ++it) {
            Key& key = it->key;
            Cowbass** pp_cb = bg_cont.seek (key);
            if (pp_cb) {
                Cowbass* p_cb = *pp_cb;
                if (NULL == p_cb) {
                    ST_FATAL ("cowbass object of key=%s is NULL"
                              , show(key).c_str());
                    //return E_IMPOSSIBLE;
                    // instead of returning error, erase the slot and continue
                    bg_cont.erase (key);
                    continue;
                }
                // decide the cowbass changes self or another new one
                bool modify_self = false;
                if (p_cb->fg_empty()) {
                    // there's no empty cowbass : empty ones are guaranteed
                    // to be recycled by return_cowbass, no data in
                    // foreground means the cowbass is created in this round
                    modify_self = true;
                } else {
                    // this bug is so stupid and made me spent one day to debug
                    // it. I thought it was a good idea to use versions of
                    // buffers to decide where the cowbass spawns. Greater
                    // version in background means buffers were not swapped so 
                    // background cowbass is probably not used and safe to 
                    // modify self. It's true. However it's also obvious 
                    // that if a cowbass is not changed this round, then it
                    // is already shared! If the code is not aware this simple
                    // fact, nightmare comes. e.g. if the cowbass is probably
                    // returned to cb_stack_ twice and consequently borrowed
                    // out twice. and the cowbass is changing while using: a
                    // potential segfault. 
                    // if (a_ver_[this->fg_no()] < a_ver_[this->bg_no()]) {
                    //     // a_ver_ is changed after spawn, if version of background
                    //     // is already greater than foreground, the buffers were
                    //     // not swapped and background is not being used yet.
                    //     modify_self = true;
                    // }
                    // we MUST decide "swappedness" by checking equality
                    // between pointers of cowbasses in two buffers
                    Map& fg_cont = *ap_cont_[this->fg_no()];
                    Cowbass** pp_cb2 = fg_cont.seek(key);
                    if (NULL == pp_cb2 || *pp_cb2 != p_cb) {
                        modify_self = true;
                    }
                }

                // do the modification
                if (modify_self) {
                    // this is a new inserted cowbass
                    // it's safe to change itself
                    p_cb->change_fg();
                    if (p_cb->fg_size() == 0) {
                        return_cowbass (p_cb);
                        bg_cont.erase (key);
                    }
                } else {
                    // foreground data are probably being used
                    Cowbass* p_new_cb = borrow_cowbass();
                    if (NULL == p_new_cb) {
                        ret = ENOMEM;
                    } else {
                        p_cb->change_other(p_new_cb);
                        if (p_new_cb->fg_size() > 0) {
                            bg_cont.insert (key, p_new_cb);
                        } else {
                            // merge cowbass is empty, delete the cowbass
                            // and erase the entry from hashmap directly
                            return_cowbass(p_new_cb);
                            bg_cont.erase (key);
                        }
                    }
                }
            } else {
                // this is possible because consecutive pairs of
                // begin_update and end_update without increasing version
                // is allowed now, a cowbass object was probably removed
                // from the hashmap in former end_update and wasn't
                // modified in this round. For this reason, there's
                // nothing to do in this branch.
                //ST_FATAL ("cowbass object to key=%s is NULL", show(*it).c_str());
                //ret = E_IMPOSSIBLE;
            }
        }
        return ret;
    }

    // Make background same with foreground,
    // as a non-primary index, this is generally invoked after version
    // increased. At this moment, background data which is formerly foreground
    // are not being queried, so we scan s_mod_key_ which stores changed
    // keys and let keys in background and foreground hashmaps point to same
    // cowbasses
    int sync ()
    {
        int ret = 0;
        int fg_no = this->fg_no();
        int bg_no = this->bg_no();
        Map& fg_cont = *ap_cont_[fg_no];
        Map& bg_cont = *ap_cont_[bg_no];
        for (typename ModSet::iterator it=s_mod_key_.begin();
             it!=s_mod_key_.end(); ++it) {
            const Key& key = it->key;

            Cowbass* p_cb1 = NULL;
            Cowbass** pp_cb1 = fg_cont.seek (key);
            if (pp_cb1) {
                p_cb1 = *pp_cb1;
                if (NULL == p_cb1) {
                    ST_FATAL ("the cowbass object to key=%s is NULL",
                              show(key).c_str());
                }
            }

            Cowbass** pp_cb2 = bg_cont.seek (key);
            if (pp_cb2) {
                Cowbass* p_cb2 = *pp_cb2;
                if (p_cb2 && p_cb2 != p_cb1) {
                    // the background cowbass is non-void and different from
                    // foreground which means foreground was spawned last
                    // round
                    return_cowbass (p_cb2);
                }
            }
                
            if (p_cb1) {
                bg_cont.insert (key, p_cb1);
            } else {
                bg_cont.erase (key);
            }
        }

        return ret;
    }


    void recycle_delayed_memory ()
    {
        mp_.recycle_delayed();
        mmp_.recycle_delayed();
        //s_mod_key_.recycle_delayed();
        ap_cont_[0]->recycle_delayed();
        ap_cont_[1]->recycle_delayed();
    }

    // Clear recorded modifications
    void clear_mods ()
    { s_mod_key_.clear(); }

    size_t cowbass_info (int buf_no, size_t* p_count
                         , size_t* p_max_len, size_t* p_min_len
                         , double* p_avg_len, double* p_avg_mod) const
    {
        int fg_no = this->fg_no();
        Map& cont = *ap_cont_[buf_no];
        size_t c = 0;
        size_t hz = 0;
        size_t max_len=0, min_len=UINT_MAX;
        int mod_per_sum = 0;
        for (typename Map::iterator it=cont.begin()
                 , it_e=cont.end()
                 ; it != it_e
                 ; ++it) {
            Cowbass* p_set = it->second;
            size_t l = p_set
                ? (buf_no==fg_no ? p_set->fg_size() : p_set->bg_size())
                : 0;
            if (l > max_len) {
                max_len = l;
            }
            if (l < min_len) {
                min_len = l;
            }
            ++ hz;
            c += l;
            mod_per_sum += p_set ? p_set->mod_percent() : 0;
        }
        if (p_count) {
            *p_count = hz;
        }
        if (p_max_len) {
            *p_max_len = hz > 0 ? max_len : 0;
        }
        if (p_min_len) {
            *p_min_len = hz > 0 ? min_len : 0;
        }
        if (p_avg_len) {
            *p_avg_len = hz > 0 ? (c/(double)hz) : 0;
        }
        if (p_avg_mod) {
            *p_avg_mod = hz > 0 ? (mod_per_sum/(double)hz) : 0;
        }
        return c;
    }

    size_t cb_mem () const
    {
        size_t cb_mem = 0;
        for (size_t i=0; i<a_cb_.size(); ++i) {
            cb_mem += a_cb_[i]->mem_without_node();
        }
        return cb_mem;
    }
        
public:
    size_t mem () const
    {
        return sizeof(*this)
            + a_tmp_erased_.capacity() * sizeof(const Tup*)
            + ap_cont_[0]->mem()
            + ap_cont_[1]->mem()
            + mp_.mem()
            + mmp_.mem()
            + cb_mem()
            + s_mod_key_.mem()
            + cb_stack_.capacity() * sizeof(Cowbass*)
            ;
    }
        
    void to_string (StringWriter& sb) const
    {
        sb << "NonPrimaryHashIndex"
           << (_StorePtr?"(ptr)":"")
           << this->header_
           << "==>"
            ;
        sb.append_format ("{address=%p", this);
        sb << " mem=" << mem()
           << " mp=" << mp_
           << " mmp=" << mmp_
           << " cb_sz=" << cb_mem()
           << " s_mod_key_sz=" << s_mod_key_.mem()
           << " cb_stack_sz=" << cb_stack_.capacity() *sizeof(Cowbass*)
           << " tmp_erased_sz="
           << a_tmp_erased_.capacity() * sizeof(const Tup*)
            ;

        size_t max_len, min_len;
        double avg_len;
        size_t cb_count;
        size_t bkt_max_len, bkt_min_len;
        double bkt_avg_len;
        double avg_mod;
        for (int i=0; i<2; ++i) {
            Map& cont = *ap_cont_[i];
            size_t sz = cowbass_info
                (i, &cb_count, &max_len, &min_len, &avg_len, &avg_mod);
            cont.bucket_info(&bkt_max_len, &bkt_min_len, &bkt_avg_len);
            sb << " cont[" << i << "]=>"
               << "{version=" << a_ver_[i]
               << " elem_cnt=" << sz
               << " hash="
               << "{size=" << cont.size()
               << " hash_size=" << cont.hash_size()
               << " free=" << cont.free_size()
               << " 2b_free=" << cont.delete_size()
               << " block_cnt=" << cont.block_size()
               << " max_len=" << bkt_max_len
               << " min_len=" << bkt_min_len
               << " avg_len=" << bkt_avg_len
               << "}"
               << " cowbass={cnt=" << cb_count
               << " changed_cnt=" << s_mod_key_.size()
               << " max_len=" << max_len
               << " min_len=" << min_len
               << " avg_len=" << avg_len
               << " avg_mod=" << avg_mod
               << "}"
                //<< " content=" << cont
               << "}"
                ;
        }
        sb << "}";
    }

    size_t size (int buf_no) const
    {
        int fg_no = this->fg_no();
        size_t c = 0;
        Map& cont = *ap_cont_[buf_no];

        if (buf_no == fg_no) {
            for (typename Map::iterator it=cont.begin()
                     , it_e=cont.end()
                     ; it != it_e; ++it) {
                c += it->second ? it->second->fg_size() : 0;
            }
        } else {
            for (typename Map::iterator it=cont.begin()
                     , it_e=cont.end()
                     ; it != it_e; ++it) {
                c += it->second ? it->second->bg_size() : 0;
            }                
        }
        return c;
    }

    bool empty (int buf_no) const
    {
        int fg_no = this->fg_no();
        Map& cont = *ap_cont_[buf_no];

        if (buf_no == fg_no) {
            for (typename Map::iterator it=cont.begin()
                     , it_e=cont.end()
                     ; it != it_e; ++it) {
                if (it->second && !it->second->fg_empty()) {
                    return false;
                }
            }
        } else {
            for (typename Map::iterator it=cont.begin()
                     , it_e=cont.end()
                     ; it != it_e; ++it) {
                if (it->second && !it->second->bg_empty()) {
                    return false;
                }
            }
        }
        return true;
    }

private:        
    Cowbass* borrow_cowbass ()
    {
        if (cb_stack_.empty()) {
            //Cowbass* p_cb = cb_pool_.createp<Cowbass>(&mp_);
#ifndef USE_NAIVE_SET
            Cowbass* p_cb = ST_NEW (Cowbass, &mp_);
#else
            Cowbass* p_cb = ST_NEW (Cowbass, &mp_, &mmp_);
#endif
            a_cb_.push_back (p_cb);
            ++ info_.cb_new_cnt;
            if (NULL == p_cb) {
                ST_FATAL ("Fail to new cowbass");
                return NULL;
            }

            return p_cb;
        } else {
            Cowbass* p_cb = cb_stack_.back();
            cb_stack_.pop_back();

            p_cb->instant_clear();
            return p_cb;
        }
    }
        
    void return_cowbass (Cowbass* p_cb) {
        if (NULL == p_cb) {
            ST_FATAL ("param[p_cb] is NULL");
            return;
        }
        p_cb->instant_clear();
        cb_stack_.push_back (p_cb);
    }

    int destroy_shared ()
    {
        std::vector<Cowbass*> a_ptr;
        a_ptr.reserve (ap_cont_[0]->size() + ap_cont_[1]->size());
        // std::cout << "cb_del_cnt=" << info_.cb_del_cnt
        //           << " size0=" << ap_cont_[0]->size()
        //           << " size1=" << ap_cont_[1]->size()
        //           << std::endl;

        for (int i=0; i<2; ++i) {
            for (typename Map::iterator it=ap_cont_[i]->begin()
                     , it_e=ap_cont_[i]->end()
                     ; it != it_e; ++it) {
                a_ptr.push_back (it->second);
            }
            ap_cont_[i]->clear();
        }
        for (size_t i=0; i<cb_stack_.size(); ++i) {
            a_ptr.push_back (cb_stack_[i]);
        }
        cb_stack_.clear();
        
        std::sort (a_ptr.begin(), a_ptr.end());
        typename std::vector<Cowbass*>::iterator new_end =
            std::unique (a_ptr.begin(), a_ptr.end());
        size_t del_cnt = info_.cb_del_cnt;
        for (typename std::vector<Cowbass*>::iterator it=a_ptr.begin()
                 ; it!=new_end; ++it) {
            delete *it;
            ++ del_cnt;
        }

        size_t new_cnt = info_.cb_new_cnt;
        
        if (new_cnt != del_cnt) {
            ST_FATAL ("cowbasses in index%s were newed %lu times but"
                      " deleted %lu times, memory leaks!"
                      , show(this->header_).c_str(), new_cnt, del_cnt);
            return EIMPOSSIBLE;
        } else {
            ST_SAY ("cowbasses in index%s were newed and deleted"
                    "%lu times", show(this->header_).c_str(), new_cnt);
            return 0;
        }
    }

        
private:
    std::vector<const Tup*> a_tmp_erased_;
    Map* ap_cont_[2];
    int a_ver_[2];
    std::vector<Cowbass*> cb_stack_;
    std::vector<Cowbass*> a_cb_;
//bsl::ResourcePool cb_pool_;
    Uni uni_;
    typename Cowbass::Pool mp_;
#ifdef USE_NAIVE_SET
    typename Cowbass::ModPool mmp_;
#endif
    _SubF sub_conv_;
    ModSet s_mod_key_;
    bool immutable_;
// stat
    mutable Info info_;
};
    
}    

#endif  // _NON_PRIMARY_HASH_INDEX_HPP_
