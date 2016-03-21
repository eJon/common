// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Modified from app/ecom/elib/ecommon-lib/el_container.h, hashing
// and equal functions become template parameters
// Author: gejun@baidu.com
// Date: Fri Jul 30 13:19:34 2010

#pragma once
#ifndef _EXT_HASH_MAP_HPP_
#define _EXT_HASH_MAP_HPP_

#include "st_utility.h"
#include "common.h"
#include "functional.hpp"

#define BEFORE_STARTING_POS ((void*)-1L)
namespace st {

/**
 * @brief
 * Template hash table( key-value pair )
 *
 * Requirements for _Value and _Key:
 *  They can be any fix-size type or class which has overloaded '=' opeartor
 *  eg. int, WString, struct, union, and any other l-value type.
 *  
 *  @note Neither pointer nor array are valid type.
 *          eg. PSTR, char[24], float[10]
 *          void* can be used as valid type. But with care!
 *
 *  - They must have a default constructor or a user-define constructor without parameters.
 * ownership policy:
 *  - _Key and _Value are copyed using '=' operator.
 *  - They are owned by ExtHashMap.
 *  - Becareful when a narrow-copy/share-copy '=' operator is defined.
 *
 *  @note
 *
 *  - 在使用iterator进行迭代遍历时，同时对hashtable中的元素进行增/删操作，后续迭代的结果是不可预期的
 *  - 默认情况下，insert操作可能引发hashtable的resize操作；resize过程中，对hashtable读操作的结果是不可预期的
 * 
 * ExtHashMap quick reference:
 *  - Insert/Retrieve:      insert / []
 *  - Search entry:         find / Lookup
 *  - Remove entry:         Remove
 *  - Iteration:            FindFirst/FindNext
 *  - Clear the table:      clear
 *  - Reserve memory(not necessary): resize/reserve
 *  - Empty  array(not necessary): RemoveAll
 */


template < class _Key
           , class _Value
           , typename _Hash = Hash<_Key>
           , typename _Equal = Equal<_Key> >
class ExtHashMap
{
public:
    void to_string (StringWriter& sb) const
    { shows_range (sb, begin(), end()); }
    
