// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// A reference couting B+ tree
// Author: jiangrujie@baidu.com
// Date: Fri July 1 11:52:37 CST 2011
#pragma once
#ifndef _BASE_COWBASS4_H_
#define _BASE_COWBASS4_H_

#include "debug.h"                     // logging
#include "array_op.h"                  // array_search/copy/insert/erase
#include "rc_memory_pool.h"            // RCMemoryPool
#include "st_shared_ptr.hpp"           // shared_ptr

namespace st {
// Cowbass is short for "Copy-On-Write Block-Accessed Sorted Set" while
// BaseCowbass4 is the basic implementaion which makes an abstraction of
// (key, value) type. It's mainly designed for as-fast-as-array traversal 
// and as-fast-as-hashmap insertions and erasures with roughly 10K 
// to 100K items. We also want to share as much as possible between 
// foreground and background cowbasses. B+ tree has the property 
// of fast traversal and improvable insertions and erasures. 
// We extend B+ tree by removing the pointer to next leaf in
// each leaf and add a reference counter to each of the branch and leaf
// to track number of owners and copy the shared node and its shared
// ancestors on write to exploit the maximum possibility for memory
// sharing. Cowbass4 is used in CowHashClusterSet/Map as the "Cluster"
// In BaseCowbass4, all the data operations are between type void* so that
// it can work with all kinds of types during runtime. Remember to cast
// the return value of BaseCowbass4 outside it.

// TODO: make interfaces more clear
const u_int COWBASS_DEFAULT_FANOUT = 32;
// type of nodes
const u_int COWBASS_LEAF = 1;
const u_int COWBASS_BRANCH = 2;

typedef bool (*EqFn)(const void*, const void*);
typedef int (*ArraySearchFn)(const void*, const char*, 
                             u_int, u_int, u_int);
typedef void (*PrntFn)(StringWriter&, const void*);

// pass the modify request from a child node to its parent
struct CowbassReqList;

// base class of Leaf and Branch
struct BaseCowbassNode {
    u_int type_;            // runtime type to distinguish Leaf and Branch
};

// entry to a child
struct Dir {
    BaseCowbassNode* p_node_;   // pointer to the child
    char first_[];              // mark the address of the key
};  //__attribute__ ((aligned(sizeof(int)), packed));

// an abstract node, Leaf and Branch are both instances of this class
template <class _Config>
struct CowbassNode : public BaseCowbassNode {
    typedef typename _Config::Alloc Alloc;              // allocator for this node
    typedef typename _Config::ParentInfo_ ParentInfo;
    typedef typename _Config::Branch_ Branch;
    typedef typename _Config::Dir_ Dir;
            
    // create a node from the pool
    static CowbassNode* create (Alloc* p_alloc, u_int item_size)
    {
        CowbassNode* p_gn = p_alloc->template alloc_object<CowbassNode>();
        p_gn->type_ = _Config::TYPE;
        p_gn->n_item_ = 0;
        p_gn->item_size_ = item_size;
        return p_gn;
    }
            
    // this node is held by this map only
    bool local () const { return Alloc::is_local(this); }

    // increase the reference counter
    void reference () { Alloc::inc_ref (this); }
            
    // decrease the reference counter,
    // if the count hits zero, this node is deallocated to pool
    void dereference (Alloc* p_alloc) { Alloc::dec_ref (this, p_alloc); }

    // begin of items
    char* begin () { return a_item_; }

    // begin of items, const version
    const char* begin () const { return a_item_; }

    // end of items
    char* end () { return a_item_ + (n_item_ * item_size_); }

    // end of items, const version
    const char* end () const { return a_item_ + (n_item_ * item_size_); }

    // Resize this node, if this node is shared, copy-construct a new one
    // Params:
    //   n  intended number of items
    // Returns: pointer to resized node which may not be this
    CowbassNode* resize (Alloc* p_alloc, int n, CopyFn copy_fn);

    // Insert an item before a position, if this node is shared,
    // copy-construct a new one
    // Returns: pointer to modified node which may not be this
    CowbassNode* insert_before (Alloc* p_alloc, const int pos,
                                const void* item, CopyFn copy_fn);

    // Erase an item from a position, if this node is shared, copy-construct
    // a new one
    // Returns: pointer to modified node which may not be this
    CowbassNode* erase_at (Alloc* p_alloc, const int pos, 
                           CopyFn copy_fn);
            
    // Update an item at a position, if this node is shared,
    // copy-construct a new one
    // Returns: pointer to modified node which may not be this
    CowbassNode* update_at (Alloc* p_alloc, int pos, const void* item, 
                            CopyFn copy_fn);

