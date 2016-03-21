// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// (smalltable1) an implementation of tuple in C++ which supports variadic
// number of types and user-assigned column number and maximum 10 columns
// Note: this class is replaced by basic_tuple.hpp and named_tuple.hpp in
//       smalltable2
// Author: gejun@baidu.com
// Date: Tue Aug 17 20:51:36 CST 2010
#pragma once
#ifndef _TUPLE_HPP_
#define _TUPLE_HPP_

#include "string_writer.hpp"
#include "st_hash.h"
#include "c_map.hpp"
#include "compare.hpp"
#include "c_common.hpp"
#include "object_hanger.h"
#include "attribute.h"

// if this macro is undefined, to_string shows tuples in form of
// (COLUMN0=VALUE0, COLUMN1=VALUE1, ...)
// otherwise (VALUE0, VALUE1, ...)
#define DONT_SHOW_ATTRIBUTE_NAME

namespace st {

const int NULL_COLUMN = -1;
const int MAX_COLUMN_NUM = 10;


// de_a extracts column_type/data_type/name()/id() from
// either column_type by DEFINE_COLUMN
// or column_type(data_type) which replaces the internal data type with the one in parenthesis
// DEFINE_COLUMN(UNIT_ID, int);
// de_a<UNIT_ID> or de_a<UNIT_ID(short)>
template <typename _T0> struct de_a;

template <typename _T0, typename _T1> struct DeaPair {};


template <typename _T0>
struct de_a : public _T0 {
    typedef _T0 Tag;
    typedef typename _T0::Type Value;
};

template <typename _T0, typename _T1>
struct de_a<_T0(_T1)> : public _T0 {
    typedef _T0 Tag;
    typedef _T1 Value;
};


template <typename _T0, typename _T1>
struct de_a<DeaPair<_T0,_T1> > : public _T0 {
    typedef _T0 Tag;
    typedef _T1 Value;
};    
    
template <> struct de_a<void> {
    typedef void Tag;
    typedef void Value;

    static const char* name() { return "void"; }
    static int id() { return hash("void"); }
};


// BasicTuple[1-10] are internal tuple types with fixed count of columns
// These fixed count tuples are double-edged sword: we define functions
// more easily and flexibly (otherwise we have to play with recursive templates
// and heavily rely on inlining), but there're a lot of seeming repetitions.
template <typename _T0> struct BasicTuple1 {
    typedef Cons<_T0,void> ColumnL;
    
    typedef BasicTuple1<_T0> Self;

    typedef typename de_a<_T0>::Value V0;

    explicit BasicTuple1 ()
    {}
        
    explicit BasicTuple1(const V0& v0)
        : v0_(v0)
    {}

    BasicTuple1(const BasicTuple1& other)
        : v0_(other.v0_)
    {}
        
    bool operator== (const Self& other) const
    { return v0_ == other.v0_; }

    bool operator!= (const Self& other) const
    { return v0_ != other.v0_; }
        
friend int valcmp (const Self& t1, const Self& t2)
    { return valcmp(t1.v0_, t2.v0_); }

friend bool operator< (const Self& t1, const Self& t2)
    { return valcmp(t1,t2) < 0; }

friend size_t hash(const Self& t)
    { return hash(t.v0_); }

    template <typename Func>
    typename Func::Result apply (Func f) const
    { return f (v0_); }

    void to_string(StringWriter& sw) const {
#ifndef DONT_SHOW_ATTRIBUTE_NAME
        sw << "(" << de_a<_T0>::name() << "=" << v0_
           << ")";
#else
        sw << "(" << v0_
           << ")";
#endif            
    }
        
    V0 v0_;
};

template <typename _T0, typename _T1>
struct BasicTuple2 {
    typedef Cons<_T0,Cons<_T1,void> > ColumnL;
    typedef BasicTuple2<_T0,_T1> Self;
    typedef typename de_a<_T0>::Value V0;
    typedef typename de_a<_T1>::Value V1;
        
    explicit BasicTuple2 ()
    {}
        
    explicit BasicTuple2(const V0& v0, const V1& v1)
        : v0_(v0), v1_(v1)
    {}

    BasicTuple2(const BasicTuple2& other)
        : v0_(other.v0_), v1_(other.v1_)
    {}
     
    bool operator== (const Self& other) const
    { return v0_ == other.v0_ && v1_ == other.v1_; }
        
    bool operator!= (const Self& other) const
    { return v0_ != other.v0_ || v1_ != other.v1_; }
        
friend int valcmp (const Self& t1, const Self& t2)
    {
        int ret = 0;
        return (ret=valcmp(t1.v0_,t2.v0_))
            ? ret : (ret=valcmp(t1.v1_,t2.v1_))
            ;
    }

friend bool operator< (const Self& t1, const Self& t2)
    { return valcmp(t1,t2) < 0; }
        
friend size_t hash(const Self& t)
    {
        size_t v = hash(t.v0_);
        v = (v << 8) + v + hash(t.v1_);
        return v;
    }
        
    template <typename Func> typename Func::Result apply (Func f) const
    { return f (v0_, v1_); }

    void to_string(StringWriter& sw) const
    {
#ifndef DONT_SHOW_ATTRIBUTE_NAME
        sw << "(" << de_a<_T0>::name() << "=" << v0_
           << "," << de_a<_T1>::name() << "=" << v1_
           << ")";
#else
        sw << "(" << v0_
           << "," << v1_
           << ")";
#endif
    }

    V0 v0_;
    V1 v1_;
};

template <typename _T0, typename _T1, typename _T2>
struct BasicTuple3 {
    typedef Cons<_T0,Cons<_T1,Cons<_T2,void> > > ColumnL;
    typedef BasicTuple3<_T0,_T1,_T2> Self;
    typedef typename de_a<_T0>::Value V0;
    typedef typename de_a<_T1>::Value V1;
    typedef typename de_a<_T2>::Value V2;

    explicit BasicTuple3 ()
    {}
        
    explicit BasicTuple3(const V0& v0, const V1& v1, const V2& v2)
        : v0_(v0)
        , v1_(v1)
        , v2_(v2)
    {}

    BasicTuple3(const BasicTuple3& other)
        : v0_(other.v0_), v1_(other.v1_), v2_(other.v2_)
    {}
    
    bool operator== (const Self& other) const
    {
        return v0_ == other.v0_
            && v1_ == other.v1_
            && v2_ == other.v2_;
    }
        
    bool operator!= (const Self& other) const
    {
        return v0_ != other.v0_
            || v1_ != other.v1_
            || v2_ != other.v2_;
    }
        
friend int valcmp (const Self& t1, const Self& t2)
    {
        int ret = 0;
        return (ret=valcmp(t1.v0_,t2.v0_))
            ? ret : (ret=valcmp(t1.v1_,t2.v1_))
            ? ret : (ret=valcmp(t1.v2_,t2.v2_))
            ;
        return ret;

    }

friend bool operator< (const Self& t1, const Self& t2)
    { return valcmp(t1,t2) < 0; }

friend size_t hash(const Self& t)
    {
        size_t v = hash(t.v0_);
        v = (v << 8) + v + hash(t.v1_);
        v = (v << 8) + v + hash(t.v2_);
        return v;
    }

    template <typename Func>
    typename Func::Result apply (Func f) const
    { return f (v0_, v1_, v2_); }
        
    void to_string(StringWriter& sw) const
    {
#ifndef DONT_SHOW_ATTRIBUTE_NAME
        sw << "(" << de_a<_T0>::name() << "=" << v0_
           << "," << de_a<_T1>::name() << "=" << v1_
           << "," << de_a<_T2>::name() << "=" << v2_
           << ")";
#else
        sw << "(" << v0_
           << "," << v1_
           << "," << v2_
           << ")";
#endif
    }

