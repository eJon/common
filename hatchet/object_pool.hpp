// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Allocate and deallocate same-size objects efficiently
// Author: gejun@baidu.com
// Date: Sat Nov 13 16:38:09 CST 2010
#pragma once
#ifndef _OBJECT_POOL_HPP_
#define _OBJECT_POOL_HPP_

#include "debug.h"
#include "fixed_array.hpp"
#include <vector>
#include <deque>

namespace st
{
    typedef uint32_t OP_ID;
    const OP_ID MP_NULL = (OP_ID)(-1);
    const size_t DEFAULT_CHUNK_SIZE = 1 << 20;  // 1M
    
    template <typename _T
              , size_t ChunkSize=DEFAULT_CHUNK_SIZE>
    struct ObjectPool {
    public:
        //! CHUNK_ELEM_NUM is always power of 2
        enum { CHUNK_ELEM_NUM_SHIFT = bit_shift<(ChunkSize/sizeof(_T))>::R
               , CHUNK_ELEM_NUM = (1 << CHUNK_ELEM_NUM_SHIFT)
               , CHUNK_ELEM_NUM_MASK = (CHUNK_ELEM_NUM-1)
        };

        static const OP_ID NULL_ID = MP_NULL;
        typedef OP_ID ID;
        
        typedef _T Value;
        
        //! group of elements
        struct Chunk {
            explicit Chunk ()
            { bm_used.set_all (0); }
            
            void set_used (const int idx)
            { bm_used.set_at (idx, true); }

            bool clear_used (const int idx)
            {
                bm_used.set_at (idx, false);
                return bm_used.has_true();
            }

            void to_string (StringWriter& sw) const
            {
                sw << bm_used.true_num();
                //sw << bm_used;
            }

            FixedArray<bool, CHUNK_ELEM_NUM> bm_used;
            _T data[CHUNK_ELEM_NUM];
        };

        explicit ObjectPool (size_t =0)
            : idx_(0)
            , n_slot_(0)
            , n_invalid_slot_(0)
        {}

        ~ObjectPool()
        {
            while (!a_chunk_.empty()) {
                delete a_chunk_.back();
                a_chunk_.pop_back();
            }
            q_free_.clear();
            q_2b_free_.clear();
            idx_ = 0;
            n_slot_ = 0;
            n_invalid_slot_ = 0;
        }

        
        //! gets pointer address identified by mp_id
        //! @param[in] mp_id index of memorypool
        _T* address(size_t mp_id) const
        { return (MP_NULL == mp_id) ? NULL : slot(mp_id); }


    private:
        bool is_valid_id (OP_ID r) const
        {
            const OP_ID c_r = r >> CHUNK_ELEM_NUM_SHIFT;
            return c_r < a_chunk_.size() && a_chunk_[c_r] != NULL;
        }

        inline Chunk*& chunk (const OP_ID id)
        { return a_chunk_[id >> CHUNK_ELEM_NUM_SHIFT]; }

        inline _T* slot (const OP_ID id) const
        { return a_chunk_[id >> CHUNK_ELEM_NUM_SHIFT]->data + (id & CHUNK_ELEM_NUM_MASK); }
        
    public:

        //! alloc a slot from the memory pool
        //! @retval handle to allocated slot, it's MP_NULL if memory runs out
        OP_ID alloc()
        {
            // check q_free_ first
            while (!q_free_.empty()) {
                // pop a OP_ID from q_free_
                OP_ID r = q_free_.front ();
                q_free_.pop_front();

                // check if the chunk on the OP_ID was deleted
                if (is_valid_id (r)) {
                    chunk(r)->set_used (r & CHUNK_ELEM_NUM_MASK);
                    ++ n_slot_;
                    return r;
                }
                else {
                    -- n_invalid_slot_;
                }
            }
            // no useable slot in q_free_

            // allocate new chunk if idx_ divides CHUNK_ELEM_NUM
            if (0 == (idx_ & CHUNK_ELEM_NUM_MASK)) {
                Chunk* p_chunk = new (std::nothrow) Chunk ();
                if (NULL == p_chunk) {
                    ST_FATAL ("Fail to new p_chunk");
                    return MP_NULL;  // we don't change much until here
                }
                if (q_del_idx_.empty()) {
                    a_chunk_.push_back (p_chunk);
                    idx_ = (a_chunk_.size() - 1) << CHUNK_ELEM_NUM_SHIFT;
                }
                else {
                    a_chunk_[q_del_idx_.front()] = p_chunk;
                    idx_ = q_del_idx_.front() << CHUNK_ELEM_NUM_SHIFT;
                    q_del_idx_.pop_front();
                }
            }

            // allocate a slot
            OP_ID r = idx_++;
            chunk(r)->set_used (r & CHUNK_ELEM_NUM_MASK);

            // count marks sum of using slots
            ++ n_slot_;
            return r;
        }