    // Move a block of items around, if this node is shared, copy-construct
    // a new one
    // Params:
    //   d  new starting position
    //   b  starting position of to-be-moved items
    //   e  ending position of to-be-moved items
    // Returns: pointer to modified node which may not be this
    CowbassNode* move (Alloc* p_alloc, const int d, const int b, const int e,
                       CopyFn copy_fn);

    // Insert an item into this CowbassNode
    // Params:
    //   prnt_pos  position of this node in parent
    //   in_reqs   a list of insert/update requests to this node
    //   out_reqs  a list of insert/update requests to parent node
    // Returns: address to inserted item, NULL means the insertion was failed
    void* insert (Alloc* p_alloc, const int prnt_pos,
                  void* dir_buf, u_int dir_size, u_int max_fanout,
                  CopyFn copy_fn, CopyFn get_key_fn, EqFn dir_eq_fn,
                  const CowbassReqList& in_reqs,
                  CowbassReqList& out_reqs /*OUT*/);

    // Choose a neighbor to merge/average, 
    // TODO:this function needs more work
    int choose_neighbor (const ParentInfo& pi);

    // Erase an item from this node
    // Params:
    //   pi       parent information
    //   in_reqs  a list of erase/update requests to this node
    //   out_reqs a list of erase/update requests to parent node,
    // Note: out_reqs/in_reqs could be the same list
    // Returns: success or not
    bool erase (Alloc* p_alloc, const ParentInfo& pi,
                void* dir_buf, u_int dir_size, u_int max_fanout,
                CopyFn copy_fn, CopyFn get_key_fn, EqFn dir_eq_fn,
                const CowbassReqList& in_reqs,
                CowbassReqList& out_reqs /*OUT*/);            

//private:
    u_int n_item_;          // number of items
    u_int item_size_;
    
    // Array of items, as Leaf, items are just items; as Branch, items are 
    // directories linking to children. layout of items are:
    // key1 child1, key2 child2, ... keyN childN. We binary-search
    // [key2,key3...keyN] to know which child to go in, notice that we
    // store key1 but use it because it's basically useless in narrowing
    // the range but storing it simplifies insert and erase
    char a_item_[];         // mark the address of the buffer which 
                            // stores all the items
};

// interfaces of Cowbass4
class BaseCowbass4 {
private:
    struct BranchConfig;
    typedef CowbassNode<BranchConfig> Branch;

    struct ParentInfo {
        Branch* p_parent_;
        u_int pos_;
    };

    struct BranchConfig {
        typedef RCMemoryPool Alloc;
        typedef Dir Dir_;
        typedef ParentInfo ParentInfo_;
        typedef Branch Branch_;
        static const u_int TYPE = COWBASS_BRANCH;
        static Dir* make_dir (Branch* p_br, void* buf, 
                CopyFn copy_fn)
        { 
            copy_fn (buf, p_br->begin());
            Dir* dir_out = static_cast<Dir*>(buf);
            dir_out->p_node_ = p_br;
            return dir_out;
        }
    };
        
    struct LeafConfig;
    typedef CowbassNode<LeafConfig> Leaf;

    struct LeafConfig {
        typedef RCMemoryPool Alloc;
        typedef Dir Dir_;
        typedef ParentInfo ParentInfo_;
        typedef Branch Branch_;
        static const u_int TYPE = COWBASS_LEAF;
        static Dir* make_dir (Leaf* p_leaf, void* buf,
                CopyFn get_key_fn)
        { 
            Dir* dir_out = static_cast<Dir*>(buf);
            dir_out->p_node_ = p_leaf;
            get_key_fn (dir_out->first_, p_leaf->begin());
            return dir_out; 
        }
    };

    static const u_int MAX_N_DIR = 3;           // max number of Dir used by CowbassNode

public:
    // mark a position
    struct BaseIterator {
        static const u_int MAX_N_PARENT = 32;   // max number of parent
        // default constructor
        BaseIterator();

        // construct from a node, this Iterator will go through all nodes
        // which are direct or indirect children of that node
        explicit BaseIterator (BaseCowbassNode* p_root, 
                               u_int item_size, u_int dir_size);

        // it->blahblah
        const void* operator->() const { return p_item_; }

        // it1 == it2
        bool operator== (const BaseIterator& rhs) const
        { return p_item_ == rhs.p_item_; }

        // it1 != it2
        bool operator!= (const BaseIterator& rhs) const
        { return p_item_ != rhs.p_item_; }

        // ugly stuff
        operator bool() const { return NULL != p_item_; }
        void set_end() { p_item_ = NULL; }
 
        
        void to_string (StringWriter& sw) const
        {
            sw << "CowbassIter{p_item=" << p_item_
               << " n_parent_=" << n_parent_
               << "}";
        }
        
