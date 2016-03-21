// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: Thu Aug 19 14:05:30 CST 2010
// Modified from app/ecom/im/im-lib/im_open_hash_rex.h, major changes are:
// 1. hash and equal are template parameters
// 2. add iterator
// 3. change get_info to size()/hash_size()/threshold()
// 4. add to_string()
// 5. rename renew() to clear()
// LinearHashMap is an internal linear probing hashmap which is generally
// fast at seek() and clear () with small items comparing to commonly used
// external probing hashmaps (HashTemplate/inc_dict_t/odict...). It wastes
// significant space for large items
#pragma once
#ifndef _LINEAR_HASH_MAP_HPP_
#define _LINEAR_HASH_MAP_HPP_

#include <stdio.h>
#include <stdint.h>
#include <new>
#include "functional.hpp"
#include "common.h"

namespace st {

//! to access this map
template <typename _Map> class LinearHashMapIterator {
public:
    LinearHashMapIterator() : idx_(0), this_(NULL) {}
    
    LinearHashMapIterator(uint32_t idx, const _Map* p) : idx_(idx), this_(p)
    { skip_null(); }
    
    class _Map::Node& operator* () const
    { return this_->m_hash_table[idx_]; }
            
    class _Map::Node* operator-> () const
    { return &(this_->m_hash_table[idx_]); }
            
    bool operator!= (const LinearHashMapIterator& rhs) const
    { return idx_ != rhs.idx_; }

    bool operator== (const LinearHashMapIterator& rhs) const
    { return idx_ == rhs.idx_; }
            
    LinearHashMapIterator& operator++ ()
    {
        ++ idx_;
        skip_null();
        return *this;
    }

private:
    void skip_null ()
    {
        for ( ; idx_ < this_->m_hash_size &&
                  this_->is_null_node(this_->m_hash_table[idx_]);
              ++idx_);
    }
    
    uint32_t idx_;
    const _Map* this_;
};

//! closed hashing hashmap
template <typename _Key,
          typename _Value,
          typename _Hash = Hash<_Key>,
          typename _Equal = Equal<_Key> >
struct LinearHashMap {
    struct Node {
        uint32_t time;
        _Key key;
        _Value value;

        void to_string(StringWriter& sw) const
        { sw << "(t=" << time << "," << key << "," << value << ")"; }
    };

template <typename _Map> friend class LinearHashMapIterator;
    typedef LinearHashMapIterator<LinearHashMap> iterator;
     
    LinearHashMap()
        : m_hash_size(0)
        , m_node_num(0)
        , m_max_node_num(0)
        , m_threshold(0)
        , m_replace(true)
        , m_hash_table(NULL)
    {}

    ~LinearHashMap()
    {
        delete [] m_hash_table;
    }

    int create (uint32_t n, uint32_t threshold, bool replace = true)
    {
        m_hash_size = n;

        if (m_hash_size < 2) {
            m_hash_size = 2;
        }

        if (threshold <= 0 || threshold >= 100) {
            return -1;
        }

        m_threshold = threshold;
        m_replace = replace;
        m_max_node_num = calc_max_node(m_hash_size);
        m_times = 1;

        m_hash_table = new (std::nothrow) Node[m_hash_size];

        if (m_hash_table == NULL) {
            return -1;
        }

        for (uint32_t i = 0; i < m_hash_size; i++) {
            set_null_node(m_hash_table[i]);
        }

        return 0;
    }

    iterator begin() const { return iterator(0, this); }

    iterator end() const { return iterator(m_hash_size, this); }

    int insert(const _Key &key, const _Value &value)
    {
        if (m_node_num >= m_max_node_num && rehash()) {
            return -1;
        }

        return add(key, value);
    }

    _Value *seek(const _Key &key) const {
        uint32_t i;

        if (seek(key, i)) {
            return &m_hash_table[i].value;
        }

        return NULL;
    }

