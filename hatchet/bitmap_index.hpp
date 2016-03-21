// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// The index corresponding to ST_BITMAP_KEY(...)

#pragma once
#ifndef _BITMAP_INDEX_H_
#define _BITMAP_INDEX_H_

#include "index_traits.hpp"   // SeekInfo
#include "bitwise_map.hpp"

namespace st {

template <class _Tuple, class _KeyAttrS>
class BitmapIndex : public c_show_base {
public:
    typedef BitwiseSet<_Tuple, typename _Tuple::template sub<_KeyAttrS> > Base;
private:
    C_ASSERT(CAP(is_valid_index, BitmapIndex), miss_interface);
    typedef _KeyAttrS KeyAttrS1;
    typedef typename Base::Key Key1;
    typedef PointerIterator<_Tuple> SeekIterator1;
    typedef PartialIterator<SeekIterator1> PartialSeekIterator1;

public:
    typedef typename Base::Pointer Pointer;
    typedef typename Base::Reference Reference;
    typedef typename Base::ConstReference ConstReference;
    typedef typename Base::Iterator Iterator;
    typedef Cons<SeekInfo<KeyAttrS1, SeekIterator1, ST_BITMAP_SCORE,
                          PartialSeekIterator1, MAYBE_ST_BITMAP_SCORE>,
                 void> SeekInfoL;
    typedef KeyAttrS1 Uniqueness;

    BitmapIndex () {}
    ~BitmapIndex () {}

    int init (size_t n_bucket = CH_DEFAULT_BITMAP_LEN)
    {
        return base_.init(n_bucket);
    }

    BitmapIndex (const BitmapIndex& rhs) : base_(rhs.base_) {}

    BitmapIndex& operator= (const BitmapIndex& rhs)
    {
        base_ = rhs.base_;
        return *this;
    }
        
//    void swap (UniqueIndex& rhs) { base_.swap(rhs.base_); }


    // Insert an item into UniqueIndex
    // Returns: address of the inserted item, NULL means insertion was failed
    Pointer insert (const _Tuple& tup) { return base_.insert(tup); }

    // Erase the value matching a key from UniqueIndex
    // Returns: erased or not
    bool erase (const Key1& key) { return base_.erase(key); }

    // Erase all items
    void clear () { base_.clear(); }
        
    // Search for the item matching a key
    // Returns: address of the item
    SeekIterator1 seek (const Key1& key) const
    { return SeekIterator1(base_.seek(key)); }
    
    void maybe_seek (PartialSeekIterator1* p_it, const Key1& key) const
    {
        Pointer p_item = base_.seek(key);
        if (p_item) {
            p_it->iterator().set_value(p_item);
            p_it->point_to_iterator();
        } else {
            copy_attr_list<_Tuple, Key1, class Key1::AttrS>::
                call(&(p_it->value()), key);
            p_it->point_to_value();
        }
    }

    void exclude_seek (PartialSeekIterator1* p_it, const Key1& key) const
    {
        Pointer p_item = base_.seek(key);
        if (p_item) {
            p_it->iterator().set_end();
            p_it->point_to_iterator();
        } else {
            copy_attr_list<_Tuple, Key1, class Key1::AttrS>::
                call(&(p_it->value()), key);
            p_it->point_to_value();
        }
    }
    
    size_t key_num (const Key1&) const { return base_.size(); }
    
    bool copy_key_part (_Tuple* p_dst, const _Tuple& src) const
    {
        copy_attr_list<_Tuple, _Tuple, _KeyAttrS>::call(p_dst, src);
        return true;
    }

    Pointer seek_tuple (const _Tuple& tup) const
    { return base_.seek_by_item(tup); }

    bool erase_tuple (const _Tuple& tup)
    { return base_.erase_by_item(tup); }

    bool resize (size_t n_bucket2) { return base_.resize(n_bucket2); }
    
    Iterator begin() const { return base_.begin(); }
    Iterator end() const { return base_.end(); }
    bool not_init() const { return base_.not_init(); }
    bool empty() const { return base_.empty(); }
    size_t size() const { return base_.size(); }
    size_t capacity() const { return base_.bucket_num(); }
    size_t mem() const { return base_.mem(); }
    const Base* base() const { return &base_; }
    
    // Print internal info to StringWriter
    void to_string (StringWriter& sw) const
    { base_.to_string(sw); }

friend std::ostream& operator<< (std::ostream& os, const BitmapIndex& bi)
    { return os << bi.base_; }

    // Print type of this container
    static void c_to_string (std::ostream& os)
    {
        os << "BitmapIndex(" << c_show(_KeyAttrS)
           << "->" << c_show(_Tuple) << ")";
    }
    
private:
    Base base_;

};


}  // namespace st

#endif  //_BITMAP_INDEX_H_
