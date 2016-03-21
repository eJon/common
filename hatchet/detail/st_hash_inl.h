// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Inline implementations of st_hash.h
// Author: gejun@baidu.com
// Date: Dec 4 17:01:02 CST 2010

namespace st {

// Code taken from http://murmurhash.googlepages.com/MurmurHash2.cpp

//-----------------------------------------------------------------------------
// MurmurHash2, by Austin Appleby

// Note - This code makes a few assumptions about how your machine behaves -

// 1. We can read a 4-byte value from any address without crashing
// 2. sizeof(int) == 4

// And it has a few limitations -

// 1. It will not work incrementally.
// 2. It will not produce the same results on little-endian and big-endian
//    machines.

unsigned int murmur_hash2(const void* key, int len, unsigned int seed)
{
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.

    const unsigned int m = 0x5bd1e995;
    const int r = 24;

    // Initialize the hash to a 'random' value

    unsigned int h = seed ^ len;

    // Mix 4 bytes at a time into the hash

    const unsigned char * data = (const unsigned char *)key;

    while(len >= 4) {
        unsigned int k = *(unsigned int *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array

    switch(len) {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0];
        h *= m;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

size_t _normalize(int v) 
{
    u_long h = ((u_long)v << 32) | v;
    h *= 11400714819323198485ULL;
    h ^= h >> 32; 
    return h;
}

size_t _normalize(unsigned int v) 
{
    u_long h = ((u_long)v << 32) | v;
    h *= 11400714819323198485ULL;
    h ^= h >> 32; 
    return h;
}

size_t hash(char v)                      { return v; }
size_t hash(unsigned char v)             { return v; }
size_t hash(short v)                     { return v; }
size_t hash(unsigned short v)            { return v; }
size_t hash(int v)                       { return v; }
size_t hash(unsigned int v)              { return v; }
size_t hash(long v)                      { return v; }
size_t hash(unsigned long v)             { return v; }
size_t hash(long long v)                 { return v; }
size_t hash(unsigned long long v)        { return v; }

size_t hash(const char* str)
{
    register size_t v = 0;
    for ( ; *str; ++str) {
        v = v * 5ul + (size_t)*str;
    }
    return v;
}

size_t hash(char* str)
{
    return hash((const char*)str);
}

size_t hash(const std::string& str)
{
    return hash(str.c_str());
}

// Calculate hash value of a buffer, similar with additive linear congruential
// random number generator, not as fast as you thought about it, use it wisely
// Regularly used primes are 3, 5, 17, 31, 257
size_t hash(const void* __restrict p_data, size_t size)
{
    const char* __restrict p_buf = (const char*) p_data;
    size_t v = 0;
        
    if (size <= sizeof(v)) {
        memcpy(&v, p_buf, size);
    } else {
        for (size_t i = 0; i < size; ++i) {
            v = v * 17ul + (size_t)p_buf[i];
        }
    }
    return v;
}

}  // namespace st