        /**
         * delayed_dealloc(OP_ID) put a node to _delete_queue.
         * Nodes in _delete_queue can not be alloced until _delete_queue is
         * merged into _free_queue.
         * @param[in] dlt_node the MP ID of the node that is to be deleted
         */
        void delayed_dealloc(OP_ID dlt_node)
        {
            if (MP_NULL != dlt_node) {
                q_2b_free_.push_back (dlt_node);
                // we don't decrease n_slot_ at here, we want using_count to reflect accurate number of _using_ slots
                //-- n_slot_;
            }
        }

        /**
         * dealloc(OP_ID) put a node to _free_queue.
         *  Nodes in _free_queue can be alloced immediately.
         * @param[in] free_node the MP ID of the node that is to be freed
         */
        void dealloc(OP_ID r)
        {
            if (unlikely (MP_NULL == r)) {
                return;
            }
            Chunk*& p_chunk = chunk(r);
            //bool del=false;
            if (!p_chunk->clear_used (r & CHUNK_ELEM_NUM_MASK) && p_chunk != chunk(idx_)) {
                delete p_chunk;
                p_chunk = NULL;  // p_chunk is a reference
                q_del_idx_.push_back (r >> CHUNK_ELEM_NUM_SHIFT);
                n_invalid_slot_ += CHUNK_ELEM_NUM - 1;
                // deletion of a chunk may make other OP_ID on the same chunk in q_free_ invalid, they'll be cleared out in alloc() in lazy manner
                //del = true;
            }
            else {
                q_free_.push_back (r);
            }
            -- n_slot_;
            /* if (del) { */
            /*     std::cout << "happened=" << show(*this) << std::endl; */
            /* } */
        }

        //! actually recycle to-be-freed memory
        void recycle_delayed()
        {
            while (!q_2b_free_.empty()) {
                dealloc (q_2b_free_.front());
                q_2b_free_.pop_front();
            }
        }
            
        //! returns chunk number of memory pool
        size_t chunk_count() const
        { return a_chunk_.size() - q_del_idx_.size(); }

        //! returns chunk capacity of memory pool
        size_t chunk_capacity() const
        { return CHUNK_ELEM_NUM; }

        //! returns capacity of memory pool
        size_t capacity() const
        { return chunk_count() * CHUNK_ELEM_NUM; }

        //! returns current element number
        size_t using_count() const
        { return n_slot_; }
        
        size_t free_count () const
        { return q_free_.size() - n_invalid_slot_; }

        size_t alloc_count() const
        {
            if (0 == (idx_ & CHUNK_ELEM_NUM_MASK)) {
                return capacity();
            }
            else {
                return capacity() - CHUNK_ELEM_NUM + (idx_ & CHUNK_ELEM_NUM_MASK);
            }
        }
        
        size_t to_be_free_count () const
        { return q_2b_free_.size(); }

        size_t mem () const
        {
            return sizeof(*this)
            + chunk_count() * sizeof(Chunk)
            + a_chunk_.capacity()*sizeof(Chunk*)
            + q_free_.size()*sizeof(OP_ID)
            + q_2b_free_.size()*sizeof(OP_ID)
            + q_del_idx_.size()*sizeof(int)
            ;
        }

        void to_string (StringWriter& sw) const
        {
            sw
            << "{using=" << using_count()
            << " free=" << free_count()
            << " alloc=" << alloc_count()
            << " using_sz=" << using_count() * sizeof(_T)
            << " free_sz=" << free_count() * sizeof(_T)
            << " occupied_sz=" << mem()
            << " chunk="
            << "{count=" << chunk_count()
            << " capacity=" << chunk_capacity()
            << " sizeof=" << sizeof(_T)*CHUNK_ELEM_NUM
            << " content=";
            shows_range_compact (sw, a_chunk_.begin(), a_chunk_.end(), (Chunk*)NULL);
            sw << "}}";
        }

    private:
        //! first un-allocated slot in the allocating chunk
        OP_ID idx_;

        //! number of using slots, alloc() increases this counter while dealloc()/delayed_dealloc() decreases this counter
        size_t n_slot_;

        //! number of slots in q_free_ whose chunks were deleted
        size_t n_invalid_slot_;
        
        std::vector<Chunk*> a_chunk_;
        std::deque<OP_ID> q_free_;
        std::deque<OP_ID> q_2b_free_;
        std::deque<int> q_del_idx_;
    };
}

using st::OP_ID;

#endif  // _OBJECT_POOL_HPP_
