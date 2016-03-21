// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Macros of logging, return code checking, value checking and debugging
// utilities
// Author: gejun@baidu.com
// Date: 2010/07/30 16:00 
#pragma once
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>      // printf
#include <iostream>     // std::cout
#include <new>          // std::nothrow
#include "st_errno.h"   // error numbers
#include "st_timer.h"   // timing
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>

namespace st {
// hint for the branch prediction
inline bool likely(bool expr) { return __builtin_expect(expr, true); }
inline bool unlikely(bool expr) { return __builtin_expect(expr, false); }
}

// Delete a pointer and set to NULL
#define ST_DELETE(_ptr_)                        \
    do {                                        \
        typeof(_ptr_) __p = _ptr_;              \
        if (NULL != __p) {                      \
            delete __p;                         \
            __p = NULL;                         \
        }                                       \
    } while(0)

// Delete an array created by new and set to NULL
#define ST_DELETE_ARRAY(_parray_)               \
    do {                                        \
        typeof(_parray_) __a = _parray_;        \
        if (NULL != __a) {                      \
            delete [] __a;                      \
            __a = NULL;                         \
        }                                       \
    } while(0)

// New a _type_
// Returns: NULL for out-of-memory rather than throwing exception
#define ST_NEW(_type_, ...) (new (std::nothrow) _type_(__VA_ARGS__))

// Call constructor of _type_ on _mem_ (as this pointer)
// Note: this is generally the way calling constructor in C++,
//       correspondingly a destructor ~_type_() could be called directly
#define ST_NEW_ON(_mem_, _type_, ...) (new (_mem_) _type_(__VA_ARGS__))

// New an _type_ array with _len_ elements
#define ST_NEW_ARRAY(_type_, _len_) (new (std::nothrow) _type_[_len_])

// Logging utilities
#define LOG_TYPE 0

#if LOG_TYPE == 0
#define ST_SAY(_fmt_, args...)                                  \
    (fprintf (stdout, "[%s:%d][%s] " _fmt_                      \
              ,__FILE__, __LINE__, __FUNCTION__, ##args)        \
     , fprintf (stdout, "\n"))

#define ST_FATAL(_fmt_, args...)                                \
    (fprintf (stdout, "[FATAL][%s:%d][%s] " _fmt_               \
              ,__FILE__, __LINE__, __FUNCTION__, ##args)        \
     , fprintf (stdout, "\n"))

#define ST_WARN(_fmt_, args...)                                 \
    (fprintf (stdout, "[WARNING][%s:%d][%s] " _fmt_             \
              ,__FILE__, __LINE__, __FUNCTION__, ##args)        \
     , fprintf (stdout, "\n"))

#define ST_TRACE(_fmt_, args...)                                \
    (fprintf (stdout, "[TRACE][%s:%d][%s] " _fmt_               \
              , __FILE__, __LINE__, __FUNCTION__, ##args)       \
     , fprintf (stdout, "\n"))

#ifndef NDEBUG
#define ST_DEBUG(_fmt_, args...)                                \
    (fprintf (stdout, "[DEBUG][%s:%d][%s] " _fmt_               \
              , __FILE__, __LINE__, __FUNCTION__, ##args)       \
     , fprintf (stdout, "\n"))
#else
#define ST_DEBUG(_fmt_, args...)
#endif
#endif

#if LOG_TYPE == 1
#include <ul_log.h>

#define ST_SAY(_fmt_, args...)                                  \
        ul_writelog(UL_LOG_WARNING, "[%s:%d][%s] " _fmt_        \
                    , __FILE__, __LINE__, __FUNCTION__, ##args)

#define ST_FATAL(_fmt_, args...)                                \
        ul_writelog(UL_LOG_FATAL, "[%s:%d][%s] " _fmt_          \
                    , __FILE__, __LINE__, __FUNCTION__, ##args)

#define ST_WARN(_fmt_, args...)                                 \
        ul_writelog(UL_LOG_WARNING, "[%s:%d][%s] " _fmt_        \
                    , __FILE__, __LINE__, __FUNCTION__, ##args)

#define ST_TRACE(_fmt_, args...)                                \
        ul_writelog(UL_LOG_TRACE, "[%s:%d][%s] " _fmt_          \
                    , __FILE__, __LINE__, __FUNCTION__, ##args)

#ifndef NDEBUG
#define ST_DEBUG(_fmt_, args...)                                \
    ul_writelog(UL_LOG_DEBUG, "[%s:%d][%s] " _fmt_              \
                , __FILE__, __LINE__, __FUNCTION__, ##args)
#else
#define ST_DEBUG(_fmt_, args...)
#endif
#endif

// Check return values
#ifndef NDEBUG
#define RETURN(_code_)                          \
    do {                                        \
        int __c = _code_;                       \
        if (0 != __c) {                         \
            ST_WARN("returning %d", __c);       \
        }                                       \
        return __c;                             \
    } while(0)

#define RETURN_EX(_code_,_goodcode_)            \
    do {                                        \
        int __c = _code_;                       \
        if (!(__c & (_goodcode_))) {            \
            ST_WARN("returning %d", __c);       \
        }                                       \
        return __c;                             \
    } while(0)
#else  // NDEBUG
#define RETURN(_code_) return (_code_)
#define RETURN_EX(_code_,_goodcode_) return (_code_)
#endif

#ifndef NDEBUG
// following macros attach side effects of reporting invalid values
#define CHECK_BOP(_bop_,_obj_,_value_)                          \
    (((_obj_) _bop_ (_value_))                                  \
     ? (ST_WARN("%s %s %s", #_obj_, #_bop_, #_value_), (_obj_)) \
     : (_obj_))

#define CHECK_NE(_obj_,_value_)  CHECK_BOP(==,_obj_,_value_)
#define CHECK_EQ(_obj_,_value_)  CHECK_BOP(!=,_obj_,_value_)
#define CHECK_GT(_obj_,_value_)  CHECK_BOP(<=,_obj_,_value_)
#define CHECK_GE(_obj_,_value_)  CHECK_BOP(<,_obj_,_value_)
#define CHECK_LT(_obj_,_value_)  CHECK_BOP(>=,_obj_,_value_)
#define CHECK_LE(_obj_,_value_)  CHECK_BOP(>,_obj_,_value_)
#else
#define CHECK_NE(_obj_,_value_)  (_obj_)
#define CHECK_EQ(_obj_,_value_)  (_obj_)
#define CHECK_GT(_obj_,_value_)  (_obj_)
#define CHECK_GE(_obj_,_value_)  (_obj_)
#define CHECK_LT(_obj_,_value_)  (_obj_)
#define CHECK_LE(_obj_,_value_)  (_obj_)
#endif

#endif  // _DEBUG_H_
