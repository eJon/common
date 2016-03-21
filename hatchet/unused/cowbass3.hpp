// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// (smalltable1) Implement of Copy-On-Write Block-Accessed Sorted Set
// Author: gejun@baidu.com
// Date: 2010-08-30 06:07
#pragma once
#ifndef _COWBASS3_HPP_
#define _COWBASS3_HPP_

// define this macro to use naive_set instead of std::set
#define USE_NAIVE_SET

#include "easy_deque.hpp"

#ifndef USE_NAIVE_SET
#include <set>
#else
#include <vector>
#endif

namespace st {
#ifdef USE_NAIVE_SET  
template <typename _T, typename _Compare>
class NaiveSet {        
    struct Mod {
        _T value;
        ModType op;

        void to_string (StringWriter& sw) const
        {
            switch (op) {
            case MOD_INSERT:
                sw << "+" << value;
                break;
            case MOD_ERASE:
                sw << "-" << value;
                break;
            default:
                sw << "(unknown-op)" << value;
            }
        }
    };

    struct CompareMod {
        bool operator() (const Mod& m1, const Mod& m2) const
        { return cmp_(m1.value, m2.value) < 0; }
            
        _Compare cmp_;
    };

    typedef EasyDeque<Mod,16> ModList;
public:
    typedef typename ModList::Node Node;
    typedef typename ModList::Pool Pool;

private:
    void sortify ()
    {
        // [a_val_.begin(), a_val_.begin()+last_sz_-1] are already ordered
        // [a_val_.begin()+last_sz_, a_val_.end()-1] are unordered
        std::stable_sort (a_val_.begin()+last_sz_, a_val_.end(), CompareMod());
        _Compare cmp;
            
        bg_.clear();
        // merge upper two ordered sections into new_one
        // there's no duplicated elements in the first section,
        // but there're probably duplicated elements in the second section
        typename ModList::iterator
            it1 = a_val_.begin()
            , it1_e = it1 + last_sz_
            , it2 = it1_e
            , it2_e = a_val_.end();
            
        int ret = 0;
        while (it2 != it2_e && it1 != it1_e) {
            const Mod& v2 = *it2;
            const Mod& v1 = *it1;
            ret = cmp (v1.value, v2.value);

            if (ret < 0) {  // v1 < v2
                bg_.push_back (v1);
                ++ it1;
            }
            else if (ret > 0) {  // v2 < v1
                for (++it2
                         ; it2!=it2_e && 0 == cmp(it2->value, v2.value)
                         ; ++it2);
                bg_.push_back (*(it2-1));
            }
            else {  // v1 == v2
                // find the last element that has the same value with v2
                for (++it2
                         ; it2!=it2_e && 0 == cmp(it2->value, v2.value)
                         ; ++it2);
                bg_.push_back (*(it2-1));
                ++ it1;
            }
        }
                
        if (it2 == it2_e) {  // it2 ends
            for (; it1 != it1_e; ++it1) {
                bg_.push_back (*it1);
            }
        }
        else {  // it1 ends
            while (it2 != it2_e) {
                const Mod& v2 = *it2;
                for (++it2
                         ; it2!=it2_e && 0 == cmp(it2->value, v2.value)
                         ; ++it2);
                bg_.push_back (*(it2-1));
            }                    
        }
            
        a_val_.swap (bg_);
        bg_.clear();

        last_sz_ = a_val_.size();
    }
        
public:
    NaiveSet (typename ModList::Pool* p_pool)
        : last_sz_(0)
        , a_val_(p_pool)
        , bg_(p_pool)
    {}
        
    typedef typename ModList::iterator const_iterator;

    const_iterator begin()
    {
        if (a_val_.size() != last_sz_) {
            sortify();
        }
        return a_val_.begin();
    }

    const_iterator end()
    {
        if (a_val_.size() != last_sz_) {
            sortify();
        }
        return a_val_.end();
    }

    size_t size()
    {
        if (a_val_.size() != last_sz_) {
            sortify ();
        }
        return a_val_.size();
    }

    bool empty()
    {
        if (a_val_.size() != last_sz_) {
            sortify ();
        }
        return a_val_.empty();
    }
        
    void insert (const _T& v)
    {
        Mod m = { v, MOD_INSERT };
        a_val_.push_back (m);
    }

    void erase (const _T& v)
    {
        Mod m = { v, MOD_ERASE };
        a_val_.push_back (m);
    }