    V0 v0_;
    V1 v1_;
    V2 v2_;
};

template <typename _T0, typename _T1, typename _T2, typename _T3>
struct BasicTuple4 {
    typedef Cons<_T0,Cons<_T1,Cons<_T2,Cons<_T3,void> > > > ColumnL;
    typedef BasicTuple4<_T0,_T1,_T2,_T3> Self;
    typedef typename de_a<_T0>::Value V0;
    typedef typename de_a<_T1>::Value V1;
    typedef typename de_a<_T2>::Value V2;
    typedef typename de_a<_T3>::Value V3;

    explicit BasicTuple4 () {}
        
    explicit BasicTuple4(const V0& v0, const V1& v1, const V2& v2, const V3& v3)
        : v0_(v0)
        , v1_(v1)
        , v2_(v2)
        , v3_(v3)
    {}

    bool operator== (const Self& other) const
    {
        return v0_ == other.v0_
            && v1_ == other.v1_
            && v2_ == other.v2_
            && v3_ == other.v3_
            ;
    }

    bool operator!= (const Self& other) const
    {
        return v0_ != other.v0_
            || v1_ != other.v1_
            || v2_ != other.v2_
            || v3_ != other.v3_
            ;
    }
        
friend int valcmp (const Self& t1, const Self& t2)
    {
        int ret = 0;
        return (ret=valcmp(t1.v0_,t2.v0_))
            ? ret : (ret=valcmp(t1.v1_,t2.v1_))
            ? ret : (ret=valcmp(t1.v2_,t2.v2_))
            ? ret : (ret=valcmp(t1.v3_,t2.v3_))
            ;
    }

friend bool operator< (const Self& t1, const Self& t2)
    { return valcmp(t1,t2) < 0; }
        
friend size_t hash(const Self& t)
    {
        size_t v = hash(t.v0_);
        v = (v << 8) + v + hash(t.v1_);
        v = (v << 8) + v + hash(t.v2_);
        v = (v << 8) + v + hash(t.v3_);
        return v;
    }

    template <typename Func>
    typename Func::Result apply (Func f) const
    { return f (v0_, v1_, v2_, v3_); }
        
    void to_string(StringWriter& sw) const
    {
#ifndef DONT_SHOW_ATTRIBUTE_NAME
        sw << "(" << de_a<_T0>::name() << "=" << v0_
           << "," << de_a<_T1>::name() << "=" << v1_
           << "," << de_a<_T2>::name() << "=" << v2_
           << "," << de_a<_T3>::name() << "=" << v3_
           << ")";
#else
        sw << "(" << v0_
           << "," << v1_
           << "," << v2_
           << "," << v3_
           << ")";
#endif            
    }

    V0 v0_;
    V1 v1_;
    V2 v2_;
    V3 v3_;
};

    
template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4>
struct BasicTuple5 {
    typedef Cons<_T0,Cons<_T1,Cons<_T2,Cons<_T3,Cons<_T4,void> > > > > ColumnL;
    typedef BasicTuple5<_T0,_T1,_T2,_T3,_T4> Self;
    typedef typename de_a<_T0>::Value V0;
    typedef typename de_a<_T1>::Value V1;
    typedef typename de_a<_T2>::Value V2;
    typedef typename de_a<_T3>::Value V3;
    typedef typename de_a<_T4>::Value V4;

    explicit BasicTuple5 () {}
        
    explicit BasicTuple5(const V0& v0, const V1& v1, const V2& v2, const V3& v3, const V4& v4)
        : v0_(v0)
        , v1_(v1)
        , v2_(v2)
        , v3_(v3)
        , v4_(v4)
    {}
        
    bool operator== (const Self& other) const
    {
        return v0_ == other.v0_
            && v1_ == other.v1_
            && v2_ == other.v2_
            && v3_ == other.v3_
            && v4_ == other.v4_
            ;
    }

    bool operator!= (const Self& other) const
    {
        return v0_ != other.v0_
            || v1_ != other.v1_
            || v2_ != other.v2_
            || v3_ != other.v3_
            || v4_ != other.v4_
            ;
    }
        
friend int valcmp (const Self& t1, const Self& t2)
    {
        int ret = 0;
        return (ret=valcmp(t1.v0_,t2.v0_))
            ? ret : (ret=valcmp(t1.v1_,t2.v1_))
            ? ret : (ret=valcmp(t1.v2_,t2.v2_))
            ? ret : (ret=valcmp(t1.v3_,t2.v3_))
            ? ret : (ret=valcmp(t1.v4_,t2.v4_))
            ;
    }

friend bool operator< (const Self& t1, const Self& t2)
    { return valcmp(t1,t2) < 0; }
        
friend size_t hash(const Self& t)
    {
        size_t v = hash(t.v0_);
        v = (v << 8) + v + hash(t.v1_);
        v = (v << 8) + v + hash(t.v2_);
        v = (v << 8) + v + hash(t.v3_);
        v = (v << 8) + v + hash(t.v4_);
        return v;
    }

    template <typename Func>
    typename Func::Result apply (Func f) const
    { return f (v0_, v1_, v2_, v3_, v4_); }

    void to_string(StringWriter& sw) const
    {
#ifndef DONT_SHOW_ATTRIBUTE_NAME
        sw << "(" << de_a<_T0>::name() << "=" << v0_
           << "," << de_a<_T1>::name() << "=" << v1_
           << "," << de_a<_T2>::name() << "=" << v2_
           << "," << de_a<_T3>::name() << "=" << v3_
           << "," << de_a<_T4>::name() << "=" << v4_
           << ")";
#else
        sw << "(" << v0_
           << "," << v1_
           << "," << v2_
           << "," << v3_
           << "," << v4_
           << ")";
#endif            
    }

    V0 v0_;
    V1 v1_;
    V2 v2_;
    V3 v3_;
    V4 v4_;
};

    
template <typename _T0, typename _T1, typename _T2, typename _T3,
          typename _T4, typename _T5>
struct BasicTuple6 {
    typedef Cons<_T0,Cons<_T1,Cons<_T2,Cons<_T3,Cons<_T4,Cons<_T5,void> > > > > > ColumnL;
    typedef BasicTuple6<_T0,_T1,_T2,_T3,_T4,_T5> Self;
    typedef typename de_a<_T0>::Value V0;
    typedef typename de_a<_T1>::Value V1;
    typedef typename de_a<_T2>::Value V2;
    typedef typename de_a<_T3>::Value V3;
    typedef typename de_a<_T4>::Value V4;
    typedef typename de_a<_T5>::Value V5;

    explicit BasicTuple6 () {}
        
    explicit BasicTuple6(const V0& v0, const V1& v1, const V2& v2,
                         const V3& v3, const V4& v4, const V5& v5)
        : v0_(v0)
        , v1_(v1)
        , v2_(v2)
        , v3_(v3)
        , v4_(v4)
        , v5_(v5)
    {}

    bool operator== (const Self& other) const
    {
        return v0_ == other.v0_
            && v1_ == other.v1_
            && v2_ == other.v2_
            && v3_ == other.v3_
            && v4_ == other.v4_
            && v5_ == other.v5_
            ;
    }

