// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Compute hash values
// Author: gejun@baidu.com
// Date: Dec 4 17:01:02 CST 2010
#pragma once
#ifndef _FUN_HASH_HPP_
#define _FUN_HASH_HPP_

#include <string>
#define _GCC_VERSION_ (__GNUC__ * 10000                                   \
                       + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

// Since gcc 4.3.0, <ext/hash_fun.h> has been moved to <backward/hash_fun.h>,
// which was deprecated formally
#if _GCC_VERSION_ >= 40300
#if __cplusplus >= 201103L

#include <functional>
#define BASE_ST_HASH_NAMESPACE std

#else // __cplusplus >= 201103L

#include <tr1/functional>
#define BASE_ST_HASH_NAMESPACE std::tr1

#endif // __cplusplus >= 201103L
#else // _GCC_VERSION_ >= 40300

#include <ext/hash_map>
#define BASE_ST_HASH_NAMESPACE __gnu_cxx

#endif // _GCC_VERSION_ >= 40300

namespace st {

// // This is for compatibility with existing code, don't use it
// typedef size_t hash_value_t; 
// #define compute_hash(...) hash(__VA_ARGS__)

template <typename _T> struct Hash : public BASE_ST_HASH_NAMESPACE::hash<_T> {};

// Hash<_T*> has conflict with Hash<const char*>. Furthermore, it's
// dangerous to use pointers as KEY
// template <typename _T> struct Hash<_T*> {
//     size_t operator()(_T* p) const { return (size_t)p; }
// };

// Since gcc 4.3.0, header file `functional' already contains hash(std::string)
// Moreover, it also has hash(_T*) which may bring unwanted result when _T=char  
#if _GCC_VERSION_ < 40300

template <> struct Hash<std::string> {
    size_t operator()(const std::string& s) const
    { return Hash<const char*>()(s.c_str()); }
};

template <> struct Hash<long long> {
    size_t operator()(long long x) const { return x; }
};

template <> struct Hash<unsigned long long> {
    size_t operator()(unsigned long long x) const { return x; }
};

#endif // _GCC_VERSION_ < 40300

template <typename _T>
inline size_t hash(const _T& v) { return Hash<_T>()(v); }

inline size_t hash(const char v[]) { return Hash<const char*>()(v); }

inline size_t hash(const void* p, size_t n)
{
    register size_t v = 0;
    for (size_t i = 0; i < n; ++i) {
        v = v * 5ul + (size_t)((const char*)p)[i];
    }
    return v;
}

}  // namespace st

#endif  // _FUN_HASH_HPP_

