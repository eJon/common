// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// A deque with fixed capacity and using space in cycle
// Author: gejun@baidu.com
// Date: Wed Sep 8 19:47:53 CST 2010
#pragma once
#ifndef _FIXED_DEQUE_HPP_
#define _FIXED_DEQUE_HPP_

#include "common.h"

namespace st {

template <typename _Item, size_t _Cap> struct FixedDeque {
    
    typedef _Item Item;  // type of item
    
    // Go through FixedDeque
    struct iterator {
        explicit iterator (size_t offset, FixedDeque* p_cq)
            : offset_(offset), p_cq_(p_cq)
        { idx_ = (p_cq->head_+offset_) % _Cap; }

        bool operator== (const iterator& other) const
        { return offset_ == other.offset_; }

        bool operator!= (const iterator& other) const
        { return offset_ != other.offset_; }
        
        _Item& operator*() const { return p_cq_->a_item_[idx_]; }

        _Item* operator->() const { return p_cq_->a_item_ + idx_; }

        size_t pos() const { return idx_; }
        
        void operator++ ()
        {
            ++ offset_;
            idx_ = (idx_+1) % _Cap;
        }
        
    private:
        size_t offset_;
        size_t idx_;
        FixedDeque* p_cq_;
    };

    // const version of iterator
    struct const_iterator {
        const_iterator (size_t offset, const FixedDeque* p_cq)
            : offset_(offset), p_cq_(p_cq)
        { idx_ = (p_cq->head_+offset_) % _Cap; }

        bool operator== (const const_iterator& other) const
        { return offset_ == other.offset_; }

        bool operator!= (const const_iterator& other) const
        { return offset_ != other.offset_; }
            
        const _Item& operator*() const { return p_cq_->a_item_[idx_]; }

        const _Item* operator->() const { return p_cq_->a_item_ + idx_; }
            
        size_t pos() const { return idx_; }
            
        void operator++ ()
        {
            ++ offset_;
            idx_ = (idx_+1) % _Cap;
        }
        
    private:
        size_t offset_;
        size_t idx_;
        const FixedDeque* p_cq_;
    };

    // Construct this deque
    explicit FixedDeque() : head_(0), tail_(_Cap-1), n_item_(0)
    {}

    // Destruct this deque
    ~FixedDeque()
    {}

    // Copy construct from another deque
    FixedDeque(const FixedDeque& other)
    {
        head_ = 0;
        tail_ = _Cap-1;
        n_item_ = 0;

        for (const_iterator it=other.begin(); it!=other.end(); ++it) {
            push_back (*it);
        }
    }
    
    // assign fro another another deque
    FixedDeque& operator=(const FixedDeque& other)
    {
        head_ = 0;
        tail_ = _Cap-1;
        n_item_ = 0;

        for (const_iterator it=other.begin(); it!=other.end(); ++it) {
            push_back (*it);
        }

        return *this;
    }
        
    // beginning position
    iterator begin() { return iterator(0, this); }

    // ending position
    iterator end() { return iterator(n_item_,this); }

    // const version of begin
    const_iterator begin() const { return const_iterator(0, this); }

    // const version of end
    const_iterator end() const { return const_iterator(n_item_,this); }

    // Insert a value into back side of the deque
    // Returns:
    //   EFULL queue is full
    //   0   success
    int push_back (const _Item& val)
    {
        size_t new_tail = (tail_+1) % _Cap;  // "right hand side"
        if (n_item_ > 0 && new_tail == head_) {
            ST_FATAL ("push_back into a full fixed_deque=%p", this);
            return EFULL;
        }
        a_item_[new_tail] = val;
        ++ n_item_;
        tail_ = new_tail;
        return 0;
    }

    // Insert a value into front side of the deque
    // Returns:
    //   EFULL  queue is full
    //   0    success
    int push_front (const _Item& val)
    {
        size_t new_head = (head_+_Cap-1) % _Cap;  // "left hand side"
        if (n_item_ > 0 && new_head == tail_) {
            ST_FATAL ("push_front into a full fixed_deque=%p", this);
            return EFULL;
        }
        a_item_[new_head] = val;
        ++ n_item_;
        head_ = new_head;
        return 0;        
    }

    // Pop a value from back side.
    // Note: the value is preserved until the position is reused
    // Returns:
    //   NULL      queue is empty
    //   otherwise pointer to stored value
    _Item* pop_back ()
    {
        if (n_item_ == 0) {
            ST_FATAL ("pop_back an empty fixed_deque=%p", this);
            return NULL;
        }
        -- n_item_;
        size_t old_tail = tail_;
        tail_ = (old_tail+_Cap-1) % _Cap;
        return a_item_ + old_tail;
    }

    // Pop a value from front side of the deque.
    // Note: the value is preserved until the position is reused
    // Returns:
    //   NULL      queue is empty
    //   otherwise pointer to the stored value
    _Item* pop_front ()
    {
        if (n_item_ == 0) {
            ST_FATAL ("pop_front an empty fixed_deque=%p", this);
            return NULL;
        }
        -- n_item_;
        size_t old_head = head_;
        head_ = (old_head+1) % _Cap;
        return a_item_ + old_head;
    }

    // Get value at front side,
    // Note: if queue is empty, the return value is undefined
    const _Item& front() const { return a_item_[head_]; }

    // const version of front()
    const _Item& front(size_t offset) const
    { return a_item_[(head_+offset) % _Cap]; }

    // Get value at back side,
    // Note: if queue is empty, the return value is undefined
    const _Item& back() const { return a_item_[tail_]; }

    // const version of back()
    const _Item& back(size_t offset) const
    { return a_item_[(tail_+_Cap-(offset%_Cap))%_Cap]; }
        
    // Get number of items in the queue
    size_t size () const { return n_item_; }

    // Know empty or not
    bool empty () const { return n_item_ == 0; }

    // Know full or not
    bool full () const { return n_item_ == _Cap; }

    // Remove all items
    // Note: value of items are preserved until positions are reused
    void clear()
    {
        //keep head_ unchanged
        tail_ = (head_+_Cap-1) % _Cap;
        n_item_ = 0;
    }

    // _Item& at (size_t pos) { return a_item_[pos]; }

    // Print content to StringWriter
    void to_string (StringWriter& sb) const
    {
        bool first = true;
        sb << size() << ":[";
        for (const_iterator it=begin(); it!=end(); ++it) {
            if (!first) {
                sb << ",";
            } else {
                first = false;
            }
            sb << *it;
        }
        sb << "]";
    }
    
private:
    size_t head_;  // position of first item
    size_t tail_;  // position of last item
    size_t n_item_;
    _Item a_item_[_Cap];
};

}
#endif  //_FIXED_DEQUE_HPP_
