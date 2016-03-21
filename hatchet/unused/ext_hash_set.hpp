// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Modified from app/ecom/elib/ecommon-lib/el_container.h
// Author: gejun@baidu.com
// Date: Thu Aug  5 11:38:50 2010

#pragma once
#ifndef _EL_HASHSET_HPP_
#define _EL_HASHSET_HPP_

#include "utility.hpp"
#include "debug.h"
#include "functional.hpp"
#include "object_hanger.h"

#define BEFORE_STARTING_POS ((void*)-1L)
namespace st {
/**
 * @brief
 * Template hash table( key-value pair )
 *
 * Requirements for _Value and _Key:
 * 	They can be any fix-size type or class which has overloaded '=' opeartor
 * 	eg. int, WString, struct, union, and any other l-value type.
 * 	
 * 	@note Neither pointer nor array are valid type.
 * 		eg. PSTR, char[24], float[10]
 * 		void* can be used as valid type. But with care!
 *
 * 	- They must have a default constructor or a user-define constructor without parameters.
 * ownership policy:
 * 	- _Key and _Value are copyed using '=' operator.
 * 	- They are owned by ExtHashSet.
 * 	- Becareful when a narrow-copy/share-copy '=' operator is defined.
 *
 * 	@note
 *
 * 	- 在使用iterator进行迭代遍历时，同时对hashtable中的元素进行增/删操作，后续迭代的结果是不可预期的
 * 	- 默认情况下，insert操作可能引发hashtable的resize操作；resize过程中，对hashtable读操作的结果是不可预期的
 * 
 * ExtHashSet quick reference:
 * 	- Insert/Retrieve:	insert / []
 * 	- Search entry:		find / Lookup
 * 	- Remove entry:		Remove
 * 	- Iteration:		FindFirst/FindNext
 * 	- Clear the table:	clear
 * 	- Reserve memory(not necessary): resize/reserve
 * 	- Empty  array(not necessary): clear
 */


template < class _Key
           , typename _Hash = Hash<_Key>
           , typename _Equal = Equal<_Key> >
class ExtHashSet
{
public:
    typedef ExtHashSet <_Key, _Hash, _Equal> this_type;


    void to_string (StringWriter& sb) const
    {
        size_t bkt_max_len, bkt_min_len;
        double bkt_avg_len;
        bucket_info(&bkt_max_len, &bkt_min_len, &bkt_avg_len);

        sb << "{elem_cnt=" << size()
           << " hash_size=" << hash_size()
           << " max_len=" << bkt_max_len
           << " min_len=" << bkt_min_len
           << " avg_len=" << bkt_avg_len
           << " free=" << free_size()
           << " 2b_free=" << delete_size()
           << " block_cnt=" << block_size()
           << " content="
            ;
        //shows_range(sb, begin(), end());
        sb << "}";
    }
    
    //! structure for interface compatible with STL's map template
    class iterator:public std::iterator < std::forward_iterator_tag, _Key >
    {
    public:
        /**             * in gcc 2.96, it seems that value_type can't be inherited from std::iterator
         */
        typedef _Key value_type;

        iterator()
        {
            ptr = NULL;
            pMap = NULL;
        }

        iterator(void *p)
        {
            ptr = (value_type *) p;
            pMap = NULL;
        }
        
        iterator(void *pEntry, const void *map)
        {
            ptr = (value_type *) pEntry;
            pMap = (const this_type *)map;
        }

        bool operator ==(const iterator & iter) const
        {
            return iter.ptr == ptr;
        }
        bool operator !=(const iterator & iter) const
        {
            return iter.ptr != ptr;
        }

        /**
         * prefix: increase and then fetch
         */
        iterator & operator ++()
        {
            _Key *pKey;
            void *pos;
            pos = (char *) ptr - sizeof(SEntry *);
            if (pMap->FindNext(pos, pKey)) {
                ptr = (value_type *) ((unsigned char *) pos + sizeof(SEntry *));
            }
            else {
                ptr = NULL;
            }
            return *this;
        }

