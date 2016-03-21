// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Compile-time math
// Author: gejun@baidu.com
// Date: Dec 4 15:08:43 CST 2010
#pragma once
#ifndef _C_MATH_HPP_
#define _C_MATH_HPP_

namespace st {
// Get number of bits of a constant unsigned integer
template <unsigned long _N> struct bit_num;

// Compute smallest R to have (1 << R) >= _N
template <unsigned long _N> struct bit_shift;

// Guard _N inside [_Min,_Max]
template <int _N, int _Min, int _Max> struct guard_num;


// -------- Implementations ----------
// bit_num
template <unsigned long _N> struct bit_num
{ static const int R = bit_num<_N/2>::R + 1; };

template <> struct bit_num <0> { static const int R = 0; };
       
// bit_shift
template <unsigned long _N> struct bit_shift
{ static const int R = bit_num<_N-1>::R; };

// guard_num
template <int _N, int _Min, int _Max> struct guard_num 
{ static const int R = (_N < _Min) ? _Min : ((_N > _Max) ? _Max : _N); };    

}
#endif  //_C_MATH_HPP_
