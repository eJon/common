// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// (smalltable1) constraints for selections
// Author: gejun@baidu.com
// Date: Tue Aug 10 14:19:24 2010

#pragma once
#ifndef _PREDICATE_HPP_
#define _PREDICATE_HPP_

#include "common.h"
#include <vector>
#include "functional.hpp"

namespace st {
template <typename _T> struct Attribute;
    
const int PRED_INVALID = 1;
const int PRED_FILTER = 2;
const int PRED_ASSIGN = 3;

const int PRED_TABLE0 = 1;
const int PRED_TABLE1 = 2;
const int PRED_TABLE2 = 4;
const int PRED_TABLE3 = 8;
const int PRED_TABLE4 = 16;
    
static int PRED_TABLE(const int i)
{
    return (int)(1 << i);
}

const int MAX_SELECT_TABLE_NUM = 5;

// base class for "xx == yy", "xx > yy" ... and UserPredicates  
struct Predicate {
    virtual ~Predicate ()
    {}
        
    // Categorize predicates into two groups
    // Note: this function is not const on this pointer because it memorizes
    //       table_mask_
    // Params:
    //   pp_table     array of pointers to tables
    //   table_num    length of the array
    //   p_table_mask tables that relates to this predicate, represented by
    //                bitwise OR of PRED_TABLE(N)
    // Returns:
    //   PRED_ASSIGN  this predicate is in form of "x==y" and either x or y
    //                are constants/variables or x,y are attributes from
    //                different tables
    //   PRED_FILTER  valid predicates that are not PRED_ASSIGN
    //   PRED_INVALID typically caused by that an attribute comes from a
    //                table that is not in pp_table
    virtual int categorize (const void* const* pp_table
                            , const int table_num) /*const*/ = 0;

    int table_mask() const { return table_mask_; }

    // Copy the pointer of assignment source to the ref_tuple
    // Note: call this only for PRED_ASSIGN,
    // Example: [ptr_to_tuple1,ptr_to_tuple2...ptr_to_cv1(where pp_cv starts),
    //          ptr_to_cv2...], pointers to tuples are in [0..table_num-1],
    //          pointers to constants or variables are in
    //          [table_num..table_num+cv_num-1]
    virtual void set_ref_tuple (const void*   // p_table
                                , const void*  // p_tuple
                                , const void* const*  // pp_table
                                , const int  //table_num
                                , ObjectHanger&)  // oh
    {}
        
    // call this only for PRED_ASSIGN
    virtual int attr_id (const void* /*p_table*/) const { return 0; }

    // check the given tuple fullfil internal constaints or not
    virtual bool passed (const char* p_tuple) = 0;

    virtual void to_string(StringWriter& sw) const
    { sw << "Predicate"; }

 
protected:
    Predicate()
        : table_mask_(0)
    {}
        
    int table_mask_;
};

struct Conjunction {
    Conjunction& operator&& (Predicate& p_b)
    {
        a_bool_.push_back (&p_b);
        return *this;
    }

    ~Conjunction ()
    {
        for (size_t i=0; i<a_bool_.size(); ++i) {
            delete a_bool_[i];
        }
        a_bool_.clear();
    }
        
    std::vector<Predicate*>& pred_list() { return a_bool_; }

    static Conjunction& create ()
    {
        Conjunction* p_conj = new Conjunction;
        return *p_conj;
    }

private:
    Conjunction () {}
    std::vector<Predicate*> a_bool_;
};

inline Conjunction& operator&& (Predicate& p_a, Predicate& p_b)
{
#if _BullseyeCoverage
#pragma BullseyeCoverage off
#endif
    return Conjunction::create() && p_a && p_b;
#if _BullseyeCoverage
#pragma BullseyeCoverage on
#endif
}
    
struct FalsePredicate : public Predicate {
    int categorize (const void* const* /*pp_table*/
                    , const int /*table_num*/)
    { return PRED_INVALID; }

    bool passed (const char* /*p_tuple*/)
    { return false; }

    void to_string(StringWriter& sw) const
    { sw << "FalsePredicate"; }