    void clear ()
    {
        a_val_.clear();
        bg_.clear();
        last_sz_ = 0;
    }
        
    template <typename _ForwardIterator>
    void clear_and_insert_sorted (const _ForwardIterator& it_b
                                  , const _ForwardIterator& it_e)
    {
        a_val_.clear();
        bg_.clear();
        Mod m;
        m.op = MOD_ERASE;
        for (_ForwardIterator it=it_b; it!=it_e; ++it) {
            m.value = *it;
            a_val_.push_back (m);
        }
        last_sz_ = a_val_.size();
    }
       
    size_t mem_without_node () const
    {
        return sizeof(*this)
            + a_val_.mem_without_node()
            + bg_.mem_without_node()
            ;
    }
        
private:
    size_t last_sz_;
    ModList a_val_;
    ModList bg_;
};
#endif
        
// Copy on write block access sorted set, which is implemented as a deque
// that spawns a new set with changes in batch, go through cowbass3 is as
// fast as primitive array. (In previous version it's implemented as a
// linked list and the iteration is slightly slower)
// Params:
//   T        value type
//   Compare  typed int (T,T), returning <0, =0, >=0 to indicate order
template <typename T, typename _Compare = Compare<T> >
struct Cowbass3 {
    typedef EasyDeque<T, 32> Deque;
    typedef typename Deque::Node Node;
    typedef Cowbass3<T, _Compare> Self;
    typedef T Value;
    typedef std::vector<Value> ErasedList;
    typedef typename Deque::Pool Pool;
        
    typedef struct {
        T value;
        ModType op;

        void to_string (StringWriter& sw) const
        {
            switch (op) {
            case MOD_INSERT:
                sw << "+" << value;
                break;
            case MOD_ERASE:
                sw << "-" << value;
                break;
            default:
                sw << "(unknown-op)" << value;
            }
        }
    } Mod;

    struct CompareMod {
        bool operator() (const Mod& m1, const Mod& m2) const
        { return cmp_(m1.value, m2.value) < 0; }
            
        _Compare cmp_;
    };

#ifndef USE_NAIVE_SET
    typedef std::set<Mod, CompareMod> ModSet;
#else
    typedef NaiveSet<T, _Compare> ModSet;
    typedef typename ModSet::Node ModNode;
    typedef typename ModSet::Pool ModPool;
#endif
        
    typedef typename ModSet::const_iterator mod_iterator;
        
    typedef typename Deque::traverse_iterator const_iterator;
        
    // iterator is same with const_iterator
    typedef const_iterator iterator; 

    struct bg_iterator {
        void operator++ ()
        {
            //RECHECK_TRUNK_AND_BRANCH:
            if (it != it_e) {
                //RECHECK_BRANCH:
                if (oit != oit_e) {
                    const Value& bv = oit->value;
                    const Value& tv = *it;
                    const int ret = cmp_ (bv, tv);
                    if (ret > 0) {  // branch > trunk
                        p_value_ = &tv;
                        ++ it;
                    } else if (ret < 0) {  // branch < trunk
                        if (MOD_INSERT == oit->op) {
                            p_value_ = &bv;
                            ++ oit;
                        } else {
                            ++ oit;
                            operator++();  // tail recursion
                            //goto RECHECK_TRUNK_AND_BRANCH;
                        }
                    } else {  // branch == trunk
                        if (MOD_INSERT == oit->op) {
                            p_value_ = &bv;
                            ++ oit;
                            ++ it;
                        } else {
                            ++ oit;
                            ++ it;
                            operator++();  //tail recursion
                            //goto RECHECK_TRUNK_AND_BRANCH;
                        }
                    }                     
                } else {  // oit == oit_e
                    p_value_ = &(*it);
                    ++ it;
                }
            } else {  // it == it_e
                while (1) {
                    if (oit != oit_e) {
                        if (MOD_ERASE == oit->op) {
                            ++ oit;
                        } else {  // MOD_INSERT
                            p_value_ = &(oit->value);
                            ++ oit;
                            break;
                        }
                    } else {
                        p_value_ = NULL;
                        break;
                    }
                }
            }
                
        }

        operator bool () const
        { return p_value_ != NULL; }

        const Value& operator* () const
        { return *p_value_; }

        const Value* operator-> () const
        { return p_value_; }
            