        /**
         * postfix: fetch and then increase
         */
        const iterator operator ++(int)
        {
            iterator tmp = *this;
            _Key *pKey;
            void *pos;
            pos = (unsigned char *) ptr - sizeof(SEntry *);
            if (pMap->FindNext(pos, pKey)) {
                ptr = (value_type *) ((unsigned char *) pos + sizeof(SEntry *));
            }
            else {
                ptr = NULL;
            }

            return tmp;
        }

        value_type & operator *()const
        {
            return (value_type &) * ptr;
        }

        value_type *operator ->() const
        {
            return (value_type *) ptr;
        }

    protected:
        value_type * ptr;
        const this_type *pMap;
    };

    typedef typename iterator::value_type value_type;
    typedef typename iterator::pointer pointer;
    typedef typename iterator::reference reference;

    /**
     * returns an iterator pointing to the beginning of the ExtHashSet
     * @return
     *	- NULL Cannot get the beginning iterator
     *	- !NULL Success
     */
    iterator begin() const
    {
        _Key *pKey;
        void *pos;
        if (FindFirst(pos, pKey)) {
            return iterator((unsigned char *) pos + sizeof(SEntry *), this);
        }
        else {
            return NULL;
        }
    }

    /**
     * destroy all entries in the map
     * @note the function does not free all the memory allcated by this map
     */
    void recycle()
    {
        assert(m_pHashTable);
        recycle_delayed();
        if (m_pHashTable != NULL) {
            if (m_nCount > 0) {
                unsigned int nHash;
                SEntry *pEntry;
                SEntry *pNext;
                for (nHash = 0; nHash < m_nHashTableSize; nHash++) {
                    for (pEntry = m_pHashTable[nHash]; pEntry != NULL;) {
                        pNext = pEntry->pNext;
                        FreeEntry(pEntry);	// recycle the memory to m_pFreeList
                        pEntry = pNext;
                    }
                }
                assert(m_nCount == 0);
                assert(m_pFreeList != NULL);
            }
            memset(m_pHashTable, 0, sizeof(SEntry *) * m_nHashTableSize);
        }
    }

    /**
     * destroy all entries in the map
     * @note the function does not free all the memory allcated by this map
     */
    void clear()
    {
        assert(m_pHashTable);
        recycle_delayed();
        if (m_pHashTable != NULL) {
            if (m_nCount > 0) {
                unsigned int nHash;
                SEntry *pEntry;
                SEntry *pNext;
                for (nHash = 0; nHash < m_nHashTableSize; nHash++) {
                    for (pEntry = m_pHashTable[nHash]; pEntry != NULL;) {
                        pNext = pEntry->pNext;
                        FreeEntry(pEntry);	// recycle the memory to m_pFreeList
                        pEntry = pNext;
                    }
                }
                assert(m_nCount == 0);
                assert(m_pFreeList != NULL);
            }
            memset(m_pHashTable, 0, sizeof(SEntry *) * m_nHashTableSize);
        }

        if (m_pBlocks != NULL) {
            m_pBlocks->FreeDataChain();
        }
        m_pBlocks = NULL;
        m_pFreeList = NULL;		// need not free m_pFreeList, as it's alias of free space in m_pBlocks
    }

    /**
     * true if the ExtHashSet's size is 0.
     */
    bool empty() const
    {
        return (m_nCount == 0);
    }

    /**
     * returns a iterator pointing to the end of the ExtHashSet
     */
    iterator end() const
    {
        return NULL;
    }


