// an implementation of Copy-On-Write Block-Accessed Sorted Set
// Author: gejun@baidu.com
// Date: 2010/07/30 13:17
#pragma once
#ifndef _COWBASS0_HPP_
#define _COWBASS0_HPP_

#include "functional.hpp"
#include "common.h"
#include "op_list.hpp"
#include "allocators.hpp"

namespace st
{
    /**
       @brief primitive block node of Cowbass0
       @param T - type of the value
       @param ElemNum - number of nodes in one block
       @param PointerType - generally MP0_ID or leave it be
    */
    template <typename T, int ElemNum, typename PointerType>
        struct old_cowbass_node_t
        {
            typedef PointerType next_type;
            typedef T value_type;

            value_type data[ElemNum];
            next_type next;
        };

    //! one specialization for old_cowbass_node_t that uses real pointer
    template <typename T, int ElemNum>
        struct old_cowbass_node_t<T, ElemNum, void>
    {
        typedef old_cowbass_node_t<T, ElemNum, void>* next_type;
        typedef T value_type;

        value_type data[ElemNum];
        next_type next;
    };

    /**
       @brief copy on write block access sorted set
       @param T -type of element
       @param CompareFunc - typed int (T,T), returning <0, =0, >=0 to indicate order
       @param ElemNum - passing to old_cowbass_node_t
       @param AllocType - type of allocator
    */
    template <
        typename T
        , typename _Compare = Compare<T>
        , int ElemNum = 32
        , typename AllocType = allocator_t<old_cowbass_node_t<T,ElemNum,MP0_ID> , MP0_ID>
        >
        struct Cowbass0
        {
            typedef typename AllocType::Pointer pointer_t;
            typedef old_cowbass_node_t<T, ElemNum, pointer_t> node_t;
            typedef T value_type;
            typedef Cowbass0<T, _Compare, ElemNum, AllocType> this_type;
            typedef typename op_list_t<T>::Op op_t;

            /**
               @brief class on operation, which is generally add or del, and value
            */

            
            /**
               @brief basic fields that describe a list
            */
            struct basic_Cowbass0
            {
                pointer_t p_head_;
                pointer_t p_tail_;
                int tail_off_;
                int count_;

                int create (AllocType* p_alloc)
                {
                    p_head_ = p_alloc->allocate ();
                    node_t* rp_head = p_alloc->value_ptr(p_head_);
                    if (NULL == rp_head)
                    {
                        RETURN (-1);
                    }
                    rp_head->next = p_alloc->null();
                    p_tail_ = p_head_;
                    tail_off_ = -1;
                    count_ = 0;
                    RETURN (0);
                }
            };

            /**
               @brief a stateful object to traverse old_cowbass
            */
            struct iterator
            {
                /*
                explicit iterator()
                {
                    rp_node_ = NULL;
                    offset_ = 0;
                    p_alloc_ = NULL;
                }
                */
                /*
                explicit iterator(pointer_t p_node, AllocType *p_alloc, int offset)
                {
                    p_alloc_ = p_alloc;
                    rp_node_ = p_alloc_->value_ptr(p_node);
                    offset_ = offset;
                }
                */
                /*
                iterator& operator= (const iterator& other)
                {
                    rp_node_ = other.rp_node_;
                    offset_ = other.offset_;
                    p_alloc_ = other.p_alloc_;
                    return *this;
                    }*/
                
                inline iterator& operator++ ()
                {
                    if (NULL != rp_node_)
                    {                        
                        ++ offset_;
                        if (offset_ >= ElemNum)
                        {
                            rp_node_ = p_alloc_->value_ptr(rp_node_->next);
                            offset_ = 0;
                        }
                    }
                    
                    return *this;
                }

                inline bool operator!= (const iterator& other) const
                {
                    return rp_node_ != other.rp_node_ || offset_ != other.offset_;
                }

                inline bool operator== (const iterator& other) const
                {
                    return rp_node_ == other.rp_node_ && offset_ == other.offset_;
                }
                
                inline const T& operator*() const
                {
                    return rp_node_->data[offset_];
                }

                inline const T* operator->() const
                {
                    return rp_node_->data + offset_;
                }

                
                //private:
                node_t *rp_node_;
                AllocType *p_alloc_;
                int offset_;
            };

            /**
               @brief constructor without parameters, its only purpose is to remove compilation problem caused by containers, DON'T call this directly!
            */
            explicit Cowbass0()
                : p_alloc_(NULL)
            {
            }

            /**
               @brief construct a old_cowbass
               @param p_alloc - pointer to the allocator
               @param cmp - comparison functor/function
            */
            explicit Cowbass0(AllocType *p_alloc)
            {
                create (p_alloc);
            }

            /**
               @brief destructor which throws memory back to allocator
            */
            ~Cowbass0()
            {
                clear();
            }


