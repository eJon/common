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

/*
 * bitmaps provide an array of bits, implemented using an an
 * array of unsigned longs.  The number of valid bits in a
 * given bitmap does _not_ need to be an exact multiple of
 * BITS_PER_LONG.
 *
 * The possible unused bits in the last, partially used word
 * of a bitmap are 'don't care'.  The implementation makes
 * no particular effort to keep them zero.  It ensures that
 * their value will not affect the results of any operation.
 * The bitmap operations that return Boolean (bitmap_empty,
 * for example) or scalar (bitmap_weight, for example) results
 * carefully filter out these unused bits from impacting their
 * results.
 *
 * These operations actually hold to a slightly stronger rule:
 * if you don't input any bitmaps to these ops that have some
 * unused bits set, then they won't output any set unused bits
 * in output bitmaps.
 */

#ifndef __ULIB_BITMAP_H
#define __ULIB_BITMAP_H

#include <string.h>
#include "detail/bitop.h"

/*
 * The available bitmap operations and their rough meaning in the
 * case that the bitmap is a single unsigned long are thus:
 *
 * Note that nbits should be always a compile time evaluable constant.
 * Otherwise many inlines will generate horrible code.
 *
 * bitmap_zero(dst, nbits)			*dst = 0UL
 * bitmap_fill(dst, nbits)			*dst = ~0UL
 * bitmap_copy(dst, src, nbits)			*dst = *src
 * bitmap_and(dst, src1, src2, nbits)		*dst = *src1 & *src2
 * bitmap_or(dst, src1, src2, nbits)		*dst = *src1 | *src2
 * bitmap_xor(dst, src1, src2, nbits)		*dst = *src1 ^ *src2
 * bitmap_andnot(dst, src1, src2, nbits)	*dst = *src1 & ~(*src2)
 * bitmap_complement(dst, src, nbits)		*dst = ~(*src)
 * bitmap_equal(src1, src2, nbits)		Are *src1 and *src2 equal?
 * bitmap_intersects(src1, src2, nbits) 	Do *src1 and *src2 overlap?
 * bitmap_subset(src1, src2, nbits)		Is *src1 a subset of *src2?
 * bitmap_empty(src, nbits)			Are all bits zero in *src?
 * bitmap_full(src, nbits)			Are all bits set in *src?
 * bitmap_weight(src, nbits)			Hamming Weight: number set bits
 * bitmap_set(dst, pos, nbits)			Set specified bit area
 * bitmap_clear(dst, pos, nbits)		Clear specified bit area
 * bitmap_shift_right(dst, src, n, nbits)	*dst = *src >> n
 * bitmap_shift_left(dst, src, n, nbits)	*dst = *src << n
 * bitmap_fold(dst, orig, sz, nbits)		dst bits = orig bits mod sz
 *
 * Also the bit operations in bitop.h apply to bitmaps.
 */

#define DECLARE_BITMAP(name,bits)		\
    unsigned long name[BITS_TO_LONGS(bits)]

#ifdef __cplusplus
extern "C" {
#endif

    extern int __bitmap_empty(const unsigned long *bitmap, int bits);
    extern int __bitmap_full(const unsigned long *bitmap, int bits);
    extern int __bitmap_equal(const unsigned long *bitmap1,
                              const unsigned long *bitmap2, int bits);
    extern void __bitmap_complement(unsigned long *dst, const unsigned long *src,
                                    int bits);
    extern void __bitmap_shift_right(unsigned long *dst,
                                     const unsigned long *src, int shift, int bits);
    extern void __bitmap_shift_left(unsigned long *dst,
                                    const unsigned long *src, int shift, int bits);
    extern int __bitmap_and(unsigned long *dst, const unsigned long *bitmap1,
                            const unsigned long *bitmap2, int bits);
    extern void __bitmap_or(unsigned long *dst, const unsigned long *bitmap1,
                            const unsigned long *bitmap2, int bits);
    extern void __bitmap_xor(unsigned long *dst, const unsigned long *bitmap1,
                             const unsigned long *bitmap2, int bits);
    extern int __bitmap_andnot(unsigned long *dst, const unsigned long *bitmap1,
                               const unsigned long *bitmap2, int bits);
    extern int __bitmap_intersects(const unsigned long *bitmap1,
                                   const unsigned long *bitmap2, int bits);
    extern int __bitmap_subset(const unsigned long *bitmap1,
                               const unsigned long *bitmap2, int bits);
    extern int __bitmap_weight(const unsigned long *bitmap, int bits);

    extern void bitmap_set(unsigned long *map, int i, int len);
    extern void bitmap_clear(unsigned long *map, int start, int nr);

    extern void bitmap_fold(unsigned long *dst, const unsigned long *orig,
                            int sz, int bits);

#ifdef __cplusplus
}
#endif

