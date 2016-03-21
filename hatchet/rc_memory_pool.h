// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Allocate and share same-size memory blocks efficiently and conveniently,
// memory blocks are reference counted and deallocated after their reference
// counter hits zero, different from shared_ptr, reference/deference are
// intended to be managed manually
//
// Author: gejun@baidu.com
// Date: Sat Nov 13 16:38:09 CST 2010
#pragma once
#ifndef _RC_MEMORY_POOL_H_
#define _RC_MEMORY_POOL_H_

// Uncomment following macro to store free nodes in queue rather stack
//#define STORE_FREE_IN_QUEUE
#include <unistd.h> 
#include "common.h"         // to_string, logging, helper macros
#include <vector>           // std::vector
#ifdef STORE_FREE_IN_QUEUE
#include <deque>            // std::deque
#endif

namespace st {

template <class _RC> class TRCMemoryPool {
public:
    struct Slot {             // basic memory block
        _RC rc_;            // number of users of this memory slot
        char mem_[];     // mark beginning of the object
    };

    struct Chunk {};           // group of slots

    static const size_t DEFAULT_CHUNK_SIZE = 1 << 22;  // 4M
    
public:        
    // Construct a reference counting memory pool
    // Params:
    //   item_size   size of allocated memory block
    //   chunk_size  memory blocks are allocated from system in chunks,
    //               this is the size.
    TRCMemoryPool (u_int item_size, u_int chunk_size = DEFAULT_CHUNK_SIZE);

    // Destroy this pool
    ~TRCMemoryPool();
        
    // Allocate a memory block from this pool
    // Note: reference counter of the memory is set to 1
    // Returns: address of allocated memory
    void* alloc_mem ();

    // Allocate a memory block and create an object in-place.
    // Note: size of the object must not exceed item_size given in
    //       constructor. Reference counter of the memory is set to 1
    // Returns: address of the object
    template <typename _T>
    _T* alloc_object () { return ST_NEW_ON(alloc_mem(), _T); }
    
    // Remove a reference to a memory block (allocated from this pool), if
    // the counter hits zero, the memory is back to this pool.
    static void dec_ref (void* p_mem, TRCMemoryPool* p_alloc);

    template <typename _DestructFn>
    static void dec_ref (void* p_mem,
                         TRCMemoryPool* p_alloc,
                         const _DestructFn& destruct_fn);

    // Remove a reference to an object(allocated from this pool), if
    // the counter hits zero, the object is destructed and the memory
    // is back to this pool
    template <typename _T>
    static void dec_ref (_T* p_object, TRCMemoryPool* p_alloc);

    // Add a reference to the memory
    static void inc_ref (void* ptr) { get_slot(ptr)->rc_.inc(); }
    
    // Get number of users of the memory
    static bool is_wild (const void* ptr)
    { return get_slot(ptr)->rc_.is_wild(); }

    static bool is_local (const void* ptr)
    { return get_slot(ptr)->rc_.is_mutable(); }

    static typename _RC::value_type get_rc_data (const void* ptr)
    { return get_slot(ptr)->rc_.internal_data(); }

    // Get the reference counter
    static _RC& get_rc (void* ptr) { return get_slot(ptr)->rc_; }
    static const _RC& get_rc (const void* ptr) { return get_slot(ptr)->rc_; }

    // Get number of chunks
    size_t chunk_num () const { return ap_chunk_.size(); }

    // Get size of a chunk
    size_t chunk_size () const { return chunk_size_; }

    // Get maximum number of slots in a chunk
    size_t chunk_slot_num () const { return n_chunk_slot_; }

    // Get maximum number of slots in this pool,
    // Note: grows when all existing slots are used
    size_t capacity () const { return chunk_num() * n_chunk_slot_; }

    // Get number of allocated slots
    size_t alloc_num () const { return n_alloc_; }

    // Get number of free slots
    size_t free_num () const { return free_list_.size(); }

    // Get memory taken by this pool
    size_t mem () const
    {
        const int page_size = getpagesize();
        return sizeof(*this) +
            chunk_num() * chunk_size() -
            (chunk_size() - chunk_off_) / page_size * page_size +
            ap_chunk_.capacity() * sizeof(Chunk*) +
            free_list_.size() * sizeof(Slot*);
    }

    // Print info
    // Note: this is printed by many data structures, make it elegant
    void to_string (StringWriter& sw) const
    {
        sw << "{n_alloc/free/chunk="
           << alloc_num() << '/' << free_num() << '/' << chunk_num()
           << " sz_slot/all=" << slot_size_ << '/' << mem();
        sw << "}";
    }

private:
    // Disallow copy
    TRCMemoryPool (const TRCMemoryPool&);
    TRCMemoryPool& operator= (const TRCMemoryPool&);
    
    // Convert pointer to Slot*
    static const u_int OBJECT_OFFSET_IN_SLOT = offsetof(Slot, mem_);

    inline static Slot* get_slot (void* ptr)
    { return reinterpret_cast<Slot*>(reinterpret_cast<char*>(ptr)
                                      - OBJECT_OFFSET_IN_SLOT); }

    inline static const Slot* get_slot (const void* ptr)
    { return reinterpret_cast<const Slot*>(reinterpret_cast<const char*>(ptr)
                                           - OBJECT_OFFSET_IN_SLOT); }

    
    // Construct a Chunk, no alignment
    static Chunk* chunk_create (u_int slot_size, u_int n_slot)
    { return reinterpret_cast<Chunk*>(ST_NEW_ARRAY(char, slot_size*n_slot)); }

    // Destroy a Chunk
    static void chunk_destroy (Chunk* p_chunk)
    { delete [] reinterpret_cast<char*>(p_chunk); }
    
    u_int n_alloc_;                 // number of allocated slots
    u_int chunk_off_;               // position of first un-allocated slot
    u_int chunk_size_;              // size of a chunk
    u_int n_chunk_slot_;            // number of slots in a chunk
    u_int slot_size_;               // size of a slot
    std::vector<Chunk*> ap_chunk_;  // chunk array
    
    // free slots
#ifdef STORE_FREE_IN_QUEUE
    std::deque<Slot*> free_list_;
#else
    // Push free slots to a stack, not a queue because the slot is
    // reference counted and generally safer to avoid doubly free,
    // so I use a stack here to bring more locality
    std::vector<Slot*> free_list_;
#endif 
};

struct DefaultRefCounter {
    typedef uint32_t value_type;
    