    bool operator!= (const Self& other) const
    {
        return v0_ != other.v0_
            || v1_ != other.v1_
            || v2_ != other.v2_
            || v3_ != other.v3_
            || v4_ != other.v4_
            || v5_ != other.v5_
            ;
    }
        
friend int valcmp (const Self& t1, const Self& t2)
    {
        int ret = 0;
        return (ret=valcmp(t1.v0_,t2.v0_))
            ? ret : (ret=valcmp(t1.v1_,t2.v1_))
            ? ret : (ret=valcmp(t1.v2_,t2.v2_))
            ? ret : (ret=valcmp(t1.v3_,t2.v3_))
            ? ret : (ret=valcmp(t1.v4_,t2.v4_))
            ? ret : (ret=valcmp(t1.v5_,t2.v5_))
            ;
    }

friend bool operator< (const Self& t1, const Self& t2)
    { return valcmp(t1,t2) < 0; }
        
friend size_t hash(const Self& t)
    {
        size_t v = hash(t.v0_);
        v = (v << 8) + v + hash(t.v1_);
        v = (v << 8) + v + hash(t.v2_);
        v = (v << 8) + v + hash(t.v3_);
        v = (v << 8) + v + hash(t.v4_);
        v = (v << 8) + v + hash(t.v5_);
        return v;
    }

    template <typename Func>
    typename Func::Result apply (Func f) const
    { return f (v0_, v1_, v2_, v3_, v4_, v5_); }
        
    void to_string(StringWriter& sw) const
    {
#ifndef DONT_SHOW_ATTRIBUTE_NAME
        sw << "(" << de_a<_T0>::name() << "=" << v0_
           << "," << de_a<_T1>::name() << "=" << v1_
           << "," << de_a<_T2>::name() << "=" << v2_
           << "," << de_a<_T3>::name() << "=" << v3_
           << "," << de_a<_T4>::name() << "=" << v4_
           << "," << de_a<_T5>::name() << "=" << v5_
           << ")";
#else
        sw << "(" << v0_
           << "," << v1_
           << "," << v2_
           << "," << v3_
           << "," << v4_
           << "," << v5_
           << ")";
#endif
    }

    V0 v0_;
    V1 v1_;
    V2 v2_;
    V3 v3_;
    V4 v4_;
    V5 v5_;
};

    
template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4, typename _T5, typename _T6>
struct BasicTuple7 {
    typedef Cons<_T0,Cons<_T1,Cons<_T2,Cons<_T3,Cons<_T4,Cons<_T5,Cons<_T6,void> > > > > > > ColumnL;
    typedef BasicTuple7<_T0,_T1,_T2,_T3,_T4,_T5,_T6> Self;
    typedef typename de_a<_T0>::Value V0;
    typedef typename de_a<_T1>::Value V1;
    typedef typename de_a<_T2>::Value V2;
    typedef typename de_a<_T3>::Value V3;
    typedef typename de_a<_T4>::Value V4;
    typedef typename de_a<_T5>::Value V5;
    typedef typename de_a<_T6>::Value V6;

    explicit BasicTuple7 ()
    {}
        
    explicit BasicTuple7(const V0& v0, const V1& v1, const V2& v2, const V3& v3, const V4& v4, const V5& v5, const V6& v6)
        : v0_(v0)
        , v1_(v1)
        , v2_(v2)
        , v3_(v3)
        , v4_(v4)
        , v5_(v5)
        , v6_(v6)
    {}

    bool operator== (const Self& other) const
    {
        return v0_ == other.v0_
            && v1_ == other.v1_
            && v2_ == other.v2_
            && v3_ == other.v3_
            && v4_ == other.v4_
            && v5_ == other.v5_
            && v6_ == other.v6_
            ;
    }

    bool operator!= (const Self& other) const
    {
        return v0_ != other.v0_
            || v1_ != other.v1_
            || v2_ != other.v2_
            || v3_ != other.v3_
            || v4_ != other.v4_
            || v5_ != other.v5_
            || v6_ != other.v6_
            ;
    }
        
friend int valcmp (const Self& t1, const Self& t2)
    {
        int ret = 0;
        return (ret=valcmp(t1.v0_,t2.v0_))
            ? ret : (ret=valcmp(t1.v1_,t2.v1_))
            ? ret : (ret=valcmp(t1.v2_,t2.v2_))
            ? ret : (ret=valcmp(t1.v3_,t2.v3_))
            ? ret : (ret=valcmp(t1.v4_,t2.v4_))
            ? ret : (ret=valcmp(t1.v5_,t2.v5_))
            ? ret : (ret=valcmp(t1.v6_,t2.v6_))
            ;
    }

friend bool operator< (const Self& t1, const Self& t2)
    { return valcmp(t1,t2) < 0; }

friend size_t hash(const Self& t)
    {
        size_t v = hash(t.v0_);
        v = (v << 8) + v + hash(t.v1_);
        v = (v << 8) + v + hash(t.v2_);
        v = (v << 8) + v + hash(t.v3_);
        v = (v << 8) + v + hash(t.v4_);
        v = (v << 8) + v + hash(t.v5_);
        v = (v << 8) + v + hash(t.v6_);
        return v;
    }
        
    template <typename Func>
    typename Func::Result apply (Func f) const
    { return f (v0_, v1_, v2_, v3_, v4_, v5_, v6_); }

    void to_string(StringWriter& sw) const {
#ifndef DONT_SHOW_ATTRIBUTE_NAME
        sw << "(" << de_a<_T0>::name() << "=" << v0_
           << "," << de_a<_T1>::name() << "=" << v1_
           << "," << de_a<_T2>::name() << "=" << v2_
           << "," << de_a<_T3>::name() << "=" << v3_
           << "," << de_a<_T4>::name() << "=" << v4_
           << "," << de_a<_T5>::name() << "=" << v5_
           << "," << de_a<_T6>::name() << "=" << v6_
           << ")";
#else
        sw << "(" << v0_
           << "," << v1_
           << "," << v2_
           << "," << v3_
           << "," << v4_
           << "," << v5_
           << "," << v6_
           << ")";            
#endif
    }

    V0 v0_;
    V1 v1_;
    V2 v2_;
    V3 v3_;
    V4 v4_;
    V5 v5_;
    V6 v6_;
};

    
template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4, typename _T5, typename _T6, typename _T7>
struct BasicTuple8 {
    typedef Cons<_T0,Cons<_T1,Cons<_T2,Cons<_T3,Cons<_T4,Cons<_T5,Cons<_T6,Cons<_T7,void> > > > > > > > ColumnL;
    typedef BasicTuple8<_T0,_T1,_T2,_T3,_T4,_T5,_T6,_T7> Self;
    typedef typename de_a<_T0>::Value V0;
    typedef typename de_a<_T1>::Value V1;
    typedef typename de_a<_T2>::Value V2;
    typedef typename de_a<_T3>::Value V3;
    typedef typename de_a<_T4>::Value V4;
    typedef typename de_a<_T5>::Value V5;
    typedef typename de_a<_T6>::Value V6;
    typedef typename de_a<_T7>::Value V7;

    explicit BasicTuple8 ()
    {}
        
    explicit BasicTuple8(const V0& v0, const V1& v1, const V2& v2, const V3& v3, const V4& v4, const V5& v5, const V6& v6, const V7& v7)
        : v0_(v0)
        , v1_(v1)
        , v2_(v2)
        , v3_(v3)
        , v4_(v4)
        , v5_(v5)
        , v6_(v6)
        , v7_(v7)
    {}

    bool operator== (const Self& other) const
    {
        return v0_ == other.v0_
            && v1_ == other.v1_
            && v2_ == other.v2_
            && v3_ == other.v3_
            && v4_ == other.v4_
            && v5_ == other.v5_
            && v6_ == other.v6_
            && v7_ == other.v7_
            ;
    }