        bg_iterator (const Self* p_cb)
            : p_value_(NULL)
            , it(p_cb->deque_.traverse_begin())
            , it_e(p_cb->deque_.traverse_end())
            , oit(p_cb->mod_set_.begin())
            , oit_e(p_cb->mod_set_.end())
        {
            operator++ ();
        }
            
    private:
        const Value* p_value_;
        const_iterator it;
        const_iterator it_e;
        typename ModSet::const_iterator oit;
        typename ModSet::const_iterator oit_e;
        _Compare cmp_;
    };

public:
        
    // @brief construct a cowbass3
    // @param [p_pool] pointer to the memorypool allocating nodes
#ifndef USE_NAIVE_SET
    explicit Cowbass3(Pool *p_pool)
        : deque_(p_pool)
        , mod_percent_(0)
    {}
#else
    explicit Cowbass3(Pool *p_pool, ModPool *p_mod_pool)
        : deque_(p_pool)
        , mod_set_(p_mod_pool)
        , mod_percent_(0)
    {}
#endif
        
    // @brief destructor, giving all memory back to allocator, notice
    // all nodes are deallocated, so if ~ctor is manually invoked,
    // invoke ctor rather than clear(which keeps an allocated node) to
    // initialize it. 
    ~Cowbass3()
    {}


    // @brief copy-constructor
#ifndef USE_NAIVE_SET
    Cowbass3 (const Self& other)
        : deque_(other.p_pool_)
    {
        //ASSERT (&other != this)
        other.change_deque_ (&deque_);
    }
#else
    Cowbass3 (const Self& other)
        : deque_(other.p_pool_)
        , mod_set_(other.p_pool_)
    {
        //ASSERT (&other != this)
        other.change_deque_ (&deque_);
    }
#endif
        
    // @brief copying content from other cowbass3, previous content is
    // cleared
    Self& operator= (const Self& other)
    {
        if (&other == this) {
            change_fg ();
        }
        else {
            instant_clear ();
            other.change_deque_ (&deque_);
        }            
        return *this;
    }
                                        
    void insert (const T& value)
    {
#ifndef USE_NAIVE_SET
        Mod m = { value, MOD_INSERT };
        // erase what are erased or inserted before
        std::pair<typename ModSet::const_iterator,bool> res = mod_set_.insert (m);
        if (!res.second) {  // existed
            // EVIL, check /usr/include/c++/<gcc-version>/bits/stl_set.h
            // to make sure following cast works
            const_cast<Mod&>(*(res.first)) = m;
        }
#else
        mod_set_.insert (value);
#endif
    }

    void erase (const T& value)
    {
#ifndef USE_NAIVE_SET
        Mod m = { value, MOD_ERASE };
            
        std::pair<typename ModSet::iterator,bool> res = mod_set_.insert (m);
        if (!res.second) {  // existed
            // EVIL, check /usr/include/c++/<gcc-version>/bits/stl_set.h
            // to make sure following cast works
            const_cast<Mod&>(*(res.first)) = m;
        }
#else
        mod_set_.erase (value);
#endif
    }

    inline const_iterator fg_begin () const
    {
        return deque_.traverse_begin();
        // when deque_ was a pointer before, following code was faster
        //return const_iterator(deque_.a_node_[0]->data, deque_.a_node_.begin());
    }
            
    /**
     * @brief get ending position of this cowbass3, a general tip is using
     * for (iterator it=begin(),it_e=end(); it!=it_e; ++it)
     * to replace
     * for (iterator it=begin(); it!=end(); ++it)
     * because end is not simply returning NULL thing as in other containers.
     */
    inline const_iterator fg_end () const
    {
        return deque_.traverse_end();
        // when deque_ was a pointer before, following code was faster
        //return const_iterator(&(deque_.at(deque_.count_)), deque_.a_node_.end());
    }

    // get number of elements of foreground
    size_t fg_size() const
    { return deque_.size(); }

    // test emptiness of foreground
    bool fg_empty() const
    { return deque_.empty(); }

    // @brief get the value at the given index
    const Value& fg_at (const size_t idx) const
    { return deque_.at(idx); }

    // @brief get the value at the given index, non-const version
    Value& fg_at (const size_t idx)
    { return deque_.at(idx); }

    // the iterator to background data, the iteration speed is much slower
    // than foreground (2~10 times, depending on relative size of mod_set_)
    bg_iterator bg_begin () const
    {
        return bg_iterator(this);
    }