    //! structure for interface compatible with STL's map template
    class iterator
        : public std::iterator < std::forward_iterator_tag, std::pair < const _Key,_Value > >
    {
    public:
        /**
         * in gcc 2.96, it seems that value_type can't be inherited from std::iterator
         */
        typedef std::pair < const _Key, _Value > value_type;
        typedef _Key key_type;

        iterator() {
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
            pMap = (const ExtHashMap *)map;
        }

        bool operator ==(const iterator & iter) const
        { return iter.ptr == ptr; }

        bool operator !=(const iterator & iter) const
        { return iter.ptr != ptr; }

        //! prefix: increase and then fetch
        iterator & operator ++()
        {
            _Key *pKey;
            _Value *pValue;
            void *pos;
            pos = (char *) ptr - sizeof(SEntry *);
            if (pMap->FindNext(pos, pKey, pValue)) {
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
            _Value *pValue;
            void *pos;
            pos = (unsigned char *) ptr - sizeof(SEntry *);
            if (pMap->FindNext(pos, pKey, pValue)) {
                ptr = (value_type *) ((unsigned char *) pos + sizeof(SEntry *));
            }
            else {
                ptr = NULL;
            }

            return tmp;
        }

        value_type & operator *()const
        { return (value_type &) * ptr; }

        value_type *operator ->() const
        { return (value_type *) ptr; }

    protected:
        value_type * ptr;
        const ExtHashMap *pMap;
    };

    typedef typename iterator::key_type key_type;
    typedef typename iterator::value_type value_type;
    typedef typename iterator::pointer pointer;
    typedef typename iterator::reference reference;

    /**
     * returns an iterator pointing to the beginning of the ExtHashMap
     * @return
     *      - NULL Cannot get the beginning iterator
     *      - !NULL Success
     */
    iterator begin() const
    {
        _Key *pKey;
        _Value *pValue;
        void *pos;
        if (FindFirst(pos, pKey, pValue)) {
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
            if (n_count_ > 0) {
                unsigned int nHash;
                SEntry *pEntry;
                SEntry *pNext;
                for (nHash = 0; nHash < n_bucket_; nHash++) {
                    for (pEntry = m_pHashTable[nHash]; pEntry != NULL;) {
                        pNext = pEntry->pNext;
                        FreeEntry(pEntry);  // recycle the memory to p_free_head_
                        pEntry = pNext;
                    }
                }
                assert(n_count_ == 0);
                assert(p_free_head_ != NULL);
            }
            memset(m_pHashTable, 0, sizeof(SEntry *) * n_bucket_);
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
            if (n_count_ > 0) {
                unsigned int nHash;
                SEntry *pEntry;
                SEntry *pNext;
                for (nHash = 0; nHash < n_bucket_; nHash++) {
                    for (pEntry = m_pHashTable[nHash]; pEntry != NULL;) {
                        pNext = pEntry->pNext;
                        FreeEntry(pEntry);  // recycle the memory to p_free_head_
                        pEntry = pNext;
                    }
                }
                assert(n_count_ == 0);
                assert(p_free_head_ != NULL);
            }
            memset(m_pHashTable, 0, sizeof(SEntry *) * n_bucket_);
        }

        if (m_pBlocks != NULL) {
            m_pBlocks->FreeDataChain();
        }
        m_pBlocks = NULL;
        p_free_head_ = NULL;         // need not free p_free_head_, as it's alias of free space in m_pBlocks
    }

    /**
     * true if the ExtHashMap's size is 0.
     */
    bool empty() const
    {
        return (n_count_ == 0);
    }

    /**
     * returns a iterator pointing to the end of the ExtHashMap
     */
    iterator end() const
    {
        return NULL;
    }


    /**
     * Inserts key into the ExtHashMap: if key already exists, update it with value;
     * SafeAdd (key, value) pair else.
     * @param[in] key
     * @param[in] value
     * @param[in] auto_resize identify auto resize hash table or not
     * @return
     *      - false if key exists and updated by value
     *      - true if key not exists and inserted
     */
    _Value* insert(const _Key & key, const _Value & value, const bool auto_resize = true)
    {
        // iterator iter;
        // iter = find(key);
        // if (iter != end())
        // {
        //     iter->second = value;
        //     return &(iter->second);
        // }
        // else
        // {
        //     return SafeAdd(key, value, auto_resize);
        // }

        _Value* p_value = seek (key);
        if (p_value) {
            *p_value = value;
            return p_value;
        }
        else {
            return SafeAdd (key, value, auto_resize);
        }
    }


    /**
     * finds an element matching key
     * @param[in] key
     * @return
     *      - NULL Cannot find matching item
     *      - !NULL iterator pointing to matching item
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

    inline _Value* seek (const _Key& key) const
    {
        SEntry *pEntry = GetEntryAt(key);
        return pEntry ? &(pEntry->value) : NULL;
    }

    //returns the size of ExtHashMap
    size_t size() const
    {
        return n_count_;
    }

    size_t hash_size () const
    {
        return n_bucket_;
    }

    size_t free_size () const
    {
        size_t cnt=0;
        for (SEntry* p=p_free_head_; p; p=p->pNext, ++cnt);
        return cnt;
    }
        
    size_t delete_size () const
    {
        size_t cnt=0;
        for (SEntry* p=p_2bfree_head_; p; p=p->pNext, ++cnt);
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
        for (size_t nBucket = 0; nBucket < n_bucket_; nBucket++) {
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
    void swap(ExtHashMap & other)
    {
        unsigned int t;
        SEntry **pt;
        SDataChain *st;
        SEntry *ft;

        t = other.n_count_;
        other.n_count_ = n_count_;
        n_count_ = t;
        t = other.block_size_;
        other.block_size_ = block_size_;
        block_size_ = t;
        t = other.n_bucket_;
        other.n_bucket_ = n_bucket_;
        n_bucket_ = t;
        pt = other.m_pHashTable;
        other.m_pHashTable = m_pHashTable;
        m_pHashTable = pt;
        st = other.m_pBlocks;
        other.m_pBlocks = m_pBlocks;
        m_pBlocks = st;
        ft = other.p_free_head_;
        other.p_free_head_ = p_free_head_;
        p_free_head_ = ft;
        ft = other.p_2bfree_head_;
        other.p_2bfree_head_= p_2bfree_head_;
        p_2bfree_head_ = ft;
    }

    // same as resize
    void reserve(unsigned int nBucket)
    {
        ReSize(nBucket);
    }

    // resize the ExtHashMap
    void resize(unsigned int nBucket)
    {
        ReSize(nBucket);
    }

    /**
     * the constructor
     * you may specify a hint size for the hash table
     *
     * Note: nBucket in 10% - 200% of total hash entries works well
     * BlockSize = 10 is sufficient in most case
     */
    explicit ExtHashMap(int nBucket = 10
                        , int nBlockSize = 0
                        , unsigned int resizeThreshold = 80
        )
    {
        n_bucket_ = 0;
        n_count_ = 0;
        m_pHashTable = NULL;
        p_free_head_ = NULL;
        p_2bfree_head_ = NULL;
        m_pBlocks = NULL;
        m_resizeThreshold = resizeThreshold;
            
        if (nBlockSize <= 0) {
            nBlockSize = 120 / sizeof(SEntry);
            if (nBlockSize < 10)
                nBlockSize = 10;
        }
        block_size_ = nBlockSize;
            
        ReSize(nBucket);
    }

    explicit ExtHashMap(const ExtHashMap & other)
    {
        *this = other;
    }

    ~ExtHashMap()
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
      ExtHashMap map;
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
    bool FindFirst(void *&pos
                   , _Key * &rKey
                   , _Value * &rValue
        ) const {
        if (n_count_ == 0) {
            pos = NULL;
            rKey = NULL;
            rValue = NULL;
            return false;
        }
        else {
            pos = BEFORE_STARTING_POS;
            return FindNext(pos, rKey, rValue);
        }
    }

    //! \sa FindFirst
    bool FindNext(void *&pos
                  , _Key * &rKey
                  , _Value * &rValue) const 
    {
        unsigned int nBucket;
        SEntry *pEntryRet = (SEntry *) pos;
        assert(m_pHashTable != NULL);       // never call on empty map

        if (pEntryRet == (SEntry *) BEFORE_STARTING_POS) {
            // find the first entry
            for (nBucket = 0; nBucket < n_bucket_; nBucket++) {
                if ((pEntryRet = m_pHashTable[nBucket]) != NULL)
                    break;
            }
            assert(pEntryRet != NULL);      // must find something
        }
        else {
            // find next entry
            if (NULL != pEntryRet->pNext)
                pEntryRet = pEntryRet->pNext;
            else {
                // go to next bucket
                unsigned int nHash =
                    hf_(pEntryRet->key) % n_bucket_;
                pEntryRet = NULL;   // set default
                for (nBucket = nHash + 1; nBucket < n_bucket_; nBucket++) {
                    if ((pEntryRet = m_pHashTable[nBucket]) != NULL)
                        break;
                }
            }
        }

        // fill in return data
        if (pEntryRet == NULL) {
            pos = NULL;
            rKey = NULL;
            rValue = NULL;
            return false;
        }
        else {
            pos = (void *) pEntryRet;
            rKey = &(pEntryRet->key);
            rValue = &(pEntryRet->value);
            return true;
        }
    }

    //! number of elements
    unsigned int GetCount() const
    {
        return n_count_;
    }

    //! whether created
    bool IsValid() const
    {
        return m_pHashTable != NULL;
    }


    //! Duplicate map, do deep copy
    const ExtHashMap &operator =(const ExtHashMap & src)
    {
        if (this != &src) {
            RemoveAll();
            ReSize(src.n_bucket_);

            void *pos;
            _Key *pKey;
            _Value *pValue;
            bool more = src.FindFirst(pos, pKey, pValue);
            while (more) {
                this->SafeAdd(*pKey, *pValue, false);
                more = src.FindNext(pos, pKey, pValue);
            }
        }
        return *this;
    }

    //! removing existing key-value pair
    //! \return whether succeessful
    bool erase(const _Key & key, bool delayed = false)
    {
        if (m_pHashTable == NULL)
            return false;           // nothing in the table

        SEntry **ppEntryPrev;
        ppEntryPrev = &m_pHashTable[hf_(key) % n_bucket_];

        SEntry *pEntry;
        for (pEntry = *ppEntryPrev; pEntry != NULL; pEntry = pEntry->pNext) {
            if (ef_(pEntry->key, key)) {
                // remove it
                *ppEntryPrev = pEntry->pNext;       // remove from list
                if (delayed) {
                    DeleteEntry(pEntry);
                }
                else {
                    FreeEntry(pEntry);
                }
                return true;
            }
            ppEntryPrev = &pEntry->pNext;
        }
        return false;                       // not found
    }

    //! removing existing key-value pair
    //! will not free memory or reuse it
    //! \return whether succeessful
    bool delayed_erase(const _Key & key)
    {
        return erase (key, true);
    }

    /**
     * 将deletelist插入freelist的头部
     */
    void recycle_delayed()
    {
        SEntry *p;
        if (p_2bfree_head_ == NULL)
            return;

        p = p_2bfree_head_;
        while (p->pNext != NULL) {
            p->key. ~ _Key();
            p->value. ~ _Value();
            p = p->pNext;
        }

        p->pNext = p_free_head_;
        p_free_head_ = p_2bfree_head_;
        p_2bfree_head_ = NULL;
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
            n_bucket_ = nHashSize;
        }
        else {
            SEntry *pEntry;
            SEntry **ppNewtable;
            unsigned int i, nNewhash, nOld;
            nOld = n_bucket_;
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
            n_bucket_ = nHashSize;
        }
    }

protected:
    //! internal structures
    struct SEntry {
        SEntry *pNext;
        _Key key;
        _Value value;
    };