    bool operator!= (const Self& other) const
    {
        return v0_ != other.v0_
            || v1_ != other.v1_
            || v2_ != other.v2_
            || v3_ != other.v3_
            || v4_ != other.v4_
            || v5_ != other.v5_
            || v6_ != other.v6_
            || v7_ != other.v7_
            ;
    }
        
friend int valcmp (const Self& t1, const Self& t2)
    {
        int ret = 0;
        return (ret=valcmp(t1.v0_,t2.v0_))
            ? ret : (ret=valcmp(t1.v1_,t2.v1_))
            ? ret : (ret=valcmp(t1.v2_,t2.v2_))
            ? ret : (ret=valcmp(t1.v3_,t2.v3_))
            ? ret : (ret=valcmp(t1.v4_,t2.v4_))
            ? ret : (ret=valcmp(t1.v5_,t2.v5_))
            ? ret : (ret=valcmp(t1.v6_,t2.v6_))
            ? ret : (ret=valcmp(t1.v7_,t2.v7_))
            ;
    }

friend bool operator< (const Self& t1, const Self& t2)
    { return valcmp(t1,t2) < 0; }

friend size_t hash(const Self& t)
    {
        size_t v = hash(t.v0_);
        v = (v << 8) + v + hash(t.v1_);
        v = (v << 8) + v + hash(t.v2_);
        v = (v << 8) + v + hash(t.v3_);
        v = (v << 8) + v + hash(t.v4_);
        v = (v << 8) + v + hash(t.v5_);
        v = (v << 8) + v + hash(t.v6_);
        v = (v << 8) + v + hash(t.v7_);
        return v;
    }

    template <typename Func>
    typename Func::Result apply (Func f) const                                                               
    { return f (v0_, v1_, v2_, v3_, v4_, v5_, v6_, v7_); }
        
    void to_string(StringWriter& sw) const
    {
#ifndef DONT_SHOW_ATTRIBUTE_NAME
        sw << "(" << de_a<_T0>::name() << "=" << v0_
           << "," << de_a<_T1>::name() << "=" << v1_
           << "," << de_a<_T2>::name() << "=" << v2_
           << "," << de_a<_T3>::name() << "=" << v3_
           << "," << de_a<_T4>::name() << "=" << v4_
           << "," << de_a<_T5>::name() << "=" << v5_
           << "," << de_a<_T6>::name() << "=" << v6_
           << "," << de_a<_T7>::name() << "=" << v7_
           << ")";
#else
        sw << "(" << v0_
           << "," << v1_
           << "," << v2_
           << "," << v3_
           << "," << v4_
           << "," << v5_
           << "," << v6_
           << "," << v7_
           << ")";
#endif            
    }

    V0 v0_;
    V1 v1_;
    V2 v2_;
    V3 v3_;
    V4 v4_;
    V5 v5_;
    V6 v6_;
    V7 v7_;
};

    
template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4, typename _T5, typename _T6, typename _T7, typename _T8>
struct BasicTuple9 {
    typedef Cons<_T0,Cons<_T1,Cons<_T2,Cons<_T3,Cons<_T4,Cons<_T5,Cons<_T6,Cons<_T7,Cons<_T8,void> > > > > > > > > ColumnL;
    typedef BasicTuple9<_T0,_T1,_T2,_T3,_T4,_T5,_T6,_T7,_T8> Self;
    typedef typename de_a<_T0>::Value V0;
    typedef typename de_a<_T1>::Value V1;
    typedef typename de_a<_T2>::Value V2;
    typedef typename de_a<_T3>::Value V3;
    typedef typename de_a<_T4>::Value V4;
    typedef typename de_a<_T5>::Value V5;
    typedef typename de_a<_T6>::Value V6;
    typedef typename de_a<_T7>::Value V7;
    typedef typename de_a<_T8>::Value V8;

    explicit BasicTuple9 ()
    {}

    explicit BasicTuple9(const V0& v0, const V1& v1, const V2& v2, const V3& v3, const V4& v4, const V5& v5, const V6& v6, const V7& v7, const V8& v8)
        : v0_(v0)
        , v1_(v1)
        , v2_(v2)
        , v3_(v3)
        , v4_(v4)
        , v5_(v5)
        , v6_(v6)
        , v7_(v7)
        , v8_(v8)
    {}

    bool operator== (const Self& other) const {
        return v0_ == other.v0_
            && v1_ == other.v1_
            && v2_ == other.v2_
            && v3_ == other.v3_
            && v4_ == other.v4_
            && v5_ == other.v5_
            && v6_ == other.v6_
            && v7_ == other.v7_
            && v8_ == other.v8_
            ;
    }

    bool operator!= (const Self& other) const {
        return v0_ != other.v0_
            || v1_ != other.v1_
            || v2_ != other.v2_
            || v3_ != other.v3_
            || v4_ != other.v4_
            || v5_ != other.v5_
            || v6_ != other.v6_
            || v7_ != other.v7_
            || v8_ != other.v8_
            ;
    }
        
friend int valcmp (const Self& t1, const Self& t2) {
    int ret = 0;
    return (ret=valcmp(t1.v0_,t2.v0_))
        ? ret : (ret=valcmp(t1.v1_,t2.v1_))
        ? ret : (ret=valcmp(t1.v2_,t2.v2_))
        ? ret : (ret=valcmp(t1.v3_,t2.v3_))
        ? ret : (ret=valcmp(t1.v4_,t2.v4_))
        ? ret : (ret=valcmp(t1.v5_,t2.v5_))
        ? ret : (ret=valcmp(t1.v6_,t2.v6_))
        ? ret : (ret=valcmp(t1.v7_,t2.v7_))
        ? ret : (ret=valcmp(t1.v8_,t2.v8_))
        ;
}

friend bool operator< (const Self& t1, const Self& t2) {
    return valcmp(t1,t2) < 0;
}
        
friend size_t hash(const Self& t) {
    size_t v = hash(t.v0_);
    v = (v << 8) + v + hash(t.v1_);
    v = (v << 8) + v + hash(t.v2_);
    v = (v << 8) + v + hash(t.v3_);
    v = (v << 8) + v + hash(t.v4_);
    v = (v << 8) + v + hash(t.v5_);
    v = (v << 8) + v + hash(t.v6_);
    v = (v << 8) + v + hash(t.v7_);
    v = (v << 8) + v + hash(t.v8_);
    return v;
}
        
    template <typename Func>
    typename Func::Result apply (Func f) const
    { return f (v0_, v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_); }

    void to_string(StringWriter& sw) const {
#ifndef DONT_SHOW_ATTRIBUTE_NAME
        sw << "(" << de_a<_T0>::name() << "=" << v0_
           << "," << de_a<_T1>::name() << "=" << v1_
           << "," << de_a<_T2>::name() << "=" << v2_
           << "," << de_a<_T3>::name() << "=" << v3_
           << "," << de_a<_T4>::name() << "=" << v4_
           << "," << de_a<_T5>::name() << "=" << v5_
           << "," << de_a<_T6>::name() << "=" << v6_
           << "," << de_a<_T7>::name() << "=" << v7_
           << "," << de_a<_T8>::name() << "=" << v8_
           << ")";
#else
        sw << "(" << v0_
           << "," << v1_
           << "," << v2_
           << "," << v3_
           << "," << v4_
           << "," << v5_
           << "," << v6_
           << "," << v7_
           << "," << v8_
           << ")";
#endif
    }