    bool bg_empty () const
    {
        if (mod_set_.empty()) {
            return fg_empty();
        }
            
        if (deque_.empty()) {  // notice here is "deque_"
            for (typename ModSet::const_iterator
                     it=mod_set_.begin()
                     ; it!=mod_set_.end(); ++it) {
                if (MOD_INSERT == it->op) {
                    return false;
                }
            }
        }
        else {
            typename ModSet::const_iterator oit = mod_set_.begin();
            typename ModSet::const_iterator oit_e = mod_set_.end();
            const_iterator it = deque_.traverse_begin();
            const_iterator it_e = deque_.traverse_end();
                
            int ret = 0;
            while (oit != oit_e && it != it_e) {
                const Value& bv = oit->value;
                Value tv = *it;
                ret = cmp_ (bv, tv);

                if (ret > 0) {  // branch > trunk
                    return false;
                }
                else if (ret < 0) {  // branch < trunk
                    if (MOD_INSERT == oit->op) {
                        return false;
                    }
                    ++ oit;
                }
                else {  // branch == trunk
                    if (MOD_INSERT == oit->op) {
                        return false;
                    }
                    ++ oit;
                    ++ it;
                }
            }
                
            if (oit == oit_e) {  // branch ends
                return !(it != it_e);
            }
            else {  // trunk ends
                for (; oit != oit_e; ++oit) {
                    if (MOD_INSERT == oit->op) {
                        return false;
                    }
                }                    
            }
        }
        return true;
    }
        
    size_t bg_size () const
    {
        if (mod_set_.empty()) {
            return fg_size();
        }
            
        size_t c = 0;
        if (deque_.empty()) {  // notice here is "deque_"
            for (typename ModSet::const_iterator
                     it=mod_set_.begin()
                     ; it!=mod_set_.end(); ++it) {
                if (MOD_INSERT == it->op) {
                    ++ c;
                }
            }
        }
        else {
            typename ModSet::const_iterator oit = mod_set_.begin();
            typename ModSet::const_iterator oit_e = mod_set_.end();
            const_iterator it = deque_.traverse_begin();
            const_iterator it_e = deque_.traverse_end();
                
            int ret = 0;
            while (oit != oit_e && it != it_e) {
                const Value& bv = oit->value;
                const Value& tv = *it;
                ret = cmp_ (bv, tv);

                if (ret > 0) {  // branch > trunk
                    ++ c;
                    ++ it;
                }
                else if (ret < 0) {  // branch < trunk
                    if (MOD_INSERT == oit->op) {
                        ++ c;
                    }
                    ++ oit;
                }
                else {  // branch == trunk
                    if (MOD_INSERT == oit->op) {
                        ++ c;
                    }
                    ++ oit;
                    ++ it;
                }
            }
                
            if (oit == oit_e) {  // branch ends
                for (; it != it_e; ++it) {
                    ++ c;
                }
            }
            else {  // trunk ends
                for (; oit != oit_e; ++oit) {
                    if (MOD_INSERT == oit->op) {
                        ++ c;
                    }
                }                    
            }
        }
        return c;
    }

        
    // @brief schedule insertions of all elements of another container
    template <typename InputIterator>
    void absorb (InputIterator it_s, InputIterator it_e)
    {
        for (InputIterator it=it_s; it!=it_e; ++it)
        {
            insert (*it);
        }
    }

    // instantly remove all elements in this cowbass 
    void instant_clear ()
    {
        deque_.clear();
        mod_set_.clear();
    }

    // schedule removal of all elements
    void clear ()
    {
#ifndef USE_NAIVE_SET
        // remove all modifications before
        mod_set_.clear();
        Mod m;
        m.op = MOD_ERASE;
        for (const_iterator it=deque_.traverse_begin()
                 ,it_e=deque_.traverse_end()
                 ; it!=it_e
                 ; ++it) {
            m.value = *it;
            mod_set_.insert (m);
        }
#else
        mod_set_.clear_and_insert_sorted (deque_.traverse_begin()
                                          ,deque_.traverse_end());
#endif

    }

    bool modified () const
    {
        return !mod_set_.empty();
    }

