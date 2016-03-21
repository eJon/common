/**

// Author: gejun@baidu.com
// Date: Thu Aug  5 11:38:50 2010
// Brief: modified from app/ecom/elib/ecommon-lib/el_container.h
 */
/************************************************************************
 *
 * Copyright (c) 2010 Baidu.com, Inc. All Rights Reserved
 * $Id: el_container.h,v 1.4 2008/07/23 09:27:56 guhao Exp $
 *
 ************************************************************************/
#pragma once
#ifndef _EL_HASHSET2_HPP_
#define _EL_HASHSET2_HPP_

#include <utility>
#include "functional.hpp"
#include "object_hanger.h"
#include <new>
#include "object_pool.hpp"

namespace st
{
    template < typename _Key
               , typename _Hash = hash<_Key>
               , typename _Equal = is_equal<_Key> >
    class ExtHashSet2
    {
    public:

        //! internal structures
        struct SEntry {
            _Key key;
            SEntry *pNext;
            OP_ID tag;
        };

        typedef ObjectPool<SEntry> Pool;

        typedef ExtHashSet2 <_Key, _Hash, _Equal> Self;

        void to_string (StringWriter& sb) const
        {
            sb << "{elem_cnt=" << size()
               << " hash_size=" << hash_size()
               << " sz_without_node=" << mem_without_node()
               << " content="
                ;
            //shows_range(sb, begin(), end());
            sb << "}";
        }
    
        //! structure for interface compatible with STL's map template
        class iterator
        {
        public:
            typedef _Key value_type;

            iterator()
                : p_cur_(NULL)
                , pp_begin_(NULL)
                , pp_end_(NULL)
            {}

            iterator(SEntry** pp_begin, SEntry** pp_end)
                : p_cur_(NULL)
                , pp_begin_(pp_begin)
                , pp_end_(pp_end)
            {
                while (pp_begin_ != pp_end_) {
                    if (*pp_begin_) {
                        p_cur_ = *pp_begin_;
                        break;
                    }
                    ++ pp_begin_;
                }
            }
        
            bool not_end() const
            { return NULL != p_cur_; }

            void operator++()
            {
                // if (NULL == p_cur_) {
                //     while (pp_begin_ != pp_end_) {
                //         if (NULL != *pp_begin_) {
                //             p_cur_ = *pp_begin_;
                //             return;
                //         }
                //         else {
                //             ++ pp_begin_;
                //         }
                //     }
                // }
                // else {
                //     p_cur_ = p_cur_->pNext;
                // }
                p_cur_ = p_cur_->pNext;
                if (NULL == p_cur_) {
                    ++ pp_begin_;
                    while (pp_begin_ != pp_end_) {
                        if (*pp_begin_) {
                            p_cur_ = *pp_begin_;
                            return;
                        }
                        ++ pp_begin_;
                    }
                }
            }

            value_type & operator* ()const
            { return p_cur_->key; }

            value_type* operator-> () const
            { return &(p_cur_->key); }

        protected:
            SEntry* p_cur_;
            SEntry** pp_begin_;
            SEntry** pp_end_;
        };

        /**
         * returns an iterator pointing to the beginning of the ExtHashSet2
         * @return
         *	- NULL Cannot get the beginning iterator
         *	- !NULL Success
         */
        iterator begin() const
        {
            return iterator(ap_entry_, ap_entry_ + hash_size_);
        }

        /**
         * destroy all entries in the map
         * @note the function does not free all the memory allcated by this map
         */
        void clear()
        {
            assert(ap_entry_);
            recycle_delayed();
            if (ap_entry_ != NULL) {
                if (count_ > 0) {
                    unsigned int nHash;
                    SEntry *pEntry;
                    SEntry *pNext;
                    for (nHash = 0; nHash < hash_size_; nHash++) {
                        for (pEntry = ap_entry_[nHash]; pEntry != NULL;) {
                            pNext = pEntry->pNext;
                            FreeEntry(pEntry);	// recycle the memory to m_pFreeList
                            pEntry = pNext;
                        }
                    }
                    assert(count_ == 0);
                }
                memset(ap_entry_, 0, sizeof(SEntry *) * hash_size_);
            }

        }

