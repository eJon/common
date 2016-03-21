// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Search/copy/insert/erase arrays
// Author: gejun@baidu.com
// Date: Fri Dec 16 11:52:37 CST 2010
#pragma once
#ifndef _ARRAY_OP_HPP_
#define _ARRAY_OP_HPP_

#include "debug.h"  // unlikely/likely

namespace st {

// Binary-search the array.
// Note: there's a little encoding in the return value where a positive value
//       means that value at the position equals input, while a negative value
//       which is the bitwise reverse of a position means that the input is
//       less than the value at the position, if the position is not left-most,
//       then obviously input is greater than the value at prior position as
//       well
template <typename _CompareXY, typename _X, typename _Y>
int array_binary_search_1 (const _X& input, const _Y* a_y, int size)
{
    _CompareXY cmp;
    // binary search in the range [0..size-1]
    int b = -1;
    int e = size-1;
    int r = cmp (input, a_y[e]);
    if (r >= 0) {
        return 0 == r ? e : (~size);
    }
    int m;
    while ((e - b) > 1) {
        m = (e+b) >> 1;
        r = cmp (input, a_y[m]);
        if (0 == r) {
            return m;
        }
        //b += (r > 0) * (m - b);
        //e += (r < 0) * (m - e);
        if (r > 0) {
            b = m;
        } else {
            e = m;
        }
    }
    // e == b+1 and always being non-negative
    // n should be resident just before e and after b
    return ~e;
}

template <typename _CompareXY, typename _X, typename _Y>
int array_binary_search_3 (const _X& input, const _Y* a_y, int size)
{
    _CompareXY cmp;
    int b = -1;
    int e = size;
    int m, r;
    int lsb = 0;

    // Here we expand the loop if the size is between 16 and 32 inclusive.
    // Note that a_y[-1] and a_y[size] is a sentinel. 
    if(size >= 16 && size <= 31) {
        b = 0;
        
        lsb = size & 0x1;
        size /= 2;
        // Ensure that a_[b] < input <= a_y[b + 8].
        if(cmp(input, a_y[b + size]) > 0) {
            b += size;
            size += lsb;
        }
        
        lsb = size & 0x1;
        size /= 2;
        // Ensure that a_[b] < input <= a_y[b + 4].
        if(cmp(input, a_y[b + size]) > 0) {
            b += size;
            size += lsb;
        }

        lsb = size & 0x1;
        size /= 2;
        // Ensure that a_[b] < input <= a_y[b + 2].
        if(cmp(input, a_y[b + size]) > 0) {
            b += size;
            size += lsb;
        }
        
        lsb = size & 0x1;
        size /= 2;
        // Ensure that a_[b] < input <= a_y[b + 1].
        if(cmp(input, a_y[b + size]) > 0) {
            b += size;
            size += lsb;
        }

        lsb = size & 0x1;
        size /= 2;
        // Ensure that a_[b] < input <= a_y[b + 1].
        if(cmp(input, a_y[b + size]) > 0) {
            b += size;
            size += lsb;
        }

        if(cmp(input, a_y[b]) != 0) {
            return ~b;
        } else {
            return b;
        }
    } else {
        while ((e - b) > 1) {
            m = (e+b) >> 1;
            r = cmp (input, a_y[m]);
            if (0 == r) {
                return m;
            }
            if (r > 0) {
                b = m;
            } else {
                e = m;
            }
        }

        return ~e;
    }
}


template <typename _CompareXY, typename _X, typename _Y>
int array_binary_search (const _X& input, const _Y* a_y, 
        int size, u_int _MAX_FANOUT)
{
    _CompareXY cmp;
    int b = -1;
    int e = size;
    int m, r;
    int half_fanout = _MAX_FANOUT >> 1;

    // _MAX_FANOUT must be power of 2 and less than or equal 64.
    assert((_MAX_FANOUT & _MAX_FANOUT - 1) == 0);
    assert(_MAX_FANOUT <= 64);
    
    // Here we expand the loop if half_fanout <= size < _MAX_FANOUT.
    // Note that a_y[-1] is a sentinel. 
    if(size >= half_fanout && size < static_cast<int>(_MAX_FANOUT)) {
        // Exactly adjust size to half_fanout.
        if(cmp(input, a_y[half_fanout - 1]) > 0) {
            b = size - half_fanout;
        }
        
        switch(half_fanout) {
        case 32:
            // Narrow the range to a_y[b] < input <= a_y[b + 16].
            // Thus we have exactly 16 elements.
            if(cmp(input, a_y[b + 16]) > 0) {
                b += 16;
            }
        case 16:
            // Ensure that a_[b] < input <= a_y[b + 8].
            if(cmp(input, a_y[b + 8]) > 0) {
                b += 8;
            }
        case 8:
            // Ensure that a_[b] < input <= a_y[b + 4].
            if(cmp(input, a_y[b + 4]) > 0) {
                b += 4;
            }
        case 4:
            // Ensure that a_[b] < input <= a_y[b + 2].
            if(cmp(input, a_y[b + 2]) > 0) {
                b += 2;
            }
        case 2:
            // Ensure that a_[b] < input <= a_y[b + 1].
            if(cmp(input, a_y[b + 1]) > 0) {
                b += 1;
            }
        case 1:
            b += 1;
            break;
        default:
            ST_FATAL("Cannot reach here");
            break;
        }
        
        if(b >= size || cmp(input, a_y[b]) != 0) {
            return ~b;
        } else {
            return b;
        }
    } else {
        while ((e - b) > 1) {
            m = (e+b) >> 1;
            r = cmp (input, a_y[m]);
            if (0 == r) {
                return m;
            }
            if (r > 0) {
                b = m;
            } else {
                e = m;
            }
        }

        return ~e;
    }
}

template <typename _CompareXY, typename _X, typename _Y>
int array_binary_search_2 (const _X& input, const _Y* a_y, int size)
{
    _CompareXY cmp;
    // binary search in the range [0..size-1]
    int b = 0;
    int e = size*2;
    int r = -1;
    int m = size;
    
    do {
        if (r > 0) {
            b = m;
        } else {
            e = m;
        }
        //b += (r > 0) * (m - b);
        //e = (r < 0) ? m : e;
    }
    while ((r = cmp (input, a_y[m = (e+b) >> 1])) && (e > (b+1)));
    
    // e == b+1 and always being non-negative
    // n should be resident just before e and after b
    return r == 0 ? m : ~(m + (r>0));
}

// Copy an array to a far place which never intersect with the source
template <typename _T>
inline void array_copy_far (_T* p_far_dest,
                            const _T* p_begin, const _T* p_end)
{
    while (p_begin < p_end) {
        *(p_far_dest++) = *(p_begin++);
    }
}

// Copy an array to a place which may intersect with the source
template <typename _T>
inline void array_copy (_T* p_dest, const _T* p_begin, const _T* p_end)
{
    if (unlikely(p_dest == p_begin)) {
        return;
    }
    if (p_dest < p_begin) {
        while (p_begin < p_end) {
            *(p_dest++) = *(p_begin++);
        }
    }
    else {  // p_dest > p_begin
        p_dest += (p_end - p_begin);
        while (p_end > p_begin) {
            *(--p_dest) = *(--p_end);
        }
    }
}

// Erase a value from an array
// Returns: erased or not
template <typename _T>
inline bool array_erase (_T* p_begin, _T* p_end, _T* p_erase)
{
    if (p_erase < p_begin || p_erase >= p_end) {
        return false;
    }
    for (; p_erase < p_end-1; ++ p_erase) {
        *p_erase = *(p_erase+1);
    }
    return true;
}


// Copy an array to a far place with a value being erased.
// Note: the place must not intersect with the source, which is not changed
//       after invokation
// Note: nip is short for not-in-place
template <typename _T>
inline void array_nip_erase_far (_T* p_far_dest,
                                 const _T* p_begin, const _T* p_end,
                                 const _T* p_erase)
{
    while (p_begin < p_erase) {
        *(p_far_dest++) = *(p_begin++);
    }
    ++ p_begin;
    while (p_begin < p_end) {
        *(p_far_dest++) = *(p_begin++);
    }
}


// Copy an array to a far place with a value being erased.
// Note: the place may intersect with the sourc
// Note: nip is short for not-in-place
// Returns: erased or not
template <typename _T>
inline bool array_nip_erase (_T* p_dest,
                             const _T* p_begin, const _T* p_end,
                             const _T* p_erase)
{
    if (unlikely (p_erase < p_begin || p_erase >= p_end)) {
        array_copy (p_dest, p_begin, p_end);
        return false;
    }
    if (p_dest < p_begin) {
        array_nip_erase_far (p_dest, p_begin, p_end, p_erase);
    } else if (p_dest == p_begin) {
        p_dest += p_erase - p_begin;
        while (p_dest+1 < p_end) {
            *p_dest = *(p_dest+1);
            ++ p_dest;
        }
    } else if (p_dest == p_begin+1) {
        p_dest += p_erase - p_begin - 1;
        while (p_dest > p_begin) {
            *p_dest = *(p_dest-1);
            -- p_dest;
        }
    } else {
        p_dest += (p_end - p_begin - 1);
        while (p_end > p_erase+1) {
            *(--p_dest) = *(--p_end);
        }
        -- p_end;
        while (p_end > p_begin) {
            *(--p_dest) = *(--p_end);
        }
    }
    return true;
}

// Insert a value into an array
// Returns: inserted or not
template <typename _T>
inline bool array_insert (_T* p_begin, _T* p_end,
                          _T* p_insert, const _T& item)
{
    if (unlikely (p_insert < p_begin || p_insert > p_end)) {
        return false;
    }
    while (p_end > p_insert) {
        *p_end = *(p_end-1);
        -- p_end;
    }
    *p_end = item;
    return true;
}


// Copy an array to a far place with a value being inserted,
// Note: the place should not intersect with the source, which is not changed
// after invokation
// Note: nip is short for not-in-place
template <typename _T>
inline void array_nip_insert_far (_T* p_far_dest,
                                  const _T* p_begin, const _T* p_end,
                                  const _T* p_insert, const _T& item)
{
    while (p_begin < p_insert) {
        *(p_far_dest++) = *(p_begin++);
    }
    *(p_far_dest++) = item;
    while (p_begin < p_end) {
        *(p_far_dest++) = *(p_begin++);
    }
}


// Copy an array to a place with a value being inserted,
// the place may intersect with the source
// Note: nip is short not-in-place
// Returns: inserted or not
template <typename _T>
inline bool array_nip_insert (_T* p_dest,
                              const _T* p_begin, const _T* p_end,
                              const _T* p_insert, const _T& item)
{
    if (unlikely (p_insert < p_begin || p_insert > p_end)) {
        array_copy (p_dest, p_begin, p_end);
        return false;
    }
    if (p_dest < p_begin-1) {
        array_nip_insert_far (p_dest, p_begin, p_end, p_insert, item);
    } else if (p_dest == p_begin-1) {
        while (p_begin < p_insert) {
            *(p_dest++) = *(p_begin++);
        }
        *p_dest = item;
    } else if (p_dest == p_begin) {
        p_dest += p_end - p_begin;
        while (p_end > p_insert) {
            *(p_dest--) = *(--p_end);
        }
        *p_dest = item;
    } else {
        p_dest += (p_end - p_begin);
        while (p_end > p_insert) {
            *(p_dest--) = *(--p_end);
        }
        *(p_dest--) = item;
        while (p_end > p_begin) {
            *(p_dest--) = *(--p_end);
        }
    }
    return true;
}
}
    
#endif  //_ARRAY_OP_HPP_
