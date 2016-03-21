// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Base class of function objects
// Author: gejun@baidu.com
// Date: Dec 4 16:28:58 CST 2010
#pragma once
#ifndef _FUN_BASE_FUNCTION_OBJECT_HPP_
#define _FUN_BASE_FUNCTION_OBJECT_HPP_

#include "string_writer.hpp"      // to_string

namespace st {

// base class of all (well-defined) function objects,
// so that type of arguments and return value are extractable
template <typename _R> struct BaseFunctionObject;

// Get type of return value of a function object
template <typename _F, typename _A1, typename _A2 = void, typename _A3 = void>
class ReturnType;

// connect two function objects into one
template <typename _F1, typename _F2> struct FunCon;

}  // namespace st

#include "detail/base_function_object_inl.hpp"

#endif  //_FUN_BASE_FUNCTION_OBJECT_HPP_