        //! true if the ExtHashSet2's size is 0.
        bool empty() const
        { return count_ == 0; }

        /**
         * Inserts key into the ExtHashSet2: if key already exists, update it with value;
         * SafeAdd (key, value) pair else.
         * @param[in] key
         * @param[in] value
         * @param[in] auto_resize identify auto resize hash table or not
         * @return
         *	- false if key exists and updated by value
         *	- true if key not exists and inserted
         */
        _Key*
        insert(const _Key & key, const bool auto_resize = true)
        {
            _Key* p_key = seek (key);
            if (p_key) {
                delayed_erase (key);
            }
            return SafeAdd (key, auto_resize);
        }

        std::pair<const _Key*, const _Key*>
        insert2(const _Key & key, const bool auto_resize = true)
        {
            _Key* p_key = seek (key);
            if (p_key) {
                delayed_erase (key);
            }
            return std::pair<const _Key*, const _Key*>(p_key, SafeAdd (key, auto_resize));
        }

        /**
         * finds an element matching key
         * @param[in] key
         * @return
         *	- NULL Cannot find matching item
         *	- !NULL iterator pointing to matching item
         */
        inline iterator find(const _Key & key)
        {
            SEntry *pEntry = GetEntryAt(key);
            if (pEntry == NULL) {
                return NULL;
            }
            else {
                return iterator((unsigned char *) pEntry + sizeof(SEntry *), this);
            }
        }

        template <typename _Key2, typename _Hash2, typename _Equal2>
        inline const _Key* seek_ref (const ObjectHanger& oh, const _Key2& key) const
        {
            // don't check
            // assert(ap_entry_ != NULL);	// never call on empty map
            // if (ap_entry_ == NULL)
            //     return NULL;

            _Hash2 hf2;
            _Equal2 ef2;

            unsigned int nHash = hf2(oh, key) % hash_size_;

            // see if it exists
            for (const SEntry *pEntry=ap_entry_[nHash]; pEntry; pEntry = pEntry->pNext) {
                if (ef2 (pEntry->key, oh, key)) {
                    return &(pEntry->key);
                }
            }
            return NULL;
        }

        inline _Key* seek (const _Key& key) const
        {
            SEntry *pEntry = GetEntryAt(key);
            return pEntry ? &(pEntry->key) : NULL;
        }
        
        //returns the size of ExtHashSet2
        size_t size() const
        {
            return count_;
        }

        size_t hash_size () const
        {
            return hash_size_;
        }

        size_t free_size () const
        {
            return p_pool_->free_count();
        }
        
        size_t delete_size () const
        {
            return p_pool_->delayed_free_count();
        }

        size_t block_size () const
        {
            return p_pool_->chunk_count();
        }

        size_t mem_without_node () const
        {
            return hash_size()*sizeof(SEntry*) +  sizeof(*this);
        }

        void bucket_info(size_t* p_max_len, size_t* p_min_len, double* p_avg_len) const
        {
            size_t max_len=0, min_len=UINT_MAX, sum_len=0;
            size_t hz = 0;
            for (size_t nBucket = 0; nBucket < hash_size_; nBucket++) {
                SEntry* e = ap_entry_[nBucket];
                if (e) {
                    ++ hz;
                    size_t len=1;
                    e = e->pNext;
                    for (; e; e=e->pNext, ++len);
                    if (len > max_len) {
                        max_len = len;
                    }
                    if (len < min_len) {
                        min_len = len;
                    }
                    sum_len += len;
                }
            }
            if (p_max_len) {
                *p_max_len = hz > 0 ? max_len : 0;
            }
            if (p_min_len) {
                *p_min_len = hz > 0 ? min_len : 0;
            }
            if (p_avg_len) {
                *p_avg_len = hz > 0 ? (sum_len/(double)hz) : 0;
            }
        }

        //swaps the contents of two maps
        void swap(ExtHashSet2 & other)
        {
            unsigned int t;
            SEntry **pt;
            SEntry *ft;

            t = other.count_;
            other.count_ = count_;
            count_ = t;

            t = other.hash_size_;
            other.hash_size_ = hash_size_;
            hash_size_ = t;

            pt = other.ap_entry_;
            other.ap_entry_ = ap_entry_;
            ap_entry_ = pt;
        }