    V0 v0_;
    V1 v1_;
    V2 v2_;
    V3 v3_;
    V4 v4_;
    V5 v5_;
    V6 v6_;
    V7 v7_;
    V8 v8_;
};


template <typename _T0, typename _T1, typename _T2, typename _T3,
          typename _T4, typename _T5, typename _T6, typename _T7,
          typename _T8, typename _T9>
struct BasicTuple10 {
    typedef Cons<_T0,Cons<_T1,Cons<_T2,Cons<_T3,Cons<_T4,Cons<_T5,Cons<_T6,Cons<_T7,Cons<_T8,Cons<_T9,void> > > > > > > > > > ColumnL;
    typedef BasicTuple10<_T0,_T1,_T2,_T3,_T4,_T5,_T6,_T7,_T8,_T9> Self;
    typedef typename de_a<_T0>::Value V0;
    typedef typename de_a<_T1>::Value V1;
    typedef typename de_a<_T2>::Value V2;
    typedef typename de_a<_T3>::Value V3;
    typedef typename de_a<_T4>::Value V4;
    typedef typename de_a<_T5>::Value V5;
    typedef typename de_a<_T6>::Value V6;
    typedef typename de_a<_T7>::Value V7;
    typedef typename de_a<_T8>::Value V8;
    typedef typename de_a<_T9>::Value V9;

    explicit BasicTuple10 () {}
        
    explicit BasicTuple10(const V0& v0, const V1& v1, const V2& v2, const V3& v3, const V4& v4, const V5& v5, const V6& v6, const V7& v7, const V8& v8, const V9& v9)
        : v0_(v0)
        , v1_(v1)
        , v2_(v2)
        , v3_(v3)
        , v4_(v4)
        , v5_(v5)
        , v6_(v6)
        , v7_(v7)
        , v8_(v8)
        , v9_(v9)
    {}

    bool operator== (const Self& other) const
    {
        return v0_ == other.v0_
            && v1_ == other.v1_
            && v2_ == other.v2_
            && v3_ == other.v3_
            && v4_ == other.v4_
            && v5_ == other.v5_
            && v6_ == other.v6_
            && v7_ == other.v7_
            && v8_ == other.v8_
            && v9_ == other.v9_
            ;
    }

    bool operator!= (const Self& other) const
    {
        return v0_ != other.v0_
            || v1_ != other.v1_
            || v2_ != other.v2_
            || v3_ != other.v3_
            || v4_ != other.v4_
            || v5_ != other.v5_
            || v6_ != other.v6_
            || v7_ != other.v7_
            || v8_ != other.v8_
            || v9_ != other.v9_
            ;
    }
        
friend int valcmp (const Self& t1, const Self& t2)
    {
        int ret = 0;
        return (ret=valcmp(t1.v0_,t2.v0_))
            ? ret : (ret=valcmp(t1.v1_,t2.v1_))
            ? ret : (ret=valcmp(t1.v2_,t2.v2_))
            ? ret : (ret=valcmp(t1.v3_,t2.v3_))
            ? ret : (ret=valcmp(t1.v4_,t2.v4_))
            ? ret : (ret=valcmp(t1.v5_,t2.v5_))
            ? ret : (ret=valcmp(t1.v6_,t2.v6_))
            ? ret : (ret=valcmp(t1.v7_,t2.v7_))
            ? ret : (ret=valcmp(t1.v8_,t2.v8_))
            ? ret : (ret=valcmp(t1.v9_,t2.v9_))
            ;
    }

friend bool operator< (const Self& t1, const Self& t2)
    { return valcmp(t1,t2) < 0; }

friend size_t hash(const Self& t)
    {
        size_t v = hash(t.v0_);
        v = (v << 8) + v + hash(t.v1_);
        v = (v << 8) + v + hash(t.v2_);
        v = (v << 8) + v + hash(t.v3_);
        v = (v << 8) + v + hash(t.v4_);
        v = (v << 8) + v + hash(t.v5_);
        v = (v << 8) + v + hash(t.v6_);
        v = (v << 8) + v + hash(t.v7_);
        v = (v << 8) + v + hash(t.v8_);
        v = (v << 8) + v + hash(t.v9_);
        return v;
    }
        
    template <typename Func>
    typename Func::Result apply (Func f) const
    { return f (v0_, v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_); }

    void to_string(StringWriter& sw) const {
#ifndef DONT_SHOW_ATTRIBUTE_NAME
        sw << "(" << de_a<_T0>::name() << "=" << v0_
           << "," << de_a<_T1>::name() << "=" << v1_
           << "," << de_a<_T2>::name() << "=" << v2_
           << "," << de_a<_T3>::name() << "=" << v3_
           << "," << de_a<_T4>::name() << "=" << v4_
           << "," << de_a<_T5>::name() << "=" << v5_
           << "," << de_a<_T6>::name() << "=" << v6_
           << "," << de_a<_T7>::name() << "=" << v7_
           << "," << de_a<_T8>::name() << "=" << v8_
           << "," << de_a<_T9>::name() << "=" << v9_
           << ")";
#else
        sw << "(" << v0_
           << "," << v1_
           << "," << v2_
           << "," << v3_
           << "," << v4_
           << "," << v5_
           << "," << v6_
           << "," << v7_
           << "," << v8_
           << "," << v9_
           << ")";
#endif
    }

    V0 v0_;
    V1 v1_;
    V2 v2_;
    V3 v3_;
    V4 v4_;
    V5 v5_;
    V6 v6_;
    V7 v7_;
    V8 v8_;
    V9 v9_;
};
    
    
// \brief SortNull groups void to the back side
template <typename _T0=void
          , typename _T1=void
          , typename _T2=void
          , typename _T3=void
          , typename _T4=void
          , typename _T5=void
          , typename _T6=void
          , typename _T7=void
          , typename _T8=void
          , typename _T9=void
          >
struct SortNull {
    enum { null__T0 = c_same<_T0,void>::R };
    typedef SortNull<_T1,_T2,_T3,_T4,_T5,_T6,_T7,_T8,_T9> NextType;
    typedef typename if_<null__T0, typename NextType::Type0, _T0>::R Type0;
    typedef typename if_<null__T0, typename NextType::Type1, typename NextType::Type0>::R Type1;
    typedef typename if_<null__T0, typename NextType::Type2, typename NextType::Type1>::R Type2;
    typedef typename if_<null__T0, typename NextType::Type3, typename NextType::Type2>::R Type3;
    typedef typename if_<null__T0, typename NextType::Type4, typename NextType::Type3>::R Type4;
    typedef typename if_<null__T0, typename NextType::Type5, typename NextType::Type4>::R Type5;
    typedef typename if_<null__T0, typename NextType::Type6, typename NextType::Type5>::R Type6;
    typedef typename if_<null__T0, typename NextType::Type7, typename NextType::Type6>::R Type7;
    typedef typename if_<null__T0, typename NextType::Type8, typename NextType::Type7>::R Type8;
    typedef typename if_<null__T0, void, typename NextType::Type8>::R Type9;

};
    
template <>
struct SortNull<> {
    typedef void Type0;
    typedef void Type1;
    typedef void Type2;
    typedef void Type3;
    typedef void Type4;
    typedef void Type5;
    typedef void Type6;
    typedef void Type7;
    typedef void Type8;
    typedef void Type9;
};

// \brief CutNull removes void and redirects to concrete type
template <typename _T0=void
          , typename _T1=void
          , typename _T2=void
          , typename _T3=void
          , typename _T4=void
          , typename _T5=void
          , typename _T6=void
          , typename _T7=void
          , typename _T8=void
          , typename _T9=void
          >
struct CutNull;

template <typename _T0>
struct CutNull<_T0> {
    typedef BasicTuple1<_T0> Type;
};

template <typename _T0, typename _T1>
struct CutNull<_T0,_T1> {
    typedef BasicTuple2<_T0,_T1> Type;
};

template <typename _T0, typename _T1, typename _T2>
struct CutNull<_T0,_T1,_T2> {
    typedef BasicTuple3<_T0,_T1,_T2> Type;
};    

template <typename _T0, typename _T1, typename _T2, typename _T3>
struct CutNull<_T0,_T1,_T2,_T3> {
    typedef BasicTuple4<_T0,_T1,_T2,_T3> Type;
};    

template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4>
struct CutNull<_T0,_T1,_T2,_T3,_T4> {
    typedef BasicTuple5<_T0,_T1,_T2,_T3,_T4> Type;
};    

template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4, typename _T5>
struct CutNull<_T0,_T1,_T2,_T3,_T4,_T5> {
    typedef BasicTuple6<_T0,_T1,_T2,_T3,_T4,_T5> Type;
};    

template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4, typename _T5, typename _T6>
struct CutNull<_T0,_T1,_T2,_T3,_T4,_T5,_T6> {
    typedef BasicTuple7<_T0,_T1,_T2,_T3,_T4,_T5,_T6> Type;
};    

template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4, typename _T5, typename _T6, typename _T7>
struct CutNull<_T0,_T1,_T2,_T3,_T4,_T5,_T6,_T7> {
    typedef BasicTuple8<_T0,_T1,_T2,_T3,_T4,_T5,_T6,_T7> Type;
};    
    
template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4, typename _T5, typename _T6, typename _T7, typename _T8>
struct CutNull<_T0,_T1,_T2,_T3,_T4,_T5,_T6,_T7,_T8> {
    typedef BasicTuple9<_T0,_T1,_T2,_T3,_T4,_T5,_T6,_T7,_T8> Type;
};    

template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4, typename _T5, typename _T6, typename _T7, typename _T8, typename _T9>
struct CutNull {
    typedef BasicTuple10<_T0,_T1,_T2,_T3,_T4,_T5,_T6,_T7,_T8,_T9> Type;
};    

// \brief the tuple class
template <typename _T0
          , typename _T1=void
          , typename _T2=void
          , typename _T3=void
          , typename _T4=void
          , typename _T5=void
          , typename _T6=void
          , typename _T7=void
          , typename _T8=void
          , typename _T9=void
          >
struct tuple_t {
    typedef _T0 Param0;
    typedef _T1 Param1;
    typedef _T2 Param2;
    typedef _T3 Param3;
    typedef _T4 Param4;
    typedef _T5 Param5;
    typedef _T6 Param6;
    typedef _T7 Param7;
    typedef _T8 Param8;
    typedef _T9 Param9;

