// An implementation of Copy-On-Write Block-Accessed Sorted Set
// Author: gejun@baidu.com
#pragma once
#ifndef _COWBASS1_HPP_
#define _COWBASS1_HPP_

#include "functional.hpp"
#include "common.h"
#include "op_list.hpp"
#include "memory_pool.h"
#include <vector>

namespace st
{
    /**
       @brief primitive block node of Cowbass1
       @param T - type of the value
       @param ElemNum - number of nodes in one block
    */
    template <typename T, int ElemNum, typename Tag=void>
    struct cowbass_node_t {
        typedef T Value;
        Value data[ElemNum];
        Tag tag_;
    };

    template <typename T, int ElemNum>
    struct cowbass_node_t<T,ElemNum,void> {
        typedef T Value;
        Value data[ElemNum];
    };

    
    /**
       @brief copy on write block access sorted set, basically it's implemented as a deque that spawns a new set with changes in batch, go through cowbass is as fast as primitive array. (In previous version it's implemented as a linked list and the iteration is slightly slower)
       @param T - value type
       @param CompareFunc - typed int (T,T), returning <0, =0, >=0 to indicate order
       @param ElemNum - number of values on one internal node
    */
    template <
        typename T
        , typename _Compare = Compare<T>
        , int _ElemNum = 32
        >
    struct Cowbass1
    {
        // round ELEM_NUM to power to 2
        enum { ELEM_NUM_SHIFT = bit_shift<_ElemNum>::R
               , ELEM_NUM = (1 << ELEM_NUM_SHIFT)
               , ELEM_NUM_MASK = (ELEM_NUM-1)
        };
        typedef Cowbass1<T, _Compare, ELEM_NUM> Self;
        typedef cowbass_node_t<T, ELEM_NUM, MP0_ID> Node;
        typedef T Value;
        typedef typename std::vector<Node*>::const_iterator node_iterator;
        //typedef op_list_t<T> op_list_type;
        typedef typename op_list_t<T>::iterator op_iterator;
        
        //enum {NODE_SIZE=sizeof(Node)};
        
        
        //! @brief a stateful object to traverse cowbass
        struct iterator
        {
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
                if (cur_ == last_)
                {
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

        //! @brief construct a cowbass
        //! @param [p_pool] pointer to the memorypool allocating nodes
        explicit Cowbass1(MemoryPool *p_pool)
        {
            count_ = 0;
            p_pool_ = p_pool;

            // reserve some memory for a_node_ to avoid repeating reallocation
            a_node_.reserve(32);
            
            // preallocation so that iterators work
            a_node_.push_back (alloc_node());
        }

        //! @brief destructor, giving all memory back to allocator, notice all nodes are deallocated, so if ~ctor is manually invoked, invoke ctor rather than clear(which keeps an allocated node) to initialize it. 
        ~Cowbass1()
        {
            for (size_t i=0; i<a_node_.size(); ++i)
            {
                dealloc_node (a_node_[i]);
            }
            a_node_.clear();
            count_ = 0;
        }


        //! @brief copy-constructor
        Cowbass1 (const Self& other)
        {
            count_ = 0;
            p_pool_ = other.p_pool;
            a_node_.reserve (32);
            a_node_.push_back (alloc_node());
            
            for (iterator it=other.begin(),it_e=other.end(); it!=it_e; ++it)
            {
                push_back (*it);
            }
        }

        //! @brief copying content from other cowbass, previous content is cleared
        Self& operator= (const Self& other)
        {
            clear();
            for (iterator it=other.begin(),it_e=other.end(); it!=it_e; ++it)
            {
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
           @brief get ending position of this cowbass, a general tip is using
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
        
        //! @brief convert this cowbass to string, with this function, a cowbass can be converted to std::string by function show
        void to_string (StringWriter& sb) const
        {
            sb.append_format ("%d:[", count_);
            for (iterator it=begin(); it != end(); ++it)
            {
                if (it != begin())
                {
                    sb << ',';
                }
                sb << *it;
            }
            sb << ']';
        }
            
        //! @brief restore this cowbass to initial state
        void clear ()
        {
            if (a_node_.empty())
            {
                a_node_.push_back (alloc_node());
            }
            else
            {
                // do not deallocate the first memory block
                for (size_t i=1; i<a_node_.size(); ++i)
                {
                    dealloc_node (a_node_[i]);
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
            for (InputIterator it=it_s; it!=it_e; ++it)
            {
                push_back (*it);
            }
            RETURN (0);
        }
        
        //! @brief initialize cowbass with the elements in [oit_b,oit_e), only empty cowbass can be called with this function.
        int assign (const op_iterator& oit_b, const op_iterator& oit_e)
        {
            clear();
            
            if (oit_b != oit_e)
            {
                for (op_iterator oit=oit_b; oit!=oit_e; ++oit)
                {
                    if (OP_ADD == oit->type)
                    {
                        this->push_back (oit->value);
                    }
                }
            }
            RETURN (0);
        }
        
        //! @brief create self with add/del operations and contents of another cowbass
        template <typename Cowbass>
        int assign (const Cowbass* p_other, const op_iterator& oit_b, const op_iterator& oit_e)
        {
            if (NULL == p_other)
            {
                ST_FATAL ("param[p_other] is NULL");
                RETURN (-1);
            }
            
            clear();

            iterator it_t = p_other->begin();
            iterator it_te = p_other->end();                
                    
            int ret = 0;
            op_iterator oit = oit_b;
            while (oit != oit_e && it_t != it_te)
            {
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
                    if (OP_ADD == oit->type)
                    {
                        this->push_back (bv);
                    }
                    ++ oit;
                }
                else  // branch == trunk
                {
                    if (OP_ADD == oit->type)
                    {
                        this->push_back (bv);
                    }
                    ++ oit;
                    ++ it_t;
                }
            }

            if (oit == oit_e)  // branch ends
            {
                for (; it_t != it_te; ++it_t)
                {
                    this->push_back (*it_t);
                }
            }
            else  // trunk ends
            {
                for (; oit != oit_e; ++oit)
                {
                    if (OP_ADD == oit->type)
                    {
                        this->push_back (oit->value);
                    }
                }                    
            }

            RETURN (0);
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

        const MemoryPool* pool() const
        {
            return p_pool_;
        }
        
    private:
        //! @brief get a node from pool
        Node* alloc_node()
        {
            MP0_ID handle = p_pool_->alloc();
            if (MP_NULL == handle)
            {
                ST_FATAL ("can't allocate memory from pool=%p", p_pool_);
                return NULL;
            }
            Node* p_node = (Node*)(p_pool_->address(handle));
            p_node->tag_ = handle;
            return p_node;
        }

        void dealloc_node (Node* p_n) {
            if (p_n && p_n->tag_ != MP_NULL) {
                MP0_ID handle = p_n->tag_;
                p_n->tag_ = MP_NULL;
                p_pool_->dealloc (handle);
            }
        }

        //! @brief append a value to backend
        Value* push_back (const Value& data)
        {
            ++count_;
            // allocate a block of memory if count_ divides ELEM_NUM, this is consistent with the preallocation
            if ((count_ >> ELEM_NUM_SHIFT) == (int)a_node_.size())
            {
                Node* p_node = alloc_node();
                if (NULL == p_node)
                {
                    ST_FATAL ("can't allocate memory from pool=%p", p_pool_);
                    return NULL;
                }
                //p_node->data[0] = data;
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
    };

}

#endif// _COWBASS1_HPP_
