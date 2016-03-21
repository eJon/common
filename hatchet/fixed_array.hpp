// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Constant-length array
// Author: gejun@baidu.com
// Date: Sat Nov 13 19:07:19 CST 2010
#pragma once
#ifndef _FIXED_ARRAY_HPP_
#define _FIXED_ARRAY_HPP_

#include "functional.hpp"

namespace st {

template <typename _T, int _Num>
struct FixedArray {
    inline _T get_at(int idx) const
    { return a_data_[idx]; }
        
    inline void set_at(int idx, const _T& val)
    { a_data_[idx] = val; }

    inline void set_all (const _T& val)
    {
        for (int i=0; i<_Num; ++i) {
            a_data_[i] = val;
        }
    }

private:
    _T a_data_[_Num];
};

inline int num_of_one (uint8_t n1)
{
    // n2 = ((n1 & 0b10101010) >> 1) + (n1 & 0b01010101)
    register uint8_t n2 = ((n1 & 0xAA) >> 1) + (n1 & 0x55);
    // n3 = ((n2 & 0b11001100) >> 2) + (n2 & 0b00110011)
    register uint8_t n3 = ((n2 & 0xCC) >> 2) + (n2 & 0x33);
    // n4 = ((n3 & 0b11110000) >> 4) + (n3 & 0b00001111)
    return ((n3 & 0xF0) >> 4) + (n3 & 0xF);
}

inline int num_of_one (uint32_t n1)
{
    // similar with uint8_t, we have more 'A' and '5'
    register uint32_t n2 = ((n1 & 0xAAAAAAAA) >> 1) + (n1 & 0x55555555);
    // more 'C' and '3'
    register uint32_t n3 = ((n2 & 0xCCCCCCCC) >> 2) + (n2 & 0x33333333);
    // more 'F0' and '0F'
    register uint32_t n4 = ((n3 & 0xF0F0F0F0) >> 4) + (n3 & 0x0F0F0F0F);
    // things go more straight
    register uint32_t n5 = ((n4 & 0xFF00FF00) >> 8) + (n4 & 0x00FF00FF);
    return ((n5 & 0xFFFF0000) >> 16) + (n5 & 0xFFFF);
}

    
template <int _Num>
struct FixedArray<bool, _Num> {
    typedef uint32_t Line;
    enum {
        LINE_BIT_NUM = (sizeof(Line) << 3)
        // This is a reported bug in g++ 4.8 that forward
        // declared enums cannot be used as template arguments
        // , LINE_BIT_NUM_NUM = bit_shift<LINE_BIT_NUM>::R
        , LINE_BIT_NUM_NUM = bit_shift<sizeof(Line) << 3>::R
        , LINE_MASK = (LINE_BIT_NUM-1)
        , RAW_LINE_NUM = (_Num >> LINE_BIT_NUM_NUM)
        , LINE_NUM=(((RAW_LINE_NUM * LINE_BIT_NUM) == _Num) ? RAW_LINE_NUM : (RAW_LINE_NUM+1))
    };

    inline bool get_at(int idx) const
    { return 0 != (a_line_[idx >> LINE_BIT_NUM_NUM] & (1 << (idx & LINE_MASK))); }
        
    inline void set_at(int idx, bool val)
    {
        Line& d = a_line_[idx >> LINE_BIT_NUM_NUM];
        Line mask = (1 << (idx & LINE_MASK));
        d = val ? (d | mask) : (d & (~mask));
    }

    inline void set_all (bool val)
    {
        memset (a_line_, (val ? 0xFF : 0), sizeof(a_line_));
    }

    bool has_true () const
    {
        for (size_t i=0; i<LINE_NUM; ++i) {
            if (a_line_[i]) {
                return true;
            }
        }
        return false;
    }

    int true_num () const
    {
        int c = 0;
        for (size_t i=0; i<LINE_NUM; ++i) {
            c += num_of_one (a_line_[i]);
        }
        return c;
    }
        
    void to_string (StringWriter& sw) const
    {
        sw << _Num << ":{";
        for (int i=0; i<_Num; ++i) {
            if (i && (0 == (i & 7))) {
                sw << " | ";
            }
            sw << (get_at(i) ? 1 : 0);
        }
        sw << "}";
    }

private:
    Line a_line_[LINE_NUM];
};
}

#endif  // _FIXED_ARRAY_HPP_