    static FalsePredicate& create ()
    {
        FalsePredicate* p_fp = new FalsePredicate;
        return *p_fp;
    }
        
private:
    FalsePredicate() {}
       
};

template <typename _Func>
struct UserPredicate1 : public Predicate {
    typedef UserPredicate1<_Func> Self;
    typedef typename _Func::Arg1 T1;
        
    static Self& create (const Attribute<T1>& attr0)
    {
        Self* p = new (std::nothrow) Self;
        p->p_attr0_ = &attr0;
        return *p;
    }

    ~UserPredicate1 ()
    {
        if (p_attr0_) {
            delete p_attr0_;
            p_attr0_ = NULL;
        }
    }

    int categorize (const void* const* pp_table
                    , const int table_num)
    {
        for (int i=0; i<table_num; ++i) {
            if (pp_table[i] == p_attr0_->p_table()) {
                this->table_mask_ = PRED_TABLE(i);
                return PRED_FILTER;
            }
        }
        return PRED_INVALID;
    }

    bool passed (const char* p_tuple)
    { return f_(*(const T1*)(p_tuple + p_attr0_->offset())); }

    void to_string(StringWriter& sw) const
    { sw << f_ << "(" << *p_attr0_ << ")"; }
        
private:
    UserPredicate1 () {}
    const Attribute<T1> *p_attr0_;
    _Func f_;
};

// stands for attributes
struct PA {};
// stands for constants
struct PC {};
// stands for pointers(variables)
struct PP {};

template <typename _T, typename _UP_SOMETHING>
struct SourceAdapter;

template <typename _T>
struct SourceAdapter<_T,PA> {
    typedef const Attribute<_T>& Param;
        
    SourceAdapter(Param attr)
        : p_attr_(&attr)
    {}

    ~SourceAdapter ()
    {
        if (p_attr_) {
            delete p_attr_;
            p_attr_ = NULL;
        }
    }
        
    const _T& value(const char* p_tuple) const
    { return *(_T*)(p_tuple + p_attr_->offset()); }

    const void* p_table() const
    { return p_attr_->p_table(); }

    void to_string (StringWriter& sw) const
    { sw << p_attr_; }

private:
    const Attribute<_T>* p_attr_;
};

template <typename _T>
struct SourceAdapter<_T,PC> {
    typedef const _T& Param;
        
    SourceAdapter (Param value)
        : value_(value)
    {}
        
    const _T& value(const char*)
    { return value_; }

    const void* p_table() const
    { return NULL; }

    void to_string (StringWriter& sw) const
    { sw << value_; }

private:
    _T value_;
};

template <typename _T>
struct SourceAdapter<_T,PP> {
    typedef const _T* Param;
        
    SourceAdapter (Param p_value)
        : p_value_(p_value)
    {}
        
    const _T& value(const char*)
    { return *p_value_; }

    const void* p_table() const
    { return NULL; }