    // @brief schedule removal of values that make f true
    // @param f: const Value& -> bool, if f(value) is true,
    // value will be scheduled to be erased
    template <typename _TrueToErase>
    const ErasedList* erase_by (const _TrueToErase& f)
    {
        a_tmp_erased_.clear();
            
        if (deque_.empty()) {  // notice here is "deque_"
            for (typename ModSet::const_iterator
                     it=mod_set_.begin()
                     ; it!=mod_set_.end(); ++it) {
                if (MOD_INSERT == it->op && f (it->value)) {
                    a_tmp_erased_.push_back (it->value);
                }
            }
        }
        else {
            typename ModSet::const_iterator oit = mod_set_.begin();
            typename ModSet::const_iterator oit_e = mod_set_.end();
            const_iterator it = deque_.traverse_begin();
            const_iterator it_e = deque_.traverse_end();
                
            int ret = 0;
            while (oit != oit_e && it != it_e) {
                const Value& bv = oit->value;
                const Value& tv = *it;
                ret = cmp_ (bv, tv);
                if (ret > 0) {  // branch > trunk
                    if (f (tv)) {
                        a_tmp_erased_.push_back (tv);
                    }
                    ++ it;
                }
                else if (ret < 0) {  // branch < trunk
                    if (MOD_INSERT == oit->op && f(oit->value)) {
                        a_tmp_erased_.push_back (oit->value);
                    }
                    ++ oit;
                }
                else {  // branch == trunk
                    if (MOD_INSERT == oit->op && f(oit->value)) {
                        a_tmp_erased_.push_back (oit->value);
                    }
                    ++ oit;
                    ++ it;
                }
            }

            if (oit == oit_e) {  // branch ends
                for (; it != it_e; ++it) {
                    if (f(*it)) {
                        a_tmp_erased_.push_back (*it);
                    }
                }
            }
            else {  // trunk ends
                for (; oit != oit_e; ++oit) {
                    if (MOD_INSERT == oit->op && f(oit->value)) {
                        a_tmp_erased_.push_back (oit->value);
                    }
                }                    
            }
        }

        // directly remove not-in-place values from mod_set_
        for (typename ErasedList::const_iterator
                 it=a_tmp_erased_.begin()
                 ; it!=a_tmp_erased_.end(); ++it) {
            erase (*it);
        }

        return &a_tmp_erased_;
    }


    // @brief schedule replacement of values which make f true with new
    // values transformed by g, notice old values are erased before any
    // new values being inserted, which makes sure that the updating
    // process is done once and only once. (otherwise the resulting set
    // may depend on how we store values or fall into deadlock)
    // @param f :: const Value& -> bool, if this function object returns
    // true for a Value, the Value will be replaced
    // @param g :: const Value& -> Value, transform the input value to a
    // new value
    template <typename _TrueToUpdate, typename _ValueMapping>
    const ErasedList& update_by (const _TrueToUpdate& f, const _ValueMapping& g)
    {
        a_tmp_erased_.clear();

        if (deque_.empty()) {  // notice here is "deque_"
            for (typename ModSet::const_iterator
                     it=mod_set_.begin()
                     ; it!=mod_set_.end(); ++it) {
                if (MOD_INSERT == it->op && f (it->value)) {
                    a_tmp_erased_.push_back (it->value);
                }
            }
        }
        else {
            typename ModSet::const_iterator oit = mod_set_.begin();
            typename ModSet::const_iterator oit_e = mod_set_.end();
            const_iterator it = deque_.traverse_begin();
            const_iterator it_e = deque_.traverse_end();
                
            int ret = 0;
            while (oit != oit_e && it != it_e) {
                const Value& bv = oit->value;
                const Value& tv = *it;
                ret = cmp_ (bv, tv);
                if (ret > 0) {  // branch > trunk
                    if (f (tv)) {
                        a_tmp_erased_.push_back (tv);
                    }
                    ++ it;
                } else if (ret < 0) {  // branch < trunk
                    if (MOD_INSERT == oit->op && f(oit->value)) {
                        a_tmp_erased_.push_back (oit->value);
                    }
                    ++ oit;
                } else {  // branch == trunk
                    if (MOD_INSERT == oit->op && f(oit->value)) {
                        a_tmp_erased_.push_back (oit->value);
                    }
                    ++ oit;
                    ++ it;
                }
            }

            if (oit == oit_e) {  // branch ends
                for (; it != it_e; ++it) {
                    if (f(*it)) {
                        a_tmp_erased_.push_back (*it);
                    }
                }
            } else {  // trunk ends
                for (; oit != oit_e; ++oit) {
                    if (MOD_INSERT == oit->op && f(oit->value)) {
                        a_tmp_erased_.push_back (oit->value);
                    }
                }                    
            }
        }

        // first delete
        for (typename ErasedList::const_iterator
                 it=a_tmp_erased_.begin()
                 ; it!=a_tmp_erased_.end(); ++it) {
            erase (*it);
        }

        // then add
        for (typename ErasedList::const_iterator
                 it=a_tmp_erased_.begin()
                 ; it!=a_tmp_erased_.end(); ++it) {
            insert (g(*it));
        }

        return a_tmp_erased_;
    }

