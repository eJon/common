// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Search/copy/insert/erase arrays
// Author: gejun@baidu.com
// Date: Fri Dec 16 11:52:37 CST 2010

#include "array_op.h"  

namespace st {

    /*
template<class _Comparator>
int array_binary_search (const void* input, const char* a_y, 
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
*/
void array_copy_far (char* p_far_dest,
                     const char* p_begin, const char* p_end,
                     u_int size, CopyFn copy_fn)
{
    while (p_begin < p_end) {
        copy_fn (p_far_dest, p_begin);
        p_far_dest += size;
        p_begin += size;
    }
}

void array_copy (char* p_dest, const char* p_begin, const char* p_end,
                 u_int size, CopyFn copy_fn)
{
    if (unlikely (p_dest == p_begin)) {
        return;
    }
    if (p_dest < p_begin) {
        while (p_begin < p_end) {
            copy_fn (p_dest, p_begin);
            p_dest += size;
            p_begin += size;
        }
    } else {  // p_dest > p_begin
        p_dest += (p_end - p_begin);
        while (p_end > p_begin) {
            p_dest -= size;
            p_end -= size;
            copy_fn (p_dest, p_end);
        }
    }
}

bool array_erase (char* p_begin, char* p_end, char* p_erase,
                  u_int size, CopyFn copy_fn)
{
    if (p_erase < p_begin || p_erase >= p_end) {
        return false;
    }
    for (; p_erase < p_end - size; p_erase += size) {
        copy_fn (p_erase, p_erase + size);
    }
    return true;
}

void array_nip_erase_far (char* p_far_dest,
                          const char* p_begin, const char* p_end,
                          const char* p_erase,
                          u_int size, CopyFn copy_fn)
{
    while (p_begin < p_erase) {
        copy_fn (p_far_dest, p_begin);
        p_far_dest += size;
        p_begin += size;
    }
    p_begin += size;
    while (p_begin < p_end) {
        copy_fn (p_far_dest, p_begin);
        p_far_dest += size;
        p_begin += size;
    }
}

bool array_nip_erase (char* p_dest,
                      const char* p_begin, const char* p_end,
                      const char* p_erase,
                      u_int size, CopyFn copy_fn)
{
    if (unlikely (p_erase < p_begin || p_erase >= p_end)) {
        array_copy (p_dest, p_begin, p_end, size, copy_fn);
        return false;
    }
    if (p_dest < p_begin) {
        array_nip_erase_far (p_dest, p_begin, p_end, p_erase,
                             size, copy_fn);
    } else if (p_dest == p_begin) {
        p_dest += p_erase - p_begin;
        while (p_dest + size < p_end) {
            copy_fn (p_dest, p_dest + size);
            p_dest += size;
        }
    } else if (p_dest == p_begin + size) {
        p_dest += p_erase - p_begin - size;
        while (p_dest > p_begin) {
            copy_fn (p_dest, p_dest - size);
            p_dest -= size;
        }
    } else {
        p_dest += (p_end - p_begin - size);
        while (p_end > p_erase + size) {
            p_dest -= size;
            p_end -= size;
            copy_fn (p_dest, p_end);
        }
        p_end -= size;
        while (p_end > p_begin) {
            p_dest -= size;
            p_end -= size;
            copy_fn (p_dest, p_end);
        }
    }
    return true;
}

// Insert a value into an array
// Returns: inserted or not
bool array_insert (char* p_begin, char* p_end,
                   char* p_insert, const void* item,
                   u_int size, CopyFn copy_fn)
{
    if (unlikely (p_insert < p_begin || p_insert > p_end)) {
        return false;
    }
    while (p_end > p_insert) {
        copy_fn (p_end, p_end - size);
        p_end -= size;
    }
    copy_fn (p_end, item);
    return true;
}

void array_nip_insert_far (char* p_far_dest,
                           const char* p_begin, const char* p_end,
                           const char* p_insert, const void* item,
                                  u_int size, CopyFn copy_fn)
{
    while (p_begin < p_insert) {
        copy_fn (p_far_dest, p_begin);
        p_far_dest += size;
        p_begin += size;
    }
    copy_fn (p_far_dest, item);
    p_far_dest += size;
    while (p_begin < p_end) {
        copy_fn (p_far_dest, p_begin);
        p_far_dest += size;
        p_begin += size;
    }
}

bool array_nip_insert (char* p_dest,
                       const char* p_begin, const char* p_end,
                       const char* p_insert, const void* item,
                       u_int size, CopyFn copy_fn)
{
    if (unlikely (p_insert < p_begin || p_insert > p_end)) {
        array_copy (p_dest, p_begin, p_end, size, copy_fn);
        return false;
    }
    if (p_dest < p_begin - size) {
        array_nip_insert_far (p_dest, p_begin, p_end, p_insert, item,
                              size, copy_fn);
    } else if (p_dest == p_begin - size) {
        while (p_begin < p_insert) {
            copy_fn (p_dest, p_begin);
            p_dest += size;
            p_begin += size;
        }
        copy_fn (p_dest, item);
    } else if (p_dest == p_begin) {
        p_dest += p_end - p_begin;
        while (p_end > p_insert) {
            p_end -= size;
            copy_fn (p_dest, p_end);
            p_dest -= size;
        }
        copy_fn (p_dest, item);
    } else {
        p_dest += (p_end - p_begin);
        while (p_end > p_insert) {
            p_end -= size;
            copy_fn (p_dest, p_end);
            p_dest -= size;
        }
        copy_fn (p_dest, item);
        p_dest -= size;
        while (p_end > p_begin) {
            p_end -= size;
            copy_fn (p_dest, p_end);
            p_dest -= size;
        }
    }
    return true;
}

}   // end namespace st