    void to_string (StringWriter& sw) const
    { sw << p_value_; }

private:
    const _T* p_value_;
};
    
template <typename _Func, typename _P1, typename _P2>
struct UserPredicate2
    : public Predicate {
    typedef typename _Func::Arg1 T1;
    typedef typename _Func::Arg2 T2;
    typedef SourceAdapter<T1,_P1> S1;
    typedef SourceAdapter<T2,_P2> S2;
    typedef UserPredicate2<_Func,_P1,_P2> Self;
        
    static Self& create (typename S1::Param s1, typename S2::Param s2)
    {
        Self* p = new Self(s1,s2);
        return *p;
    }

    int categorize (const void* const* pp_table
                    , const int table_num)
    {
        int m = 0;

        if (c_same<_P1, PA>::R) {
            int t0 = PRED_INVALID;
            for (int i=0; i<table_num; ++i) {
                if (pp_table[i] == s1_.p_table()) {
                    t0 = PRED_FILTER;
                    m |= PRED_TABLE(i);
                    break;
                }
            }
            if (PRED_INVALID == t0) {
                return PRED_INVALID;
            }
        }

        if (c_same<_P2, PA>::R) {
            int t1 = PRED_INVALID;
            for (int i=0; i<table_num; ++i) {
                if (pp_table[i] == s2_.p_table()) {
                    t1 = PRED_FILTER;
                    m |= PRED_TABLE(i);
                    break;
                }
            }
            if (PRED_INVALID == t1) {
                return PRED_INVALID;
            }
        }

        this->table_mask_ = m;
        return PRED_FILTER;
    }

    bool passed (const char* p_tuple)
    { return f_ (s1_.value(p_tuple), s2_.value(p_tuple)); }

    void to_string(StringWriter& sw) const
    { sw << f_ << "(" << s1_ << "," << s2_ << ")"; }
        
private:
    UserPredicate2 (typename S1::Param s1, typename S2::Param s2)
        : s1_(s1)
        , s2_(s2)
    {}

    S1 s1_;
    S2 s2_;
    _Func f_;
};


    

template <typename _Left, typename _Right>
struct Equi;

template <typename _T>
struct Equi<Attribute<_T>, _T>
    : public Predicate {
    typedef Equi<Attribute<_T>, _T> Self;
        
    static Self& create (const Attribute<_T>& attr, const _T& value)
    {
        Self* p = new Self;
        p->p_attr_ = &attr;
        p->value_ = value;
        return *p;
    }

    ~Equi ()
    {
        ST_DELETE (p_attr_);
    }
        
    int categorize (const void* const* pp_table
                    , const int table_num)
    {
        for (int i=0; i<table_num; ++i) {
            if (pp_table[i] == p_attr_->p_table()) {
                this->table_mask_ = PRED_TABLE(i);
                return PRED_ASSIGN;
            }
        }
        return PRED_INVALID;
    }

    int attr_id (const void* p_table) const
    { return (p_attr_->p_table() == p_table) ? p_attr_->id() : 0; }

    void set_ref_tuple (const void* /*p_table*/
                        , const void* p_ref_tuple
                        , const void* const* /*pp_table*/
                        , const int table_num
                        , ObjectHanger& oh)
    {
        ObjectHanger::Position* p_rp =
            ((ObjectHanger::Position*)p_ref_tuple) + p_attr_->ref_offset();
        *p_rp = oh.push_back (&value_);
        p_rp->idx += table_num;
    }
        
    bool passed (const char* p_tuple)
    { return *(const _T*)(p_tuple + p_attr_->offset()) == value_; }

    void to_string(StringWriter& sw) const
    { sw << "Equi(" << *p_attr_ << "," << value_ << ")"; }

private:
    Equi () {}
    const Attribute<_T>* p_attr_;
    _T value_;
};

template <typename _T>
struct Equi<Attribute<_T>, _T*>
    : public Predicate {
    typedef Equi<Attribute<_T>, _T*> Self;
        
    static Self& create (const Attribute<_T>& attr, const _T* p_value)
    {
        Self* p = new Self;
        p->p_attr_ = &attr;
        p->p_value_ = p_value;
        return *p;
    }

    ~Equi ()
    {
        if (p_attr_) {
            delete p_attr_;
            p_attr_ = NULL;
        }
    }

    int categorize (const void* const* pp_table
                    , const int table_num)
    {
        for (int i=0; i<table_num; ++i) {
            if (pp_table[i] == p_attr_->p_table()) {
                this->table_mask_ = PRED_TABLE(i);
                return PRED_ASSIGN;
            }
        }
        return PRED_INVALID;
    }

    int attr_id (const void* p_table) const
    { return (p_attr_->p_table() == p_table) ? p_attr_->id() : 0; }
        
    void set_ref_tuple (const void* /*p_table*/
                        , const void* p_ref_tuple
                        , const void* const* /*pp_table*/
                        , const int table_num
                        , ObjectHanger& oh)
    {
        ObjectHanger::Position* p_rp =
            ((ObjectHanger::Position*)p_ref_tuple) + p_attr_->ref_offset();
        *p_rp = oh.push_back (p_value_);
        p_rp->idx += table_num;
    }
        
    bool passed (const char* p_tuple)
    { return *(const _T*)(p_tuple + p_attr_->offset()) == *p_value_; }

    void to_string(StringWriter& sw) const
    { sw << "Equi(" << *p_attr_ << "," << p_value_ << ")"; }

private:
    Equi () {}
    const Attribute<_T>* p_attr_;
    const _T* p_value_;
};

    
template <typename _T>
struct Equi<Attribute<_T>, Attribute<_T> >
    : public Predicate {
    typedef Equi<Attribute<_T>, Attribute<_T> > Self;
        
    static Self& create (const Attribute<_T>& attr0, const Attribute<_T>& attr1)
    {
        Self* p = new Self;
        p->p_attr0_ = &attr0;
        p->p_attr1_ = &attr1;
        return *p;
    }

    ~Equi()
    {
        if (p_attr0_) {
            delete p_attr0_;
            p_attr0_ = NULL;
        }
        if (p_attr1_) {
            delete p_attr1_;
            p_attr1_ = NULL;
        }            
    }
        
    int categorize (const void* const* pp_table
                    , const int table_num)
    {
        int t0 = PRED_INVALID;
        int m0 = 0;
        for (int i=0; i<table_num; ++i) {
            if (pp_table[i] == p_attr0_->p_table()) {
                t0 = PRED_ASSIGN;
                m0 = PRED_TABLE(i);
                break;
            }
        }
        int t1 = PRED_INVALID;
        int m1 = 0;
        for (int i=0; i<table_num; ++i) {
            if (pp_table[i] == p_attr1_->p_table()) {
                t1 = PRED_ASSIGN;
                m1 = PRED_TABLE(i);
                break;
            }
        }
        if (PRED_INVALID == t0 || PRED_INVALID == t1) {
            return PRED_INVALID;
        }
        this->table_mask_ = (m0 | m1);
        return (m0 == m1) ? PRED_FILTER : PRED_ASSIGN;
    }

    int attr_id (const void* p_table) const
    {
        if (p_attr0_->p_table() == p_table) {
            return p_attr0_->id();
        }
        if (p_attr1_->p_table() == p_table) {
            return p_attr1_->id();
        }
        return 0;
    }

    void set_ref_tuple (const void* p_table
                        , const void* p_ref_tuple
                        , const void* const* pp_table
                        , const int table_num
                        , ObjectHanger&) /*oh*/
    {
        if (p_attr0_->p_table() == p_table) {
            ObjectHanger::Position* p_rp =
                ((ObjectHanger::Position*)p_ref_tuple) + p_attr0_->ref_offset();
            p_rp->offset = p_attr1_->offset();
            for (int i=0; i<table_num; ++i) {
                if (p_attr1_->p_table() == pp_table[i]) {
                    p_rp->idx = i;
                    break;
                }
            }
        }
        if (p_attr1_->p_table() == p_table) {
            ObjectHanger::Position* p_rp =
                ((ObjectHanger::Position*)p_ref_tuple) + p_attr1_->ref_offset();
            p_rp->offset = p_attr0_->offset();
            for (int i=0; i<table_num; ++i) {
                if (p_attr0_->p_table() == pp_table[i]) {
                    p_rp->idx = i;
                    break;
                }
            }
        }
    }
        
    bool passed (const char* p_tuple)
    {
        return *(const _T*)(p_tuple + p_attr0_->offset())
            == *(_T*)((char*)p_tuple + p_attr1_->offset());
    }

    void to_string(StringWriter& sw) const
    { sw << "Equi(" << *p_attr0_ << "," << *p_attr1_ << ")"; }

private:
    const Attribute<_T>* p_attr0_;
    const Attribute<_T>* p_attr1_;
};

    
template <typename _T>
struct Attribute {
    typedef Attribute<_T> Self;
        