    //! linked list
    struct SDataChain {
        // warning variable length structure
        SDataChain *pNext;
        unsigned int nSize;                 // allocated size, added to align on 8 byte boundary(it's not a requirement)

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
            pHead = p;                      // change head (adds in reverse order for simplicity)
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
    SEntry ** m_pHashTable;         //the entry table whose index is the hash_value
    struct SDataChain *m_pBlocks;   // trunk-allocated space to store the hash entries
    SEntry *p_free_head_;            // pointer to the available room in the trunk
    SEntry *p_2bfree_head_;          // pointer to the pending available room in the trunk
    unsigned int n_bucket_;  //!< size of hash table
    unsigned int block_size_;              //!< # of batch allocated SEntry objects
    unsigned int n_count_;                  //!< key-value pairs in the table
    unsigned int m_resizeThreshold;
    _Hash hf_;
    _Equal ef_;
    
    // Implementation


    /**
     * safely add a new entry(key value pair),  key must be unique
     * if key is same, memory allocated for old key will leak
     */
    _Value *SafeAdd(const _Key & key, const _Value & value, bool auto_resize)
    {
        unsigned int nHash;
        SEntry *pEntry;
        assert(m_pHashTable != NULL);
        assert(GetEntryAt(key) == NULL);

        // always add a new entry
        pEntry = SafeNewEntry(key, value, auto_resize);
        nHash = hf_(key) % n_bucket_;

        // this is moved into SafeNewEntry
        //  // set value for new entry
        // pEntry->value = value;

        // put into hash table
        pEntry->pNext = m_pHashTable[nHash];
        m_pHashTable[nHash] = pEntry;
        return &(pEntry->value);    // return pointer to created element
    }



