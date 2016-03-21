/*
 * Bitmap implementation.
 *
 * Copyright (C) 2011  Zi-long Tan (tzlloch@gmail.com)
 *
 * This file is part of ULIB.
 * 
 * ULIB is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * ULIB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "bitmap.h"

int __bitmap_empty(const unsigned long *bitmap, int bits)
{
    int k, lim = bits/BITS_PER_LONG;
    for (k = 0; k < lim; ++k)
        if (bitmap[k])
            return 0;

    if (bits % BITS_PER_LONG)
        if (bitmap[k] & BITMAP_LAST_WORD_MASK(bits))
            return 0;

    return 1;
}

int __bitmap_full(const unsigned long *bitmap, int bits)
{
    int k, lim = bits/BITS_PER_LONG;
    for (k = 0; k < lim; ++k)
        if (~bitmap[k])
            return 0;

    if (bits % BITS_PER_LONG)
        if (~bitmap[k] & BITMAP_LAST_WORD_MASK(bits))
            return 0;

    return 1;
}

int __bitmap_equal(const unsigned long *bitmap1,
		   const unsigned long *bitmap2, int bits)
{
    int k, lim = bits/BITS_PER_LONG;
    for (k = 0; k < lim; ++k)
        if (bitmap1[k] != bitmap2[k])
            return 0;

    if (bits % BITS_PER_LONG)
        if ((bitmap1[k] ^ bitmap2[k]) & BITMAP_LAST_WORD_MASK(bits))
            return 0;

    return 1;
}

void __bitmap_complement(unsigned long *dst, const unsigned long *src, int bits)
{
    int k, lim = bits/BITS_PER_LONG;
    for (k = 0; k < lim; ++k)
        dst[k] = ~src[k];

    if (bits % BITS_PER_LONG)
        dst[k] = ~src[k] & BITMAP_LAST_WORD_MASK(bits);
}

/**
 * __bitmap_shift_right - logical right shift of the bits in a bitmap
 *   @dst : destination bitmap
 *   @src : source bitmap
 *   @shift : shift by this many bits
 *   @bits : bitmap size, in bits
 *
 * Shifting right (dividing) means moving bits in the MS -> LS bit
 * direction.  Zeros are fed into the vacated MS positions and the
 * LS bits shifted off the bottom are lost.
 */
void __bitmap_shift_right(unsigned long *dst,
			  const unsigned long *src, int shift, int bits)
{
    int k, lim = BITS_TO_LONGS(bits), left = bits % BITS_PER_LONG;
    int off = shift/BITS_PER_LONG, rem = shift % BITS_PER_LONG;
    unsigned long mask = (1UL << left) - 1;
    for (k = 0; off + k < lim; ++k) {
        unsigned long upper, lower;

        /*
         * If shift is not word aligned, take lower rem bits of
         * word above and make them the top rem bits of result.
         */
        if (!rem || off + k + 1 >= lim)
            upper = 0;
        else {
            upper = src[off + k + 1];
            if (off + k + 1 == lim - 1 && left)
                upper &= mask;
        }
        lower = src[off + k];
        if (left && off + k == lim - 1)
            lower &= mask;
        dst[k] = upper << (BITS_PER_LONG - rem) | lower >> rem;
        if (left && k == lim - 1)
            dst[k] &= mask;
    }
    if (off)
        memset(&dst[lim - off], 0, off*sizeof(unsigned long));
}

/**
 * __bitmap_shift_left - logical left shift of the bits in a bitmap
 *   @dst : destination bitmap
 *   @src : source bitmap
 *   @shift : shift by this many bits
 *   @bits : bitmap size, in bits
 *
 * Shifting left (multiplying) means moving bits in the LS -> MS
 * direction.  Zeros are fed into the vacated LS bit positions
 * and those MS bits shifted off the top are lost.
 */

void __bitmap_shift_left(unsigned long *dst,
			 const unsigned long *src, int shift, int bits)
{
    int k, lim = BITS_TO_LONGS(bits), left = bits % BITS_PER_LONG;
    int off = shift/BITS_PER_LONG, rem = shift % BITS_PER_LONG;
    for (k = lim - off - 1; k >= 0; --k) {
        unsigned long upper, lower;

        /*
         * If shift is not word aligned, take upper rem bits of
         * word below and make them the bottom rem bits of result.
         */
        if (rem && k > 0)
            lower = src[k - 1];
        else
            lower = 0;
        upper = src[k];
        if (left && k == lim - 1)
            upper &= (1UL << left) - 1;
        dst[k + off] = lower  >> (BITS_PER_LONG - rem) | upper << rem;
        if (left && k + off == lim - 1)
            dst[k + off] &= (1UL << left) - 1;
    }
    if (off)
        memset(dst, 0, off*sizeof(unsigned long));
}