    static Self& create (const void* p_host, const char* desc
                         , int id, int ref_offset, int offset)
    {
        Self* p = new Self;
        p->p_host_ = p_host;
        p->desc_ = desc;
        p->id_ = id;
        p->ref_offset_ = ref_offset;
        p->offset_ = offset;
        return *p;
    }

    const void* p_table() const
    { return p_host_; }
        
    int id() const
    { return id_; }

    int ref_offset() const
    { return ref_offset_; }

    int offset() const
    { return offset_; }

    void to_string(StringWriter& sw) const
    {
        sw.append_format("%s(tbl=%p,T=%s,roff=%d,off=%d)"
                         , desc_.c_str(), p_host_, typeid(_T).name()
                         , ref_offset_, offset_);
    }

    Equi<Self,Self>& operator== (Self& other)
    { return Equi<Self,Self>::create(*this, other); }
        
    Equi<Self,_T>& operator== (const _T& value)
    { return Equi<Self,_T>::create(*this, value); }

    Equi<Self,_T*>& operator== (const _T* p_value)
    { return Equi<Self,_T*>::create(*this, p_value); }

    UserPredicate2<LessEqual<_T>,PA,PA>& operator<= (const Self& other) const
    { return UserPredicate2<LessEqual<_T>,PA,PA>::create(*this, other); }

