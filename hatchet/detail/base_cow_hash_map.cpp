// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Implement base_cow_hash_map.h
// Author: gejun@baidu.com
// Date: Dec.27 15:28:29 CST 2010
#include "base_cow_hash_map.h"
#include "st_utility.h"           // std_pair_first
#include "debug.h"                // logging

namespace st {


BaseCowHashMap::BaseCowHashMap ()
    : n_item_(0)
    , nbucket_(CH_DEFAULT_NBUCKET)
    , item_size_(0)
    , load_factor_(CH_DEFAULT_LOAD_FACTOR)
    , ap_entry_(NULL)
    , item_handler_(NULL)
    , alloc_creator_(false)
{}

BaseCowHashMap::~BaseCowHashMap ()
{
    clear();
    ST_DELETE_ARRAY(ap_entry_);
}

int BaseCowHashMap::init(
    u_int item_size,
    size_t nbucket,
    u_int load_factor,
    CowHashMapItemHandler* item_handler,
    std::string desc)
{
    if (!not_init()) {
        ST_FATAL("%s is already initialized", desc.c_str());
        return ECONFLICT;
    }

    item_size_ = item_size;
    n_item_ = 0;
    nbucket_ = find_near_prime(
        (nbucket < CH_MIN_NBUCKET) ? CH_MIN_NBUCKET : nbucket);
    load_factor_ =
        (load_factor < CH_MIN_LOAD_FACTOR
         ? CH_MIN_LOAD_FACTOR
         : (load_factor > CH_MAX_LOAD_FACTOR
            ? CH_MAX_LOAD_FACTOR : load_factor));
    item_handler_ = item_handler;
    alloc_creator_ = true;
    desc_ = desc;
                                
    sp_alloc_.reset(ST_NEW(Alloc, node_size()));
    if (NULL == sp_alloc_.get()) {
        ST_FATAL("Fail to new allocator");
        return ENOMEM;
    }

    ap_entry_ = ST_NEW_ARRAY(Bucket*, nbucket_ + 1);
    if (NULL == ap_entry_) {
        ST_FATAL("Fail to new ap_entry_");
        return ENOMEM;
    }
    memset(ap_entry_, 0, sizeof(Bucket*) * nbucket_);
    ap_entry_[nbucket_] = END_BUCKET;
    return 0;
}

// copy construct
BaseCowHashMap::BaseCowHashMap(const BaseCowHashMap& rhs)
{
    if (rhs.not_init()) {
        ST_FATAL("source is not initialized");
        // and we keep ourself uninitialized
        return;
    }
            
    ap_entry_ = ST_NEW_ARRAY(Bucket*, rhs.nbucket_ + 1);
    if (NULL == ap_entry_) {
        ST_FATAL("Fail to new ap_entry_");
        return;
    }
            
    sp_alloc_ = rhs.sp_alloc_;
    for (size_t i = 0; i < rhs.nbucket_; ++i) {
        ap_entry_[i] = rhs.ap_entry_[i];
        chain_reference(ap_entry_[i], NULL);
    }
    ap_entry_[rhs.nbucket_] = END_BUCKET;

    item_size_ = rhs.item_size_;
    n_item_ = rhs.n_item_;
    nbucket_ = rhs.nbucket_;
    load_factor_ = rhs.load_factor_;
    item_handler_ = rhs.item_handler_;
    alloc_creator_ = false;
    desc_ = rhs.desc_;
}

// operator=
BaseCowHashMap&
BaseCowHashMap::operator=(const BaseCowHashMap& rhs)
{
    // load_factor is not changed in this function
            
    if (&rhs == this) {  // assign to self, we do nothing here
        return *this;
    }
            
    if (not_init()) {
        // just call copy-constructor
        ST_NEW_ON(this, BaseCowHashMap, rhs);
        return *this;
    }
            
    if (rhs.not_init()) {  // source is not initialized 
        this->~BaseCowHashMap();  // destroy self
        return *this;
    }

    if (sp_alloc_ == rhs.sp_alloc_) {
        // sharing alloctor
        // making numbers of buckets same improves sharing ratio
        if (nbucket_ != rhs.nbucket_) {
            Bucket** ap_entry2 = ST_NEW_ARRAY(Bucket*, rhs.nbucket_ + 1);
            if (NULL == ap_entry2) {
                ST_FATAL("Fail to new ap_entry2");
                return *this;
            }

            clear();
                    
            delete [] ap_entry_;
            ap_entry_ = ap_entry2;
                                        
            for (size_t i=0; i<rhs.nbucket_; ++i) {
                ap_entry_[i] = rhs.ap_entry_[i];
                chain_reference(ap_entry_[i], NULL);
            }
            ap_entry_[rhs.nbucket_] = END_BUCKET;
        } else {
            for (size_t i=0; i<nbucket_; ++i) {
                if (ap_entry_[i] != rhs.ap_entry_[i]) {
                    chain_dereference(ap_entry_[i], NULL);
                    ap_entry_[i] = rhs.ap_entry_[i];
                    chain_reference(ap_entry_[i], NULL);
                }
            }
        }
        item_size_ = rhs.item_size_;
        nbucket_ = rhs.nbucket_;
        n_item_ = rhs.n_item_;
    } else {
        // separate allocators, we should be conservative with memory
        if ((nbucket_ * load_factor_) <= (rhs.n_item_ * 100)) {
            u_int nbucket2 = find_near_prime
                (rhs.n_item_ * 100 / load_factor_);
            Bucket** ap_entry2 = ST_NEW_ARRAY(Bucket*, nbucket2 + 1);
            if (NULL == ap_entry2) {
                ST_FATAL("Fail to new ap_entry2");
                return *this;
            }
            memset(ap_entry2, 0, sizeof(Bucket*) * nbucket2);
            ap_entry2[nbucket2] = END_BUCKET;

            clear();
                    
            delete [] ap_entry_;
            ap_entry_ = ap_entry2;
            nbucket_ = nbucket2;
        } else {
            clear();
        }
        
        item_size_ = rhs.item_size_;

        Bucket** pp_entry  = rhs.ap_entry_;
        Bucket** pp_entry_end  = rhs.ap_entry_ + rhs.nbucket_;
        char key_buf[item_size_];  // a key is no more than an item
        for ( ; pp_entry != pp_entry_end; ++pp_entry) {
            if (NULL != *pp_entry) {
                for (Bucket* p_node = *pp_entry; NULL != p_node;
                     p_node = p_node->p_next_) {
                    item_handler_->get_key(key_buf, p_node->item_);
                    insert(key_buf, p_node->item_);
                }
            }
        }
    }
    
    return *this;
}

void BaseCowHashMap::swap (BaseCowHashMap& rhs)
{
    std::swap(rhs.n_item_, n_item_);
    std::swap(rhs.nbucket_, nbucket_);
    std::swap(rhs.item_size_, item_size_);
    std::swap(rhs.load_factor_, load_factor_);
    std::swap(rhs.ap_entry_, ap_entry_);
    std::swap(rhs.item_handler_, item_handler_);
    rhs.sp_alloc_.swap(sp_alloc_);
    std::swap(rhs.alloc_creator_, alloc_creator_);
    std::swap(rhs.desc_, desc_);
}

void* BaseCowHashMap::insert (const void* key, const void* item)
{
    u_int bkt = item_handler_->hash_key(key) % nbucket_;

    // find equal item
    Bucket* p_item_node = ap_entry_[bkt];
    while (NULL != p_item_node && !item_handler_->eq_key_item(key, p_item_node->item_)) {
        p_item_node = p_item_node->p_next_;
    }

    if (NULL == p_item_node) {
        // no equal item, append a new node to the bucket
        if (resize_if_necessary()) {
            bkt = item_handler_->hash_key(key) % nbucket_;  // re-calculate bucket
        }
        Bucket* p_head_node = alloc_node_(item);
        p_head_node->p_next_ = ap_entry_[bkt];
        ap_entry_[bkt] = p_head_node;
        ++ n_item_;
        return p_head_node->item_;
    }

    // p_item_node->item_ equal item
    if (local_node(p_item_node)) {
        // local node, modify item_ in-place
        item_handler_->copy_item(p_item_node->item_, item);
        return p_item_node->item_;
    }
    // p_item_node is shared

    // insert the new node
    Bucket* p_new_node = alloc_node_(item);
    p_new_node->p_next_ = ap_entry_[bkt];
    Bucket** pp_prior_next = &(p_new_node->p_next_);

    // find first shared
    Bucket* p_shared_node = ap_entry_[bkt];
    while (p_shared_node != p_item_node && local_node(p_shared_node)) {
        pp_prior_next = &(p_shared_node->p_next_);
        p_shared_node = p_shared_node->p_next_;
    }

    // copy until p_item_node
    while (p_shared_node != p_item_node) {
        Bucket* p_cow_node = alloc_node_(p_shared_node->item_);
        // p_shared_node->n_ref_ must be >= 2
        dec_node_ref_(p_shared_node);
                
        *pp_prior_next = p_cow_node;
        pp_prior_next = &(p_cow_node->p_next_);

        p_shared_node = p_shared_node->p_next_;
    }
            
    // skip p_item_node
    *pp_prior_next = p_item_node->p_next_;
    // p_item_node->n_ref_ must be >=2
    dec_node_ref_(p_item_node);
            
    ap_entry_[bkt] = p_new_node;

    return p_new_node->item_;
}

int BaseCowHashMap::erase (const void* key)
{
    const size_t bkt = item_handler_->hash_key(key) % nbucket_;
    Bucket* p_item_node = ap_entry_[bkt];
    Bucket** pp_prior_next = &ap_entry_[bkt];
    while (NULL != p_item_node &&
           !item_handler_->eq_key_item(key, p_item_node->item_)) {
        pp_prior_next = &(p_item_node->p_next_);
        p_item_node = p_item_node->p_next_;
    }

    if (NULL == p_item_node) {  // no equal item
        return 0;
    }

    // p_item_node equal item
    if (local_node(p_item_node)) {
        // local node, skip and deallocate p_item_node
        // all nodes before p_item_node must be local and safe to edit
        *pp_prior_next = p_item_node->p_next_; 
        dec_node_ref_(p_item_node);
        -- n_item_;
        return 1;
    }

    // p_item_node is shared
    // find first shared
    // we don't want to change ap_entry_ directly
    Bucket* p_head_node = ap_entry_[bkt];
    pp_prior_next = &p_head_node;
    Bucket* p_shared_node = ap_entry_[bkt];
    while (p_shared_node != p_item_node && local_node(p_shared_node)) {
        pp_prior_next = &(p_shared_node->p_next_);
        p_shared_node = p_shared_node->p_next_;
    }

    // copy until p_item_node
    while (p_shared_node != p_item_node) {
        Bucket* p_cow_node = alloc_node_(p_shared_node->item_);
        // p_shared_node->n_ref_ must be >= 2
        dec_node_ref_(p_shared_node);
                
        *pp_prior_next = p_cow_node;
        pp_prior_next = &(p_cow_node->p_next_);

        p_shared_node = p_shared_node->p_next_;
    }
    // skip p_item_node
    *pp_prior_next = p_item_node->p_next_;
    // p_item_node->n_ref_ must be >= 2
    dec_node_ref_(p_item_node);
    -- n_item_;
            
    ap_entry_[bkt] = p_head_node;
    return 1;
}

void BaseCowHashMap::clear ()
{
    if (NULL != ap_entry_) {
        for (size_t i=0; i<nbucket_; ++i) {
            chain_dereference(ap_entry_[i], NULL);
            ap_entry_[i] = NULL;
        }
    }
    n_item_ = 0;
}

void* BaseCowHashMap::seek (const void* key) const
{
    const size_t bkt = item_handler_->hash_key(key) % nbucket_;
    Bucket* p_node = ap_entry_[bkt];
    while (NULL != p_node) {
        if (item_handler_->eq_key_item(key, p_node->item_)) {
            return p_node->item_;
        }
        p_node = p_node->p_next_;
    }
    return NULL;
}

bool BaseCowHashMap::resize (size_t nbucket2)
{            
    nbucket2 = find_near_prime(nbucket2);
            
    if (nbucket_ == nbucket2) {
        return false;
    }

    ST_TRACE("%s resized from %lu to %lu",
             desc_.c_str(), nbucket_, nbucket2);
            
    Bucket** ap_entry2 = ST_NEW_ARRAY(Bucket*, nbucket2 + 1);
    if (NULL == ap_entry2) {
        ST_FATAL("Fail to new ap_entry2");
        return false;
    }
    memset(ap_entry2, 0, sizeof(Bucket*)*nbucket2);
    ap_entry2[nbucket2] = END_BUCKET;

    if (ap_entry_) {
        char key_buf[item_size_];  // a key is not more than item
        for (size_t i=0; i<nbucket_; ++i) {
            Bucket* p_node = ap_entry_[i];
            while (NULL != p_node) {
                Bucket* p_next_node = p_node->p_next_;

                item_handler_->get_key(key_buf, p_node->item_);
                const size_t bkt = item_handler_->hash_key(key_buf) % nbucket2;
                if (local_node(p_node)) {
                    // ancestors of a local node must be all local, 
                    // it's safe to move local node anywhere
                    p_node->p_next_ = ap_entry2[bkt];
                    ap_entry2[bkt] = p_node;
                } else {  // p_node is shared
                    if (NULL == ap_entry2[bkt] && NULL == p_node->p_next_) {
                        // since we choose prime number as nbucket, items are
                        // hardly hashed to same bucket (in the sense of offset)
                        // with different numbers of bucket. if that happens,
                        // (hash(key1)-hash(key2)) divides bucket1*bucket2
                        // (generally 2*bucket1^2 when BaseCowHashMap grows), which
                        // is unlikely. so we take simple solution here, if
                        // next of the node and new_entry are both NULL, the
                        // node can be shared; otherwise the node is copied
                        ap_entry2[bkt] = p_node;
                    } else {
                        Bucket* p_cow_node = alloc_node_(p_node->item_);
                        p_cow_node->p_next_ = ap_entry2[bkt];
                        ap_entry2[bkt] = p_cow_node;
                        dec_node_ref_(p_node);
                    }
                }

                p_node = p_next_node;
            }
            //ap_entry_[i] = NULL;
        }
        delete [] ap_entry_;
    }

    nbucket_ = nbucket2;
    ap_entry_ = ap_entry2;
            
    return true;
}

size_t BaseCowHashMap::mem () const
{
    const long div = sp_alloc_.use_count();
    return sizeof(*this) +
        (sizeof(Bucket*) * nbucket_) +
        (div >= 1 ? (sp_alloc_->mem() / div) : 0);
}

void BaseCowHashMap::bucket_info (
    size_t* p_max_len, size_t* p_min_len, double* p_avg_len) const
{
    if (not_init()) {
        *p_max_len = 0;
        *p_min_len = 0;
        *p_avg_len = 0;
        return;
    }
            
    size_t max_len=0, min_len=UINT_MAX, sum_len=0;
    size_t n_non_empty = 0;
    for (size_t bkt = 0; bkt < nbucket_; bkt++) {
        Bucket* pp_entry = ap_entry_[bkt];
        if (NULL != pp_entry) {
            ++ n_non_empty;
            size_t len=1;
            for (Bucket* p_node = pp_entry->p_next_
                     ; p_node
                     ; p_node=p_node->p_next_, ++len);
            if (len > max_len) {
                max_len = len;
            }
            if (len < min_len) {
                min_len = len;
            }
            sum_len += len;
        }
    }

    *p_max_len = n_non_empty > 0 ? max_len : 0;
    *p_min_len = n_non_empty > 0 ? min_len : 0;
    *p_avg_len = n_non_empty > 0 ? (sum_len/(double)n_non_empty) : 0;
}

void BaseCowHashMap::to_string(StringWriter& sw) const
{
    size_t max_len = 0, min_len = 0;
    double avg_len = 0;
    bucket_info(&max_len, &min_len, &avg_len);

    sw << "CowHashMap{" << desc_
       << " Mem=" << mem()
       << " NItem=" << n_item_
       << " Bkt/Max/Min/Avg="<< nbucket_ << '/' << max_len << '/' << min_len << '/' << avg_len
       << " Alloc=" << sp_alloc_.use_count() << '/' << (void*)sp_alloc_.get();
    if (alloc_owner()) {
        sw << '/' << sp_alloc_.get();
    }
    
    // if (!not_init()) {
    //     sw << " items=";
    //     shows_range(sw, begin(), end());
    // }
    sw << "}";
}

std::ostream& operator<< (std::ostream& os, const BaseCowHashMap& m)
{
    size_t max_len = 0, min_len = 0;
    double avg_len = 0;
    m.bucket_info(&max_len, &min_len, &avg_len);

    os << "BaseCowHashTable{" << m.desc_
       << " Mem=" << m.mem()
       << " NItem=" << m.n_item_
       << " Bkt/Max/Min/Avg=" << m.nbucket_
       << '/' << max_len << '/' << min_len << '/' << avg_len
       << " Alloc=" << m.sp_alloc_.use_count() << '/' << (void*)m.sp_alloc_.get();
    if (m.alloc_owner()) {
        os << '/' << m.sp_alloc_.get();
    }
    
    os << "}";
    return os;
}

void BaseCowHashMap::chain_reference (Bucket* p_begin, Bucket* p_end)
{
    for (; p_begin != p_end; p_begin = p_begin->p_next_) {
        inc_node_ref_(p_begin);
    }
}

void BaseCowHashMap::chain_dereference (Bucket* p_begin, Bucket* p_end)
{
    while (p_begin != p_end) {
        Bucket* p_node = p_begin;
        p_begin = p_begin->p_next_;
        dec_node_ref_(p_node);
    }
}

Bucket* BaseCowHashMap::chain_copy (Bucket* p_begin, Bucket* p_end)
{
    Bucket* p_head = NULL;
    Bucket** pp_prior_next = &p_head;
    for (; p_begin != p_end; p_begin = p_begin->p_next_) {
        Bucket* p_node = alloc_node_(p_begin->item_);
        p_node->p_next_ = NULL;
        *pp_prior_next = p_node;
        pp_prior_next = &(p_node->p_next_);
    }
    return p_head;
}


}  // namespace st