int __bitmap_and(unsigned long *dst, const unsigned long *bitmap1,
		 const unsigned long *bitmap2, int bits)
{
    int k;
    int nr = BITS_TO_LONGS(bits);
    unsigned long result = 0;

    for (k = 0; k < nr; k++)
        result |= (dst[k] = bitmap1[k] & bitmap2[k]);
    return result != 0;
}

void __bitmap_or(unsigned long *dst, const unsigned long *bitmap1,
		 const unsigned long *bitmap2, int bits)
{
    int k;
    int nr = BITS_TO_LONGS(bits);

    for (k = 0; k < nr; k++)
        dst[k] = bitmap1[k] | bitmap2[k];
}

void __bitmap_xor(unsigned long *dst, const unsigned long *bitmap1,
		  const unsigned long *bitmap2, int bits)
{
    int k;
    int nr = BITS_TO_LONGS(bits);

    for (k = 0; k < nr; k++)
        dst[k] = bitmap1[k] ^ bitmap2[k];
}

int __bitmap_andnot(unsigned long *dst, const unsigned long *bitmap1,
		    const unsigned long *bitmap2, int bits)
{
    int k;
    int nr = BITS_TO_LONGS(bits);
    unsigned long result = 0;

    for (k = 0; k < nr; k++)
        result |= (dst[k] = bitmap1[k] & ~bitmap2[k]);
    return result != 0;
}

int __bitmap_intersects(const unsigned long *bitmap1,
			const unsigned long *bitmap2, int bits)
{
    int k, lim = bits/BITS_PER_LONG;
    for (k = 0; k < lim; ++k)
        if (bitmap1[k] & bitmap2[k])
            return 1;

    if (bits % BITS_PER_LONG)
        if ((bitmap1[k] & bitmap2[k]) & BITMAP_LAST_WORD_MASK(bits))
            return 1;
    return 0;
}

int __bitmap_subset(const unsigned long *bitmap1,
		    const unsigned long *bitmap2, int bits)
{
    int k, lim = bits/BITS_PER_LONG;
    for (k = 0; k < lim; ++k)
        if (bitmap1[k] & ~bitmap2[k])
            return 0;

    if (bits % BITS_PER_LONG)
        if ((bitmap1[k] & ~bitmap2[k]) & BITMAP_LAST_WORD_MASK(bits))
            return 0;
    return 1;
}

int __bitmap_weight(const unsigned long *bitmap, int bits)
{
    int k, w = 0, lim = bits/BITS_PER_LONG;

    for (k = 0; k < lim; k++)
        w += __builtin_popcountl(bitmap[k]);

    if (bits % BITS_PER_LONG)
        w += __builtin_popcountl(bitmap[k] & BITMAP_LAST_WORD_MASK(bits));

    return w;
}

#define BITMAP_FIRST_WORD_MASK(start) (~0UL << ((start) % BITS_PER_LONG))

void bitmap_set(unsigned long *map, int start, int nr)
{
    unsigned long *p = map + BIT_WORD(start);
    const int size = start + nr;
    int bits_to_set = BITS_PER_LONG - (start % BITS_PER_LONG);
    unsigned long mask_to_set = BITMAP_FIRST_WORD_MASK(start);

    while (nr - bits_to_set >= 0) {
        *p |= mask_to_set;
        nr -= bits_to_set;
        bits_to_set = BITS_PER_LONG;
        mask_to_set = ~0UL;
        p++;
    }
    if (nr) {
        mask_to_set &= BITMAP_LAST_WORD_MASK(size);
        *p |= mask_to_set;
    }
}

void bitmap_clear(unsigned long *map, int start, int nr)
{
    unsigned long *p = map + BIT_WORD(start);
    const int size = start + nr;
    int bits_to_clear = BITS_PER_LONG - (start % BITS_PER_LONG);
    unsigned long mask_to_clear = BITMAP_FIRST_WORD_MASK(start);

    while (nr - bits_to_clear >= 0) {
        *p &= ~mask_to_clear;
        nr -= bits_to_clear;
        bits_to_clear = BITS_PER_LONG;
        mask_to_clear = ~0UL;
        p++;
    }
    if (nr) {
        mask_to_clear &= BITMAP_LAST_WORD_MASK(size);
        *p &= ~mask_to_clear;
    }
}

/**
 * bitmap_fold - fold larger bitmap into smaller, modulo specified size
 *	@dst: resulting smaller bitmap
 *	@orig: original larger bitmap
 *	@sz: specified size
 *	@bits: number of bits in each of these bitmaps
 *
 * For each bit oldbit in @orig, set bit oldbit mod @sz in @dst.
 * Clear all other bits in @dst.  See further the comment and
 * Example [2] for bitmap_onto() for why and how to use this.
 */
void bitmap_fold(unsigned long *dst, const unsigned long *orig,
		 int sz, int bits)
{
    int oldbit;

    if (dst == orig)	/* following doesn't handle inplace mappings */
        return;
    bitmap_zero(dst, bits);

    for_each_set_bit(oldbit, orig, bits)
        set_bit(oldbit % sz, dst);
}
