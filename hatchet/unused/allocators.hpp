// a collection of same-size-element allocators used in smalltable
// Author: gejun@baidu.com
// Date: Tue Aug  3 18:48:42 2010
#pragma once
#ifndef _ALLOCATORS_HPP_
#define _ALLOCATORS_HPP_

#include "memory_pool.h"
#include "common.h"

namespace st
{
    //! @brief this abstract class just shows interfaces that a functional allocator MUST have
    template <typename InputValue, typename InputPointer=InputValue*>
    struct allocator_t
    {
        //! type of this class
        typedef allocator_t<InputValue, InputPointer> Self;
        
        //! @brief type of pointer to allocated memory
        typedef InputPointer Pointer;
        
        //! @brief type of allocated value
        typedef InputValue Value;
        
        //! @brief allocate space for a value and properly initialize it
        inline Pointer allocate ();

        //! @brief return the space back, generally the content should be kept and destructor should be delayed until recycle_delayed is called
        inline void deallocate (Pointer);

        //! @brief get the real storage address from Pointer
        inline Value* value_ptr (Pointer);

        //! @brief get the Pointer corresponding to NULL
        inline Pointer null() const;

        //! @brief generally this function destructs objects previously passed to "deallocate" and make the space re-allocatable
        inline void recycle_delayed ();
    };

    //! @brief allocate objects of type T and return virtual pointers (MP0_ID)
    template <typename T>
    struct allocator_t<T, MP0_ID>
    {
        //! type of this class
        typedef allocator_t<T,MP0_ID> Self;
        
        //! type of storage pointer
        typedef MP0_ID Pointer; 
        
        //! type of storage value
        typedef T Value;
        
        //! constructor
        explicit allocator_t ()
            : mp_(sizeof(T))
        {}

        //! @brief get space from mp_ and call ctor of T
        inline Pointer allocate ()
        {
            MP0_ID p = mp_.alloc();
            if (MP_NULL != p) {
                new (mp_.address(p)) T();
            }
            return p;
        }

        //! @brief call ~ctor of T and return space to mp_
        inline void deallocate (Pointer p)
        {
            if (MP_NULL != p) {
                ((T*)mp_.address(p))->~T();
            }
            mp_.delayed_dealloc (p);
        }

        //! really free "deallocated" memory
        inline void recycle_delayed ()
        { mp_.recycle_delayed (); }
        
        inline Value* value_ptr (Pointer ptr)
        { return (Value*)(mp_.address(ptr)); }
        
        inline Pointer null() const
        { return MP_NULL; }

    private:
        MemoryPool mp_;
    };

    //! allocate T* instead of MP0_ID
    template <typename T>
    struct allocator_t<T, T*>
    {
    private:
        MemoryPool mp_;
        //! use tag to save storing MP0_ID
        struct WithTag {
            T value;
            MP0_ID tag;
        };
            
    public:
        //! type of this class
        typedef allocator_t<T,T*> Self;
        
        //! type of handle to allocated memory
        typedef T* Pointer;
        
        //! type of allocated memory
        typedef T Value;
        
        //! constructor
        explicit allocator_t ()
            : mp_ (sizeof(WithTag))
        {}

        //! @note this function calls ctor
        inline Pointer allocate ()
        {
            MP0_ID p = mp_.alloc();
            if (MP_NULL == p) {
                return NULL;
            }
            else {
                WithTag *p_wt = new (mp_.address(p)) WithTag();
                p_wt->tag = p;
                return &(p_wt->value);
            }
        }

        //! @note this function calls ~ctor
        inline void deallocate (Pointer ptr)
        {
            if (NULL != ptr) {
                WithTag* p_wt = (WithTag*)ptr;
                p_wt->value.~T();
                mp_.delayed_dealloc (p_wt->tag);
            }
        }
        
        //! really free "deallocated" memory
        inline void recycle_delayed ()
        { mp_.recycle_delayed (); }

        //! convert Pointer to real pointer
        inline Value* value_ptr (Pointer ptr)
        { return ptr; }
        
        //! return the Pointer to real NULL
        inline Pointer null() const
        { return NULL; }
    };
}

#endif /* _ALLOCATORS_HPP_ */
