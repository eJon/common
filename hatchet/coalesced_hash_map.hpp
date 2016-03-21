// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// I'M NOT COMPLETED, DONT USE ME UNTIL THIS LINE IS REMOVED
// Author: gejun@baidu.com
// Date: Thu Nov 25 14:07:39 CST 2010
#pragma once
#ifndef _COALESCED_HASH_MAP_HPP_
#define _COALESCED_HASH_MAP_HPP_

#include "functional.hpp"
#include "common.h"

namespace st
{
    const u_int MIN_BUCKET_CAP = 1024;
    const u_int DEFAULT_LOAD_FACTOR = 80;
    const u_int DEFAULT_CELLAR_FACTOR = 20;

    //! coalesced hash map
    template <typename _Key
              , typename _Value
              , typename _Hash = Hash<_Key>
              , typename _Equal = Equal<_Key> >
    class CoalescedHashMap {
        //! basic pair
        struct Node {
            //! end of all chains
            const static u_int null = (u_int)-1;

            //! mark unused nodes
            const static u_int unused = (u_int)-2;
        
            _Key key;
            _Value value;
            u_int next;

            //! pretty print this node
            void to_string(StringWriter& sw) const
            {
                sw << "(" << key << "," << value << ",";
                if (Node::null == next) {
                    sw << "null";
                }
                else if (Node::unused == next) {
                    sw << "unused";
                }
                else {
                    sw << next;
                }
                sw << ")"; }
        };

    public:
        //! to access this map
        struct const_iterator {
            const Node& operator* () const
            { return this_->a_node_[chain_]; }
            
            const Node* operator-> () const
            { return &(this_->a_node_[chain_]); }
        
            bool operator!= (const const_iterator& other) const
            { return idx_ != other.idx_; }

            //! test equivalence of two iterators
            bool operator== (const const_iterator& other) const
            { return idx_ == other.idx_; }

            //! initialize this iterator
            void setup (const CoalescedHashMap* p_map, u_int idx)
            {
                this_ = p_map;
                idx_ = idx;
                skip_null ();
                chain_ = idx_;
            }

            //! move iterator ahead for one step
            const_iterator& operator++ ()
            {
                //ST_NOTICE ("go from %u to %u", chain_, this_->a_node_[chain_].next);
                chain_ = this_->a_node_[chain_].next;
                if (Node::null == chain_) {
                    ++ idx_;
                    skip_null();
                    //ST_NOTICE ("null cause from %u to %u", chain_, idx_);
                    chain_ = idx_;
                }
                return *this;
            }

        private:
            void skip_null ()
            { for (; Node::unused == this_->a_node_[idx_].next; ++ idx_); }
            
            u_int idx_;
            u_int chain_;
            const CoalescedHashMap* this_;
        };

        typedef const_iterator iterator;
        
        //! get beginning position of this hashmap
        const_iterator begin () const
        {
            const_iterator it;
            it.setup (this, 0);
            return it;
        }

        //! get ending position of this hashmap
        const_iterator end () const
        {
            const_iterator it;
            it.setup (this, bucket_cap_);
            return it;
        }

        //! constructor, basically set fields to zeros,
        //! should call create(3) to truely initialize
        CoalescedHashMap()
            : bucket_cap_(0)
            , bucket_cap_mask_(0)
            , bucket_cap_shift_(0)
            , bucket_count_(0)
            , load_factor_(0)
            , free_head_(0)
            , cellar_cap_(0)
            , cellar_count_(0)
            , cellar_factor_(0)
            , a_node_(NULL)
        {}

        //! destructor
        ~CoalescedHashMap()
        {
            if (a_node_) {
                delete [] a_node_;
            }
        }
        
