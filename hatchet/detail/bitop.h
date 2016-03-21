/*
 * Bit operations.
 *
 * Copyright (C) 2010  Zi-long Tan (tzlloch@gmail.com)
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

#ifndef __ULIB_BIT_H
#define __ULIB_BIT_H

#include <stdint.h>

#define BITS_PER_BYTE       8
#define BITS_PER_LONG       ((int)(sizeof(long) * BITS_PER_BYTE))

#define DIV_ROUND_UP(n,d)   (((n) + (d) - 1) / (d))
#define BITS_TO_LONGS(nr)   DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))
#define BIT_WORD(nr)        ((nr) / BITS_PER_LONG)
#define BIT_MASK(nr)        (1UL << ((nr) % BITS_PER_LONG))
#define ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define ALIGN(x, a)         ALIGN_MASK(x, (typeof(x))(a) - 1)
#define ROR64(x, r)         ((x) >> (r) | (x) << (64 - (r)))

/* round integer to power of 2 boundary */
#define ROUND_UP32(x) ({                                        \
            (x)--;                                              \
            (x) |= (x) >> 1;                                    \
            (x) |= (x) >> 2;                                    \
            (x) |= (x) >> 4;                                    \
            (x) |= (x) >> 8;                                    \
            (x) |= (x) >> 16;                                   \
            (x)++;                                              \
        })

/* round integer to power of 2 boundary */
#define ROUND_UP64(x) ({                                        \
            (x)--;                                              \
            (x) |= (x) >> 1;                                    \
            (x) |= (x) >> 2;                                    \
            (x) |= (x) >> 4;                                    \
            (x) |= (x) >> 8;                                    \
            (x) |= (x) >> 16;                                   \
            (x) |= (x) >> 32;                                   \
            (x)++;                                              \
        })

static inline void set_bit(int nr, volatile unsigned long *addr)
{
    *(addr + BIT_WORD(nr)) |= BIT_MASK(nr);
}

static inline void clear_bit(int nr, volatile unsigned long *addr)
{
    *(addr + BIT_WORD(nr)) &= ~BIT_MASK(nr);
}

static inline void change_bit(int nr, volatile unsigned long *addr)
{
    *(addr + BIT_WORD(nr)) ^= BIT_MASK(nr);
}

static inline int test_bit(int nr, const volatile unsigned long *addr)
{
    return 1UL & (addr[BIT_WORD(nr)] >> (nr & (BITS_PER_LONG-1)));
}

/**
 * find_first_bit - finds the first set bit
 * @addr: memory region
 * @size: size of memory
 */
static inline unsigned long
find_first_bit(const unsigned long *addr, unsigned long size)
{
    const unsigned long *p = addr;
    unsigned long result = 0;
    unsigned long tmp;

    while (size & ~(BITS_PER_LONG-1)) {
        if ((tmp = *(p++)))
            goto found;
        result += BITS_PER_LONG;
        size -= BITS_PER_LONG;
    }
    if (!size)
        return result;

    tmp = (*p) & (~0UL >> (BITS_PER_LONG - size));
    if (tmp == 0UL)         /* Are any bits set? */
        return result + size;   /* Nope. */
found:
    return result + __builtin_ffsl(tmp) - 1;
}

/**
 * find_first_bit - finds the next set bit
 * @addr: memory region
 * @size: size of memory
 */
static inline unsigned long
find_next_bit(const unsigned long *addr, unsigned long size, unsigned long offset)
{
    const unsigned long *p = addr + BIT_WORD(offset);
    unsigned long result = offset & ~(BITS_PER_LONG-1);
    unsigned long tmp;

    if (offset >= size)
        return size;
    size -= result;
    offset %= BITS_PER_LONG;
    if (offset) {
        tmp = *(p++);
        tmp &= (~0UL << offset);
        if (size < BITS_PER_LONG)
            goto found_first;
        if (tmp)
            goto found_middle;
        size -= BITS_PER_LONG;
        result += BITS_PER_LONG;
    }
    while (size & ~(BITS_PER_LONG-1)) {
        if ((tmp = *(p++)))
            goto found_middle;
        result += BITS_PER_LONG;
        size -= BITS_PER_LONG;
    }
    if (!size)
        return result;
    tmp = *p;

found_first:
    tmp &= (~0UL >> (BITS_PER_LONG - size));
    if (tmp == 0UL)         /* Are any bits set? */
        return result + size;   /* Nope. */
found_middle:
    return result + __builtin_ffsl(tmp) - 1;
}

#define for_each_set_bit(bit, addr, size)                       \
    for ((bit) = find_first_bit((addr), (size));                \
         (bit) < (size);                                        \
         (bit) = find_next_bit((addr), (size), (bit) + 1))

#endif  /* __ULIB_BIT_H */
