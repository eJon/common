// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// External probing hashset/map with reference counting
// Author: gejun@baidu.com
// Date: Dec.27 15:28:29 CST 2010
#pragma once
#ifndef _HD_MAP_HPP_
#define _HD_MAP_HPP_

#include "cow_hash_map.hpp"     // based on CowHashMap

namespace st {
struct VariadicValue {
    VariadicValue (long offset2, size_t length2)
        : offset(offset2), length(length2) {}
    
    long offset;
    size_t length;
};

template <class _Key,
          class _Hash  = Hash<_Key>,
          class _Equal = Equal<_Key> >
class HDMap {
private:
    typedef CowHashMap<_Key, VariadicValue, _Hash, _Equal> Base;
public:
    typedef typename Base::Allocator Allocator;
    typedef _Key Key;
    typedef long Pointer;
    typedef class Base::Iterator Iterator;
    
    HDMap () : fp_(NULL) {}
    ~HDMap ()
    {
        if (fp_) {
            fclose(fp_);
            fp_ = NULL;
        }
    }
    
    int init (const char* value_fpath,
              size_t n_bucket        = CH_DEFAULT_NBUCKET,
              u_int load_factor      = CH_DEFAULT_LOAD_FACTOR)
    {
        struct stat file_info_dummy;
        if (0 == stat(value_fpath, &file_info_dummy)) {
            ST_WARN("`%s' already exists, override", value_fpath);
        } 
        value_fpath_  = value_fpath;

        fp_ = fopen(value_fpath, "wb");
        return base_.init(n_bucket, load_factor);
    }

    HDMap (const HDMap& rhs) : base_(rhs.base_)
    {
        char buf[16];
        int i = 2;
        struct stat file_info_dummy;
        do {
            snprintf(buf, sizeof(buf), "%d", i++);
            value_fpath_ = rhs.value_fpath_ + buf;
        } while (0 == stat(value_fpath_.c_str(), &file_info_dummy));
        
        fp_ = fopen(value_fpath_.c_str(), "wb");
    }

    HDMap& operator= (const HDMap& rhs)
    {
        if (not_init()) {
            new (this) HDMap(rhs);
            return *this;
        }
        
        base_ = rhs.base_;
        // TODO
        return *this;
    }
        
    //void swap (HDMap & rhs) { base_.swap(rhs.base_); }

    // Reserve number of buckets
    bool reserve (const size_t n_bucket) { return base_.reserve(n_bucket); }
        
    // Insert an item into HDMap
    // Returns: address of the inserted item, NULL means insertion was failed
    Pointer insert (const Key& key, const void* value, size_t length)
    {
        if (NULL == value) {
            ST_FATAL("Param[value] is NULL");
            return -1;  // correct offset in file is never negative
        }
        long pos = ftell(fp_);
        size_t ret = fwrite(value, length, 1, fp_);
        if  (ret != 1) {
            ST_WARN("Fail to write");  // elaborate this error log
            fseek(fp_, pos, SEEK_SET);
            return -1;
        }
        if (NULL == base_.insert(key, VariadicValue(pos, length))) {
            ST_FATAL("Fail to insert into base_");
            return -1;
        }
        return pos;
    }

    // Erase the value matching a key from HDMap
    // Returns: erased or not
    bool erase (const Key& key) { return base_.erase(key); }

    // Erase all items
    void clear () { base_.clear(); }

    bool exist (const Key& key) const { return base_.seek(key); }
    
    // Search for the item matching a key
    // Returns: address of the item
    size_t seek (const Key& key, void** p_value, size_t* p_length) const
    {
        if (NULL == p_value || NULL == p_length) {
            ST_FATAL("Param[p_value or p_length] is NULL");
            return 0;
        }
        
        typename Base::Pointer p = base_.seek(key);
        if (!p) {
            return 0;
        }

        fflush(fp_);
        
        FILE* fp = fopen(value_fpath_.c_str(), "rb");
        if (NULL == fp) {
            ST_FATAL("Fail to open value file `%s'", value_fpath_.c_str());
            return 0;
        }

        size_t ret = 0;
        do {
            const size_t old_length = *p_value ? *p_length : 0;
            if (p->length > old_length) {
                const size_t new_length = std::max(old_length * 2, p->length);
                if (new_length != old_length) {
                    void* new_value = malloc(new_length);
                    if (NULL == new_value) {
                        ST_FATAL("Fail to malloc %lu bytes", new_length);
                        break;
                    }
                    if (*p_value) {
                        free(*p_value);
                    }
                    *p_value = new_value;
                    *p_length = new_length;
                }
            }
            if (0 != fseek(fp, p->offset, SEEK_SET)) {
                ST_FATAL("Fail to seek to %ld of `%s'",
                         p->offset, value_fpath_.c_str());
                break;
            }
            ret = fread(*p_value, p->length, 1, fp);
            ret = (ret != 1 ? 0 : p->length);
        } while (0);
        fclose(fp);
        return ret;
    }

    // Change number of buckets
    // Params:
    //   n_bucket2  intended number of buckets
    // Returns: resized or not
    bool resize (size_t n_bucket2) { return base_.resize(n_bucket2); }

    // Get beginning position of this mapping
    Iterator begin () const { return base_.begin(); }

    // Get ending position of this mapping
    Iterator end () const { return base_.end(); }

    // Know initialized or not
    bool not_init () const { return base_.not_init(); }
        
    // Know empty or not
    bool empty () const { return base_.empty(); }
        
    // Get number of items
    size_t size () const { return base_.size(); }

    // Get number of buckets
    size_t bucket_num () const { return base_.bucket_num(); }

    // Get memory taken by this instance
    size_t mem () const { return base_.mem(); }

    // Get pointer to internal allocator
    const Allocator* alloc () const { return base_.alloc(); }

    // Number of users of the allocator
    long alloc_use_count ()  const { return base_.alloc_use_count(); }

    // An allocator belongs to at-most one container
    bool alloc_owner () const { return base_.alloc_owner(); }

    // Get max/min/average length of chains
    // input pointers must be non-NULL
    void bucket_info (size_t* p_max_len, size_t* p_min_len,
                      double* p_avg_len) const
    { base_.bucket_info(p_max_len, p_min_len, p_avg_len);}

    // Get the actual load factor
    u_int load_factor () const { return base_.load_factor(); }

    // Print internal info to StringWriter
    void to_string (StringWriter& sw) const
    { base_.to_string(sw); }

private:
    Base base_;
    FILE* fp_;
    std::string value_fpath_;
};


// Print type of this container
template <typename _Key, class _Hash, class _Equal>
struct c_show_impl<HDMap<_Key, _Hash, _Equal> > {
    static void c_to_string (std::ostream& os)
    {
        os << c_show(_Key) << "->void*";
    }
};

}  // namespace st

#endif  //_HD_MAP_HPP_