        //! create cellar data
        //! @param bucket_cap: rounded power of 2
        //! @param load_factor: if element_count*100 > bucket_cap*load_factor,
        //! rehashing occurs
        //! @param cellar_factor: cellar_size = bucket_cap*cellar_factor/100
        //! @return 0/E_NULL: fail to new a_node_
        int create (u_int bucket_cap=MIN_BUCKET_CAP
                    , u_int load_factor=DEFAULT_LOAD_FACTOR
                    , u_int cellar_factor=DEFAULT_CELLAR_FACTOR)
        {
            if (load_factor <= 0 || load_factor >= 100) {
                ST_FATAL ("param[load_factor] should be an integer inside "
                          "[1,100], use default value %u", DEFAULT_LOAD_FACTOR);
                load_factor_ = DEFAULT_LOAD_FACTOR;
            }
            else {
                load_factor_ = load_factor;
            }

            if (bucket_cap < 2) {
                bucket_cap = 2;
                bucket_cap_shift_ = 1;
            }
            else {
                // round bucket_cap to be power to 2
                u_int rounded = 4;
                bucket_cap_shift_ = 2;
                for (; rounded < bucket_cap; rounded = (rounded<<1), ++ bucket_cap_shift_);
            }
        
            bucket_cap_ = (1 << bucket_cap_shift_);
            bucket_cap_mask_ = bucket_cap_ - 1;

            const int DEFAULT_CELLAR_FACTOR = 18;
            if (cellar_factor > 50) {
                ST_FATAL ("cellar_factor should be integer inside [0,50],"
                          " pick default value %u", DEFAULT_CELLAR_FACTOR);
                cellar_factor = DEFAULT_CELLAR_FACTOR;
            }
            cellar_factor_ = cellar_factor;
            cellar_cap_ = cellar_factor_ * bucket_cap_ / 100;

            // [0, bucket_cap_-1] stores first-hit values
            // [bucket_cap_, bucket_cap_ + cellar_cap_-1] stores chained values
            u_int length = bucket_cap_ + cellar_cap_;
            a_node_ = new (std::nothrow) Node[length];
            if (NULL == a_node_) {
                ST_FATAL ("Fail to new a_node_");
                return ENOMEM;
            }

            clear (false);
        
            //ST_NOTICE ("create done, bucket_cap=%u, cellar_size=%u"
            // , bucket_cap_, cellar_cap_);
        
            return 0;
        }
    

        //! swap internal data with another instance, after swapping,
        //! both instances do not change their storage addresses while
        //! data are exchanged, this is useful for rehashing
        void swap (CoalescedHashMap& other)
        {
            std::swap (bucket_cap_, other.bucket_cap_);
            std::swap (bucket_cap_mask_, other.bucket_cap_mask_);
            std::swap (bucket_cap_shift_, other.bucket_cap_shift_);
            std::swap (bucket_count_, other.bucket_count_);
            std::swap (cellar_count_, other.cellar_count_);
            std::swap (free_head_, other.free_head_);
            std::swap (cellar_factor_, other.cellar_factor_);
            std::swap (cellar_cap_, other.cellar_cap_);
            std::swap (load_factor_, other.load_factor_);
            std::swap (a_node_, other.a_node_);
        }
        
        //! insert an element
        //! @retval true means that the key does not exist, false is opposite
        bool insert (const _Key& key, const _Value& value)
        {
            rehash ();
        
            u_int idx = hashf_(key) & bucket_cap_mask_;
        
            if (Node::unused == a_node_[idx].next) {
                // unused node
                a_node_[idx].key = key;
                a_node_[idx].value = value;
                a_node_[idx].next = Node::null;
                ++ bucket_count_;
                return true;
                //ST_NOTICE ("insert to idx=%u", idx);
            }
            else {
                u_int p = idx;
                // walk through the chain
                for (; Node::null != p && !eqf_ (a_node_[p].key, key)
                         ; p = a_node_[p].next);
                
                if (Node::null == p) {
                    // the node with equal key does not exist
                    
                    // get one free node
                    u_int n = pop_free();

                    //ST_NOTICE ("insert to free_head_cellar=%u", n);
                
                    // assign key and value into the new node
                    a_node_[n].key = key;
                    a_node_[n].value = value;

                    // append this node before first of the cellar chain
                    a_node_[n].next = a_node_[idx].next;
                    
                    // let idx points this new node
                    a_node_[idx].next = n;

                    // increase counter
                    ++ cellar_count_;

                    return true;
                }
                else {
                    //ST_NOTICE ("insert to cellar=%u", p);
                    // the node has equal key, just overwrite value
                    a_node_[p].value = value;
                    return false;
                }
            }
        }

