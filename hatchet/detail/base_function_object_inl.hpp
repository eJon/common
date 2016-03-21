// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Inline implementation of base_function_object.hpp
// Author: gejun@baidu.com
// Date: Dec 4 16:28:58 CST 2010

namespace st {

template <typename _R> struct BaseFunctionObject {
    typedef _R Result;
    static const int N_ARG = 0;
            
    void to_string (StringWriter& sw) const
    { sw << "::" << typeid(_R).name(); }
};

// Specialize function objects with one parameter
template <typename _R, typename _T1>
struct BaseFunctionObject<_R (_T1)> {
    typedef _R Result;
    typedef _T1 Arg1;
    static const int N_ARG = 1;

    void to_string (StringWriter& sw) const
    {
        sw << "::" << typeid(_T1).name()
           << "->" << typeid(_R).name(); 
    }
};

// Specialize function objects with two parameters
template <typename _R, typename _T1, typename _T2>
struct BaseFunctionObject<_R (_T1,_T2)> {
    typedef _R Result;
    typedef _T1 Arg1;
    typedef _T2 Arg2;
    static const int N_ARG = 2;

    void to_string (StringWriter& sw) const
    {
        sw << "::" << typeid(_T1).name()
           << "->" << typeid(_T2).name()
           << "->" << typeid(_R).name(); 
    }
};

// Specialize function objects with three parameters
template <typename _R, typename _T1, typename _T2, typename _T3>
struct BaseFunctionObject<_R (_T1,_T2,_T3)> {
    typedef _R Result;
    typedef _T1 Arg1;
    typedef _T2 Arg2;
    typedef _T3 Arg3;
    static const int N_ARG = 3;

    void to_string (StringWriter& sw) const
    {
        sw << "::" << typeid(_T1).name()
           << "->" << typeid(_T2).name()
           << "->" << typeid(_T3).name()
           << "->" << typeid(_R).name(); 
    }
};

// ReturnType
template <typename _F, typename _A1, typename _A2, typename _A3>
class ReturnType {
    static const _F d_f_;
    static const _A1 d_a1_;
    static const _A2 d_a2_;
    static const _A3 d_a3_;
public:
    typedef typeof(d_f_(d_a1_, d_a2_, d_a3_)) R;
};

template <typename _F, typename _A1, typename _A2>
class ReturnType<_F,_A1,_A2> {
    static const _F d_f_;
    static const _A1 d_a1_;
    static const _A2 d_a2_;
public:
    typedef typeof(d_f_(d_a1_, d_a2_)) R;
};

template <typename _F, typename _A1> class ReturnType<_F,_A1> {
    static const _F d_f_;
    static const _A1 d_a1_;
public:
    typedef typeof(d_f_(d_a1_)) R;
};

// FunCon
template <typename _F1, typename _F2> struct FunCon {
    // f1 * f2 where f2 accepts one argument
    template <typename _A1>
    typename ReturnType<_F1,typename ReturnType<_F2,_A1>::R>::R
    operator() (const _A1& a1) const
    { return _F1()(_F2()(a1)); }

    // f1 * f2 where f2 accepts two arguments
    template <typename _A1, typename _A2>
    typename ReturnType<_F1,typename ReturnType<_F2,_A1,_A2>::R>::R
    operator() (const _A1& a1, const _A2& a2) const
    { return _F1()(_F2()(a1, a2)); }
};

}  // namespace st