    /**
     * get NewEntry without ReSize the ExtHashMap
     *  - Benefits: 
     *      MP-Safe in 1-Write n-Read situations
     *  - Shortcomings:
     *      Losing Efficience when hash talbe is full
     */
    SEntry *SafeNewEntry(const _Key & key, const _Value& value, const bool auto_resize)
    {
        //      SEntry's are singly linked all the time
        //      static const _Key zeroKey = _Key();
        //      static const _Value zeroElement = _Value();
        int i;
        SEntry * pEntry;

        if (auto_resize
            && (n_count_ * 100) >= (n_bucket_ * m_resizeThreshold))
        {
            ReSize(n_bucket_+100);  // get next prime, check the prime array in common.h
        }
            
        if (p_free_head_ == NULL) {
            // add another block
            SDataChain *newBlock =
                SDataChain::Create(m_pBlocks, block_size_,
                                   sizeof(SEntry));
            // chain them into free list
            pEntry = (SEntry *) newBlock->data();

            // free in reverse order to make it easier to debug
            pEntry += block_size_ - 1;
            for (i = block_size_ - 1; i >= 0; i--, pEntry--) {
                pEntry->pNext = p_free_head_;
                p_free_head_ = pEntry;
            }
        }
        assert(p_free_head_ != NULL);        // we must have something

        pEntry = p_free_head_;
        p_free_head_ = p_free_head_->pNext;
        n_count_++;

        new (&pEntry->key) _Key(key);
        new (&pEntry->value) _Value(value);
        return pEntry;
    }