        // go to next node
        inline void operator++();
            
    private:
        const void* p_item_;
        const void* p_item_end_;
        u_int n_parent_;
        u_int item_size_;
        u_int dir_size_;
        ParentInfo a_parent_[MAX_N_PARENT];
    };

    typedef BaseIterator Iterator;
    typedef LeafConfig::Alloc LeafAlloc;
    typedef BranchConfig::Alloc BranchAlloc;

    // default constructor
    BaseCowbass4();
        
    // Initialize a BaseCowbass4, must be called before using
    // Note: p_leaf_alloc/p_branch_alloc should be both NULL or both 
    //       non-NULL, only one NULL are treated as both NULL, the 
    //       restriction guarantees that leaf/branch are both shared 
    //       or both local.
    // Returns:
    //   0               success
    //   ECONFLICT       already initialized
    //   ENOMEM          fail to new anything
    int init (u_int item_size, u_int key_size, u_int max_fanout,
              CopyFn item_copy_fn, CopyFn dir_copy_fn, 
              CopyFn get_key_fn, EqFn dir_eq_fn, 
              ArraySearchFn arr_search_dir_fn, 
              ArraySearchFn arr_search_item_fn);

    // destroy this BaseCowbass4
    ~BaseCowbass4() { clear(); }
        
    // Construct from another BaseCowbass4.
    // Note: if allocators of rhs are shared, share them as well; 
    //       otherwise create local allocators
    BaseCowbass4 (const BaseCowbass4& rhs);

    // Assign from another BaseCowbass4.
    // Note: node-level sharing is enable iff allocators are same
    BaseCowbass4& operator= (const BaseCowbass4& rhs);
        
    // erase all items
    void clear()
    {
        mod_nodes (dereference_leaf (sp_leaf_alloc_.get()),
                   dereference_branch (sp_branch_alloc_.get()));
        p_root_ = NULL;
        n_all_ = 0;
    }

    // Insert an item into BaseCowbass4.
    // Returns: address of inserted item, NULL means that the insertion was
    //          failed
    void* insert (const void* item);

    // Erase an item matching a key from BaseCowbass4
    // Returns: erased or not
    bool erase (const void* key);

    // beginning position
    Iterator begin () const 
    { return Iterator (p_root_, item_size_, dir_size_); }

    // ending position
    // Note: static method
    static Iterator end () { return Iterator (); }
    
    // Seek the item matching a key
    // Returns: address of the item, NULL means no item matches the key
    inline const void* seek (const void* key) const;

    template <class _LeafMod, class _BranchMod>
    void mod_nodes (const _LeafMod& leaf_mod, const _BranchMod& br_mod);
    
    // show all the children of the node and itself with indent format
    static void shows_with_indent (StringWriter& sw, int indent,
                                   const BaseCowbassNode* p_node,
                                   PrntFn prnt_key_fn,
                                   PrntFn prnt_item_fn);

    // print important information
    void to_string (StringWriter& sw) const;

    // empty or not
    bool empty() const { return  0 == n_all_; }

    const BaseCowbassNode* get_root() const 
    { return p_root_; }
        
    // number of items in this Cowbass4
    size_t size() const { return n_all_; }

    // initialized or not
    bool not_init() const { return NULL == sp_leaf_alloc_.get(); }
    
    const LeafAlloc* leaf_alloc() const { return sp_leaf_alloc_.get(); }

    long leaf_alloc_use_count() const { return sp_leaf_alloc_.use_count(); }

    const BranchAlloc* branch_alloc() const { return sp_branch_alloc_.get(); }

    long branch_alloc_use_count() const { return sp_branch_alloc_.use_count(); }

    bool leaf_alloc_owner() const
    { return alloc_creator_ || sp_leaf_alloc_.use_count() == 1; }

    bool branch_alloc_owner() const
    { return alloc_creator_ || sp_branch_alloc_.use_count() == 1; }
    
    size_t mem() const
    {
        return sizeof(*this)
            + (leaf_alloc_owner() ? sp_leaf_alloc_->mem() : 0)
            + (branch_alloc_owner() ? sp_branch_alloc_->mem() : 0);
    }
    
private:
    static void shows_with_indent (StringWriter& sw, const int indent,
                                   const Dir* dir, 
                                   PrntFn prnt_key_fn,
                                   PrntFn prnt_item_fn);

    static void shows_with_indent (StringWriter& sw, const int indent,
                                   const void* item, PrntFn prnt_fn);