    // apply mod_set_ to foreground
    int change_fg ()
    {
        Deque bg(deque_.pool_ptr());
        change_deque_ (&bg);
        deque_.swap(bg);

        // clear modifications
        mod_set_.clear();

        return 0;
    }

    int change_other (Self* p_cb)
    {
        if (p_cb == this) {
            change_fg ();
        } else {
            p_cb->instant_clear ();
            change_deque_ (&p_cb->deque_);
                
            // clear modifications
            mod_set_.clear();
        }

        return 0;
    }

        
    // printing information
    void to_string (StringWriter& sb) const
    {
        sb << "{cnt=" << deque_.size();
        sb << " mod=";
        shows (sb, mod_set_.begin(), mod_set_.end()
               , StringWriter::MAX_N_PRINT);
        sb << " content=" << deque_
           << "}"
            ;
        //sb << deque_;
    }

private:
    int change_deque_ (Deque* p_bg) const
    {
        if (NULL == p_bg) {
            ST_FATAL ("param[p_bg] is NULL");
            return EINVAL;
        }
        if (&deque_ == p_bg) {
            ST_FATAL ("p_bg must be different from &deque_");
            return EIMPOSSIBLE;
        }

        // update mod_percent
        int new_mod_percent =
            mod_set_.size()*100/(1+mod_set_.size()+deque_.size());
        if (mod_percent_ != 0) {
            mod_percent_ = (mod_percent_ + new_mod_percent) / 2;
        } else {
            mod_percent_ = new_mod_percent;
        }
            
        p_bg->clear();

        if (deque_.empty()) {  // notice here is "deque_"
            for (typename ModSet::const_iterator
                     it=mod_set_.begin()
                     ; it!=mod_set_.end(); ++it) {
                if (MOD_INSERT == it->op) {
                    p_bg->push_back (it->value);
                }
            }
        }
        else {
            typename ModSet::const_iterator oit = mod_set_.begin();
            typename ModSet::const_iterator oit_e = mod_set_.end();
            const_iterator it = deque_.traverse_begin();
            const_iterator it_e = deque_.traverse_end();
                
            int ret = 0;
            while (oit != oit_e && it != it_e) {
                const Value& bv = oit->value;
                const Value& tv = *it;
                ret = cmp_ (bv, tv);

                if (ret > 0) {  // branch > trunk
                    p_bg->push_back (tv);
                    ++ it;
                }
                else if (ret < 0) {  // branch < trunk
                    if (MOD_INSERT == oit->op) {
                        p_bg->push_back (bv);
                    }
                    ++ oit;
                }
                else {  // branch == trunk
                    if (MOD_INSERT == oit->op) {
                        p_bg->push_back (bv);
                    }
                    ++ oit;
                    ++ it;
                }
            }
                
            if (oit == oit_e) {  // branch ends
                for (; it != it_e; ++it) {
                    p_bg->push_back (*it);
                }
            }
            else {  // trunk ends
                for (; oit != oit_e; ++oit) {
                    if (MOD_INSERT == oit->op) {
                        p_bg->push_back (oit->value);
                    }
                }                    
            }
        }
        return 0;
    }

public:
    short mod_percent () const
    { return mod_percent_; }

    mod_iterator mod_begin() const
    { return mod_set_.begin(); }

    mod_iterator mod_end() const
    { return mod_set_.end(); }
        
    size_t mod_size () const
    { return mod_set_.size(); }

    size_t mem_without_node() const
    {
        return a_tmp_erased_.capacity() * sizeof(Value)
            + sizeof(*this)
            + mod_set_.mem_without_node()
            + deque_.mem_without_node()
            ;
    }
        
private:
    Deque deque_;
#ifndef USE_NAIVE_SET
    ModSet mod_set_;
#else
    mutable ModSet mod_set_;
#endif
    mutable short mod_percent_;
    _Compare cmp_;
    ErasedList a_tmp_erased_;
};
}

#endif// _COWBASS3_HPP_