    //! find entry (or return NULL)
    inline SEntry *GetEntryAt(const _Key & key) const
    {
        assert(m_pHashTable != NULL);       // never call on empty map
        if (m_pHashTable == NULL)
            return NULL;

        unsigned int nHash = hf_(key) % n_bucket_;

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
        pEntry->value. ~ _Value();
        pEntry->pNext = p_free_head_;
        p_free_head_ = pEntry;
        n_count_--;
    }

    void DeleteEntry(SEntry * pEntry)
    {
        pEntry->pNext = p_2bfree_head_;
        p_2bfree_head_ = pEntry;
        n_count_--;
    }


    //! remove all entries, free all the allocated memory
    void RemoveAll()
    {
        recycle_delayed();
        if (m_pHashTable != NULL) {                     // destroy elements
            if (n_count_ > 0) {
                unsigned int nHash;
                for (nHash = 0; nHash < n_bucket_; nHash++) {
                    SEntry *pEntry;
                    for (pEntry = m_pHashTable[nHash]; pEntry != NULL;
                         pEntry = pEntry->pNext)
                    {
                        pEntry->key. ~ _Key();
                        pEntry->value. ~ _Value();
                    }
                }
            }
            // free hash table
            delete[]m_pHashTable;
            m_pHashTable = NULL;
        }

        n_count_ = 0;
        if (m_pBlocks != NULL) {
            m_pBlocks->FreeDataChain();
            m_pBlocks = NULL;
        }
        /*
          p_free_head_ pointed memory are part of m_pBlocks pointed,
          therefore it need not free
        */
        p_free_head_ = NULL;
        n_bucket_ = 0;
        block_size_ = 0;
    }

public:
    //! for performace testing
    void stat()
    {
        if (m_pHashTable && n_count_) {   // destroy elements
            const unsigned int BIN_CNT = 10;
            unsigned int nHash;
            unsigned int nUsed = 0, nMax = 0, nTotal = 0, nLength;
            unsigned int ayDist[BIN_CNT];
            memset(ayDist, 0, sizeof(ayDist));
            for (nHash = 0; nHash < n_bucket_; nHash++) {
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
                 n_bucket_, n_count_, nUsed,
                 100.0 * nUsed / n_bucket_);
            printf("Link length: average %.2f, max %d\n",
                   1.0 * nTotal / nUsed, nMax);
            for (unsigned int i = 0; i < BIN_CNT; ++i)
                printf("%d\t%d\n", i, ayDist[i]);
        }
    }
};


}

#endif  //_EXT_HASH_MAP_HPP_