    typedef tuple_t<  typename if_<c_void<_T0>::R,void,DeaPair<_T0,ObjectHanger::Position> >::R
                      , typename if_<c_void<_T1>::R,void,DeaPair<_T1,ObjectHanger::Position> >::R
                      , typename if_<c_void<_T2>::R,void,DeaPair<_T2,ObjectHanger::Position> >::R
                      , typename if_<c_void<_T3>::R,void,DeaPair<_T3,ObjectHanger::Position> >::R
                      , typename if_<c_void<_T4>::R,void,DeaPair<_T4,ObjectHanger::Position> >::R
                      , typename if_<c_void<_T5>::R,void,DeaPair<_T5,ObjectHanger::Position> >::R
                      , typename if_<c_void<_T6>::R,void,DeaPair<_T6,ObjectHanger::Position> >::R
                      , typename if_<c_void<_T7>::R,void,DeaPair<_T7,ObjectHanger::Position> >::R
                      , typename if_<c_void<_T8>::R,void,DeaPair<_T8,ObjectHanger::Position> >::R
                      , typename if_<c_void<_T9>::R,void,DeaPair<_T9,ObjectHanger::Position> >::R
                      > RefTuple;

    typedef tuple_t<_T0,_T1,_T2,_T3,_T4,_T5,_T6,_T7,_T8,_T9> Self;
    typedef SortNull<_T0,_T1,_T2,_T3,_T4,_T5,_T6,_T7,_T8,_T9> Repacked;
    typedef typename CutNull<typename Repacked::Type0
                             , typename Repacked::Type1
                             , typename Repacked::Type2
                             , typename Repacked::Type3
                             , typename Repacked::Type4
                             , typename Repacked::Type5
                             , typename Repacked::Type6
                             , typename Repacked::Type7
                             , typename Repacked::Type8
                             , typename Repacked::Type9
                             >::Type Type;
    typedef typename Type::ColumnL ColumnL;
    Type data_;


    // \brief DirectField converts compile-time number to attribute type, offset
    template <int N, typename Dummy=void>
    struct DirectField;

    template <typename Dummy>
    struct DirectField<0,Dummy> {
        enum {OFFSET=offsetof(Type, v0_)};
        typedef typename Type::V0 Value;
    };
    template <typename Dummy>
    struct DirectField<1,Dummy> {
        enum {OFFSET=offsetof(Type, v1_)};
        typedef typename Type::V1 Value;
    };
    template <typename Dummy>
    struct DirectField<2,Dummy> {
        enum {OFFSET=offsetof(Type, v2_)};
        typedef typename Type::V2 Value;
    };
    template <typename Dummy>
    struct DirectField<3,Dummy> {
        enum {OFFSET=offsetof(Type, v3_)};
        typedef typename Type::V3 Value;
    };
    template <typename Dummy>
    struct DirectField<4,Dummy> {
        enum {OFFSET=offsetof(Type, v4_)};
        typedef typename Type::V4 Value;
    };
    template <typename Dummy>
    struct DirectField<5,Dummy> {
        enum {OFFSET=offsetof(Type, v5_)};
        typedef typename Type::V5 Value;
    };
    template <typename Dummy>
    struct DirectField<6,Dummy> {
        enum {OFFSET=offsetof(Type, v6_)};
        typedef typename Type::V6 Value;
    };
    template <typename Dummy>
    struct DirectField<7,Dummy> {
        enum {OFFSET=offsetof(Type, v7_)};
        typedef typename Type::V7 Value;
    };
    template <typename Dummy>
    struct DirectField<8,Dummy> {
        enum {OFFSET=offsetof(Type, v8_)};
        typedef typename Type::V8 Value;
    };
    template <typename Dummy>
    struct DirectField<9,Dummy> {
        enum {OFFSET=offsetof(Type, v9_)};
        typedef typename Type::V9 Value;
    };

        
    // \brief Field converts Tag to attribute type, offset
    template <typename T>
    struct Field {
        typedef typename if_<
            c_same<T,void>::R
            , void
            , typename if_<
                  c_same<T,typename de_a<_T0>::Tag>::R
                  , _T0
                  , typename if_<
                        c_same<T,typename de_a<_T1>::Tag>::R
                        , _T1
                        , typename if_<
                              c_same<T,typename de_a<_T2>::Tag>::R
                              , _T2
                              , typename if_<
                                    c_same<T,typename de_a<_T3>::Tag>::R
                                    , _T3
                                    , typename if_<
                                          c_same<T,typename de_a<_T4>::Tag>::R
                                          , _T4
                                          , typename if_<
                                                c_same<T,typename de_a<_T5>::Tag>::R
                                                , _T5
                                                , typename if_<
                                                      c_same<T,typename de_a<_T6>::Tag>::R
                                                      , _T6
                                                      , typename if_<
                                                            c_same<T,typename de_a<_T7>::Tag>::R
                                                            , _T7
                                                            , typename if_<
                                                                  c_same<T,typename de_a<_T8>::Tag>::R
                                                                  , _T8
                                                                  , typename if_<
                                                                        c_same<T,typename de_a<_T9>::Tag>::R
                                                                        , _T9
                                                                        , void
                                                                        >::R
                                                                  >::R
                                                            >::R
                                                      >::R
                                                >::R
                                          >::R
                                    >::R
                              >::R
                        >::R
                  >::R
            >::R
        Type;
            