    void init () { n_ = 1; }
    void inc () { ++ n_; }
    void dec () { -- n_; }
    bool is_wild () const { return 0 == n_; }
    bool is_mutable () const { return 1 == n_; }
    value_type internal_data () const { return n_; }

private:
    value_type n_;
};

struct MinorRefCounter {
    typedef uint32_t value_type;

    void init () { n_ = 1; }
    void inc () { ++ n_; }
    void dec () { -- n_; }
    bool is_wild () const { return 0 == combo_; }
    bool is_mutable () const { return 1 == n_; }
    value_type internal_data () const { return combo_; }

    void inc_minor () { ++ m_; }
    void dec_minor () { -- m_; }
    value_type get_minor () const { return m_; }
    
private:
    union {
        struct {
            value_type n_ : 8;
            value_type m_ : 24;
        };
        value_type combo_;
    };
};

typedef TRCMemoryPool<DefaultRefCounter> RCMemoryPool;


// ----------------
//  Implementations
// ----------------
template <class _RC>
TRCMemoryPool<_RC>::TRCMemoryPool (u_int item_size, u_int chunk_size)
{
    ap_chunk_.reserve(64);
#ifndef STORE_FREE_IN_QUEUE
    free_list_.reserve(1024);
#endif
    n_alloc_ = 0;
    slot_size_ = sizeof(Slot) + item_size;  // no alignment
    n_chunk_slot_ = chunk_size / slot_size_;
    chunk_size_ = slot_size_ * n_chunk_slot_;
    chunk_off_ = chunk_size_;  // Makes a chunk created before first allocation
}

template <class _RC>
TRCMemoryPool<_RC>::~TRCMemoryPool()
{
    while (!ap_chunk_.empty()) {
        chunk_destroy(ap_chunk_.back());
        ap_chunk_.pop_back();
    }
}

template <class _RC>
void* TRCMemoryPool<_RC>::alloc_mem ()
{
    // check free_list_ first
    if (!free_list_.empty()) {
#ifdef STORE_FREE_IN_QUEUE
        Slot* p_slot = free_list_.front();
        free_list_.pop_front();
#else
        Slot* p_slot = free_list_.back();
        free_list_.pop_back();
#endif
        ++ n_alloc_;
        p_slot->rc_.init();
        return p_slot->mem_;
    }
    // no slot in free_list_

    if (chunk_off_ >= chunk_size_) {
        Chunk* p_chunk = chunk_create(slot_size_, n_chunk_slot_);
        if (NULL == p_chunk) {
            ST_FATAL("Fail to new p_chunk");
            return NULL;
        }
        ap_chunk_.push_back(p_chunk);
        chunk_off_ = 0;
    }
            
    ++ n_alloc_;
    Slot* p_slot = reinterpret_cast<Slot*>
        (reinterpret_cast<char*>(ap_chunk_.back()) + chunk_off_);
    chunk_off_ += slot_size_;
    p_slot->rc_.init();
    return p_slot->mem_;
}

template <class _RC>
void TRCMemoryPool<_RC>::dec_ref (void* p_mem, TRCMemoryPool* p_alloc)
{
    Slot* p_slot = reinterpret_cast<Slot*>
        (reinterpret_cast<char*>(p_mem) - OBJECT_OFFSET_IN_SLOT);
    if (unlikely (p_slot->rc_.is_wild())) {
        ST_FATAL("doubly free ptr=%p, rc=%d", p_mem, p_slot->rc_.internal_data());
        return;
    }
    p_slot->rc_.dec();
    if (p_slot->rc_.is_wild()) {
        p_alloc->free_list_.push_back(p_slot);
        -- p_alloc->n_alloc_;  
    }
}

template <class _RC>
template <class _DestructFn>
void TRCMemoryPool<_RC>::dec_ref (void* p_mem, TRCMemoryPool* p_alloc,
                                  const _DestructFn& destruct_fn)
{
    Slot* p_slot = reinterpret_cast<Slot*>
        (reinterpret_cast<char*>(p_mem) - OBJECT_OFFSET_IN_SLOT);
    if (unlikely (p_slot->rc_.is_wild())) {
        ST_FATAL("doubly free ptr=%p, rc=%d", p_mem, p_slot->rc_.internal_data());
        return;
    }
    p_slot->rc_.dec();
    if (p_slot->rc_.is_wild()) {
        destruct_fn(p_mem);
        p_alloc->free_list_.push_back(p_slot);
        -- p_alloc->n_alloc_;  
    }
}

template <class _RC>
template <typename _T>
void TRCMemoryPool<_RC>::dec_ref (_T* p_object, TRCMemoryPool* p_alloc)
{
    Slot* p_slot = get_slot(p_object);
    if (unlikely (p_slot->rc_.is_wild())) {
        ST_FATAL("doubly free ptr=%p, rc=%d", p_object, p_slot->rc_.internal_data());
        return;
    }
    p_slot->rc_.dec();
    if (p_slot->rc_.is_wild()) {
        p_object->~_T();
        p_alloc->free_list_.push_back(p_slot);
        -- p_alloc->n_alloc_;  
    }
}

}  // namespace st

#endif  // _RC_MEMORY_POOL_H_