            /**
               @brief initialize a old_cowbass, if the constructor with parameters was called, this function is unnessary
            */
            int create (AllocType *p_alloc)
            {
                if (NULL == p_alloc)
                {
                    ST_FATAL ("null param");
                    RETURN (-1);
                }

                if (sizeof(typename AllocType::Value) != sizeof(node_t))
                {
                    ST_FATAL ("sizeof of AllocType::Value does not match sizeof(node_t)");
                    RETURN (-1);
                }

                p_alloc_ = p_alloc;
                basic.p_head_ = p_alloc_->null();
                clear();

                RETURN (0);
            }

            /**
               @brief copying content from other old_cowbass, previous content is cleared
            */
            Cowbass0 (const this_type& other)
            {
                clear();
                for (iterator it=other.begin(),it_e=other.end(); it!=it_e; ++it)
                {
                    push_tail (&basic, *it);
                }
                ops_ = other.ops_;
            }

            /**
               @brief get beginning position of this old_cowbass
            */
            inline iterator begin () const
            {
                iterator it;
                it.p_alloc_ = p_alloc_;
                it.rp_node_ = p_alloc_->value_ptr(basic.p_head_);
                it.offset_ = 0;
                return it;
                //return iterator(basic.p_head_, p_alloc_, 0);
            }
            
            /**
               @brief get ending position of this old_cowbass, a general tip is using
               for (iterator it=begin(),it_e=end(); it!=it_e; ++it)
               to replace
               for (iterator it=begin(); it!=end(); ++it)
               because end is not simply returning NULL thing as in other containers.
            */
            inline iterator end () const
            {
                iterator it;
                if (basic.tail_off_ >= ElemNum-1)
                {
                    it.p_alloc_ = p_alloc_;
                    it.rp_node_ = NULL;
                    it.offset_ = 0;
                    //return iterator(p_alloc_->null(), p_alloc_, 0);
                }
                else
                {
                    it.p_alloc_ = p_alloc_;
                    it.rp_node_ = p_alloc_->value_ptr(basic.p_tail_);
                    it.offset_ = basic.tail_off_+1;
                    //return iterator(basic.p_tail_, p_alloc_, basic.tail_off_+1);
                }
                return it;
            }
            

            /**
               @brief empty or not
            */
            inline bool empty () const
            {
                return 0 == basic.count_;
            }

            /**
               @brief number of element
            */
            inline size_t count () const
            {
                return basic.count_;
            }


            /**
               @brief get all elements of another container which is abstracted as iterators
            */
            template <typename InputIterator>
            int from_other (InputIterator it_s, InputIterator it_e)
            {
                for (InputIterator it=it_s; it!=it_e; ++it)
                {
                    add (*it);
                }
                RETURN (0);
            }

            /**
               @brief comparison with another container which is abstracted as iterators
            */
            template <typename InputIterator>
            bool compare (InputIterator it_s, InputIterator it_e)
            {
                InputIterator it2 = it_s;
                int i=0;
                iterator it1=begin();
                for (; it1 != end(); ++it1, ++it2, ++i)
                {
                    if (*it1 != *it2)
                    {
                        return false;
                    }
                }
                if (i != basic.count_)
                {
                    return false;
                }
                if (it1 != end())
                {
                    return false;
                }
                if (it2 != it_e)
                {
                    return false;
                }
                return true;
            }

            /**
               @brief convert this old_cowbass to string, with this function, a old_cowbass can be converted to std::string by function show
            */
            void to_string (StringWriter& sb) const
            {
                sb.append_format ("%d:[", basic.count_);
                for (iterator it=begin(); it != end(); ++it)
                {
                    if (it != begin())
                    {
                        sb << ',';
                    }
                    sb << *it;
                }
                sb << ']';
                //sb << sb.sep() << "ops=" << a_op_;
            }
            

            /**
               @brief add an element into old_cowbass, the change does not take place until freeze is invoked, if the element exists, update it with the new
            */
            void add (const T& data)
            {
                ops_.add (data);
            }

            /**
               @brief delete an element from old_cowbass, the change does not take place until freeze is invoked, if the element does not exist, nothing happens
            */
            void del (const T& data)
            {
                ops_.del (data);
            }

            /**
               @brief restore this old_cowbass to initial state
            */
            int clear ()
            {
                ops_.clear ();
                
                pointer_t p = basic.p_head_;
                node_t* rp = p_alloc_->value_ptr(p);
                while (p!=p_alloc_->null())
                {
                    //ST_DEBUG ("dellocate p=%u rp=%p null=%u %p",p,rp, p_alloc_->null(), p_alloc_->value_ptr(p_alloc_->null()));
                    p_alloc_->deallocate (p);
                    p=rp->next;
                    rp=p_alloc_->value_ptr(p);
                }
                basic.create (p_alloc_);
                RETURN (0);
            }

