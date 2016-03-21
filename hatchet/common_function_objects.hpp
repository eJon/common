// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Often used function objects
// Author: gejun@baidu.com
// Date: Dec 4 15:34:50 CST 2010
#pragma once
#ifndef _COMMON_FUNCTION_OBJECTS_HPP_
#define _COMMON_FUNCTION_OBJECTS_HPP_

#include "base_function_object.hpp"

namespace st {

// Returns: Input
template <typename _Any>
struct f_identity : public BaseFunctionObject< _Any(_Any) > {
    const _Any& operator() (const _Any& x) const { return x; }
    _Any& operator() (_Any& x) const { return x; }
};

// Returns: always true
template <typename _Any>
struct f_true : public BaseFunctionObject< bool(_Any) >
{ bool operator () (const _Any&) const { return true; } };

// Returns: always false
template <typename _Any>
struct f_false : public BaseFunctionObject< bool(_Any) >
{ bool operator () (const _Any&) const { return false; } };
    
// +
template <typename _N>
struct f_plus : public BaseFunctionObject<_N(_N, _N)>
{ _N operator () (const _N& x, const _N& y) const { return x + y; } };

// -
template <typename _N>
struct f_minus : public BaseFunctionObject<_N(_N,_N)>
{ _N operator () (const _N& x, const _N& y) const { return x - y; } };

// *
template <typename _N>
struct f_multiply : public BaseFunctionObject<_N(_N,_N)>
{ _N operator () (const _N& x, const _N& y) const { return x * y; } };

// /
template <typename _N>
struct f_divide : public BaseFunctionObject<_N(_N,_N)>
{ _N operator () (const _N& x, const _N& y) const { return x / y; } };

// Returns: input integer is even or not
template <typename _T>
struct f_even : public BaseFunctionObject<bool(_T)>
{ bool operator() (const _T& v) const { return (v & 1) == 0; } };

// Returns: input integer is odd or not
template <typename _N>
struct f_odd : public BaseFunctionObject<bool(_N)>
{ bool operator() (const _N& v) const { return (v & 1) == 1; } };

// Returns: operator|| of two inputs
template <typename _N>
struct f_or : public BaseFunctionObject<_N(_N,_N)>
{ _N operator() (const _N& x, const _N& y) const { return x || y; } };

// Returns: operator&& of two inputs
template <typename _N>
struct f_and : public BaseFunctionObject<_N(_N,_N)>
{ _N operator() (const _N& x, const _N& y) const { return x && y; } };

}

#endif  //_COMMON_FUNCTION_OBJECTS_HPP_