        // same as resize
        void reserve(unsigned int nBucket)
        {
            ReSize(nBucket);
        }

        // resize the ExtHashSet2
        void resize(unsigned int nBucket)
        {
            ReSize(nBucket);
        }

        /**
         * the constructor
         * you may specify a hint size for the hash table
         *
         * BlockSize = 10 is sufficient in most case
         */
        explicit ExtHashSet2(Pool* p_pool
                            , int nBucket = 10
                            , unsigned int resizeThreshold = 80
                            )
        {
            p_pool_ = p_pool;
            hash_size_ = 0;
            count_ = 0;
            ap_entry_ = NULL;
            resize_threshold_ = resizeThreshold;            
            ReSize(nBucket);
        }

        explicit ExtHashSet2(const Self & other)
        {
            *this = other;
        }

        ~ExtHashSet2()
        {
            RemoveAll();
        }

        //! number of elements
        unsigned int GetCount() const
        {
            return count_;
        }

        //! whether created
        bool IsValid() const
        {
            return ap_entry_ != NULL;
        }


        //! Duplicate map, do deep copy
        const Self &operator =(const ExtHashSet2 & src)
        {
            // keep resize_threshold_
            if (this != &src) {
                RemoveAll();
                ReSize(src.hash_size_);

                void *pos;
                _Key *pKey;
                bool more = src.FindFirst(pos, pKey);
                while (more) {
                    this->SafeAdd(*pKey, false);
                    more = src.FindNext(pos, pKey);
                }
            }
            return *this;
        }

        //! removing existing key-value pair
        //! \return whether succeessful
        _Key* erase(const _Key & key, bool delayed = false)
        {
            if (ap_entry_ == NULL)
                return NULL;		// nothing in the table

            SEntry **ppEntryPrev;
            ppEntryPrev = &ap_entry_[hf_(key) % hash_size_];

            SEntry *pEntry;
            for (pEntry = *ppEntryPrev; pEntry != NULL; pEntry = pEntry->pNext) {
                if (ef_(pEntry->key, key)) {
                    // remove it
                    *ppEntryPrev = pEntry->pNext;	// remove from list
                    if (delayed) {
                        DeleteEntry(pEntry);
                        return &(pEntry->key);
                    }
                    else {
                        FreeEntry(pEntry);
                        // it's freed
                        return NULL;
                    }
                }
                ppEntryPrev = &pEntry->pNext;
            }
            return NULL;			// not found
        }

        //! removing existing key-value pair
        //! will not free memory or reuse it
        //! \return whether succeessful
        _Key* delayed_erase(const _Key & key)
        {
            return erase (key, true);
        }

        /**
         * 将deletelist插入freelist的头部
         */
        void recycle_delayed()
        {
            p_pool_->recycle_delayed();
        }



        /*!
          \brief set the size of hashtable
          \note always call this function before other operations
          \param nHashSize the possible total entries in this map
        */
        void ReSize(unsigned int nBucket)
        {
            unsigned int i;
            unsigned int nHashSize;

            unsigned int nListSize =
                sizeof(internal_prime_list) / sizeof(internal_prime_list[0]);
            for (i = 0; i < nListSize; i++) {
                if (internal_prime_list[i] >= nBucket)
                    break;
            }
            nHashSize = internal_prime_list[i];

            if (NULL == ap_entry_) {
                ap_entry_ = new (std::nothrow) SEntry *[nHashSize];
                if (NULL == ap_entry_) {
                    ST_FATAL ("Fail to new ap_entry_");
                    return;
                }
                memset(ap_entry_, 0, sizeof(SEntry *) * nHashSize);
                hash_size_ = nHashSize;
            }
            else {
                SEntry *pEntry;
                SEntry **ppNewtable;
                unsigned int i, nNewhash, nOld;
                nOld = hash_size_;
                ppNewtable = new (std::nothrow) SEntry *[nHashSize];
                if (NULL == ppNewtable) {
                    ST_FATAL ("Fail to new ppNewtable, quit %s", __FUNCTION__);
                    return;
                }

                memset(ppNewtable, 0, sizeof(SEntry *) * nHashSize);

                for (i = 0; i < nOld; i++) {
                    pEntry = ap_entry_[i];
                    while (pEntry) {
                        nNewhash = hf_(pEntry->key) % nHashSize;
                        ap_entry_[i] = pEntry->pNext;
                        pEntry->pNext = ppNewtable[nNewhash];
                        ppNewtable[nNewhash] = pEntry;
                        pEntry = ap_entry_[i];
                    }
                }

                delete[]ap_entry_;
                ap_entry_ = ppNewtable;
                hash_size_ = nHashSize;
            }
        }
        