            /**
               @brief merge add/del operations into this old_cowbass
            */
            int freeze ()
            {
                ops_.sort<_Compare>();
                ops_.unique<_Compare>();

                if (basic.count_)
                {
                    basic_Cowbass0 new_basic;
                    if (new_basic.create(p_alloc_) < 0)
                    {
                        ST_FATAL ("Fail to create new_basic");
                        return -1;
                    }
                    
                    typename op_list_t<T>::iterator it_be = ops_.end();
                    typename op_list_t<T>::iterator it_b = ops_.begin();
                    iterator it_t = begin();
                    iterator it_te = end();                

                    
                    // cout << "[before merge]: " << show(a_op_);
                
                    int ret = 0;
                    while (it_b != it_be && it_t != it_te)
                    {
                        const T& bv = it_b->value;
                        const T& tv = *it_t;
                        ret = cmp_ (bv, tv);
                        if (ret > 0)  // branch > trunk
                        {
                            push_tail (&new_basic, tv);
                            ++ it_t;
                        }
                        else if (ret < 0)  // branch < trunk
                        {
                            if (OP_ADD == it_b->type)
                            {
                                push_tail (&new_basic, bv);
                            }
                            ++ it_b;
                        }
                        else  // branch == trunk
                        {
                            if (OP_ADD == it_b->type)
                            {
                                push_tail (&new_basic, bv);
                            }
                            ++ it_b;
                            ++ it_t;
                        }
                    }

                    if (it_b == it_be)  // branch ends
                    {
                        for (; it_t != it_te; ++it_t)
                        {
                            push_tail (&new_basic, *it_t);
                        }
                    }
                    else  // trunk ends
                    {
                        for (; it_b != it_be; ++it_b)
                        {
                            if (OP_ADD == it_b->type)
                            {
                                push_tail (&new_basic, it_b->value);
                            }
                        }                    
                    }

                    // assign with new_basic
                    basic = new_basic;
                }
                else
                {
                    for (typename op_list_t<T>::iterator it_b = ops_.begin(); it_b!=ops_.end();++it_b)
                    {
                        if (OP_ADD == it_b->type)
                        {
                            push_tail (&basic, it_b->value);
                        }
                    }
                }

                // clear operations
                ops_.clear();

                RETURN (0);                
            }
            

            /**
               @brief get the pointer to last element
            */
            T* tail () const 
            {
                node_t* rp_tail = p_alloc_->value_ptr(basic.p_tail_);
                
                if (NULL == rp_tail)
                {
                    return NULL;
                }
                return rp_tail->data + basic.tail_off_ - 1;
            }

        private:

            /*
              T* push_head (T data)
              {
              pointer_t p_newhead = basic.p_head_;

              if (0 == head_off_)
              {    
              p_newhead = p_alloc_->allocate();
              node_t* rp_newhead = p_alloc_->value_ptr(p_newhead);
              if (NULL == rp_newhead)
              {
              return NULL;
              }
              rp_newhead->next = basic.p_head_;               
              head_off_ = num-1;
              }
              else
              {
              --head_off_;
              }

              // copy data
              rp_newhead->data[head_off_] = data;
                
              // modify original head
              if (p_alloc_->null() == basic.p_head_)
              {
              basic.p_tail_ = p_newhead;
              basic.tail_off_ = head_off_ + 1;
              }
              basic.p_head_ = p_newhead;

              // return pointer to stored data
              return rp_newhead->data + head_off_;
              }
            */

            /**
               @brief append a data onto tail
            */
            T* push_tail (basic_Cowbass0* p_basic, T data)
            {
                node_t* rp_tail = p_alloc_->value_ptr(p_basic->p_tail_);

                ++ p_basic->tail_off_;
                
                if (p_basic->tail_off_ >= ElemNum)
                {
                    pointer_t p_newtail = p_alloc_->allocate();
                    node_t* rp_newtail = p_alloc_->value_ptr(p_newtail);
                    if (NULL == rp_newtail)
                    {
                        return NULL;
                    }
                    rp_newtail->next = p_alloc_->null();
                    p_basic->tail_off_ = 0;
                    
                    if (rp_tail)
                    {
                        rp_tail->next = p_newtail;
                    }
                    p_basic->p_tail_ = p_newtail;
                    rp_tail = rp_newtail;
                }

                ++ p_basic->count_;
                rp_tail->data[p_basic->tail_off_] = data;

                return rp_tail->data + p_basic->tail_off_;
            }            

            basic_Cowbass0 basic;
            AllocType *p_alloc_;
            _Compare cmp_;
            op_list_t<T> ops_; 
        };

}

#endif// _COWBASS0_HPP_
