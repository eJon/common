// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Commonly used functions
// Author: gejun@baidu.com
// Date: Fri Jul 30 13:16:06 2010
#pragma once
#ifndef _ST_UTILITY_HPP_
#define _ST_UTILITY_HPP_

#include <utility>  // std::pair

// Get size of an array allocated on stack
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(_a_)  (sizeof(_a_) / sizeof(*(_a_)))
#endif

namespace st {
// Align an unsigned int to be power of 2
inline u_int align_pow2 (u_int n)
{ return (n & (n-1)) ? (1 << (sizeof(n)*CHAR_BIT - __builtin_clz(n))) : n; }

// find next prime number
inline u_int find_near_prime (u_int n_bucket)
{
    // Following prime sequence is copied from STL
    static const unsigned long __stl_prime_list[] = {
        53ul,         97ul,         193ul,       389ul,       769ul,
        1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
        49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
        1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
        50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
        1610612741ul, 3221225473ul, 4294967291ul
    };
    const size_t n_prime =
        sizeof(__stl_prime_list)/sizeof(__stl_prime_list[0]);
    
    for (size_t i = 0; i < n_prime; i++) {
        if (__stl_prime_list[i] >= n_bucket) {
            return __stl_prime_list[i];
        }
    }
    return n_bucket;
}

// function objects for std::pair
template <typename _P> struct std_pair_first {
    typename _P::first_type operator() (const _P& pair) const
    { return pair.first; }
};

template <typename _P> struct std_pair_second {
    typename _P::second_type operator() (const _P& pair) const
    { return pair.second; }
};

// Extend std::pair with third field
template <typename _F, typename _S, typename _T>
struct pair3 {
    explicit pair3 (_F i_first, _S i_second, _T i_third)
        : first(i_first), second(i_second), third(i_third)
    {}
        
    _F first;
    _S second;
    _T third;
};

}  // namespace st

#endif  //_ST_UTILITY_HPP_