    protected:    
        // Implementation


        /**
         * safely add a new entry(key value pair),  key must be unique
         * if key is same, memory allocated for old key will leak
         */
        _Key *SafeAdd(const _Key & key, bool auto_resize)
        {
            unsigned int nHash;
            assert(ap_entry_ != NULL);
            assert(GetEntryAt(key) == NULL);

            // always add a new entry
            SEntry *pEntry = SafeNewEntry(key, auto_resize);
            nHash = hf_(key) % hash_size_;

            // put into hash table
            pEntry->pNext = ap_entry_[nHash];
            ap_entry_[nHash] = pEntry;
            return &(pEntry->key);	// return pointer to created element
        }



        /**
         * get NewEntry without ReSize the ExtHashSet2
         *  - Benefits: 
         *	MP-Safe in 1-Write n-Read situations
         *  - Shortcomings:
         *	Losing Efficience when hash talbe is full
         */
        SEntry* SafeNewEntry(const _Key & key, const bool auto_resize)
        {
            if (auto_resize
                && (count_ * 100) >= (hash_size_ * resize_threshold_))
            {
                ReSize(hash_size_+100);  // get next prime, check the prime array in common.h
            }

            OP_ID handle = p_pool_->alloc();
            SEntry* p_e = p_pool_->address(handle);
            p_e->key = key;
            p_e->tag = handle;
            ++ count_;
            return p_e;
        }

        //! find entry (or return NULL)
        inline SEntry *GetEntryAt(const _Key & key) const
        {
            assert(ap_entry_ != NULL);	// never call on empty map
            if (ap_entry_ == NULL)
                return NULL;

            unsigned int nHash = hf_(key) % hash_size_;

            // see if it exists
            SEntry *pEntry;
            for (pEntry = ap_entry_[nHash]; pEntry != NULL;
                 pEntry = pEntry->pNext)
            {
                if (ef_(pEntry->key, key) == true)
                    return pEntry;
            }
            return NULL;
        }

        //! free entry
        void FreeEntry(SEntry * pEntry)
        {
            if (pEntry) {
                p_pool_->dealloc (pEntry->tag);
                -- count_;
            }
        }

        void DeleteEntry(SEntry * pEntry)
        {
            if (pEntry) {
                p_pool_->delayed_dealloc (pEntry->tag);
                -- count_;
            }
        }


        //! remove all entries, free all the allocated memory
        void RemoveAll()
        {
            recycle_delayed();
            if (ap_entry_ != NULL) {
                // destroy elements
                if (count_ > 0) {
                    unsigned int nHash;
                    for (nHash = 0; nHash < hash_size_; ++nHash) {
                        SEntry *pEntry, *pNext;
                        for (pEntry = ap_entry_[nHash]; pEntry != NULL;)
                        {
                            pNext = pEntry->pNext;
                            FreeEntry(pEntry);	// recycle the memory to m_pFreeList
                            pEntry = pNext;
                        }
                    }
                }
                // free hash table
                delete[]ap_entry_;
                ap_entry_ = NULL;
            }

            count_ = 0;
            /*
              m_pFreeList pointed memory are part of m_pBlocks pointed,
              therefore it need not free
            */
            hash_size_ = 0;
        }


    private:
        SEntry ** ap_entry_;	  //the entry table whose index is the hash_value
        Pool* p_pool_;
        unsigned int hash_size_;     //!< size of hash table
        unsigned int count_;	       //!< key-value pairs in the table
        unsigned int resize_threshold_;
        _Hash hf_;
        _Equal ef_;
    };
}

#endif  //_EL_HASHSET2_HPP_

