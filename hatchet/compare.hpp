// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Function objects for comparisons
// Author: gejun@baidu.com
// Date: Dec 4 17:01:02 CST 2010
#pragma once
#ifndef _FUN_COMPARE_HPP_
#define _FUN_COMPARE_HPP_

#include "base_function_object.hpp"

namespace st {    
// Compare values like strcmp
// Returns: 
//   a negative integer   LHS <  RHS
//   0                    LHS == RHS
//   a positive integer   LHS > RHS
// Note: don't assume that "negative integer" is -1 or "positive integer" is 1
template <typename _T> int valcmp(const _T& v1, const _T& v2)
{ return v1 < v2 ? -1 : (v1 == v2 ? 0 : 1); }

// All `valcmp' fuctions return integer, especially for `valcmp(long, long)'. 
// Otherwise, caller may use integer to store return value, which can be
// truncated to 0 (indicate v1==v2) while its actural return type is `long'
inline int valcmp(long long v1, long long v2)      { return (v1 < v2? -1: (v1 == v2? 0: 1)); }
inline int valcmp(long v1, long v2)                { return (v1 < v2? -1: (v1 == v2? 0: 1)); }
inline int valcmp(int v1, int v2)                  { return v1 - v2; }
inline int valcmp(short v1, short v2)              { return (int)v1 - (int)v2; }
inline int valcmp(char v1, char v2)                { return (int)v1 - (int)v2; }
inline int valcmp(const char* k1, const char* k2)  { return strcmp(k1, k2); }

// Reversed comparison result of inputs
template <typename _T> int reversed_valcmp(const _T& v1, const _T& v2)
{ return valcmp(v2, v1); }

// Function object corresponding to valcmp
template <typename _N>
struct Compare : public BaseFunctionObject<int(_N,_N)> {
    int operator()(const _N& v1, const _N& v2) const
    { return valcmp(v1, v2); }
};

// Function object corresponding to reversed_valcmp
template <typename _N>
struct ReversedCompare : public BaseFunctionObject<int(_N,_N)> {
    int operator()(const _N& v1, const _N& v2) const 
    { return valcmp(v2, v1); }
};

// Common function objects for comparisons
// inequality
template <typename _N>
struct GreaterThan : public BaseFunctionObject<bool(_N,_N)> { 
    bool operator()(const _N& v1, const _N& v2) const
    { return v1 > v2; } 
};

// >=
template <typename _N>
struct GreaterEqual : public BaseFunctionObject<bool(_N,_N)> { 
    bool operator()(const _N& v1, const _N& v2) const 
    { return v1 >= v2; } 
};

// <
template <typename _N>
struct LessThan : public BaseFunctionObject<bool(_N,_N)> {
    bool operator()(const _N& v1, const _N& v2) const 
    { return v1 < v2; }
};

// <=
template <typename _N>
struct LessEqual : public BaseFunctionObject<bool(_N,_N)> { 
    bool operator()(const _N& v1, const _N& v2) const 
    { return v1 <= v2; } 
};

// ==
template <typename _Any>
struct Equal : public BaseFunctionObject<bool(_Any,_Any)> { 
    bool operator()(const _Any& v1, const _Any& v2) const
    { return v1 == v2; } 
};
    
// !=
template <typename _N>
struct NotEqual : public BaseFunctionObject<bool(_N,_N)> { 
    bool operator()(const _N& v1, const _N& v2) const
    { return v1 != v2; } 
};

}
#endif  //_FUN_COMPARE_HPP_