        enum {IDX=c_void<T>::R ? NULL_COLUMN
              : c_same<T,typename de_a<typename Repacked::Type0>::Tag>::R ? 0
              : c_same<T,typename de_a<typename Repacked::Type1>::Tag>::R ? 1
              : c_same<T,typename de_a<typename Repacked::Type2>::Tag>::R ? 2
              : c_same<T,typename de_a<typename Repacked::Type3>::Tag>::R ? 3
              : c_same<T,typename de_a<typename Repacked::Type4>::Tag>::R ? 4
              : c_same<T,typename de_a<typename Repacked::Type5>::Tag>::R ? 5
              : c_same<T,typename de_a<typename Repacked::Type6>::Tag>::R ? 6
              : c_same<T,typename de_a<typename Repacked::Type7>::Tag>::R ? 7
              : c_same<T,typename de_a<typename Repacked::Type8>::Tag>::R ? 8
              : c_same<T,typename de_a<typename Repacked::Type9>::Tag>::R ? 9
              : NULL_COLUMN
                  
              , OFFSET=DirectField<IDX>::OFFSET
        };
            
        typedef typename de_a<Type>::Value Value;
    };


    // \brief SubFunc gets sub_tuple
    template <typename _S0
              , typename _S1=void
              , typename _S2=void
              , typename _S3=void
              , typename _S4=void
              , typename _S5=void
              , typename _S6=void
              , typename _S7=void
              , typename _S8=void
              , typename _S9=void>
    struct SubFunc;

    template <typename _S0>
    struct SubFunc<_S0> : public base_function_object<tuple_t<typename Field<_S0>::Type>(Self) > {
        typedef Self FullTuple;
        typedef tuple_t <
            typename Field<_S0>::Type
            > Tuple;
        Tuple operator() (const Self& t) const
        { return Tuple (t.at<_S0>()); }

        Tuple operator() (const ObjectHanger& oh, const RefTuple& t) const
        { return Tuple(*(const typename Field<_S0>::Value*)oh.field(t.at<_S0>())); }
    };

    template <typename _S0, typename _S1>
    struct SubFunc<_S0,_S1>
        : public base_function_object<tuple_t<typename Field<_S0>::Type
                                              , typename Field<_S1>::Type
                                              >(Self) > {
        typedef Self FullTuple;
        typedef tuple_t <
            typename Field<_S0>::Type
            , typename Field<_S1>::Type
            > Tuple;
        Tuple operator() (const Self& t) const {
            return Tuple (t.at<_S0>()
                          , t.at<_S1>()
                );
        }
        Tuple operator() (const ObjectHanger& oh, const RefTuple& t) const
        {
            return Tuple(*(const typename Field<_S0>::Value*)oh.field(t.at<_S0>())
                         , *(const typename Field<_S1>::Value*)oh.field(t.at<_S1>())
                );
        }
    };

    template <typename _S0, typename _S1, typename _S2>
    struct SubFunc<_S0,_S1,_S2>
        : public base_function_object<tuple_t<typename Field<_S0>::Type
                                              , typename Field<_S1>::Type
                                              , typename Field<_S2>::Type
                                              >(Self) > {
        typedef Self FullTuple;
        typedef tuple_t <
            typename Field<_S0>::Type
            , typename Field<_S1>::Type
            , typename Field<_S2>::Type
            > Tuple;
        Tuple operator() (const Self& t) const {
            return Tuple (t.at<_S0>()
                          , t.at<_S1>()
                          , t.at<_S2>()
                );
        }
        Tuple operator() (const ObjectHanger& oh, const RefTuple& t) const
        {
            return Tuple(*(const typename Field<_S0>::Value*)oh.field(t.at<_S0>())
                         , *(const typename Field<_S1>::Value*)oh.field(t.at<_S1>())
                         , *(const typename Field<_S2>::Value*)oh.field(t.at<_S2>())
                );
        }
    };        

    template <typename _S0, typename _S1, typename _S2, typename _S3>
    struct SubFunc<_S0,_S1,_S2,_S3> {
        typedef Self FullTuple;
        typedef tuple_t <
            typename Field<_S0>::Type
            , typename Field<_S1>::Type
            , typename Field<_S2>::Type
            , typename Field<_S3>::Type
            > Tuple;
        Tuple operator() (const Self& t) const {
            return Tuple (t.at<_S0>()
                          , t.at<_S1>()
                          , t.at<_S2>()
                          , t.at<_S3>()
                );
        }
        Tuple operator() (const ObjectHanger& oh, const RefTuple& t) const
        {
            return Tuple(*(const typename Field<_S0>::Value*)oh.field(t.at<_S0>())
                         , *(const typename Field<_S1>::Value*)oh.field(t.at<_S1>())
                         , *(const typename Field<_S2>::Value*)oh.field(t.at<_S2>())
                         , *(const typename Field<_S3>::Value*)oh.field(t.at<_S3>())
                );
        }
    };

    template <typename _S0, typename _S1, typename _S2, typename _S3, typename _S4>
    struct SubFunc<_S0,_S1,_S2,_S3,_S4> {
        typedef Self FullTuple;
        typedef tuple_t <
            typename Field<_S0>::Type
            , typename Field<_S1>::Type
            , typename Field<_S2>::Type
            , typename Field<_S3>::Type
            , typename Field<_S4>::Type
            > Tuple;
        Tuple operator() (const Self& t) const {
            return Tuple (t.at<_S0>()
                          , t.at<_S1>()
                          , t.at<_S2>()
                          , t.at<_S3>()
                          , t.at<_S4>()
                );
        }
    };        
        
    template <typename _S0, typename _S1, typename _S2, typename _S3, typename _S4, typename _S5>
    struct SubFunc<_S0,_S1,_S2,_S3,_S4,_S5> {
        typedef Self FullTuple;
        typedef tuple_t <
            typename Field<_S0>::Type
            , typename Field<_S1>::Type
            , typename Field<_S2>::Type
            , typename Field<_S3>::Type
            , typename Field<_S4>::Type
            , typename Field<_S5>::Type
            > Tuple;
        Tuple operator() (const Self& t) const {
            return Tuple (t.at<_S0>()
                          , t.at<_S1>()
                          , t.at<_S2>()
                          , t.at<_S3>()
                          , t.at<_S4>()
                          , t.at<_S5>()
                );
        }
    };        

    template <typename _S0, typename _S1, typename _S2, typename _S3, typename _S4, typename _S5, typename _S6>
    struct SubFunc<_S0,_S1,_S2,_S3,_S4,_S5,_S6> {
        typedef Self FullTuple;
        typedef tuple_t <
            typename Field<_S0>::Type
            , typename Field<_S1>::Type
            , typename Field<_S2>::Type
            , typename Field<_S3>::Type
            , typename Field<_S4>::Type
            , typename Field<_S5>::Type
            , typename Field<_S6>::Type
            > Tuple;
        Tuple operator() (const Self& t) const {
            return Tuple (t.at<_S0>()
                          , t.at<_S1>()
                          , t.at<_S2>()
                          , t.at<_S3>()
                          , t.at<_S4>()
                          , t.at<_S5>()
                          , t.at<_S6>()
                );
        }
    };        