    /**
     * Inserts key into the ExtHashSet: if key already exists, update it with value;
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
        // assert(m_pHashTable != NULL);	// never call on empty map
        // if (m_pHashTable == NULL)
        //     return NULL;

        _Hash2 hf2;
        _Equal2 ef2;

        unsigned int nHash = hf2(oh, key) % m_nHashTableSize;

        // see if it exists
        for (const SEntry *pEntry=m_pHashTable[nHash]; pEntry; pEntry = pEntry->pNext) {
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
        
    //returns the size of ExtHashSet
    size_t size() const
    {
        return m_nCount;
    }

    size_t hash_size () const
    {
        return m_nHashTableSize;
    }

    size_t free_size () const
    {
        size_t cnt=0;
        for (SEntry* p=m_pFreeList; p; p=p->pNext, ++cnt);
        return cnt;
    }
        
    size_t delete_size () const
    {
        size_t cnt=0;
        for (SEntry* p=m_pDeleteList; p; p=p->pNext, ++cnt);
        return cnt;
    }

    size_t block_size () const
    {
        size_t cnt=0;
        for (SDataChain* p=m_pBlocks; p; p=p->pNext, ++cnt);
        return cnt;
    }

    size_t mem () const
    {
        size_t block_mem=0;
        for (SDataChain* p=m_pBlocks; p; p=p->pNext) {
            block_mem += p->nSize;
        }
        return sizeof(*this)
            + hash_size()*sizeof(SEntry*)
            + block_mem
            ;
    }

    void bucket_info(size_t* p_max_len, size_t* p_min_len, double* p_avg_len) const
    {
        size_t max_len=0, min_len=UINT_MAX, sum_len=0;
        size_t hz = 0;
        for (size_t nBucket = 0; nBucket < m_nHashTableSize; nBucket++) {
            SEntry* e = m_pHashTable[nBucket];
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
    void swap(ExtHashSet & other)
    {
        unsigned int t;
        SEntry **pt;
        SDataChain *st;
        SEntry *ft;

        t = other.m_nCount;
        other.m_nCount = m_nCount;
        m_nCount = t;
        t = other.m_nBlockSize;
        other.m_nBlockSize = m_nBlockSize;
        m_nBlockSize = t;
        t = other.m_nHashTableSize;
        other.m_nHashTableSize = m_nHashTableSize;
        m_nHashTableSize = t;
        pt = other.m_pHashTable;
        other.m_pHashTable = m_pHashTable;
        m_pHashTable = pt;
        st = other.m_pBlocks;
        other.m_pBlocks = m_pBlocks;
        m_pBlocks = st;
        ft = other.m_pFreeList;
        other.m_pFreeList = m_pFreeList;
        m_pFreeList = ft;
        ft = other.m_pDeleteList;
        other.m_pDeleteList= m_pDeleteList;
        m_pDeleteList = ft;
    }

    // same as resize
    void reserve(unsigned int nBucket)
    {
        ReSize(nBucket);
    }

    // resize the ExtHashSet
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
    explicit ExtHashSet(int nBucket = 10
                        , int nBlockSize = 0
                        , unsigned int resizeThreshold = 90
        )
    {
        m_nHashTableSize = 0;
        m_nCount = 0;
        m_pHashTable = NULL;
        m_pFreeList = NULL;
        m_pDeleteList = NULL;
        m_pBlocks = NULL;
        m_resizeThreshold = resizeThreshold;
            
        if (nBlockSize <= 0) {
            nBlockSize = 120 / sizeof(SEntry);
            if (nBlockSize < 10)
                nBlockSize = 10;
        }
        m_nBlockSize = nBlockSize;
            
        ReSize(nBucket);
    }

    explicit ExtHashSet(const this_type & other)
    {
        *this = other;
    }

    ~ExtHashSet()
    {
        RemoveAll();
    }

    /*! iterating all key-value pairs
      \return false if table is empty or iteration is over.
      \param rNext [in, out] internal used
      \param rKey      [out] pointer to key
      \param rValue    [out] pointer to value

      example using FindFirst/FindNext
      \code
      ExtHashSet map;
      void* pos;
      _Key* pKey;
      _Value* pValue;
      for( map.FindFirst(pos, pKey, pValue); pos != NULL;
      map.FindNext(pos, pKey, pValue) )
      {
      make_use_of_pKey_and_pValue;
      }

      // another example
      bool more = map.FindFirst(pos, pKey, pValue);
      while( more ) {
      make_use_of_pKey_and_pValue;
      more = map.FindNext(pos, pKey, pValue);
      }
      \endcode
    */
    bool FindFirst(void *&pos, _Key * &rKey) const
    {
        if (m_nCount == 0) {
            pos = NULL;
            rKey = NULL;
            return false;
        }
        else {
            pos = BEFORE_STARTING_POS;
            return FindNext(pos, rKey);
        }
    }