        //! erase an element
        void erase (const _Key& key)
        {
            u_int idx = hashf_(key) & bucket_cap_mask_;

            if (Node::unused != a_node_[idx].next) {
                // check the very first node
                if (eqf_ (a_node_[idx].key, key)) {
                    u_int next = a_node_[idx].next;
                    if (Node::null == next) {
                        // mark next to unused and we are done
                        a_node_[idx].next = Node::unused;
                        -- bucket_count_;
                    }
                    else {
                        // copy next node from cellar to buckets
                        a_node_[idx] = a_node_[next];

                        // append the skipped node to free queue
                        push_free (next);

                        -- cellar_count_;
                    }
                }
                else {
                    u_int p0 = idx;  // parent node of p
                    u_int p = a_node_[idx].next;
                    // walk through the chain
                    for (; Node::null != p && !eqf_ (a_node_[p].key, key)
                             ; p0 = p, p = a_node_[p].next);

                    if (Node::null == p) {
                        return;
                    }
                
                    // skip the to-be-erased node
                    a_node_[p0].next = a_node_[p].next;
                
                    // append the skipped node to free queue
                    push_free (p);

                    -- cellar_count_;
                }
            
                //rehash ();
            }
        }

        //! seek an element
        //! @retval storage pointer to the value,
        //! if matching does not occur, NULL is returned
        _Value* seek (const _Key& key) const
        {
            u_int idx = hashf_(key) & bucket_cap_mask_;
            
            if (Node::unused != a_node_[idx].next) {
                if (eqf_ (a_node_[idx].key, key)) {
                    return &(a_node_[idx].value);
                }
                
                u_int p = a_node_[idx].next;
                // walk through the chain
                for (; Node::null != p && !eqf_ (a_node_[p].key, key)
                         ; p = a_node_[p].next);
                
                if (Node::null != p) {
                    return &(a_node_[p].value);
                }
            }

            return NULL;
        }
                         

        //! erase all elements
        //! @param[do_shrink] true to rehash after clearing
        void clear (bool do_shrink = false)
        {
            // mark nodes inside [0, bucket_cap_-1] unused
            for (u_int i=0; i<bucket_cap_; ++i) {
                a_node_[i].next = Node::unused;
            }

            // first cellar
            free_head_ = bucket_cap_;

            // chain nodes inside [0, bucket_cap_+cellar_cap_-2], they're
            // all free nodes
            u_int length = bucket_cap_ + cellar_cap_;
            for (u_int i=bucket_cap_; i<length; ++i) {
                a_node_[i].next = i+1;
            }
            // set next of last node to null to end the chain
            a_node_[length-1].next = Node::null;

            // clear counters
            bucket_count_ = 0;
            cellar_count_ = 0;
        
            if (do_shrink) {
                rehash ();
            }
        }

        //! count of elements
        size_t size () const
        { return bucket_count_ + cellar_count_; }

        //! capacity of elements
        u_int capacity () const
        { return bucket_cap_ + cellar_cap_; }

        //! an integer in [1,100] which is percentage of maximum elements
        //! stored in buckets comparing to capacity of buckets
        u_int load_factor () const
        { return load_factor_; }

        //! capacity_of_cellar * 100 / capacity_of_buckets
        u_int cellar_factor () const
        { return cellar_factor_; }
    
        //! memory consumption
        size_t mem () const
        {
            return sizeof(*this)
                + (sizeof (Node) * capacity())
                ;
        }

