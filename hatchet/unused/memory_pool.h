/************************************************************************
 *
 * Copyright (c) 2007 Baidu.com, Inc. All Rights Reserved
 * $Id: memory_pool.h,v 1.2 2008/09/27 05:20:44 guhao Exp $
 *
 ************************************************************************/

/**
 *
 *  memory_pool
 * 
 *  Copyright (C) 2005-2007 by Helios Xu, Martin Ye
 * 
 *  Email: xuhui@baidu.com
 *
 *  This file defines class MemoryPool.
 *
 *  @notice
 *  64位程序依赖该类时，需要在目标程序编译选项中添加 __64BIT__ 宏
 * 
 * Update List:
 *  - add 64-bit platform support, by Martin, 05/2007
 * 
 * -------------------------------------------------------------------------
 *
 * Definition:
 *
 *  - MP ID: addess in a memory pool. Each MP0_ID can be illustrated as
 \verbatim    
 | chunk idx |     offset     |
 32-bit    |--10bits---|----21bit-------|
 64-bit    |--18bits---|----21bit-------|
 \endverbatim
 * 
 * Macros and Constants:
 *  - MP_NULL - NULL of Memory Pool. The top limit of a memory pool
 *                is 2G on 32-bit platform while 512G on 64-bit platform.
 *                So we defines NULL as a big number that MP ID can
 *                never reached.
 *  - MAXCHUNKNUM - max chunk number. Each chunk is a block of real memory.
 *                      MemoryPool calls malloc() to get a chunk each time. Each
 *                      chunk can hold a number of elements. It's value is 2^10
 *                      on 32-bit platform while 2^18 on 64-bit platform.
 *  - MAXCHUNKSIZE - top limit of chunk size. It's value is 2^21.
 *  - OFFSETMASK - a const for get offset from a MP ID.
 *  - IDXMASK - a const for get chunk idx form a MP ID.
 * 
 * 示例/测试环境演示:
 * \include test_memorypool.cpp
 */


#ifndef _EL_MEM_POOL_H
#define _EL_MEM_POOL_H

#include "debug.h"

typedef size_t MP0_ID;

#ifdef __64BIT__
const MP0_ID  MP_NULL = 0xFFFFFFFFFFFFFFFF;
const size_t MAXCHUNKNUM = 0x40000;     // 2^18
const size_t MAXCHUNKSIZE = 0x00200000;  // 2^21
const size_t OFFSETMASK = 0x001FFFFF;
const size_t IDXMASK = 0x3FFFF << 21;
#else
const MP0_ID  MP_NULL = 0xFFFFFFFF;
const size_t MAXCHUNKNUM = 0x400;       // 2^10
const size_t MAXCHUNKSIZE = 0x00200000;  // 2^21
const size_t OFFSETMASK = 0x001FFFFF;
const size_t IDXMASK = 0x3FF << 21;
#endif

struct element_node_t
{
    MP0_ID next;
};

/**
 * queue structure: first in, first out
 */
struct mp_queue_t
{
    MP0_ID head;
    MP0_ID tail;
};

/**
 * chuck node
 */
struct chunk_node_t
{
    element_node_t *chunk_begin;	//指向内存块起始地址的指针

};

/**
 * memory pool
 */
class MemoryPool
{
 private:
    chunk_node_t _chunk_list[MAXCHUNKNUM];	// chunk 数组
    size_t _chunk_size;		//内存块大小
    size_t _chunk_capacity;	//每个内存块容纳的元素个数
    size_t _element_size;		//元素大小
    size_t _node_size;			//内存节点大小
    size_t _chunk_number;		// 内存块个数
    size_t _current_element_number;	//当前元素个数
    mp_queue_t _free_queue;		//可分配节点队列
    mp_queue_t _delete_queue;	//准可分配队列


 public:
    /**
     * MemoryPool(size_t) is the constructor of class MemoryPool. It intialize a
     * memory pool with a specified element size.
     *
     * This function only specify the element size, but not malloc real memory.
     * 
     * @param[in] element_size size of element that will be put into this pool
     */
	
 MemoryPool(size_t element_size):_element_size(element_size),
        _chunk_number(0), _current_element_number(0)
    {
        _free_queue.head = _free_queue.tail = MP_NULL;
        _delete_queue.head = _delete_queue.tail = MP_NULL;
        _node_size = element_size + sizeof(element_node_t);
        _chunk_capacity = MAXCHUNKSIZE / _node_size;
        _chunk_size = _chunk_capacity * _node_size;
        for (size_t i = 0; i < MAXCHUNKNUM; i++)
        {
            _chunk_list[i].chunk_begin = NULL;
        }
    }