    template <typename _S0, typename _S1, typename _S2, typename _S3, typename _S4, typename _S5, typename _S6, typename _S7>
    struct SubFunc<_S0,_S1,_S2,_S3,_S4,_S5,_S6,_S7> {
        typedef Self FullTuple;
        typedef tuple_t <
            typename Field<_S0>::Type
            , typename Field<_S1>::Type
            , typename Field<_S2>::Type
            , typename Field<_S3>::Type
            , typename Field<_S4>::Type
            , typename Field<_S5>::Type
            , typename Field<_S6>::Type
            , typename Field<_S7>::Type
            > Tuple;
        Tuple operator() (const Self& t) const {
            return Tuple (t.at<_S0>()
                          , t.at<_S1>()
                          , t.at<_S2>()
                          , t.at<_S3>()
                          , t.at<_S4>()
                          , t.at<_S5>()
                          , t.at<_S6>()
                          , t.at<_S7>()
                );
        }
    };        

    template <typename _S0, typename _S1, typename _S2, typename _S3, typename _S4, typename _S5, typename _S6, typename _S7, typename _S8>
    struct SubFunc<_S0,_S1,_S2,_S3,_S4,_S5,_S6,_S7,_S8> {
        typedef Self FullTuple;
        typedef tuple_t <
            typename Field<_S0>::Type
            , typename Field<_S1>::Type
            , typename Field<_S2>::Type
            , typename Field<_S3>::Type
            , typename Field<_S4>::Type
            , typename Field<_S5>::Type
            , typename Field<_S6>::Type
            , typename Field<_S7>::Type
            , typename Field<_S8>::Type
            > Tuple;
        Tuple operator() (const Self& t) const {
            return Tuple (t.at<_S0>()
                          , t.at<_S1>()
                          , t.at<_S2>()
                          , t.at<_S3>()
                          , t.at<_S4>()
                          , t.at<_S5>()
                          , t.at<_S6>()
                          , t.at<_S7>()
                          , t.at<_S8>()
                );
        }
    };        
        
    template <typename _S0, typename _S1, typename _S2, typename _S3, typename _S4, typename _S5, typename _S6, typename _S7, typename _S8, typename _S9>
    struct SubFunc {
        typedef Self FullTuple;
        typedef tuple_t <
            typename Field<_S0>::Type
            , typename Field<_S1>::Type
            , typename Field<_S2>::Type
            , typename Field<_S3>::Type
            , typename Field<_S4>::Type
            , typename Field<_S5>::Type
            , typename Field<_S6>::Type
            , typename Field<_S7>::Type
            , typename Field<_S8>::Type
            , typename Field<_S9>::Type
            > Tuple;
        Tuple operator() (const Self& t) const {
            return Tuple (t.at<_S0>()
                          , t.at<_S1>()
                          , t.at<_S2>()
                          , t.at<_S3>()
                          , t.at<_S4>()
                          , t.at<_S5>()
                          , t.at<_S6>()
                          , t.at<_S7>()
                          , t.at<_S8>()
                          , t.at<_S9>()
                );
        }
    };        

    // static Self from_ref(const RefTuple& ref_tuple)
    // {
    //     return SubFunc<typename Repacked::Type0
    //         , typename Repacked::Type1
    //         , typename Repacked::Type2
    //         , typename Repacked::Type3
    //         , typename Repacked::Type4
    //         , typename Repacked::Type5
    //         , typename Repacked::Type6
    //         , typename Repacked::Type7
    //         , typename Repacked::Type8
    //         , typename Repacked::Type9
    //         >() (ref_tuple);
    // }

    explicit tuple_t()
    {}
    
    template <typename V0>
    explicit tuple_t(const V0& v0)
        : data_(v0)
    {}

    template <typename V0, typename V1>
    explicit tuple_t(const V0& v0, const V1& v1)
        : data_(v0,v1)
    {}

    template <typename V0, typename V1, typename V2>
    explicit tuple_t(const V0& v0, const V1& v1, const V2& v2)
        : data_(v0,v1,v2)
    {}

    template <typename V0, typename V1, typename V2, typename V3>
    explicit tuple_t(const V0& v0, const V1& v1, const V2& v2, const V3& v3)
        : data_(v0,v1,v2,v3)
    {}

    template <typename V0, typename V1, typename V2, typename V3, typename V4>
    explicit tuple_t(const V0& v0, const V1& v1, const V2& v2, const V3& v3, const V4& v4)
        : data_(v0,v1,v2,v3,v4)
    {}

    template <typename V0, typename V1, typename V2, typename V3, typename V4, typename V5>
    explicit tuple_t(const V0& v0, const V1& v1, const V2& v2, const V3& v3, const V4& v4, const V5& v5)
        : data_(v0,v1,v2,v3,v4,v5)
    {}

    template <typename V0, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6>
    explicit tuple_t(const V0& v0, const V1& v1, const V2& v2, const V3& v3, const V4& v4, const V5& v5, const V6& v6)
        : data_(v0,v1,v2,v3,v4,v5,v6)
    {}

    template <typename V0, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7>
    explicit tuple_t(const V0& v0, const V1& v1, const V2& v2, const V3& v3, const V4& v4, const V5& v5, const V6& v6, const V7& v7)
        : data_(v0,v1,v2,v3,v4,v5,v6,v7)
    {}

    template <typename V0, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7, typename V8>
    explicit tuple_t(const V0& v0, const V1& v1, const V2& v2, const V3& v3, const V4& v4, const V5& v5, const V6& v6, const V7& v7, const V8& v8)
        : data_(v0,v1,v2,v3,v4,v5,v6,v7,v8)
    {}

    template <typename V0, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7, typename V8, typename V9>
    explicit tuple_t(const V0& v0, const V1& v1, const V2& v2, const V3& v3, const V4& v4, const V5& v5, const V6& v6, const V7& v7, const V8& v8, const V9& v9)
        : data_(v0,v1,v2,v3,v4,v5,v6,v7,v8,v9)
    {}
        
        
friend int valcmp (const Self& t1, const Self& t2)
    { return valcmp(t1.data_, t2.data_); }

friend bool operator< (const Self& t1, const Self& t2)
    { return valcmp(t1.data_, t2.data_) < 0; }

    bool operator== (const Self& other) const
    { return data_ == other.data_; }

    bool operator!= (const Self& other) const
    { return data_ != other.data_; }

    // get value(reference) at the field
    template <typename T> typename Field<T>::Value& at ()
    { return (*((typename Field<T>::Value*)((char*)this + Field<T>::OFFSET))); }

    // get const value(reference) at the field
    template <typename T> const typename Field<T>::Value& at () const
    { return (*((const typename Field<T>::Value*)((const char*)this + Field<T>::OFFSET))); }
        
    template <int I> typename DirectField<I>::Value& at_idx ()
    { return (*((typename DirectField<I>::Value*)((char*)(this) + DirectField<I>::OFFSET))); }

    template <int I> const typename DirectField<I>::Value& at_idx () const
    { return (*((const typename DirectField<I>::Value*)((const char*)(this) + DirectField<I>::OFFSET))); }
        
    template <typename T>
    int offset () const
    { return Field<T>::OFFSET; }
        
    template <typename T>
    int idx () const
    { return Field<T>::IDX; }

friend size_t hash(const Self& t)
    { return hash(t.data_); }
        
    template <typename Func>
    struct TupleFunc : public base_function_object<typename Func::Result(Self)> {
        TupleFunc (Func f=Func())
            : f_(f)
        {}
            
        typename Func::Result operator() (const Self& t) const
        { return t.data_.apply(f_); }

    private:                                                            
        Func f_;                                                        
    };                                                                  

    void to_string(StringWriter& sw) const
    { data_.to_string (sw); }
};

}

#endif  //_TUPLE_HPP_
