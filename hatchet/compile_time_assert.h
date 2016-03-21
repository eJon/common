// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Compile-time assertion with readable error
// Author: gejun@baidu.com
// Date: Dec.6  4 13:58:44 CST 2010
#pragma once
#ifndef _C_ASSERT_H_
#define _C_ASSERT_H_

#define USE_OLD_COMPILE_TIME_ASSERT
#ifdef USE_OLD_COMPILE_TIME_ASSERT

// Use this macro to assert constant boolean expressions.
// Params:
//   _c_expr_   the constant expression to be checked
//   _msg_      an error infomation which conforms naming conventions of
//              variables, namely using only alphabets/numbers/underscores,
//              no blanks. For example "cannot_accept_a_number_bigger_than_128"
//              is good while "this number is out-of-range" is bad
//
// when an asssertion like "C_ASSERT(false, you_should_not_be_here)" broke,
// a compilation error is printed in console:
// foo.cpp:401: error: no matching function for call to `CompileTimeAssert<
// false>::CompileTimeAssert(ERROR_401::you_should_not_be_here)'
//
// You can assert:
//  at global area
// ----------------
// C_ASSERT(false, you_should_not_be_here);
// int main () ....
//
//  inside a class
// ----------------
// struct Cool {
//     C_ASSERT(1 == 0, Not_Cool);
// };
//
//  inside a function
// -------------------
// int foo (...)
// {
//     C_ASSERT (value < 10, some_values_are_invalid);
// }
template <bool> struct CTAssert { CTAssert(...) {}; };
template <> struct CTAssert<false> {};

// expand __LINE__, should not be called outside this file
#define ST_CONCAT2(a, b) ST_CONCAT2_(a, b)
#define ST_CONCAT2_(a, b) a##b

#define C_ASSERT(_expr_, _msg_)                                         \
    class ST_CONCAT2(_msg_##___,__LINE__) {};                           \
    enum { ST_CONCAT2(ENUM_##_msg_##___,__LINE__) =                     \
           sizeof((CTAssert<!!(_expr_)>(ST_CONCAT2(_msg_##___, __LINE__)()))) }; \
    

// Assert `_type1_' and `_type2_' are same type
#define C_ASSERT_SAME(_type1_, _type2_, _msg_)          \
    C_ASSERT((st::c_same<_type1_, _type2_ >::R), _msg_)

// Assert `_type1_' and `_type2_' are different types
#define C_ASSERT_NOT_SAME(_type1_, _type2_, _msg_)      \
    C_ASSERT((!st::c_same<_type1_, _type2_ >::R), _msg_)

// Assert type `_type_' is not void
#define C_ASSERT_NOT_VOID(_type_, _msg_)        \
    C_ASSERT(!st::c_void<_type_ >::R, _msg_)

// Assert type `_type_' is void
#define C_ASSERT_VOID(_type_, _msg_)            \
    C_ASSERT(st::c_void<_type_ >::R, _msg_)

#else //

// Use this macro to assert constant boolean expressions.
// Params:
//   _c_expr_   the constant expression to be checked
//   _msg_      an error infomation which conforms naming conventions of
//              variables, namely using only alphabets/numbers/underscores,
//              no blanks. For example "cannot_accept_a_number_bigger_than_128"
//              is good while "this number is out-of-range" is bad
//
// when an asssertion like "C_ASSERT(false, you_should_not_be_here)" break,
// a compilation error is printed in console:
// foo.cpp:401: error: size of array `you_should_not_be_here' is negative
//
// You can assert:
//  at global area
// ----------------
// C_ASSERT(false, you_should_not_be_here);
// int main () ....
//
//  inside a class
// ----------------
// struct Cool {
//     C_ASSERT(1 == 0, Not_Cool);
// };
//
//  inside a function
// -------------------
// int foo (...)
// {
//     C_ASSERT (value < 10, some_values_are_invalid);
// }
template <bool> struct CTAssert {};
#define C_ASSERT(_c_expr_, _msg_)                                       \
    typedef CTAssert<(bool(_c_expr_))> _msg_[bool(_c_expr_) ? 1 : -1]

// Assert `_type1_' and `_type2_' are same type
#define C_ASSERT_SAME(_type1_, _type2_, _msg_)          \
    C_ASSERT((st::c_same<_type1_, _type2_ >::R), _msg_)

// Assert `_type1_' and `_type2_' are different types
#define C_ASSERT_NOT_SAME(_type1_, _type2_, _msg_)      \
    C_ASSERT((!st::c_same<_type1_, _type2_ >::R), _msg_)

// Assert type `_type_' is not void
#define C_ASSERT_NOT_VOID(_type_, _msg_)        \
    C_ASSERT(!st::c_void<_type_ >::R, _msg_)

// Assert type `_type_' is void
#define C_ASSERT_VOID(_type_, _msg_)            \
    C_ASSERT(st::c_void<_type_ >::R, _msg_)

#endif // USE_OLD_COMPILE_TIME_ASSERT

#endif  // _C_ASSERT_H_