    /**
     * MemoryPool(int, size_t) is the overloaded constructor of class MemoryPool.
     * It intialize a memory pool with a specified element number and element size,
     * then it try to malloc enough memory to hold these element. If there are
     * not enough memory, it will use up all available.
     * @param[in] element_num number of elements that will be put into this pool
     * @param[in] element_size size of element that will be put into this pool
     */
 MemoryPool(size_t element_num,
               size_t element_size):_element_size(element_size),
        _current_element_number(0)
	{
            _free_queue.head = _free_queue.tail = MP_NULL;
            _delete_queue.head = _delete_queue.tail = MP_NULL;
            for (size_t i = 0; i < MAXCHUNKNUM; i++)
            {
                _chunk_list[i].chunk_begin = NULL;
            }
            reserve(element_num);
	}

    /**
     * ~MemoryPool() is destructor of class Memory.
     * It free all memory to the system.
     */
    ~MemoryPool()
    {
        for (size_t i = 0; i < _chunk_number; i++)
        {
            free(_chunk_list[i].chunk_begin);
        }
    }

    /**
     * reserve(int) try to malloc enough memory to hold a specified number of elements.
     * If there are not enough memory, it will use up all available.
     *
     * @param[in] element_num number of elements that will be put into this pool
     *
     * @note MemoryPool(size) + reserver(num) = Memory(num, size)
     */
    void reserve(size_t element_num)
    {
        // compute the size of each chunk and how many chunks needed
        _node_size = _element_size + sizeof(element_node_t);
        _chunk_capacity = MAXCHUNKSIZE / _node_size;
        _chunk_size = _chunk_capacity * _node_size;

        size_t chunk_needed = element_num / _chunk_capacity + 1;
        _chunk_number = 0;

        // alloc _chunk_number chunks
        // if alloc failed, modify _chunk_number as real chunk number alloced
        for (size_t i = 0; i < chunk_needed && i < MAXCHUNKNUM; i++)
        {
            if (_add_a_chunk() == 0)
            {
                break;
            }
        }

    }

    /**
     * gets pointer address identified by mp_id
     * @param[in] mp_id index of memorypool
     */
    inline void *address(size_t mp_id) const
    {
        if (mp_id == MP_NULL)
            return NULL;
        else
            return (void *) ((char *) _get_node_address(mp_id) +
                             sizeof(element_node_t));
    };

    /**
     * alloc() alloc a node from the memory pool. It first try to look for a
     * node in free_queue. If free_queue is empty, then it calls _add_a_chunk()
     * to malloc a block of memory from OS.
     * If _add_a_chunk still failed, this means memory has run out,
     * it returns MP_NULL.
     *
     * @return
     *	- MP0_ID	the MP ID of the node that alloced
     *	- MP_NULL	when there a no node alloced because real memory has run out
     */
    inline MP0_ID alloc()
    {
        MP0_ID ret;

        ret = _pop_out(_free_queue);	// try to take of the first node from _free_queue

        // if _free_queue is empty, add a new chunk
        // this need a malloc() from real memory
        if (ret == MP_NULL)
        {
            if (0 == _add_a_chunk())
            {					// add chunk failed
#ifdef DEBUG
                cerr << "memory run out, no node alloced" << endl;
#endif
                return MP_NULL;
            }
            ret = _pop_out(_free_queue);
        }
        if (ret != MP_NULL)
        {
            _current_element_number++;
        }
        return ret;
    }

    /**
     * delayed_dealloc(MP0_ID) put a node to _delete_queue.
     * Nodes in _delete_queue can not be alloced until _delete_queue is
     * merged into _free_queue.
     * @param[in] dlt_node the MP ID of the node that is to be deleted
     */
	
    inline void delayed_dealloc(MP0_ID dlt_node)
    {
        _push_back(_delete_queue, dlt_node);
        _current_element_number--;
    }

    /**
     * dealloc(MP0_ID) put a node to _free_queue.
     *  Nodes in _free_queue can be alloced immediately.
     * @param[in] free_node the MP ID of the node that is to be freed
     */
    inline void dealloc(MP0_ID free_node)
    {
        _push_back(_free_queue, free_node);
        _current_element_number--;
    }

    /**
     * recycle_delayed() merge _delete_queue into _free_queue
     */
    inline void recycle_delayed()
    {
        _free_a_queue(_delete_queue);
    }

    //! returns chunk number of memory pool
    inline size_t chunk_count() const
    {
        return _chunk_number;
    };

