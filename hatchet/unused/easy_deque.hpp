// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// (smalltable1) A simple and fast deque sharing pool
// Author: gejun@baidu.com
// Date: Fri Nov 12 18:02:49 CST 2010
#pragma once

#ifndef _EASY_DEQUE_HPP_
#define _EASY_DEQUE_HPP_

#define USE_CONST_IT

#include "functional.hpp"
#include "common.h"
#include <vector>
#include "object_pool.hpp"

namespace st {
template <typename _T, int _ElemNum> class EasyDeque {
private:
    // round ELEM_NUM to power to 2
    enum {
        ELEM_NUM_SHIFT = bit_shift<_ElemNum>::R
        , ELEM_NUM = (1 << ELEM_NUM_SHIFT)
        , ELEM_NUM_MASK = (ELEM_NUM-1)
    };
public:
    typedef OP_ID ID;

    //! block of element
    struct Node {
        _T data_[ELEM_NUM];
        ID mp_id_;
    };

    typedef ObjectPool<Node> Pool;

private:
    typedef std::vector<Node*> NodeArray;
    typedef EasyDeque<_T,_ElemNum> Self;
        
public:
#ifndef USE_CONST_IT
    //! @brief traverse elements efficiently
    class traverse_iterator {
    public:
        traverse_iterator()
            : cur_(NULL)
            , last_(NULL)
        {}
            
        traverse_iterator(const _T *cur
                          , typename NodeArray::const_iterator node)
            : cur_(cur)
            , last_(cur + ELEM_NUM)
            , node_it_(node)
        {}

        // currently iterator does not need operator=
        // iterator& operator= (const iterator& other)
        // {
        //     node_it_ = other.node_it_;
        //     cur_ = other.cur_;
        //     last_ = other.last_;
        //     return *this;
        // }
                
        void operator++ ()
        {
            ++cur_;
            if (cur_ == last_) {
                ++node_it_;
                cur_ = (*node_it_)->data_;
                last_ = cur_ + ELEM_NUM;
            }
        }

        bool operator!= (const traverse_iterator& other) const
        { return cur_ != other.cur_; }

        bool operator== (const traverse_iterator& other) const
        { return cur_ == other.cur_; }
                
        const _T& operator*() const
        { return *cur_; }

        const _T* operator->() const
        { return cur_; }

    private:
        const _T* cur_;
        const _T* last_;
        typename NodeArray::const_iterator node_it_;
    };
#else
    //! random iterator that could be passed stl algorithms
    class const_iterator {
    public:
        typedef std::random_access_iterator_tag iterator_category;
        typedef _T value_type;
        typedef int difference_type;
        typedef const _T* pointer;
        typedef const _T& reference;

        const_iterator()
        {}
            
        const_iterator(int idx, const Self* p_dq)
            : idx_(idx)
            , p_dq_(p_dq)
        {}
            
        bool operator!= (const const_iterator& other) const
        { return idx_ != other.idx_; }

        bool operator== (const const_iterator& other) const
        { return idx_ == other.idx_; }
                
        reference operator*() const
        { return p_dq_->at(idx_); }

        pointer operator->() const
        { return &p_dq_->at(idx_); }

        const_iterator& operator++()
        {
            ++idx_;
            return *this;
        }

        const_iterator
        operator++(int)
        { return const_iterator(idx_++, p_dq_); }

        // Bidirectional const_iterator requirements
        const_iterator&
        operator--()
        {
            --idx_;
            return *this;
        }

        const_iterator operator--(int)
        { return const_iterator(idx_--, p_dq_); }

        // Random access const_iterator requirements
        reference operator[](difference_type __n) const
        { return p_dq_->at(idx_+__n); }

        const_iterator& operator+=(difference_type __n)
        { idx_ += __n; return *this; }

        const_iterator operator+(difference_type __n) const
        { return const_iterator(idx_ + __n, p_dq_); }

        const_iterator& operator-=(difference_type __n)
        { idx_ -= __n; return *this; }

        const_iterator operator-(difference_type __n) const
        { return const_iterator(idx_ - __n, p_dq_); }

        difference_type operator-(const const_iterator& other) const
        { return idx_ - other.idx_; }

        bool operator< (const const_iterator& other) const
        { return idx_ < other.idx_; }

        bool operator> (const const_iterator& other) const
        { return idx_ > other.idx_; }
            
    private:
        int idx_;
        const Self* p_dq_;
    };

    typedef const_iterator traverse_iterator;
#endif  // USE_CONST_IT

    //! random iterator that could be passed stl algorithms
    class iterator {
    public:
        typedef std::random_access_iterator_tag iterator_category;
        typedef _T value_type;
        typedef int difference_type;
        typedef _T* pointer;
        typedef _T& reference;

        iterator()
        {}
            
        iterator(int idx, Self* p_dq)
            : idx_(idx)
            , p_dq_(p_dq)
        {}
            
        bool operator!= (const iterator& other) const
        { return idx_ != other.idx_; }

        bool operator== (const iterator& other) const
        { return idx_ == other.idx_; }
                
        reference operator*() const
        { return p_dq_->at(idx_); }

        pointer operator->() const
        { return &p_dq_->at(idx_); }

        iterator& operator++()
        {
            ++idx_;
            return *this;
        }

        iterator operator++(int)
        { return iterator(idx_++, p_dq_); }

        // Bidirectional iterator requirements
        iterator& operator--()
        {
            --idx_;
            return *this;
        }

        iterator operator--(int)
        { return iterator(idx_--, p_dq_); }

        // Random access iterator requirements
        reference operator[](difference_type __n) const
        { return p_dq_->at(idx_+__n); }

        iterator& operator+=(difference_type __n)
        { idx_ += __n; return *this; }

        iterator operator+(difference_type __n) const
        { return iterator(idx_ + __n, p_dq_); }

        iterator& operator-=(difference_type __n)
        { idx_ -= __n; return *this; }

        iterator operator-(difference_type __n) const
        { return iterator(idx_ - __n, p_dq_); }

        difference_type operator-(const iterator& other) const
        { return idx_ - other.idx_; }

        bool operator< (const iterator& other) const
        { return idx_ < other.idx_; }

        bool operator> (const iterator& other) const
        { return idx_ > other.idx_; }
            
    private:
        int idx_;
        Self* p_dq_;
    };

        
    //! @brief construct a cowbass3
    //! @param [p_pool] pointer to the memorypool allocating nodes
    explicit EasyDeque (Pool *p_pool)
        : count_(0)
        , p_pool_(p_pool)
    {
#ifndef USE_CONST_IT
        // reserve some memory for a_node_ to avoid repeating reallocation
        a_node_.reserve(ELEM_NUM);
        // preallocation so that iterators work
        a_node_.push_back (alloc_node());
#endif
    }

    ~EasyDeque()
    {
        for (size_t i=0; i<a_node_.size(); ++i) {
            dealloc_node (a_node_[i]);
        }
        a_node_.clear();
        count_ = 0;
    }

    void swap (Self& other)
    {
        int tmp_count = count_;
        count_ = other.count_;
        other.count_ = tmp_count;

        typeof(p_pool_) tmp_pool = p_pool_;
        p_pool_ = other.p_pool_;
        other.p_pool_ = tmp_pool;

        a_node_.swap (other.a_node_);
    }

    // EasyDeque (const EasyDeque& other)
    // {
    //     count_ = 0;
    //     a_node_.push_back (alloc_node());
    //     for (const_iterator it=other.begin(),it_e=other.end(); it!=it_e; ++it)
    //     {
    //         push_back (*it);
    //     }
    // }            

    // EasyDeque& operator= (const EasyDeque& other)
    // {
    //     clear();
    //     for (const_iterator it=other.begin(),it_e=other.end(); it!=it_e; ++it)
    //     {
    //         push_back (*it);
    //     }
    //     return *this;
    // }
            
    //! @brief get the value at the place
    const _T& at (size_t idx) const
    { return a_node_[idx >> ELEM_NUM_SHIFT]->data_[idx & ELEM_NUM_MASK]; }

    const _T& operator[] (size_t idx) const
    { return a_node_[idx >> ELEM_NUM_SHIFT]->data_[idx & ELEM_NUM_MASK]; }
        
    //! @brief get the value at the place, non-const version
    _T& at (size_t idx)
    { return a_node_[idx >> ELEM_NUM_SHIFT]->data_[idx & ELEM_NUM_MASK]; }
        
    _T& operator[] (size_t idx)
    { return a_node_[idx >> ELEM_NUM_SHIFT]->data_[idx & ELEM_NUM_MASK]; }
        
    //private:
    //! @brief empty or not
    bool empty () const
    { return 0 == count_; }

    //! @brief number of element
    size_t size () const
    { return count_; }

#ifndef USE_CONST_IT
    //! @brief append a value to backend
    _T* push_back (const _T& data_)
    {
        ++count_;
        // allocate a block of memory if count_ divides ELEM_NUM,
        // this is consistent with the preallocation
        if ((count_ >> ELEM_NUM_SHIFT) == (int)a_node_.size()) {
            Node* p_node = alloc_node();
            if (NULL == p_node) {
                ST_FATAL ("can't allocate memory from pool=%p", p_pool_);
                return NULL;
            }
            a_node_.push_back (p_node);
        }

        _T* p_value = &(at(count_-1));
        *p_value = data_;
        return p_value;
    }
#else
    //! @brief append a value to backend
    void push_back (const _T& data_)
    {
        // allocate a block of memory if count_ divides ELEM_NUM,
        // this is consistent with the preallocation
        if ((count_ & ELEM_NUM_MASK) == 0) {
            Node* p_node = alloc_node();
            if (unlikely (NULL == p_node)) {
                ST_FATAL ("can't allocate memory from pool=%p", p_pool_);
                return;
            }
            a_node_.push_back (p_node);
        }
            
        at(count_) = data_;
        ++count_;
    }
#endif
        
    //! @brief get beginning position of this cowbass3
    traverse_iterator traverse_begin () const
#ifndef USE_CONST_IT
    { return traverse_iterator(a_node_[0]->data_, a_node_.begin()); }
#else
    { return const_iterator(0, this); }
#endif
        
    //! @brief get ending position of this cowbass3, a general tip is using
    //! for (iterator it=begin(),it_e=end(); it!=it_e; ++it)
    //! to replace
    //! for (iterator it=begin(); it!=end(); ++it)
    //! to save repetive calling of end()
    traverse_iterator traverse_end () const
#ifndef USE_CONST_IT
    { return traverse_iterator (&at(count_), a_node_.end()); }
#else
    { return const_iterator(count_, this); }
#endif

    //! begin of this deque
    iterator begin()
    { return iterator(0, this); }

    //! end of this deque
    iterator end()
    { return iterator(count_, this); }
        
    //! @brief convert this cowbass3 to string, with this function,
    //! a cowbass3 can be converted to std::string by function show
    void to_string (StringWriter& sb) const
    { shows (sb, traverse_begin(), traverse_end()
             , StringWriter::MAX_N_PRINT); }

    //! @brief restore this cowbass3 to initial state
    void clear ()
    {
#ifndef USE_CONST_IT
        if (a_node_.empty()) {
            a_node_.push_back (alloc_node());
        } else {
            // do not deallocate the first memory block
            for (size_t i=1; i<a_node_.size(); ++i) {
                dealloc_node (a_node_[i]);
            }
            a_node_.resize(1);
        }
#else
        while (!a_node_.empty()) {
            dealloc_node (a_node_.back());
            a_node_.pop_back();
        }
#endif
        count_ = 0;
    }

    Pool* pool_ptr () const
    { return p_pool_; }

    size_t mem_without_node () const
    {
        return sizeof(*this)
            + a_node_.capacity() * sizeof(Node*)
            ;
    }

private:
    //! @brief get a node from pool
    Node* alloc_node()
    {
        ID handle = p_pool_->alloc();
        if (MP_NULL == handle) {
            ST_FATAL ("can't allocate memory from pool=%p", p_pool_);
            return NULL;
        }
        Node* p_node = p_pool_->address(handle);
        p_node->mp_id_ = handle;
        return p_node;
    }

    void dealloc_node (Node* p_n)
    {
        if (p_n && p_n->mp_id_ != MP_NULL) {
            ID handle = p_n->mp_id_;
            p_n->mp_id_ = MP_NULL;
            p_pool_->dealloc (handle);
        }
    }
        
private:
    int count_;
    NodeArray a_node_;
    Pool *p_pool_;
};
}

#endif  // _EASY_DEQUE_HPP_