    int erase(const _Key &key) {
        uint32_t i, j;

        if (seek(key, i)) {
            m_node_num--;
            while (true) {
                set_null_node(m_hash_table[i]);

                j = i;

                uint32_t r;

                do {
                    i++;
                    
                    if (i == m_hash_size) {
                        i = 0;
                    }

                    if (is_null_node(m_hash_table[i])) {
                        return 0;
                    }

                    r = hf(m_hash_table[i].key) % m_hash_size;
                }
                while ((j < r && r <= i) || (i < j && (r > j || r <= i)));

                m_hash_table[j] = m_hash_table[i];
            }
        }

        return -1;
    }

    size_t size () const { return m_node_num; }

    uint32_t hash_size () const { return m_hash_size; }

    uint32_t threshold () const { return m_threshold; }
        
    int clear()
    {            
        m_times++;
        set_null_node(m_hash_table[m_times % m_hash_size]);
        m_node_num = 0;
        if ((hash_size()*sizeof(Node)) > 50000000) {  // >~ 50M
            rehash(128);
        }
        return 0;
    }

    size_t mem () const
    { return sizeof(*this) + sizeof(Node)*m_hash_size; }

    void to_string(StringWriter& sb) const
    {
        shows_range (sb, begin(), end());
    }

private:
    bool is_null_node(const Node &node) const
    { return node.time != m_times; }

    void set_null_node(Node &node)
    { node.time = m_times - 1; }

    void set_taken_node(Node &node)
    { node.time = m_times; }
    
    bool seek(const _Key &key, uint32_t &i) const {
        i = hf(key) % m_hash_size;

        if (is_null_node(m_hash_table[i])) {
            return false;
        }

        if (key_eq(m_hash_table[i].key, key)) {
            return true;
        }

        uint32_t idx = i++;

        while (i < m_hash_size) {
            if (is_null_node(m_hash_table[i])) {
                return false;
            }

            if (key_eq(m_hash_table[i].key, key)) {
                return true;
            }

            i++;
        }

        i = 0;

        while (i < idx) {
            if (is_null_node(m_hash_table[i])) {
                return false;
            }

            if (key_eq(m_hash_table[i].key, key)) {
                return true;
            }

            i++;
        }

        return false;
    }

    int add(const _Key &key, const _Value &value)
    {
        uint32_t i;

        if (!seek(key, i)) {
            m_node_num++;
        }
        else if (!m_replace) {
            return 1;
        }

        m_hash_table[i].key = key;
        m_hash_table[i].value = value;

        set_taken_node(m_hash_table[i]);

        return 0;
    }

    uint32_t calc_max_node(uint32_t hash_size) const
    { return (uint64_t)hash_size * m_threshold / 100; }

    int rehash(uint32_t hash_size=0)
    {
        Node *old_hash = m_hash_table;
        uint32_t old_size = m_hash_size;

        do {
            m_hash_size = (0 == hash_size) ? (m_hash_size * 2 + 1) : hash_size;
            m_max_node_num = calc_max_node(m_hash_size);
        }
        while (m_max_node_num < m_node_num + 1);

        m_hash_table = new (std::nothrow) Node[m_hash_size];

        if (m_hash_table == NULL) {
            m_hash_table = old_hash;
            m_hash_size = old_size;

            return -1;
        }

        m_max_node_num = calc_max_node(m_hash_size);
        m_node_num = 0;

        for (uint32_t i = 0; i < m_hash_size; i++) {
            set_null_node(m_hash_table[i]);
        }

        for (uint32_t i = 0; i < old_size; i++) {
            if (!is_null_node(old_hash[i])) {
                add(old_hash[i].key, old_hash[i].value);
            }
        }

        delete [] old_hash;  // CAUTION: rehash() 之后立即释放原有内存空间

        return 0;
    }

private:
    uint32_t m_hash_size;
    uint32_t m_node_num;
    uint32_t m_max_node_num;
    uint32_t m_threshold;
    uint32_t m_times;
    bool m_replace;
    Node *m_hash_table;
    _Equal key_eq;
    _Hash hf;
};

}

#endif  //_LINEAR_HASH_MAP_HPP_
