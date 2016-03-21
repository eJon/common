// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Provide a macro to choose functions/templates by number of arguments
// Author: gejun@baidu.com
// Date: Sub. Feb. 27 18:41:24 CST 2011

#ifndef _CHOOSE_VARIADIC_H_
#define _CHOOSE_VARIADIC_H_

// Example:
// To choose between templates: tpl0<>, tpl1<_>, tpl2<_,_>, you may define:
// #define tpl(...) CHOOSE_VARIADIC_TPL(tpl, __VA_ARGS__)
// Then tpl(), tpl(_), tpl(_,_) ... will be evaluated to
// tpl0<>, tpl1<_>, tpl2<_> ...respectively
// Since macros can't escape `<' and `>', User should guarantee parameters 
// to the template do not have something like `<... , ...>', this is treated
// as two parameters, not one.
//
// Choosing between macros is very similar with templates except that
// You should use CHOOSE_VARIADIC_MACRO instead of CHOOSE_VARIADIC_TPL
//
// CHOOSE_VARIADIC_MACRO could also be used with functions, but since
// C++ functions are already overloadable, this is rarely the case.

// Get number of __VA_ARGS__(from 0 to 20)
#define VA_ARGS_NUM(...)                                  \
    VA_ARGS_NUM_IMPL1(just_place_holder, ##__VA_ARGS__)
#define VA_ARGS_NUM_IMPL1(...)                                            \
    VA_ARGS_NUM_IMPL(__VA_ARGS__,                                         \
                   20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
#define VA_ARGS_NUM_IMPL(_0_,_1_,_2_,_3_,_4_,_5_,_6_,_7_,_8_,_9_,_10_,_11_, \
                       _12,_13_,_14_,_15_,_16_,_17_,_18_,_19_,_20_,N,...) N

// Evaluate to macro##N(...) where N is number of arguments between brackets
#define CHOOSE_VARIADIC_MACRO(macro, ...)                                     \
    CHOOSE_VARIADIC_MACRO_(macro, VA_ARGS_NUM(__VA_ARGS__), __VA_ARGS__)
#define CHOOSE_VARIADIC_MACRO_(macro, n, ...)         \
    CHOOSE_VARIADIC_MACRO__(macro, n, __VA_ARGS__)
#define CHOOSE_VARIADIC_MACRO__(macro, n, ...)         \
    macro##n(__VA_ARGS__)

// Evaluate to tpl##N<...> where N is number of arguments between brackets
#define CHOOSE_VARIADIC_TPL(tpl, ...)                                   \
    CHOOSE_VARIADIC_TPL_(tpl, VA_ARGS_NUM(__VA_ARGS__), __VA_ARGS__)
#define CHOOSE_VARIADIC_TPL_(tpl, n, ...)       \
    CHOOSE_VARIADIC_TPL__(tpl, n, __VA_ARGS__)
#define CHOOSE_VARIADIC_TPL__(tpl, n, ...)      \
    tpl##n<__VA_ARGS__>

#endif  // _CHOOSE_VARIADIC_H_