#define BITMAP_LAST_WORD_MASK(nbits)                                    \
    (                                                                   \
        ((nbits) % BITS_PER_LONG) ?                                     \
        (1UL<<((nbits) % BITS_PER_LONG))-1 : ~0UL                       \
                                                                )

#define small_const_nbits(nbits)                                \
    (__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG)

static inline void bitmap_zero(unsigned long *dst, int nbits)
{
    if (small_const_nbits(nbits))
        *dst = 0UL;
    else {
        int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
        memset(dst, 0, len);
    }
}

static inline void bitmap_fill(unsigned long *dst, int nbits)
{
    size_t nlongs = BITS_TO_LONGS(nbits);
    if (!small_const_nbits(nbits)) {
        int len = (nlongs - 1) * sizeof(unsigned long);
        memset(dst, 0xff,  len);
    }
    dst[nlongs - 1] = BITMAP_LAST_WORD_MASK(nbits);
}

static inline void bitmap_copy(unsigned long *dst, const unsigned long *src,
			       int nbits)
{
    if (small_const_nbits(nbits))
        *dst = *src;
    else {
        int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
        memcpy(dst, src, len);
    }
}

static inline int bitmap_and(unsigned long *dst, const unsigned long *src1,
			     const unsigned long *src2, int nbits)
{
    if (small_const_nbits(nbits))
        return (*dst = *src1 & *src2) != 0;
    return __bitmap_and(dst, src1, src2, nbits);
}

static inline void bitmap_or(unsigned long *dst, const unsigned long *src1,
			     const unsigned long *src2, int nbits)
{
    if (small_const_nbits(nbits))
        *dst = *src1 | *src2;
    else
        __bitmap_or(dst, src1, src2, nbits);
}

static inline void bitmap_xor(unsigned long *dst, const unsigned long *src1,
			      const unsigned long *src2, int nbits)
{
    if (small_const_nbits(nbits))
        *dst = *src1 ^ *src2;
    else
        __bitmap_xor(dst, src1, src2, nbits);
}

static inline int bitmap_andnot(unsigned long *dst, const unsigned long *src1,
				const unsigned long *src2, int nbits)
{
    if (small_const_nbits(nbits))
        return (*dst = *src1 & ~(*src2)) != 0;
    return __bitmap_andnot(dst, src1, src2, nbits);
}

static inline void bitmap_complement(unsigned long *dst, const unsigned long *src,
				     int nbits)
{
    if (small_const_nbits(nbits))
        *dst = ~(*src) & BITMAP_LAST_WORD_MASK(nbits);
    else
        __bitmap_complement(dst, src, nbits);
}

static inline int bitmap_equal(const unsigned long *src1,
			       const unsigned long *src2, int nbits)
{
    if (small_const_nbits(nbits))
        return ! ((*src1 ^ *src2) & BITMAP_LAST_WORD_MASK(nbits));
    else
        return __bitmap_equal(src1, src2, nbits);
}

static inline int bitmap_intersects(const unsigned long *src1,
				    const unsigned long *src2, int nbits)
{
    if (small_const_nbits(nbits))
        return ((*src1 & *src2) & BITMAP_LAST_WORD_MASK(nbits)) != 0;
    else
        return __bitmap_intersects(src1, src2, nbits);
}

static inline int bitmap_subset(const unsigned long *src1,
				const unsigned long *src2, int nbits)
{
    if (small_const_nbits(nbits))
        return ! ((*src1 & ~(*src2)) & BITMAP_LAST_WORD_MASK(nbits));
    else
        return __bitmap_subset(src1, src2, nbits);
}

static inline int bitmap_empty(const unsigned long *src, int nbits)
{
    if (small_const_nbits(nbits))
        return ! (*src & BITMAP_LAST_WORD_MASK(nbits));
    else
        return __bitmap_empty(src, nbits);
}

static inline int bitmap_full(const unsigned long *src, int nbits)
{
    if (small_const_nbits(nbits))
        return ! (~(*src) & BITMAP_LAST_WORD_MASK(nbits));
    else
        return __bitmap_full(src, nbits);
}

static inline int bitmap_weight(const unsigned long *src, int nbits)
{
    if (small_const_nbits(nbits))
        return __builtin_popcountl(*src & BITMAP_LAST_WORD_MASK(nbits));
    return __bitmap_weight(src, nbits);
}

static inline void bitmap_shift_right(unsigned long *dst,
				      const unsigned long *src, int n, int nbits)
{
    if (small_const_nbits(nbits))
        *dst = *src >> n;
    else
        __bitmap_shift_right(dst, src, n, nbits);
}

static inline void bitmap_shift_left(unsigned long *dst,
				     const unsigned long *src, int n, int nbits)
{
    if (small_const_nbits(nbits))
        *dst = (*src << n) & BITMAP_LAST_WORD_MASK(nbits);
    else
        __bitmap_shift_left(dst, src, n, nbits);
}

#endif /* __ULIB_BITMAP_H */