    template <class _Node>
    static void shows_node_with_indent (StringWriter& sw, int indent,
                                        const _Node* p_node, 
                                        PrntFn prnt_key_fn,
                                        PrntFn prnt_item_fn);
    struct reference_leaf
    { void operator() (Leaf* p_leaf) const { p_leaf->reference(); } };

    struct dereference_leaf {
        explicit dereference_leaf(LeafAlloc* p_alloc) : p_alloc_(p_alloc) {}
        void operator() (Leaf* p_leaf) const { p_leaf->dereference(p_alloc_); }
    private:
        LeafAlloc* p_alloc_;
    };
        
    struct reference_branch
    { void operator() (Branch* p_branch) const { p_branch->reference(); } };

    struct dereference_branch {
        explicit dereference_branch(BranchAlloc* p_alloc) : p_alloc_(p_alloc) {}
        void operator() (Branch* p_branch) const
        { p_branch->dereference(p_alloc_); }
    private:
        BranchAlloc* p_alloc_;
    };

    Leaf* seek_leaf (const void* key, int* p_pos,
                     ParentInfo* a_parent, int* pn_parent) const;
        
private:
    BaseCowbassNode* p_root_;
    size_t alloc_creator_ : 1;
    size_t n_all_ : 63;
    u_int item_size_;
    u_int dir_size_;
    u_int key_size_;
    u_int max_fanout_;
    u_int max_n_parent_;
    CopyFn item_copy_fn_;
    CopyFn dir_copy_fn_;
    CopyFn get_key_fn_;
    EqFn dir_eq_fn_;
    ArraySearchFn arr_search_dir_fn_; 
    ArraySearchFn arr_search_item_fn_;
    st_boost::shared_ptr<LeafAlloc> sp_leaf_alloc_;
    st_boost::shared_ptr<BranchAlloc> sp_branch_alloc_;    
};

const void* BaseCowbass4::seek (const void* key) const
{
    if (unlikely(NULL == p_root_)) {
        return NULL;
    }

    const BaseCowbassNode* p_node = p_root_;
    while (COWBASS_BRANCH == p_node->type_) {
        const Branch* p_br = static_cast<const Branch*>(p_node);
        // binary search in [key1..keyN]
        const int pos = arr_search_dir_fn_ (key, 
                                            p_br->begin() + dir_size_,
                                            dir_size_,
                                            p_br->n_item_ - 1, 
                                            max_fanout_);
        if (pos >= 0) {
            p_node = reinterpret_cast<const Dir*>
                (p_br->begin() + (pos + 1) * dir_size_)->p_node_;
            while (COWBASS_BRANCH == p_node->type_) {
                p_br = static_cast<const Branch*>(p_node);
                p_node = reinterpret_cast<const Dir*>
                    (p_br->begin())->p_node_;
            }
            return static_cast<const Leaf*>(p_node)->begin();
        }
                
        p_node = reinterpret_cast<const Dir*>
            (p_br->begin() + ~pos * dir_size_)->p_node_;
    }

    const Leaf* p_leaf = static_cast<const Leaf*>(p_node);
    const int pos = arr_search_item_fn_ (key, 
                                         p_leaf->begin(), 
                                         item_size_,
                                         p_leaf->n_item_,
                                         max_fanout_);
    return (pos >= 0) ? (p_leaf->begin() + pos * item_size_) : NULL;
}


void BaseCowbass4::BaseIterator::operator++ ()
{
    p_item_ = static_cast<const char*>(p_item_) + item_size_;
    if (p_item_ == p_item_end_) {
        while (n_parent_ > 0) {
            ParentInfo& pi = a_parent_[n_parent_-1];
            const Branch* p_br = pi.p_parent_;
            ++ pi.pos_;
            if (pi.pos_ < p_br->n_item_) {
                BaseCowbassNode* p_node = reinterpret_cast<const Dir*>
                    (p_br->begin() + pi.pos_ * dir_size_)->p_node_;
                while (COWBASS_BRANCH == p_node->type_) {
                    Branch* p_br2 = static_cast<Branch*>(p_node);
                    a_parent_[n_parent_].p_parent_ = p_br2;
                    a_parent_[n_parent_].pos_ = 0;
                    ++ n_parent_;
                    //cout << " " << n_parent_ << endl;
                    p_node = reinterpret_cast<Dir*>
                        (p_br2->begin())->p_node_;
                }
                const Leaf* p_leaf = static_cast<const Leaf*>(p_node);
                p_item_ = p_leaf->begin();
                p_item_end_ = static_cast<const char*>
                    (p_item_) + p_leaf->n_item_ * item_size_;
                return;
            }
            -- n_parent_;
        }
        p_item_ = NULL;
    }
}

}  // namespace st
    
#endif  //_BASE_COWBASS4_H_