        //! print internal info of this hashmap
        void to_string(StringWriter& sw) const
        {
            sw << "{bucket_cap=" << bucket_cap_
                //<< " bucket_cap_mask=" << bucket_cap_mask_
                //<< " bucket_cap_shift=" << bucket_cap_shift_
               << " bucket_count=" << bucket_count_
               << " load_factor=" << load_factor_
               << " cellar_size=" << cellar_cap_
               << " cellar_count=" << cellar_count_
               << " cellar_factor=" << cellar_factor_
               << " free_head=" << free_head_
               << " mem=" << mem()
               << " content=";
            shows_range (sw, a_node_, a_node_+size());
            sw << "}";
        }
        
    private:
        void push_free (u_int idx)
        {
            a_node_[idx].next = free_head_;
            free_head_ = idx;
        }

        u_int pop_free ()
        {
            u_int idx = free_head_;
            free_head_ = a_node_[free_head_].next;
            return idx;
        }
    
        int rehash ()
        {
            u_int new_bucket_cap = bucket_cap_;
            u_int new_bucket_cap_shift = bucket_cap_shift_;
            u_int demand_bucket_cap = bucket_count_ * 100 / load_factor_;
            if (demand_bucket_cap < MIN_BUCKET_CAP) {
                demand_bucket_cap = MIN_BUCKET_CAP;
            }

            // ST_NOTICE ("demand_bucket_cap=%u, bucket_cap=%u, "
            //            "bucket_count=%u, cellar_count=%u"
            //            , demand_bucket_cap, bucket_cap_
            //            , bucket_count_, cellar_count_);
        
            if (demand_bucket_cap > bucket_cap_) {
                // growth
                for (; demand_bucket_cap > new_bucket_cap
                         ; new_bucket_cap = (new_bucket_cap<<1)
                         , ++ new_bucket_cap_shift);
            }
            else if (cellar_count_ == cellar_cap_) {  // no cellars
                if (demand_bucket_cap <= (bucket_cap_ << 1)) {
                    new_bucket_cap = (bucket_cap_ << 1);
                    new_bucket_cap_shift ++;
                }
                else {
                    for (; demand_bucket_cap > new_bucket_cap
                             ; new_bucket_cap = (new_bucket_cap<<1)
                             , ++ new_bucket_cap_shift);
                }
            }
            // else if ((demand_bucket_cap<<1) <= bucket_cap_) {
            //     // shrink
            //     for (; demand_bucket_cap <= new_bucket_cap
            //            ; new_bucket_cap = (new_bucket_cap>>1)
            //            , -- new_bucket_cap_shift);
            //     new_bucket_cap = new_bucket_cap << 1;
            //     ++ new_bucket_cap_shift;
            // }
            else {
                return 0;
            }
        
            ST_TRACE ("rehash from %u to %u, shift=%u"
                       , bucket_cap_, new_bucket_cap, new_bucket_cap_shift);
            //ST_TRACE ("this=%s", show(*this).c_str());

            CoalescedHashMap tmp;
            tmp.create (new_bucket_cap, load_factor_, cellar_factor_);
            u_int c = 0;
            //ST_TRACE ("before rehashing");
            for (const_iterator it=begin(); it != end(); ++it, ++c) {
                //ST_TRACE ("insert %u -> %u", it->key, it->value);
                tmp.insert (it->key, it->value);
            }
            if (c != size()) {
                ST_FATAL ("c=%u, size=%lu", c, size());
            }

            swap (tmp);
        
            return 0;
        }

    private:
        u_int bucket_cap_;
        u_int bucket_cap_mask_;
        u_int bucket_cap_shift_;
        u_int bucket_count_;
        u_int load_factor_;
        u_int free_head_;
        u_int cellar_cap_;
        u_int cellar_count_;
        u_int cellar_factor_;
        Node* a_node_;
        _Hash hashf_;
        _Equal eqf_;
    };

}

#endif  //_COALESCED_HASH_MAP_HPP_
