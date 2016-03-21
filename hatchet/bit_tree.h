// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// A full tree specialized for storing bits and finding first zero
// Author: gejun@baidu.com
// Date: Dec 28 14:45:30 CST 2010
#pragma once
#ifndef _BIT_TREE_H_
#define _BIT_TREE_H_

#include <limits.h>
#include <new>
#include "common.h"
#include "c_math.hpp"

namespace st {
class BitTree {
public:
    typedef unsigned long Element;
    static const int N_ELEM_BIT = sizeof(Element)*CHAR_BIT;
    static const int N_ELEM_SHIFT = bit_shift<N_ELEM_BIT>::R;
    static const Element N_ELEM_MASK = ((Element)1 << N_ELEM_SHIFT)-1;
        
public:
    explicit BitTree (u_int len)
    {
        orig_len_ = len;

        level_ = -1;
        for (; len; ++ level_, len /= N_ELEM_BIT);

        n_meta_bit_ = 0;
        for (int i=0; i<level_; ++i) {
            n_meta_bit_ = (n_meta_bit_ + 1) * N_ELEM_BIT;
        }
        n_meta_elem_ = n_meta_bit_ / N_ELEM_BIT;

        u_int tmp = orig_len_/N_ELEM_BIT;
        n_elem_ = n_meta_elem_
            + ((tmp*N_ELEM_BIT == orig_len_) ? tmp : (tmp+1));

        a_elem_ = new (std::nothrow) Element[n_elem_];
        if (NULL == a_elem_) {
            ST_FATAL ("Fail to new a_elem_");
            return;
        }

        clear();
    }

    ~BitTree ()
    {
        if (a_elem_) {
            delete [] a_elem_;
            a_elem_ = NULL;
        }
    }

    void clear ()
    {
        memset (a_elem_, 0, n_elem_*sizeof(Element));
    }

    void set (const int idx, const bool x)
    {
        int idx2 = n_meta_bit_ + idx;

        while (idx2 >= 0) {
            Element& e = a_elem_[(idx2 >> N_ELEM_SHIFT)];
            const Element e2 = x
                ? (e | ((Element)1 << (idx2 & N_ELEM_MASK)))
                : (e & ~((Element)1 << (idx2 & N_ELEM_MASK)));
                
            if (~ (x ? e2 : e) || e2 == e) {
                e = e2;
                return;
            }
                
            e = e2;
            idx2 = (idx2 >> N_ELEM_SHIFT) - 1;                
        }
    }

    bool get (const int idx) const
    {
        const int idx2 = n_meta_bit_ + idx;
        return (a_elem_[(idx2 >> N_ELEM_SHIFT)] >> (idx2 & N_ELEM_MASK)) & 0x1;
    }

    bool get_raw (const int idx) const
    {
        return (a_elem_[(idx >> N_ELEM_SHIFT)] >> (idx & N_ELEM_MASK)) & 0x1;
    }
        
    u_int capacity () const
    { return (n_elem_ - n_meta_elem_) * N_ELEM_BIT; }

    int find_first_zero () const
    {
        //const Element v = ~a_elem_[0];
        if (unlikely(!(~a_elem_[0]))) {
            return orig_len_;
        }
        u_int idx = __builtin_ctzl(~a_elem_[0]) + 1;
        while (idx < n_elem_) {
            idx = (idx << N_ELEM_SHIFT) + __builtin_ctzl(~a_elem_[idx]) + 1;
        }
        return idx - n_meta_bit_ - 1;
    }

    void to_string (StringWriter& sw) const
    {
        int idx = 0;
        int level_len = N_ELEM_BIT;
        for (int j=0; j<level_; ++j) {
            sw << "level" << j << "(" << level_len << "): ";
            for (int i=idx; i<idx+level_len; ++i) {
                if (i && (0 == (i & 7))) {
                    sw << " | ";
                }
                sw << (get_raw(i) ? 1 : 0);
            }
            idx += level_len;
            level_len <<= N_ELEM_SHIFT;
            sw << "\n";
        }
        const int elem_end = n_elem_*N_ELEM_BIT;
        sw << "items(" << elem_end-idx << "): ";
        for (int i=idx; i<elem_end; ++i) {
            if (i && (0 == (i & 7))) {
                sw << " | ";
            }
            sw << (get_raw(i) ? 1 : 0);
        }
    }

    Element* a_elem_;
    u_int n_elem_;
    u_int n_meta_bit_;
    u_int n_meta_elem_;
    u_int orig_len_;
    int level_;
};
    
}

#endif  // _BIT_TREE_H_