    UserPredicate2<LessEqual<_T>,PA,PC>& operator<= (const _T& other) const
    { return UserPredicate2<LessEqual<_T>,PA,PC>::create(*this, other); }

    UserPredicate2<LessEqual<_T>,PA,PP>& operator<= (const _T* other) const
    { return UserPredicate2<LessEqual<_T>,PA,PP>::create(*this, other); }
        
    UserPredicate2<LessThan<_T>,PA,PA>& operator< (const Self& other) const
    { return UserPredicate2<LessThan<_T>,PA,PA>::create(*this, other); }

    UserPredicate2<LessThan<_T>,PA,PC>& operator< (const _T& other) const
    { return UserPredicate2<LessThan<_T>,PA,PC>::create(*this, other); }

    UserPredicate2<LessThan<_T>,PA,PP>& operator< (const _T* other) const
    { return UserPredicate2<LessThan<_T>,PA,PP>::create(*this, other); }

    UserPredicate2<GreaterEqual<_T>,PA,PA>& operator>= (const Self& other) const
    { return UserPredicate2<GreaterEqual<_T>,PA,PA>::create(*this, other); }

    UserPredicate2<GreaterEqual<_T>,PA,PC>& operator>= (const _T& other) const
    { return UserPredicate2<GreaterEqual<_T>,PA,PC>::create(*this, other); }

    UserPredicate2<GreaterEqual<_T>,PA,PP>& operator>= (const _T* other) const
    { return UserPredicate2<GreaterEqual<_T>,PA,PP>::create(*this, other); }
        
    UserPredicate2<GreaterThan<_T>,PA,PA>& operator> (const Self& other) const
    { return UserPredicate2<GreaterThan<_T>,PA,PA>::create(*this, other); }

    UserPredicate2<GreaterThan<_T>,PA,PC>& operator> (const _T& other) const
    { return UserPredicate2<GreaterThan<_T>,PA,PC>::create(*this, other); }

    UserPredicate2<GreaterThan<_T>,PA,PP>& operator> (const _T* other) const
    { return UserPredicate2<GreaterThan<_T>,PA,PP>::create(*this, other); }

    UserPredicate2<NotEqual<_T>,PA,PA>& operator!= (const Self& other) const
    { return UserPredicate2<NotEqual<_T>,PA,PA>::create(*this, other); }

    UserPredicate2<NotEqual<_T>,PA,PC>& operator!= (const _T& other) const
    { return UserPredicate2<NotEqual<_T>,PA,PC>::create(*this, other); }

    UserPredicate2<NotEqual<_T>,PA,PP>& operator!= (const _T* other) const
    { return UserPredicate2<NotEqual<_T>,PA,PP>::create(*this, other); }
        
private:
    Attribute () {}
    const void* p_host_;
    std::string desc_;
    int id_;
    int ref_offset_;
    int offset_;
};
         
}

#endif  // _PREDICATE_HPP_
