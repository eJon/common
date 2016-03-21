// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Very basics of an index
// Author: gejun@baidu.com
// Date: Aug 2 20:57:03 CST 2011
#pragma once
#ifndef _INDEX_TRAITS_H_
#define _INDEX_TRAITS_H_

#include "partial_iterator.hpp"    // PartialIterator

namespace st {

// Possible values of IndexInfo::Score
const int ST_BITMAP_SCORE               = 9;
const int MAYBE_ST_BITMAP_SCORE         = 8;
const int COW_HASH_SCORE                = 7;
const int MAYBE_COW_HASH_SCORE          = 6;
const int COW_HASH_CLUSTER2_SCORE       = 5;
const int MAYBE_COW_HASH_CLUSTER2_SCORE = 4;
const int COW_HASH_CLUSTER1_SCORE       = 3;
const int MAYBE_COW_HASH_CLUSTER1_SCORE = 2;
const int COW_TABLE_SCORE               = 0;

// Test if an index has all necessary interfaces
template <class _Index>
struct is_valid_index {
    static const bool R = true;
};

template <class _KeyAttrS,              // Attributes of the key
          class _SeekIterator,          // "seek"ed by the key
          int _SEEK_SCORE,              // indicate speed of the seek
          class _PartialSeekIterator,   // partially "seek"ed by the key
          int _PARTIAL_SEEK_SCORE>      // speed of partial seek
struct SeekInfo : public c_show_base {
    typedef _KeyAttrS KeyAttrS;
    typedef _SeekIterator SeekIterator;
    static const int SEEK_SCORE = _SEEK_SCORE;
    typedef _PartialSeekIterator PartialSeekIterator;
    static const int PARTIAL_SEEK_SCORE = _PARTIAL_SEEK_SCORE;

    static void c_to_string(std::ostream& os)
    {
        os << "{Key=" << c_show(_KeyAttrS)
           << " Seek=" << _SEEK_SCORE
           << " PartialSeek=" << _PARTIAL_SEEK_SCORE
           << "}";
    }
};

template <int _POS, class _SeekInfo>
struct IndexInfo : public c_show_base {
    static const int POS = _POS;
    typedef _SeekInfo SeekInfo;

    static void c_to_string(std::ostream& os)
    {
        os << "{Pos=" << _POS
           << " Info=" << c_show(_SeekInfo)
           << "}";
    }
};

template <int _POS, class _Uniqueness>
struct UniqueInfo : public c_show_base {
    static const int POS = _POS;
    typedef _Uniqueness Uniqueness;

    static void c_to_string(std::ostream& os)
    {
        os << _POS << "->" << c_show(_Uniqueness);
    }
};

}  // namespace st

#endif  //_INDEX_TRAITS_H_
