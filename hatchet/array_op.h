// Copyright (c); 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Search/copy/insert/erase arrays
// Author: jiangrujie@baidu.com
// Date: Fri Dec 16 11:52:37 CST 2010

#pragma once
#ifndef _ARRAY_OP_H_
#define _ARRAY_OP_H_

#include "debug.h"

namespace st {

// Function to copy an item of a certain type 
// to a given address
typedef void (*CopyFn)(void*, const void*);    

// Function to Compare two items of a certain type.
// Returns 1, 0, -1 if the left value is greater than,
// equal to, or less than the right value.
typedef int (*CmpFn)(const void*, const void*);

// This binary search function returns the array position of value
// if it has been found. Otherwise, the function returns a negative
// value pos, where ~pos is the exact insert-position for the 
// value in the sorted array. Since we found out this is the 
// hot spot for cowbass4, we also made some optimizations here.
template<class _Comparator>
inline int array_binary_search (const void* input, const char* a_y, 
                                u_int item_size, u_int arr_size,
                                u_int max_fanout)
{
    _Comparator cmp_fn;
    
    int b = -1;
    int e = arr_size;
    int mid, ret;
    u_int half_fanout = max_fanout >> 1;

#ifndef NDEBUG    
    // _MAX_FANOUT must be power of 2 and less than or equal 64.
    assert ((max_fanout & max_fanout - 1) == 0);
    assert (max_fanout <= 64);
#endif
    
    // Here we expand the loop if half_fanout <= size < _MAX_FANOUT.
    // Note that a_y[-1] is a sentinel. 
    if (arr_size >= half_fanout && arr_size < max_fanout) {
        // Exactly adjust size to half_fanout.
        if (cmp_fn (input, a_y + (half_fanout - 1) * item_size) > 0) {
            b = arr_size - half_fanout;
        }
        
        switch (half_fanout) {
        case 32:
            // Narrow the range to a_y[b] < input <= a_y[b + 16].
            // Thus we have exactly 16 elements.
            if (cmp_fn (input, a_y + (b + 16) * item_size) > 0) {
                b += 16;
            }
        case 16:
            // Ensure that a_[b] < input <= a_y[b + 8].
            if (cmp_fn (input, a_y + (b + 8) * item_size) > 0) {
                b += 8;
            }
        case 8:
            // Ensure that a_[b] < input <= a_y[b + 4].
            if (cmp_fn (input, a_y + (b + 4) * item_size) > 0) {
                b += 4;
            }
        case 4:
            // Ensure that a_[b] < input <= a_y[b + 2].
            if (cmp_fn (input, a_y + (b + 2) * item_size) > 0) {
                b += 2;
            }
        case 2:
            // Ensure that a_[b] < input <= a_y[b + 1].
            if (cmp_fn (input, a_y + (b + 1) * item_size) > 0) {
                b += 1;
            }
        case 1:
            b += 1;
            break;
        default:
            ST_FATAL("Unexpected max_fanout");
            break;
        }
        
        if (b >= static_cast<int>(arr_size) || 
                cmp_fn (input, a_y + b * item_size) != 0) {
            return ~b;
        } else {
            return b;
        }
    } else {
        while ((e - b) > 1) {
            mid = (e+b) >> 1;
            ret = cmp_fn (input, a_y + mid * item_size);
            if (0 == ret) {
                return mid;
            }
            if (ret > 0) {
                b = mid;
            } else {
                e = mid;
            }
        }

        return ~e;
    }
}
    
// Copy an array to a far place which never intersect with the source
void array_copy_far (char* p_far_dest,
                     const char* p_begin, const char* p_end,
                     u_int size, CopyFn copy_fn);

// Copy an array to a place which may intersect with the source
void array_copy (char* p_dest, const char* p_begin, const char* p_end,
                 u_int size, CopyFn copy_fn);

// Erase a value from an array
// Returns: erased or not
bool array_erase (char* p_begin, char* p_end, char* p_erase,
                         u_int size, CopyFn copy_fn);

// Copy an array to a far place with a value being erased.
// Note: the place must not intersect with the source, which is not changed
//       after invokation
// Note: nip is short for not-in-place
void array_nip_erase_far (char* p_far_dest,
                          const char* p_begin, const char* p_end,
                          const char* p_erase,
                          u_int size, CopyFn copy_fn);

// Copy an array to a far place with a value being erased.
// Note: the place may intersect with the sourc
// Note: nip is short for not-in-place
// Returns: erased or not
bool array_nip_erase (char* p_dest,
                      const char* p_begin, const char* p_end,
                      const char* p_erase,
                      u_int size, CopyFn copy_fn);

// Insert a value into an array
// Returns: inserted or not
bool array_insert (char* p_begin, char* p_end,
                   char* p_insert, const void* item,
                   u_int size, CopyFn copy_fn);

// Copy an array to a far place with a value being inserted,
// Note: the place should not intersect with the source, which is not changed
// after invokation
// Note: nip is short for not-in-place
void array_nip_insert_far (char* p_far_dest,
                           const char* p_begin, const char* p_end,
                           const char* p_insert, const void* item,
                           u_int size, CopyFn copy_fn);

// Copy an array to a place with a value being inserted,
// the place may intersect with the source
// Note: nip is short not-in-place
// Returns: inserted or not
bool array_nip_insert (char* p_dest,
                       const char* p_begin, const char* p_end,
                       const char* p_insert, const void* item,
                       u_int size, CopyFn copy_fn);

}   // end namespace st

#endif  //_ARRAY_OP_H_