    //! \sa FindFirst
    bool FindNext(void *&pos
                  , _Key * &rKey) const
    {
        unsigned int nBucket;
        SEntry *pEntryRet = (SEntry *) pos;
        assert(m_pHashTable != NULL);	// never call on empty map

        if (pEntryRet == (SEntry *) BEFORE_STARTING_POS) {
            // find the first entry
            for (nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
                if ((pEntryRet = m_pHashTable[nBucket]) != NULL)
                    break;
            assert(pEntryRet != NULL);	// must find something
        }
        else {						// find next entry
            if (NULL != pEntryRet->pNext)
                pEntryRet = pEntryRet->pNext;
            else {					// go to next bucket
                unsigned int nHash =
                    hf_(pEntryRet->key) % m_nHashTableSize;
                pEntryRet = NULL;	// set default
                for (nBucket = nHash + 1; nBucket < m_nHashTableSize;
                     nBucket++)
                    if ((pEntryRet = m_pHashTable[nBucket]) != NULL)
                        break;
            }
        }

        // fill in return data
        if (pEntryRet == NULL) {
            pos = NULL;
            rKey = NULL;
            return false;
        }
        else {
            pos = (void *) pEntryRet;
            rKey = &(pEntryRet->key);
            return true;
        }
    }

    //! number of elements
    unsigned int GetCount() const
    {
        return m_nCount;
    }

    //! whether created
    bool IsValid() const
    {
        return m_pHashTable != NULL;
    }


    //! Duplicate map, do deep copy
    const this_type &operator =(const ExtHashSet & src)
    {
        // keep m_resizeThreshold
        if (this != &src) {
            RemoveAll();
            ReSize(src.m_nHashTableSize);

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
        if (m_pHashTable == NULL)
            return NULL;		// nothing in the table

        SEntry **ppEntryPrev;
        ppEntryPrev = &m_pHashTable[hf_(key) % m_nHashTableSize];

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
        SEntry *p;
        if (m_pDeleteList == NULL)
            return;

        p = m_pDeleteList;
        while (p->pNext != NULL) {
            p->key. ~ _Key();
            p = p->pNext;
        }

        p->pNext = m_pFreeList;
        m_pFreeList = m_pDeleteList;
        m_pDeleteList = NULL;
    }



    /*!
      \brief set the size of hashtable
      \note always call this function before other operations
      \param nHashSize the possible total entries in this map
    */
    void ReSize(unsigned int nBucket)
    {
        unsigned int nHashSize = find_near_prime (nBucket);

        if (NULL == m_pHashTable) {
            m_pHashTable = new (std::nothrow) SEntry *[nHashSize];
            if (NULL == m_pHashTable) {
                ST_FATAL ("Fail to new m_pHashTable");
                return;
            }
            memset(m_pHashTable, 0, sizeof(SEntry *) * nHashSize);
            m_nHashTableSize = nHashSize;
        }
        else {
            SEntry *pEntry;
            SEntry **ppNewtable;
            unsigned int i, nNewhash, nOld;
            nOld = m_nHashTableSize;
            ppNewtable = new (std::nothrow) SEntry *[nHashSize];
            if (NULL == ppNewtable) {
                ST_FATAL ("Fail to new ppNewtable, quit %s", __FUNCTION__);
                return;
            }

            memset(ppNewtable, 0, sizeof(SEntry *) * nHashSize);

            for (i = 0; i < nOld; i++) {
                pEntry = m_pHashTable[i];
                while (pEntry) {
                    nNewhash = hf_(pEntry->key) % nHashSize;
                    m_pHashTable[i] = pEntry->pNext;
                    pEntry->pNext = ppNewtable[nNewhash];
                    ppNewtable[nNewhash] = pEntry;
                    pEntry = m_pHashTable[i];
                }
            }

            delete[]m_pHashTable;
            m_pHashTable = ppNewtable;
            m_nHashTableSize = nHashSize;
        }
    }

protected:
    //! internal structures
    struct SEntry {
        SEntry *pNext;
        _Key key;
    };

    //! linked list
    struct SDataChain {
        // warning variable length structure
        SDataChain *pNext;
        unsigned int nSize;			// allocated size, added to align on 8 byte boundary(it's not a requirement)

        //! memory: pNext, nSize, data, pNext, nSize, data ...
        void *data()
        {
            return this + 1;
        }

        //! like 'calloc' but no zero fill
        static SDataChain *Create(SDataChain * &pHead, unsigned int nMax,
                                  unsigned int cbElement)
        {
            assert(nMax > 0 && cbElement > 0);
            unsigned int nSize = sizeof(SDataChain) + nMax * cbElement;
            SDataChain *p = (SDataChain *) new (std::nothrow) unsigned char[nSize];
            if (NULL == p) {
                ST_FATAL ("Fail to new SDataChain");
                return NULL;
            }
            p->nSize = nSize;
            p->pNext = pHead;
            pHead = p;			// change head (adds in reverse order for simplicity)
            return p;
        }

        //! free this one and links
        void FreeDataChain()	
        {
            SDataChain *p = this;
            while (p != NULL) {
                unsigned char *bytes = (unsigned char *) p;
                SDataChain *pNext = p->pNext;
                delete[]bytes;
                p = pNext;
            }
        }
    };

protected:
    SEntry ** m_pHashTable;		//the entry table whose index is the hash_value
    struct SDataChain *m_pBlocks;	// trunk-allocated space to store the hash entries
    SEntry *m_pFreeList;		// pointer to the available room in the trunk
    SEntry *m_pDeleteList;		// pointer to the pending available room in the trunk
    unsigned int m_nHashTableSize;	//!< size of hash table
    unsigned int m_nBlockSize;		//!< # of batch allocated SEntry objects
    unsigned int m_nCount;			//!< key-value pairs in the table
    unsigned int m_resizeThreshold;
    _Hash hf_;
    _Equal ef_;
    
    // Implementation


    /**
     * safely add a new entry(key value pair),  key must be unique
     * if key is same, memory allocated for old key will leak
     */
    _Key *SafeAdd(const _Key & key, bool auto_resize)
    {
        unsigned int nHash;
        SEntry *pEntry;
        assert(m_pHashTable != NULL);
        assert(GetEntryAt(key) == NULL);

        // always add a new entry
        pEntry = SafeNewEntry(key, auto_resize);
        nHash = hf_(key) % m_nHashTableSize;

        // put into hash table
        pEntry->pNext = m_pHashTable[nHash];
        m_pHashTable[nHash] = pEntry;
        return &(pEntry->key);	// return pointer to created element
    }



    /**
     * get NewEntry without ReSize the ExtHashSet
     *  - Benefits: 
     *	MP-Safe in 1-Write n-Read situations
     *  - Shortcomings:
     *	Losing Efficience when hash talbe is full
     */
    SEntry *SafeNewEntry(const _Key & key, const bool auto_resize)
    {
        //      SEntry's are singly linked all the time
        //      static const _Key zeroKey = _Key();
        int i;
        SEntry * pEntry;

        if (auto_resize
            && (m_nCount * 100) >= (m_nHashTableSize * m_resizeThreshold))
        {
            ReSize(m_nHashTableSize+100);  // get next prime, check the prime array in common.h
        }
            
        if (m_pFreeList == NULL) {
            // add another block
            SDataChain *newBlock =
                SDataChain::Create(m_pBlocks, m_nBlockSize,
                                   sizeof(SEntry));
            // chain them into free list
            pEntry = (SEntry *) newBlock->data();

            // free in reverse order to make it easier to debug
            pEntry += m_nBlockSize - 1;
            for (i = m_nBlockSize - 1; i >= 0; i--, pEntry--) {
                pEntry->pNext = m_pFreeList;
                m_pFreeList = pEntry;
            }
        }
        assert(m_pFreeList != NULL);	// we must have something

        pEntry = m_pFreeList;
        m_pFreeList = m_pFreeList->pNext;
        m_nCount++;

        new (&pEntry->key) _Key(key);
        return pEntry;
    }

    //! find entry (or return NULL)
    inline SEntry *GetEntryAt(const _Key & key) const
    {
        assert(m_pHashTable != NULL);	// never call on empty map
        if (m_pHashTable == NULL)
            return NULL;

        unsigned int nHash = hf_(key) % m_nHashTableSize;

        // see if it exists
        SEntry *pEntry;
        for (pEntry = m_pHashTable[nHash]; pEntry != NULL;
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
        // free up string data
        pEntry->key. ~ _Key();
        pEntry->pNext = m_pFreeList;
        m_pFreeList = pEntry;
        m_nCount--;
    }

    void DeleteEntry(SEntry * pEntry)
    {
        pEntry->pNext = m_pDeleteList;
        m_pDeleteList = pEntry;
        m_nCount--;
    }


    //! remove all entries, free all the allocated memory
    void RemoveAll()
    {
        recycle_delayed();
        if (m_pHashTable != NULL) {						// destroy elements
            if (m_nCount > 0) {
                unsigned int nHash;
                for (nHash = 0; nHash < m_nHashTableSize; nHash++) {
                    SEntry *pEntry;
                    for (pEntry = m_pHashTable[nHash]; pEntry != NULL;
                         pEntry = pEntry->pNext)
                    {
                        pEntry->key. ~ _Key();
                    }
                }
            }
            // free hash table
            delete[]m_pHashTable;
            m_pHashTable = NULL;
        }

        m_nCount = 0;
        if (m_pBlocks != NULL) {
            m_pBlocks->FreeDataChain();
            m_pBlocks = NULL;
        }
        /*
          m_pFreeList pointed memory are part of m_pBlocks pointed,
          therefore it need not free
        */
        m_pFreeList = NULL;
        m_nHashTableSize = 0;
        m_nBlockSize = 0;
    }

public:
    //! for performace testing
    void stat()
    {
        if (m_pHashTable && m_nCount) {						// destroy elements
            const unsigned int BIN_CNT = 10;
            unsigned int nHash;
            unsigned int nUsed = 0, nMax = 0, nTotal = 0, nLength;
            unsigned int ayDist[BIN_CNT];
            memset(ayDist, 0, sizeof(ayDist));
            for (nHash = 0; nHash < m_nHashTableSize; nHash++) {
                SEntry *pEntry;
                nLength = 0;
                for (pEntry = m_pHashTable[nHash]; pEntry != NULL;
                     pEntry = pEntry->pNext)
                {
                    ++nLength;
                }
                nTotal += nLength;
                if (nLength > 0)
                    ++nUsed;
                if (nLength > nMax)
                    nMax = nLength;
                if (nLength < BIN_CNT)
                    ++ayDist[nLength];
            }
            printf
                ("%d table size, %d items, %d table-cell used, %.2f%% coverage\n",
                 m_nHashTableSize, m_nCount, nUsed,
                 100.0 * nUsed / m_nHashTableSize);
            printf("Link length: average %.2f, max %d\n",
                   1.0 * nTotal / nUsed, nMax);
            for (unsigned int i = 0; i < BIN_CNT; ++i)
                printf("%d\t%d\n", i, ayDist[i]);
        }
    }
};


}

#endif  //_EL_HASHSET_HPP_

