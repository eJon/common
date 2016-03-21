// An implementation of Copy-On-Write Block-Accessed Sorted Set
// Author: gejun@baidu.com
#pragma once
#ifndef _COWBASS2_HPP_
#define _COWBASS2_HPP_

#include "functional.hpp"
#include "debug.h"
#include "unused/monitee.hpp"
#include "op_list.hpp"
#include "memory_pool.h"
#include <vector>

namespace st
{
    /**
       @brief primitive block node of cowbass2_t
       @param T - type of the value
       @param ElemNum - number of nodes in one block
    */
    template <typename T, int ElemNum, typename Tag=void>
    struct cowbass2_node_t {
        typedef T Value;
        Value data[ElemNum];
        Tag tag_;
    };

    template <typename T, int ElemNum>
    struct cowbass2_node_t<T,ElemNum,void> {
        typedef T Value;
        Value data[ElemNum];
    };

    /**
       @brief copy on write block access sorted set, basically it's implemented as a deque that spawns a new set with changes in batch, go through cowbass2 is as fast as primitive array. (In previous version it's implemented as a linked list and the iteration is slightly slower)
       @param T - value type
       @param CompareFunc - typed int (T,T), returning <0, =0, >=0 to indicate order
       @param ElemNum - number of values on one internal node
    */
    template <
        typename T
        , typename _Compare = Compare<T>
        , int _ElemNum = 32
        >
    struct cowbass2_t
        : public Monitee<cowbass2_t<T, _Compare, _ElemNum> >
    {
        // round ELEM_NUM to power to 2
        enum { ELEM_NUM_SHIFT = bit_shift<_ElemNum>::R
               , ELEM_NUM = (1 << ELEM_NUM_SHIFT)
               , ELEM_NUM_MASK = (ELEM_NUM-1)
        };
        typedef cowbass2_t<T, _Compare, _ElemNum> Self;
        typedef cowbass2_node_t<T, ELEM_NUM, MP0_ID> Node;
        typedef T Value;
        typedef typename std::vector<Node*>::const_iterator node_iterator;
        //typedef op_list_t<T> op_list_type;
        typedef typename op_list_t<T>::iterator op_iterator;
        
        //enum {NODE_SIZE=sizeof(Node)};

        //! this is required by Monitee
        int action_on_open_version (const Self* p_prev)
        {
            //cout << "calling " << __func__ << endl;
            p_prev_ = p_prev;
            return 0;
        }
        
        //! this is required by Monitee
        int action_on_close_version (const Self* p_prev)
        {
            if (NULL == p_ops_) {
                ST_FATAL ("p_ops_ is NULL");
                return ENOTINIT;
            }

            if (p_prev != p_prev_) {
                ST_FATAL ("p_prev passed in by open_version and close_version does not match!");
                return EIMPOSSIBLE;
            }

            p_ops_->sort<_Compare>();
            p_ops_->unique<_Compare>();

            //cout << "ops=" << show(*(p_ops_)) << endl;
                
            if (NULL == p_prev || p_prev->empty()) {
                assign (p_ops_->begin(), p_ops_->end());
            }
            else {
                assign (p_prev, p_ops_->begin(), p_ops_->end());
            }

            //cout << "p_this=" << show(*p_this) << endl;
                
            p_ops_->clear();

            p_prev_ = NULL;  // clear stored p_prev_

            return 0;
        }
        
        int action_on_zero_referenced ()
        {
            clear();
            return 0;
        }

        Self* new_shared_clone() const
        {
            return new (std::nothrow) Self(*this);
        }
        
        //! @brief a stateful object to traverse cowbass2
        struct iterator {
            iterator()
            {
                cur_ = NULL;
                last_ = NULL;
            }

            iterator(const Value *cur, node_iterator node)
                : cur_(cur), last_(cur + ELEM_NUM), pn_it_(node)
            {
            }

            // currently iterator does not need operator=
            // iterator& operator= (const iterator& other)
            // {
            //     pn_it_ = other.pn_it_;
            //     cur_ = other.cur_;
            //     last_ = other.last_;
            //     return *this;
            // }
                
            inline iterator& operator++ ()
            {
                ++cur_;
                if (cur_ == last_) {
                    ++pn_it_;
                    cur_ = (*pn_it_)->data;
                    last_ = cur_ + ELEM_NUM;
                }
                return *this;
            }

            inline bool operator!= (const iterator& other) const
            {
                return cur_ != other.cur_;
            }

            inline bool operator== (const iterator& other) const
            {
                return cur_ == other.cur_;
            }
                
            inline const Value& operator*() const
            {
                return *cur_;
            }

            inline const Value* operator->() const
            {
                return cur_;
            }
            
        private:
            const Value* cur_;
            const Value* last_;
            node_iterator pn_it_;
        };

        //! @brief construct a cowbass2
        //! @param [p_pool] pointer to the memorypool allocating nodes
        explicit cowbass2_t(MemoryPool* p_pool)
        {
            count_ = 0;
            p_pool_ = p_pool;
            p_ops_ = new (std::nothrow) op_list_t<T>;
            
            // reserve some memory for a_node_ to avoid repeating reallocation
            a_node_.reserve(32);
            
            // preallocation so that iterators work
            a_node_.push_back (alloc_node());
        }

        //! @brief destructor, giving all memory back to allocator, notice all nodes are deallocated, so if ~ctor is manually invoked, invoke ctor rather than clear(which keeps an allocated node) to initialize it. 
        ~cowbass2_t()
        {
            for (size_t i=0; i<a_node_.size(); ++i) {
                p_pool_->delayed_dealloc (a_node_[i]->tag_);
            }
            a_node_.clear();
            count_ = 0;
        }


        //! @brief copy-constructor
        cowbass2_t (const Self& other)
            : Monitee<Self>()
        {
            count_ = 0;
            p_pool_ = other.p_pool_;
            p_ops_ = other.p_ops_;
            a_node_.reserve (32);
            a_node_.push_back (alloc_node());
            
            for (iterator it=other.begin(),it_e=other.end(); it!=it_e; ++it) {
                push_back (*it);
            }
        }

        //! @brief copying content from other cowbass, previous content is cleared
        Self& operator= (const Self& other)
        {
            clear();
            for (iterator it=other.begin(),it_e=other.end(); it!=it_e; ++it) {
                push_back (*it);
            }
            return *this;
        }
        
        //! @brief get beginning position of this cowbass
        inline iterator begin () const
        {
            return iterator(a_node_[0]->data, a_node_.begin());
        }
            
        /**
           @brief get ending position of this cowbass2, a general tip is using
           for (iterator it=begin(),it_e=end(); it!=it_e; ++it)
           to replace
           for (iterator it=begin(); it!=end(); ++it)
           because end is not simply returning NULL thing as in other containers.
        */
        inline iterator end () const
        {
            return iterator(&at(count_), a_node_.end());
        }
            
        //! @brief empty or not
        inline bool empty () const
        {
            return 0 == count_;
        }

        //! @brief number of element
        inline size_t size () const
        {
            return count_;
        }
        
        //! @brief convert this cowbass2 to string, with this function, a cowbass2 can be converted to std::string by function show
        void to_string (StringWriter& sb) const
        {
            sb.append_format ("%d:[", count_);
            for (iterator it=begin(); it != end(); ++it) {
                if (it != begin()) {
                    sb << ',';
                }
                sb << *it;
            }
            sb << ']';
        }
            
        //! @brief restore this cowbass2 to initial state
        void clear ()
        {
            if (a_node_.empty()) {
                a_node_.push_back (alloc_node());
            }
            else {
                // do not deallocate the first memory block
                for (size_t i=1; i<a_node_.size(); ++i) {
                    p_pool_->delayed_dealloc (a_node_[i]->tag_);
                }
                a_node_.resize(1);
            }
            count_ = 0;
        }

        //! @brief get all elements of another container which is abstracted as iterators
        template <typename InputIterator>
        int assign (InputIterator it_s, InputIterator it_e)
        {
            clear();
            for (InputIterator it=it_s; it!=it_e; ++it) {
                push_back (*it);
            }
            return 0;
        }
        
        //! @brief initialize cowbass2 with the elements in [oit_b,oit_e), only empty cowbass2 can be called with this function.
        int assign (const op_iterator& oit_b, const op_iterator& oit_e)
        {
            clear();
            
            if (oit_b != oit_e) {
                for (op_iterator oit=oit_b; oit!=oit_e; ++oit) {
                    if (OP_ADD == oit->type) {
                        this->push_back (oit->value);
                    }
                }
            }
            return 0;
        }
        
        //! @brief create self with add/del operations and contents of another cowbass2
        template <typename Cowbass2>
        int assign (const Cowbass2* p_other, const op_iterator& oit_b, const op_iterator& oit_e)
        {
            if (NULL == p_other) {
                ST_FATAL ("param[p_other] is NULL");
                return EINVAL;
            }
            
            clear();

            iterator it_t = p_other->begin();
            iterator it_te = p_other->end();                
                    
            int ret = 0;
            op_iterator oit = oit_b;
            while (oit != oit_e && it_t != it_te) {
                const Value& bv = oit->value;
                const Value& tv = *it_t;
                ret = cmp_ (bv, tv);
                if (ret > 0)  // branch > trunk
                {
                    this->push_back (tv);
                    ++ it_t;
                }
                else if (ret < 0)  // branch < trunk
                {
                    if (OP_ADD == oit->type) {
                        this->push_back (bv);
                    }
                    ++ oit;
                }
                else  // branch == trunk
                {
                    if (OP_ADD == oit->type) {
                        this->push_back (bv);
                    }
                    ++ oit;
                    ++ it_t;
                }
            }

            if (oit == oit_e)  // branch ends
            {
                for (; it_t != it_te; ++it_t) {
                    this->push_back (*it_t);
                }
            }
            else  // trunk ends
            {
                for (; oit != oit_e; ++oit) {
                    if (OP_ADD == oit->type) {
                        this->push_back (oit->value);
                    }
                }                    
            }

            return 0;
        }
            

        //! @brief get the value at the place
        inline const Value& at (const size_t idx) const
        {
            return a_node_[idx >> ELEM_NUM_SHIFT]->data[idx & ELEM_NUM_MASK];
        }

        //! @brief get the value at the place, non-const version
        inline Value& at (const size_t idx)
        {
            return a_node_[idx >> ELEM_NUM_SHIFT]->data[idx & ELEM_NUM_MASK];
        }

        //! @brief wrapper over at
        inline Value& operator[] (const size_t idx)
        {
            return at(idx);
        }

        //! @brief wrapper over at
        inline const Value& operator[] (const size_t idx) const
        {
            return at(idx);
        }

        int insert (const T& value)
        {
            if (!this->is_mutable()) {
                ST_FATAL ("Forbidden inserting into an immutable cowbass=%p", this);
                return EIMMUTABLE;
            }
            p_ops_->add (value);
            return 0;
        }

        int erase (const T& value)
        {
            if (!this->is_mutable()) {
                ST_FATAL ("Forbidden erasing from an immutable cowbass=%p", this);
                return EIMMUTABLE;
            }
            p_ops_->del (value);
            return 0;
        }

        int erase_all ()
        {
            if (!this->is_mutable()) {
                ST_FATAL ("Forbidden erasing from an immutable cowbass=%p", this);
                return EIMMUTABLE;
            }
            p_ops_->clear();
            // the data is inside p_prev_, not this
            if (p_prev_) {
                for (iterator it=p_prev_->begin(); it!=p_prev_->end(); ++it) {
                    p_ops_->del (*it);
                }
            }
            return 0;
        }

        
        //! FilterFunc returns true is the value needs to be filtered out.
        template <typename FilterFunc>
        int erase_if (const T& value) {
            if (!this->is_mutable()) {
                ST_FATAL ("Forbidden erasing from an immutable cowbass=%p", this);
                return EIMMUTABLE;
            }
            return 0;
        }
        
    private:
        //! @brief get a node from pool
        Node* alloc_node()
        {
            MP0_ID handle = p_pool_->alloc();
            if (MP_NULL == handle) {
                ST_FATAL ("can't allocate memory from pool=%p", p_pool_);
                return NULL;
            }
            Node* p_node = (Node*)(p_pool_->address(handle));
            p_node->tag_ = handle;
            return p_node;
        }

        //! @brief append a value to backend
        Value* push_back (const Value& data)
        {
            ++count_;
            // allocate a block of memory if count_ divides ELEM_NUM, this is consistent with the preallocation
            if ((count_ >> ELEM_NUM_SHIFT) == (int)a_node_.size()) {
                Node* p_node = alloc_node();
                if (NULL == p_node) {
                    ST_FATAL ("can't allocate memory from pool=%p", p_pool_);
                    return NULL;
                }
                p_node->data[0] = data;
                a_node_.push_back (p_node);
                //printf ("add new node, count_=%d\n", count_);
            }

            Value* p_value = &(at(count_-1));
            *p_value = data;
            return p_value;
        }

        std::vector<Node*> a_node_;
        int count_;
        MemoryPool *p_pool_;
        _Compare cmp_;
        op_list_t<T> *p_ops_;
        const Self* p_prev_;
    };

}

#endif// _COWBASS2_HPP_