    //! returns chunk capacity of memory pool
    inline size_t chunk_capacity() const
    {
        return _chunk_capacity;
    };

    //! returns capacity of memory pool
    inline size_t capacity() const
    {
        return _chunk_number * _chunk_capacity;
    };

    //! returns current element number
    inline size_t alloc_count() const
    { return _current_element_number; }


 private:
    ////==========================_push_back()=============================== **
    //   _push_back(mp_queue_t&, MP0_ID) push a node to the end of a queue
    //
    //   Input:
    //     queue    -   the queue that the node will be added to
    //     mpid -   the MP ID of the node
    //
    ////=================================================================== **
    inline void _push_back(mp_queue_t & queue, MP0_ID mpid)
    {
        element_node_t *end_node;
        if (mpid == MP_NULL)
        {						// invalid input
            return;
        }
        if (queue.head == MP_NULL)
        {						// as the first node
            queue.head = mpid;
            queue.tail = mpid;
        }
        else
        {						// add to the tail of the queue
            end_node = _get_node_address(queue.tail);
            end_node->next = mpid;
            queue.tail = mpid;
        }

        // this is important!!!
        // let the tail node's next point to MP_NULL
        end_node = _get_node_address(queue.tail);
        end_node->next = MP_NULL;
    }

    ////==========================_pop_out()================================== **
    //   _pop_out(mp_queue_t&) : take a node out from a queue
    //
    //   Input:
    //     queue    -   the queue that the node will be taken out
    //
    ////===================================================================== **
    MP0_ID _pop_out(mp_queue_t & queue)
    {
        MP0_ID ret;
        ret = queue.head;
        element_node_t *hdnode;
        hdnode = (element_node_t *) _get_node_address(queue.head);	// if the queue is empty, hdnode would be NULL
        if (NULL != hdnode)
        {						// if the queue is not empty
            queue.head = hdnode->next;
            hdnode->next = MP_NULL;
            if (queue.head == MP_NULL)
            {					// if the last node has been taken off
                queue.tail = MP_NULL;
            }
        }

        return ret;
    }

    ////==========================_add_a_chunk()============================= **
    //  _add_a_chunk() calls malloc() to add a memory chunk to _chunk_list.
    //     It failed either memory has run out or memory pool is full.
    //
    //   Return:
    //     0    -   failed
    //     1    -   succeed
    //
    ////==================================================================== **
    int _add_a_chunk()
    {
        // has reached the top limit of chunk number
        if (_chunk_number == MAXCHUNKNUM)
        {
            ST_FATAL ("reached max chunk number, no chunk added ");
            return 0;
        }

        size_t chunk_idx = _chunk_number;
        _chunk_list[chunk_idx].chunk_begin =
            (element_node_t *) malloc(_chunk_size);
        if (NULL == _chunk_list[chunk_idx].chunk_begin)
        {
            // this can only happen when system overload is too heavy
            // memory run out, malloc return NULL
            ST_FATAL ("no chunk added because memory run out");
            return 0;
        }

        // format this chunk as a queue
        size_t offset = 0;
        for (size_t i = 0; i < _chunk_capacity; i++)
        {
            size_t ndid = (chunk_idx << 21) | (offset & OFFSETMASK);
            offset += _node_size;
            _push_back(_free_queue, ndid);
        }
        _chunk_number++;

        return 1;
    }

    ////==========================_free_a_queue()============================== **
    //   mp_free_queue(mp_queue_t&) merge a queue into free_queue
    //
    //   Input:
    //     queue    -   the head node of the queue that is to be merged
    //
    ////===================================================================== **
    void _free_a_queue(mp_queue_t & queue)
    {
        if (queue.head == MP_NULL)
        {
            return;
        }

        if (_free_queue.head == MP_NULL)
        {
            _free_queue.head = queue.head;
            _free_queue.tail = queue.tail;
        }
        else
        {
            element_node_t *node =
                (element_node_t *) _get_node_address(_free_queue.tail);
            node->next = queue.head;
            _free_queue.tail = queue.tail;
        }
        queue.head = MP_NULL;
        queue.tail = MP_NULL;
    }

    inline element_node_t *_get_node_address(size_t mp_id) const
    {							//根据mp地址获得实际内存地址
        if (mp_id == MP_NULL)
        {
            return NULL;
        }
        else
        {
            if ((mp_id >> 21) > _chunk_number)
            {
                // cout << "invalid MPID" << endl;
                return NULL;
            }
            return (element_node_t *) ((char *) _chunk_list[mp_id >> 21].
                                       chunk_begin + (mp_id & OFFSETMASK));
        }
    }
};


#endif
