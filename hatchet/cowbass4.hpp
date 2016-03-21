// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// A reference couting B+ tree
// Author: gejun@baidu.com
// Date: Fri Dec 16 11:52:37 CST 2010
#pragma once
#ifndef _COWBASS4_HPP_
#define _COWBASS4_HPP_

#include "c_common.hpp"                // TCAP
#include "compare.hpp"                 // Compare, ReturnType
#include "c_math.hpp"                  // bit_shift
#include "debug.h"                     // logging

#if defined (_ST3)

#include "base_cowbass4.h"

namespace st {
    
// This class is the wrapper of BaseCowbass4 using templates.
template <typename _Item, class _GetKey, 
          u_int _MAX_FANOUT = COWBASS_DEFAULT_FANOUT, 
          class _Cmp = Compare<TCAP(ReturnType, _GetKey, _Item)> >
class Cowbass4 {
public:
    // type of key part
    typedef TCAP(ReturnType, _GetKey, _Item) Key;
        
    // maximum number of children
    static const u_int MAX_FANOUT = (_MAX_FANOUT >> 1) << 1;

private:
    struct compare_key_and_item {
        int operator() (const void* key, const void* item) const
        {
            return _Cmp()(*static_cast<const Key*>(key),
                          _GetKey()
                            (*reinterpret_cast<const _Item*>
                                (item)));
        }
    };

    // comparator for array_binary_search
    struct compare_key_and_dir {
        int operator() (const void* key, const void* dir)
        {
            const Dir* p_dir = static_cast<const Dir*>(dir);
            return _Cmp() (*static_cast<const Key*>(key),
                           *reinterpret_cast<const Key*>
                            (p_dir->first_));
        }
    };

public:
    typedef BaseCowbass4::LeafAlloc LeafAlloc;
    typedef BaseCowbass4::BranchAlloc BranchAlloc;
    typedef _Item Value;
    typedef const Value* Pointer;
    typedef const Value& Reference;

    // a wrapper of BaseCowbass4::BaseIterator
    struct ConstIterator {
        typedef _Item Value;
        typedef const Value* Pointer;
        typedef const Value& Reference;
        
        // default constructor
        ConstIterator () {}

        explicit ConstIterator (BaseCowbass4::Iterator it)
            : it_(it) {}

        // *it
        const _Item& operator*() const 
        { return *static_cast<const _Item*>(it_.operator->()); }

        // it->blahblah
        const _Item* operator->() const 
        { return static_cast<const _Item*>(it_.operator->()); }

        // it1 == it2
        bool operator== (const ConstIterator& rhs) const
        { return it_ == rhs.it_; }

        // it1 != it2
        bool operator!= (const ConstIterator& rhs) const
        { return it_ != rhs.it_; }

        // ugly stuff
        operator bool() const { return it_.operator bool(); }
        void set_end() { it_.set_end(); }
 
        
        void to_string (StringWriter& sw) const
        { it_.to_string (sw); }
        
        
        // go to next node
        void operator++() { ++it_; }

    private:
        BaseCowbass4::Iterator it_;
    };

    typedef ConstIterator Iterator;

    // default constructor
    Cowbass4 () {}
        
    // Initialize a Cowbass4, must be called before using
    // Note: It should pass all the data operation function pointers
    //       to BaseCowbass4 so that base_ is able to work. Sizes of
    //       the types are also needed.
    // Returns: 0/ENOMEM
    int init ()
    {
        return base_.init (sizeof (_Item), sizeof (Key), MAX_FANOUT,
                           item_copy_fn, dir_copy_fn, get_key_fn,
                           dir_eq_fn, 
                           array_binary_search<compare_key_and_dir>, 
                           array_binary_search<compare_key_and_item>); 
    }

    // destroy this Cowbass4
    ~Cowbass4 () { base_.clear (); }
        
    // Construct from another Cowbass4.
    // Note: if allocators of rhs are shared, share them as well; 
    //       otherwise create local allocators
    Cowbass4 (const Cowbass4& rhs) : base_ (rhs.base_) {}

    // Assign from another Cowbass4.
    // Note: node-level sharing is enable iff allocators are same
    Cowbass4& operator= (const Cowbass4& rhs)
    {
        base_ = rhs.base_;
        return *this;
    }

    void clear () { base_.clear (); }
        
    // beginning position
    ConstIterator begin () const { return ConstIterator (base_.begin ()); }

    // ending position
    // Note: static method
    static ConstIterator end () { return ConstIterator (BaseCowbass4::end ()); }

    // Insert an item into Cowbass4.
    // Returns: address of inserted item, NULL means that the insertion was
    //          failed
    _Item* insert (const _Item& item)
    { return static_cast<_Item*>(base_.insert (&item)); }

    // Erase an item matching a key from Cowbass4
    // Returns: erased or not
    bool erase (const Key& key) { return base_.erase (&key); }

    // Seek the item matching a key
    // Returns: address of the item, NULL means no item matches the key
    inline const _Item* seek (const Key& key) const
    { return static_cast<const _Item*>(base_.seek (&key)); }

    template <class _LeafMod, class _BranchMod>
    void mod_nodes (const _LeafMod& leaf_mod, const _BranchMod& br_mod)
    { base_.mod_nodes (leaf_mod, br_mod); }
    
    // print important information
    void to_string (StringWriter& sw) const
    {
        base_.to_string(sw);
        //sw << "\n";
        //BaseCowbass4::shows_with_indent (sw, 2, base_.get_root(), 
        //                                 prnt_key_fn, prnt_item_fn);
    }

    // empty or not
    bool empty () const { return base_.empty (); }
        
    // number of items in this Cowbass4
    size_t size () const { return base_.size (); }

    // initialized or not
    bool not_init () const { return base_.not_init (); }
    
    const LeafAlloc* leaf_alloc () const 
    { return base_.leaf_alloc (); }

    long leaf_alloc_use_count () const 
    { return base_.leaf_alloc_use_count (); }

    const BranchAlloc* branch_alloc () const 
    { return base_.branch_alloc (); }

    long branch_alloc_use_count () const 
    { return base_.branch_alloc_use_count (); }

    bool leaf_alloc_owner () const
    { return base_.leaf_alloc_owner (); }

    bool branch_alloc_owner () const
    { return base_.branch_alloc_owner (); }
    
    size_t mem () const { return base_.mem (); }
        
private:
    BaseCowbass4 base_;
    
    static void item_copy_fn (void* dst, const void* src)
    { *static_cast<_Item*>(dst) = *static_cast<const _Item*>(src); }

    static void dir_copy_fn (void* dst, const void* src)
    { 
        Dir* p_dst = static_cast<Dir*>(dst);
        const Dir* p_src = static_cast<const Dir*>(src);
        p_dst->p_node_ = p_src->p_node_;
        *reinterpret_cast<Key*>(p_dst->first_) = 
            *reinterpret_cast<const Key*>(p_src->first_);
    }

    static void get_key_fn (void* key, const void* item)
    {
        new (key) Key(_GetKey()(*static_cast<const _Item*>(item)));
    }

    static bool dir_eq_fn (const void* dir1, const void* dir2)
    {
        const Dir* p_dir1 = static_cast<const Dir*>(dir1);
        const Dir* p_dir2 = static_cast<const Dir*>(dir2);
        return (p_dir1->p_node_ == p_dir2->p_node_) &&
            (*reinterpret_cast<const Key*>(p_dir1->first_) ==
             *reinterpret_cast<const Key*>(p_dir2->first_));
    }
    
    static void prnt_key_fn (StringWriter& sw, const void* key)
    { sw << *static_cast<const Key*>(key); }
    
    static void prnt_item_fn (StringWriter& sw, const void* item)
    { sw << *static_cast<const _Item*>(item); }
};


// Print type of Cowbass4
template <typename _Item, class _GetKey, u_int _MAX_FANOUT, class _Cmp>
struct c_show_impl<Cowbass4<_Item, _GetKey, _MAX_FANOUT, _Cmp> > {
    static void c_to_string (std::ostream& os)
    {
        os << "Cowbass4{" << c_show(TCAP(ReturnType, _GetKey, _Item))
           << "->" << _MAX_FANOUT << '*' << sizeof(_Item)
           << ':' << c_show(_Item)
           << "}";
    }
};


}  // namespace st

#else // end _ST3 

#include "array_op.hpp"                // array_search/copy/insert/erase
#include "rc_memory_pool.h"            // RCMemoryPool
#include "st_shared_ptr.hpp"           // shared_ptr

namespace st {
// Cowbass is short for "Copy-On-Write Block-Accessed Sorted Set" while
// Cowbass4 is the fifth implementation of the name. It's mainly designed
// for as-fast-as-array traversal and as-fast-as-hashmap insertions and
// erasures with roughly 10K to 100K items. We also want to share as
// much as possible between foreground and background cowbasses. B+ tree
// has the property of fast traversal and improvable insertions and
// erasures. We extend B+ tree by removing the pointer to next leaf in
// each leaf and add a reference counter to each of the branch and leaf
// to track number of owners and copy the shared node and its shared
// ancestors on write to exploit the maximum possibility for memory
// sharing. Cowbass4 is used in CowHashClusterSet/Map as the "Cluster"

// TODO: make interfaces more clear
const u_int COWBASS_DEFAULT_FANOUT = 32;
// type of nodes
const u_int COWBASS_LEAF = 1;
const u_int COWBASS_BRANCH = 2;

template <typename _Item,  // type of items
          class _GetKey,  // key getter
          u_int _MAX_FANOUT = COWBASS_DEFAULT_FANOUT,
          class _Cmp = Compare<TCAP(ReturnType, _GetKey, _Item)> >
class Cowbass4;

template <typename _Data> struct CowbassReqList {
    static const u_int MAX_N_REQ = 2;  // maximum length

    struct Req {
        u_int pos_;
        _Data data_;
    };

    
    CowbassReqList () : n_ie_(0), n_update_(0) {}

    void push_insert (int pos, const _Data& d)
    {
        assert (0 == n_update_);
        assert (n_ie_ < MAX_N_REQ);
        Req& req = a_req_[n_ie_++];
        req.pos_ = pos;
        req.data_ = d;
    }

    void push_erase (int pos)
    {
        assert (0 == n_update_);
        assert (n_ie_ < MAX_N_REQ);
        Req& req = a_req_[n_ie_++];
        req.pos_ = pos;
    }

    void push_update (int pos, const _Data& d)
    {
        assert ((n_ie_+n_update_) < MAX_N_REQ);
        Req& req = a_req_[n_ie_+(n_update_++)];
        req.pos_ = pos;
        req.data_ = d;
    }

    u_int ie_begin() const { return 0; }
                
    u_int ie_end() const { return n_ie_; }

    u_int up_begin() const { return n_ie_; }
                
    u_int up_end() const { return n_ie_ + n_update_; }

    bool empty () const { return 0 == n_ie_ && 0 == n_update_; }

    u_int size() const { return n_ie_ + n_update_; }
                
    Req& operator[] (int i) { return a_req_[i]; }

    const Req& operator[] (int i) const { return a_req_[i]; }
            
    void clear ()
    {
        n_ie_ = 0;
        n_update_ = 0;
    }

private:
    Req a_req_[MAX_N_REQ];
    u_int n_ie_;
    u_int n_update_;
};

// base class of Leaf and Branch
struct BaseCowbassNode {
    u_int type_;  // runtime type to distinguish Leaf and Branch
};

// entry to a child
template <typename _Key> struct CowbassDir {
    // default constructor
    CowbassDir() {}
    
    explicit CowbassDir (const _Key& first, BaseCowbassNode* p_child)
        : first_(first), p_node_(p_child)
    {}

    // dir1 != dir2
    bool operator!= (const CowbassDir& rhs) const
    { return first_ != rhs.first_ || p_node_ != rhs.p_node_; }

    // dir1 == dir2
    bool operator== (const CowbassDir& rhs) const
    { return first_ == rhs.first_ && p_node_ == rhs.p_node_; }
    
    _Key first_;  // first key of the child
    BaseCowbassNode* p_node_;  // pointer to the child
};  //__attribute__ ((aligned(sizeof(int)), packed));

// an abstract node, Leaf and Branch are both instances of this class
template <typename _Item, class _Config>
struct CowbassNode : public BaseCowbassNode {
    typedef typename _Config::Alloc Alloc;  // allocator for this node
    typedef typename _Config::ParentInfo_ ParentInfo;
    typedef typename _Config::Branch_ Branch;
    typedef typename _Config::Dir_ Dir;
    static const u_int MAX_FANOUT = _Config::MAX_FANOUT_;
    static const u_int HALF_MAX_FANOUT = _Config::HALF_MAX_FANOUT_;
            
    // create a node from the pool
    static CowbassNode* create (Alloc* p_alloc)
    {
        CowbassNode* p_gn = p_alloc->template alloc_object<CowbassNode>();
        p_gn->type_ = _Config::TYPE;
        p_gn->n_item_ = 0;
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
    _Item* begin () { return a_item_; }

    // begin of items, const version
    const _Item* begin () const { return a_item_; }

    // end of items
    _Item* end () { return a_item_ + n_item_; }

    // end of items, const version
    const _Item* end () const { return a_item_ + n_item_; }

    // Resize this node, if this node is shared, copy-construct a new one
    // Params:
    //   n  intended number of items
    // Returns: pointer to resized node which may not be this
    CowbassNode* resize (Alloc* p_alloc, int n);

    // Insert an item before a position, if this node is shared,
    // copy-construct a new one
    // Returns: pointer to modified node which may not be this
    CowbassNode* insert_before (Alloc* p_alloc, const int pos,
                                const _Item& item);

    // Erase an item from a position, if this node is shared, copy-construct
    // a new one
    // Returns: pointer to modified node which may not be this
    CowbassNode* erase_at (Alloc* p_alloc, const int pos);
            
    // Update an item at a position, if this node is shared,
    // copy-construct a new one
    // Returns: pointer to modified node which may not be this
    CowbassNode* update_at (Alloc* p_alloc, int pos, const _Item& item);

    // Move a block of items around, if this node is shared, copy-construct
    // a new one
    // Params:
    //   d  new starting position
    //   b  starting position of to-be-moved items
    //   e  ending position of to-be-moved items
    // Returns: pointer to modified node which may not be this
    CowbassNode* move (Alloc* p_alloc, const int d, const int b, const int e);

    // Insert an item into this CowbassNode
    // Params:
    //   prnt_pos  position of this node in parent
    //   in_reqs   a list of insert/update requests to this node
    //   out_reqs  a list of insert/update requests to parent node
    // Returns: address to inserted item, NULL means the insertion was failed
    _Item* insert (Alloc* p_alloc, const int prnt_pos,
                   const CowbassReqList<_Item>& in_reqs,
                   CowbassReqList<Dir>& out_reqs /*OUT*/);

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
                const CowbassReqList<_Item>& in_reqs,
                CowbassReqList<Dir>& out_reqs /*OUT*/);            

//private:
    u_int n_item_;   // number of items

    // Array of items, as Leaf, items are just items; as Branch, items are 
    // directories linking to children. layout of items are:
    // key1 child1, key2 child2, ... keyN childN. We binary-search
    // [key2,key3...keyN] to know which child to go in, notice that we
    // store key1 but use it because it's basically useless in narrowing
    // the range but storing it simplifies insert and erase
    _Item a_item_[MAX_FANOUT];
};

// interfaces of Cowbass4
template <typename _Item, class _GetKey, u_int _MAX_FANOUT, class _Cmp>
class Cowbass4 {
public:
    // type of key part
    typedef TCAP(ReturnType, _GetKey, _Item) Key;

    typedef CowbassDir<Key> Dir;
        
    // make MaxFanOut always even (so that half counting is eaiser)
    static const u_int HALF_MAX_FANOUT = ((_MAX_FANOUT+1)>>1);
        
    // maximum number of children
    static const u_int MAX_FANOUT = (HALF_MAX_FANOUT<<1);

    // Maximum depth, We may store MAX_FANOUT ^ MAX_N_PARENT = 2^30
    // (1 billion) in this cowbass
    static const u_int MAX_N_PARENT = 30 / CAP(bit_shift, MAX_FANOUT);        

private:
    struct BranchConfig;
    typedef CowbassNode<Dir, BranchConfig> Branch;

    struct ParentInfo {
        Branch* p_parent_;
        u_int pos_;
    };

    struct BranchConfig {
        typedef RCMemoryPool Alloc;
        typedef Dir Dir_;
        typedef ParentInfo ParentInfo_;
        typedef Branch Branch_;
        enum { MAX_FANOUT_ = MAX_FANOUT,
               HALF_MAX_FANOUT_ = HALF_MAX_FANOUT };
        static const u_int TYPE = COWBASS_BRANCH;
        static Dir make_dir (Branch* p_br)
        { return Dir (p_br->a_item_[0].first_, p_br); }
    };
        
    struct LeafConfig;
    typedef CowbassNode<_Item, LeafConfig> Leaf;

    struct LeafConfig {
        typedef RCMemoryPool Alloc;
        typedef Dir Dir_;
        typedef ParentInfo ParentInfo_;
        typedef Branch Branch_;
        enum { MAX_FANOUT_ = MAX_FANOUT,
               HALF_MAX_FANOUT_ = HALF_MAX_FANOUT };
        static const u_int TYPE = COWBASS_LEAF;
        static Dir make_dir (Leaf* p_leaf)
        {
            static _GetKey get_key_;
            return Dir(get_key_(p_leaf->a_item_[0]), p_leaf);
        }
    };

    // comparator for array_binary_search
    struct compare_key_and_item {
        int operator() (const Key& k, const _Item& item) const
        { return _Cmp()(k, _GetKey()(item)); }
    };
        
    // comparator for array_binary_search
    struct compare_key_and_dir {
        int operator() (const Key& k, const Dir& d) const
        { return _Cmp()(k, d.first_); }
    };

public:
    typedef typename LeafConfig::Alloc LeafAlloc;
    typedef typename BranchConfig::Alloc BranchAlloc;
    typedef _Item Value;
    typedef const Value* Pointer;
    typedef const Value& Reference;

    // mark a position
    struct ConstIterator {
        typedef _Item Value;
        typedef const Value* Pointer;
        typedef const Value& Reference;
        
        // default constructor
        ConstIterator () : p_item_(NULL), p_item_end_(NULL) {}

        // construct from a node, this Iterator will go through all nodes
        // which are direct or indirect children of that node
        explicit ConstIterator (BaseCowbassNode* p_root)
        {
            if (NULL == p_root) {
                p_item_ = NULL;
            } else {
                n_parent_ = 0;
                while (COWBASS_BRANCH == p_root->type_) {
                    a_parent_[n_parent_].p_parent_ =
                        static_cast<Branch*>(p_root);
                    a_parent_[n_parent_].pos_ = 0;
                    ++ n_parent_;
                    p_root = static_cast<const Branch*>(p_root)->
                        a_item_[0].p_node_;
                }
                const Leaf* p_leaf = static_cast<const Leaf*>(p_root);
                p_item_ = p_leaf->a_item_;
                p_item_end_ = p_leaf->a_item_ + p_leaf->n_item_;
            }
        }

        // *it
        const _Item& operator* () const { return *p_item_; }

        // it->blahblah
        const _Item* operator-> () const { return p_item_; }

        // it1 == it2
        bool operator== (const ConstIterator& rhs) const
        { return p_item_ == rhs.p_item_; }

        // it1 != it2
        bool operator!= (const ConstIterator& rhs) const
        { return p_item_ != rhs.p_item_; }

        // ugly stuff
        operator bool () const { return NULL != p_item_; }
        void set_end () { p_item_ = NULL; }
 
        void to_string (StringWriter& sw) const
        {
            sw << "CowbassIter{p_item=" << p_item_
               << " n_parent_=" << n_parent_
               << "}";
        }
        
        // go to next node
        void operator++ ()
        {
            ++ p_item_;
            if (p_item_ == p_item_end_) {
                while (n_parent_ > 0) {
                    ParentInfo& pi = a_parent_[n_parent_-1];
                    const Branch* p_br = pi.p_parent_;
                    ++ pi.pos_;
                    if (pi.pos_ < p_br->n_item_) {
                        BaseCowbassNode* p_node = p_br->a_item_[pi.pos_].p_node_;
                        while (COWBASS_BRANCH == p_node->type_) {
                            a_parent_[n_parent_].p_parent_ =
                                static_cast<Branch*>(p_node);
                            a_parent_[n_parent_].pos_ = 0;
                            ++ n_parent_;
                            //cout << " " << n_parent_ << endl;
                            p_node = static_cast<const Branch*>(p_node)->
                                a_item_[0].p_node_;
                        }
                        const Leaf* p_leaf = static_cast<const Leaf*>(p_node);
                        p_item_ = p_leaf->a_item_;
                        p_item_end_ = p_item_ + p_leaf->n_item_;
                        return;
                    }
                    -- n_parent_;
                }
                p_item_ = NULL;
            }
        }

    private:
        const _Item* p_item_;
        const _Item* p_item_end_;
        u_int n_parent_;
        ParentInfo a_parent_[MAX_N_PARENT];
    };

    typedef ConstIterator Iterator;

    // default constructor
    Cowbass4 () : p_root_(NULL), alloc_creator_(0), n_all_(0) {}
        
    // Initialize a Cowbass4, must be called before using
    // Note: p_leaf_alloc/p_branch_alloc should be both NULL or both 
    //       non-NULL, only one NULL are treated as both NULL, the 
    //       restriction guarantees that leaf/branch are both shared 
    //       or both local.
    // Returns: 0/ENOMEM
    int init ();

    // destroy this Cowbass4
    ~Cowbass4 () { clear (); }
        
    // Construct from another Cowbass4.
    // Note: if allocators of rhs are shared, share them as well; 
    //       otherwise create local allocators
    Cowbass4 (const Cowbass4& rhs);

    // Assign from another Cowbass4.
    // Note: node-level sharing is enable iff allocators are same
    Cowbass4& operator= (const Cowbass4& rhs);
        
    // erase all items
    void clear ()
    {
        mod_nodes(dereference_leaf(sp_leaf_alloc_.get()),
                  dereference_branch(sp_branch_alloc_.get()));
        p_root_ = NULL;
        n_all_ = 0;
    }

    // beginning position
    ConstIterator begin () const { return ConstIterator(p_root_); }

    // ending position
    // Note: static method
    static ConstIterator end () { return ConstIterator(NULL); }

    // Insert an item into Cowbass4.
    // Returns: address of inserted item, NULL means that the insertion was
    //          failed
    _Item* insert (const _Item& item);

    // Erase an item matching a key from Cowbass4
    // Returns: erased or not
    bool erase (const Key& key);

    // Seek the item matching a key
    // Returns: address of the item, NULL means no item matches the key
    inline const _Item* seek (const Key& key) const;

    template <class _LeafMod, class _BranchMod>
    void mod_nodes (const _LeafMod& leaf_mod, const _BranchMod& br_mod);
    
    static void shows_with_indent (StringWriter& sw, const int indent,
                                   const Dir& dir)
    {
        for (int i=0; i<indent; ++i) { sw << ' '; }
            
        sw << "k=" << dir.first_ << " ";
        shows_with_indent (sw, indent, dir.p_node_);
    }

    template <typename _Any> static void shows_with_indent
    (StringWriter& sw, const int indent, const _Any& v)
    {
        for (int i=0; i<indent; ++i) {
            sw << ' ';
        }
        sw << v;
    }

    // call show_with_indent of concrete type
    static void shows_with_indent (StringWriter& sw, const int indent,
                                   const BaseCowbassNode* p_node)
    {
        switch (p_node->type_) {
        case COWBASS_LEAF:
            shows_node_with_indent (sw, indent, static_cast<const Leaf*>(p_node));
            break;
        case COWBASS_BRANCH:
            shows_node_with_indent (sw, indent, static_cast<const Branch*>(p_node));
            break;
        }
    }

    template <class _Node>
    static void shows_node_with_indent (StringWriter& sw, int indent,
                                        const _Node* p_node)
    {
        sw << ((COWBASS_LEAF==p_node->type_) ? "LEAF":"BR")
           << "(nr=" << typename _Node::Alloc::get_rc_data(p_node) << "):{\n"; 
        for (u_int i=0; i < p_node->n_item_; ++i) {
            shows_with_indent(sw, indent+2, p_node->a_item_[i]);
            sw << "\n";
        }
        for (int j=0; j<indent;++j) {
            sw << ' ';
        }
        sw << "}";
    }

    // print important information
    void to_string (StringWriter& sw) const
    {
        sw << "{items=";
        shows_range (sw, begin(), end());

        sw << " leaf_alloc=" << sp_leaf_alloc_.use_count()
           << '/' << (void*)sp_leaf_alloc_.get();
        if (leaf_alloc_owner()) {
            sw << '/' << sp_leaf_alloc_.get();
        }

        sw << " branch_alloc=" << sp_branch_alloc_.use_count()
           << '/' << (void*)sp_branch_alloc_.get();
        if (branch_alloc_owner()) {
            sw << '/' << sp_branch_alloc_.get();
        }
            
        sw << '}';
    }

    // empty or not
    bool empty () const { return  0 == n_all_; }
        
    // number of items in this Cowbass4
    size_t size () const { return n_all_; }

    // initialized or not
    bool not_init () const { return NULL == sp_leaf_alloc_.get(); }
    
    const LeafAlloc* leaf_alloc () const { return sp_leaf_alloc_.get(); }

    long leaf_alloc_use_count () const { return sp_leaf_alloc_.use_count(); }

    const BranchAlloc* branch_alloc () const { return sp_branch_alloc_.get(); }

    long branch_alloc_use_count () const { return sp_branch_alloc_.use_count(); }

    bool leaf_alloc_owner () const
    { return alloc_creator_ || sp_leaf_alloc_.use_count() == 1; }

    bool branch_alloc_owner () const
    { return alloc_creator_ || sp_branch_alloc_.use_count() == 1; }
    
    size_t mem () const
    {
        return sizeof(*this)
            + (leaf_alloc_owner() ? sp_leaf_alloc_->mem() : 0)
            + (branch_alloc_owner() ? sp_branch_alloc_->mem() : 0);
    }
    
private:
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

    Leaf* seek_leaf (const Key& key, int* p_pos,
                     ParentInfo* a_parent, int* pn_parent) const;
        
private:
    BaseCowbassNode* p_root_;
    size_t alloc_creator_ : 1;
    size_t n_all_ : 63;
    st_boost::shared_ptr<LeafAlloc> sp_leaf_alloc_;
    st_boost::shared_ptr<BranchAlloc> sp_branch_alloc_;    
};


template <typename _Item, class _Config>
CowbassNode<_Item, _Config>*
CowbassNode<_Item, _Config>::resize (Alloc* p_alloc, int n)
{
    if (local()) {
        n_item_ = n;
        return this;
    } else {  // shared
        CowbassNode* p_gn = CowbassNode::create(p_alloc);
        array_copy_far (p_gn->begin(), begin(), begin()+n);
        p_gn->n_item_ = n;
        this->dereference(p_alloc);
        return p_gn;
    }
}

template <typename _Item, class _Config>
CowbassNode<_Item, _Config>*
CowbassNode<_Item, _Config>::
insert_before (Alloc* p_alloc, const int pos, const _Item& item)
{
    if (local()) {
        array_insert (begin(), end(), begin()+pos, item);
        ++ n_item_;
        return this;
    } else {  // shared
        CowbassNode* p_gn = CowbassNode::create(p_alloc);
        array_nip_insert_far (p_gn->begin(), begin(), end(), begin()+pos, item);
        p_gn->n_item_ = n_item_ + 1;
        this->dereference(p_alloc);
        return p_gn;
    }
}

template <typename _Item, class _Config>
CowbassNode<_Item, _Config>*
CowbassNode<_Item, _Config>::erase_at (Alloc* p_alloc, const int pos)
{
    if (local()) {
        array_erase (begin(), end(), begin()+pos);
        -- n_item_;
        return this;
    } else {  // shared
        CowbassNode* p_gn = CowbassNode::create(p_alloc);
        array_nip_erase_far (p_gn->begin(), begin(), end(), begin()+pos);
        p_gn->n_item_ = n_item_ - 1;
        this->dereference(p_alloc);
        return p_gn;
    }
}

template <typename _Item, class _Config>
CowbassNode<_Item, _Config>*
CowbassNode<_Item, _Config>::
update_at (Alloc* p_alloc, int pos, const _Item& item)
{
    if (local()) {
        a_item_[pos] = item;
        return this;
    } else {  // shared
        CowbassNode* p_gn = create (p_alloc);
        array_copy_far (p_gn->begin(), begin(), end());
        p_gn->n_item_ = n_item_;
        p_gn->a_item_[pos] = item;
        this->dereference (p_alloc);
        return p_gn;
    }
}

template <typename _Item, class _Config>
CowbassNode<_Item, _Config>*
CowbassNode<_Item, _Config>::
move (Alloc* p_alloc, const int d, const int b, const int e)
{
    if (local()) {
        array_copy (begin()+d, begin()+b, begin()+e);
        n_item_ = d + e - b;
        return this;
    } else {  // shared
        CowbassNode* p_node = create(p_alloc);
        array_copy_far (p_node->begin()+d, begin()+b, begin()+e);
        p_node->n_item_ = d + e - b;
        this->dereference (p_alloc);
        return p_node;
    }
}


template <typename _Item, class _Config>
_Item*
CowbassNode<_Item, _Config>::
insert (Alloc* p_alloc, const int prnt_pos,
        const CowbassReqList<_Item>& in_reqs,
        CowbassReqList<Dir>& out_reqs /*OUT*/)
{
    const u_int n_insert_req = in_reqs.ie_end() - in_reqs.ie_begin();
    assert (n_insert_req <= 1);

    const u_int n_item2 = n_insert_req + n_item_;
    u_int insert_pos = UINT_MAX;
    CowbassNode* p_node = this;
    Dir orig_dir = _Config::make_dir(p_node);
                
    if (n_item2 <= MAX_FANOUT) {
        // no need to split
        if (n_insert_req) {
            const typename CowbassReqList<_Item>::Req& req =
                in_reqs[in_reqs.ie_begin()];
            insert_pos = req.pos_;
            p_node = p_node->insert_before
                (p_alloc, insert_pos, req.data_);
        }
        for (u_int i=in_reqs.up_begin(); i<in_reqs.up_end(); ++i) {
            const typename CowbassReqList<_Item>::Req& req = in_reqs[i];
            p_node = p_node->update_at
                (p_alloc, req.pos_ + (req.pos_ > insert_pos), req.data_);
        }
                    
        out_reqs.clear();
        Dir curr_dir = _Config::make_dir(p_node);
        if (curr_dir != orig_dir) {
            out_reqs.push_update (prnt_pos, curr_dir);
        }
        return p_node->a_item_ + insert_pos;
    }

    // n_insert_req >= 1
    // (n_item2 > MAX_FANOUT)
    // need to split, we only support splitting into two leaves now
    CowbassNode* p_node2 = create (p_alloc);
    const typename CowbassReqList<_Item>::Req& req = in_reqs[0];
    insert_pos = req.pos_;
    _Item* p_store = NULL;
    if (req.pos_ < HALF_MAX_FANOUT) {
        // assignment to p_node2 MUST happen before changing old node
        array_copy_far (p_node2->begin(),
                        p_node->end()-HALF_MAX_FANOUT, p_node->end());
        p_node2->n_item_ = HALF_MAX_FANOUT;

        // make local
        p_node = p_node->resize (p_alloc, p_node->n_item_ - HALF_MAX_FANOUT);
        p_node = p_node->insert_before (p_alloc, insert_pos, req.data_);
        p_store = p_node->a_item_ + insert_pos;
    } else {
        array_nip_insert_far (p_node2->begin(),
                              p_node->begin() + HALF_MAX_FANOUT,
                              p_node->end(),
                              p_node->begin() + insert_pos,
                              req.data_);
        p_node2->n_item_ = p_node->n_item_ - HALF_MAX_FANOUT + 1;
        p_store = p_node2->a_item_ + insert_pos - HALF_MAX_FANOUT;
                    
        p_node = p_node->resize (p_alloc, HALF_MAX_FANOUT);
    }

    for (u_int i=in_reqs.up_begin(); i<in_reqs.up_end(); ++i) {
        const typename CowbassReqList<_Item>::Req& req2 = in_reqs[i];
        const u_int pos = req2.pos_ + (req2.pos_ > insert_pos);
        if (pos < p_node->n_item_) {
            p_node = p_node->update_at (p_alloc, pos, req2.data_);
        } else {
            p_node2 = p_node2->update_at (p_alloc,
                                          pos-p_node->n_item_, req2.data_);
        }
    }

    out_reqs.clear();
    if (p_node2) {
        out_reqs.push_insert (prnt_pos+1, _Config::make_dir(p_node2));
    }
    const Dir curr_dir = _Config::make_dir(p_node);
    if (curr_dir != orig_dir) {
        out_reqs.push_update (prnt_pos, curr_dir);
    }
    return p_store;
}


template <typename _Item, class _Config>
int CowbassNode<_Item, _Config>::choose_neighbor (const ParentInfo& pi)
{
    Branch* p_parent = pi.p_parent_;
    //static const int DOMINATE_SCORE = MAX_FANOUT << 2;
    int sum1 = INT_MAX;
    int sum2 = INT_MAX;
                
    if (pi.pos_ < p_parent->n_item_-1) {  // not the most right one
        BaseCowbassNode* p_right_node = p_parent->a_item_[pi.pos_+1].p_node_;
        if (_Config::TYPE != p_right_node->type_) {
            ST_FATAL ("Bad structure");
            return pi.pos_;
        }
        sum1 = n_item_ + static_cast<CowbassNode*>(p_right_node)->n_item_;
        // sum1 -= Alloc::is_local(p_right_node)
        //     * (sum1 <= MAX_FANOUT+1) * DOMINATE_SCORE;
    }
                
    if (pi.pos_ > 0) {  // not the most left one
        BaseCowbassNode* p_left_node = p_parent->a_item_[pi.pos_-1].p_node_;
        if (_Config::TYPE != p_left_node->type_) {
            ST_FATAL ("Bad structure");
            return pi.pos_;
        }
        sum2 = n_item_
            + static_cast<CowbassNode*>(p_left_node)->n_item_;
        // sum2 -= Alloc::is_local(p_left_node)
        //     * (sum2 <= MAX_FANOUT+1) * DOMINATE_SCORE;
    }

    if (INT_MAX == sum1 && INT_MAX == sum2) {
        ST_FATAL ("bad structure");
        return pi.pos_;
    }

    return pi.pos_ + ((sum1 < sum2) << 1) - 1;
}

template <typename _Item, class _Config>
bool CowbassNode<_Item, _Config>::
erase (Alloc* p_alloc, const ParentInfo& pi,
       const CowbassReqList<_Item>& in_reqs,
       CowbassReqList<Dir>& out_reqs /*OUT*/)
{
    const u_int n_erase_req = in_reqs.ie_end() - in_reqs.ie_begin();
    assert (n_erase_req <= 1);

    const u_int n_item2 = n_item_ - n_erase_req;                
    CowbassNode* p_node = this;
    u_int erase_pos = INT_MAX;
    Dir orig_dir = _Config::make_dir(p_node);
                
    if (n_item2 >= HALF_MAX_FANOUT) {
        // this node does not collapse
        if (n_erase_req) {
            const typename CowbassReqList<_Item>::Req& req = in_reqs[in_reqs.ie_begin()];
            erase_pos = req.pos_;
            p_node = p_node->erase_at (p_alloc, erase_pos);
        }

        for (u_int i=in_reqs.up_begin(); i<in_reqs.up_end(); ++i) {
            const typename CowbassReqList<_Item>::Req& req = in_reqs[i];
            p_node = p_node->update_at
                (p_alloc, req.pos_ - (req.pos_ > erase_pos), req.data_);
        }

        out_reqs.clear();
        Dir curr_dir = _Config::make_dir(p_node);
        if (curr_dir != orig_dir) {
            out_reqs.push_update (pi.pos_, curr_dir);
        }
        return true;
    }
                
    // n_item2 < HALF_MAX_FANOUT and n_erase_req >= 1
    const typename CowbassReqList<_Item>::Req& req = in_reqs[in_reqs.ie_begin()];
    erase_pos = req.pos_;

    const u_int neighbor_pos = choose_neighbor (pi);
    if (pi.pos_ == neighbor_pos) {
        ST_FATAL ("Fail to find neighbor");
        return false;
    }

    CowbassNode* p_neighbor =
        static_cast<CowbassNode*>(pi.p_parent_->a_item_[neighbor_pos].p_node_);
    Dir orig_neighbor_dir = _Config::make_dir(p_neighbor);
    u_int sum = p_node->n_item_ + p_neighbor->n_item_ - 1;
    bool LHS = (pi.pos_ < neighbor_pos);
    if (sum <= MAX_FANOUT) {
        // merge with neighbor
        CowbassNode* p_merged = NULL;
        int update_offset = 0;
        if (LHS) {
            if (p_node->local()) {
                array_erase (p_node->begin(), p_node->end(),
                             p_node->begin()+erase_pos);
                -- p_node->n_item_;
                array_copy_far (p_node->end(),
                                p_neighbor->begin(), p_neighbor->end());
                p_node->n_item_ += p_neighbor->n_item_;

                p_neighbor->dereference (p_alloc);
                p_merged = p_node;
            } else {  // p_node is shared
                p_neighbor = p_neighbor->move
                    (p_alloc, p_node->n_item_ - 1, 0, p_neighbor->n_item_);
                // p_neighbor is local now, and has correct n_item_
                array_nip_erase_far (p_neighbor->begin(),
                                     p_node->begin(),
                                     p_node->end(),
                                     p_node->begin()+erase_pos);

                p_node->dereference (p_alloc);
                p_merged = p_neighbor;
            }                            
        } else {  // RHS
            update_offset = p_neighbor->n_item_;
                        
            if (p_node->local()) {
                array_nip_erase (p_node->begin() + p_neighbor->n_item_,
                                 p_node->begin(),
                                 p_node->end(),
                                 p_node->begin() + erase_pos);
                array_copy_far (p_node->begin(),
                                p_neighbor->begin(), p_neighbor->end());
                p_node->n_item_ += p_neighbor->n_item_ - 1;
                            
                p_neighbor->dereference(p_alloc);
                p_merged = p_node;
            } else {  // p_node is shared
                // make local
                p_neighbor = p_neighbor->resize (p_alloc, p_neighbor->n_item_);
                array_nip_erase_far (p_neighbor->end(),
                                     p_node->begin(), p_node->end(),
                                     p_node->begin()+erase_pos);
                p_neighbor->n_item_ += p_node->n_item_ - 1;

                p_node->dereference(p_alloc);
                p_merged = p_neighbor;
            }
        }

        for (u_int i=in_reqs.up_begin(); i<in_reqs.up_end(); ++i) {
            const typename CowbassReqList<_Item>::Req& req = in_reqs[i];
            p_merged = p_merged->update_at
                (p_alloc,
                 update_offset + req.pos_ - (req.pos_ > erase_pos),
                 req.data_);
        }

        out_reqs.clear();
        if (p_merged == p_node) {
            out_reqs.push_erase (neighbor_pos);
            Dir curr_dir = _Config::make_dir(p_node);
            if (curr_dir != orig_dir) {
                out_reqs.push_update (pi.pos_, curr_dir);
            }
        } else {  // p_merged == p_neighbor
            out_reqs.push_erase (pi.pos_);
            Dir curr_neighbor_dir = _Config::make_dir(p_neighbor);
            if (curr_neighbor_dir != orig_neighbor_dir) {
                out_reqs.push_update (neighbor_pos, curr_neighbor_dir);
            }
        }

        return true;
    }

    // sum > MAX_FANOUT, average p_node and p_neighbor
    const int n_move = p_neighbor->n_item_ - (sum >> 1);
    int update_offset = 0;
    if (LHS) {
        // make local
        p_node = p_node->erase_at (p_alloc, erase_pos); 
        array_copy_far(p_node->end(),
                       p_neighbor->begin(), p_neighbor->begin() + n_move);
        p_node->n_item_ += n_move;
        p_neighbor = p_neighbor->move(p_alloc, 0, n_move, p_neighbor->n_item_);
    } else{  // RHS
        update_offset = n_move;
        // make local
        p_node = p_node->resize (p_alloc, p_node->n_item_); 
        array_nip_erase (p_node->begin() + n_move,
                         p_node->begin(), p_node->end(),
                         p_node->begin()+erase_pos);
        p_node->n_item_ += n_move - 1;
        array_copy_far (p_node->begin(),
                        p_neighbor->end()-n_move, p_neighbor->end());
                    
        p_neighbor = p_neighbor->resize (p_alloc,
                                         p_neighbor->n_item_ - n_move);
    }

    for (u_int i=in_reqs.up_begin(); i<in_reqs.up_end(); ++i) {
        const typename CowbassReqList<_Item>::Req& req = in_reqs[i];
        p_node = p_node->update_at
            (p_alloc,
             update_offset + req.pos_ - (req.pos_ > erase_pos),
             req.data_);
    }

    out_reqs.clear();
    Dir curr_dir = _Config::make_dir(p_node);
    if (curr_dir != orig_dir) {
        out_reqs.push_update (pi.pos_, curr_dir);
    }
    Dir curr_neighbor_dir = _Config::make_dir(p_neighbor);
    if (curr_neighbor_dir != orig_neighbor_dir) {
        out_reqs.push_update (neighbor_pos, curr_neighbor_dir);
    }
                 
    return true;
}

template <typename _Item, class _GetKey, u_int _MAX_FANOUT, class _Cmp>
int Cowbass4<_Item, _GetKey, _MAX_FANOUT, _Cmp>::init ()
{
    C_ASSERT(sizeof(Cowbass4) == 48, bad_size);

    if (!not_init ()) {
        ST_FATAL ("This Cowbass4 is already initialized!");
        return -1;
    }
            
    p_root_ = NULL;
    alloc_creator_ = 1;
    n_all_ = 0;

    sp_leaf_alloc_.reset(ST_NEW (LeafAlloc, sizeof(Leaf)));
    if (NULL == sp_leaf_alloc_.get()) {
        ST_FATAL ("Fail to new sp_leaf_alloc_");
        return ENOMEM;
    }

    sp_branch_alloc_.reset(ST_NEW (BranchAlloc, sizeof(Branch)));
    if (NULL == sp_branch_alloc_.get()) {
        ST_FATAL ("Fail to new sp_branch_alloc_");
        return ENOMEM;
    }

    return 0;
}

template <typename _Item, class _GetKey, u_int _MAX_FANOUT, class _Cmp>
Cowbass4<_Item, _GetKey, _MAX_FANOUT, _Cmp>::
Cowbass4 (const Cowbass4<_Item, _GetKey, _MAX_FANOUT, _Cmp>& rhs)
{            
    if (rhs.not_init ()) {
        ST_FATAL ("source is not initialized");
        // and we keep ourself uninitialized
        return;
    }

    sp_leaf_alloc_ = rhs.sp_leaf_alloc_;
    sp_branch_alloc_ = rhs.sp_branch_alloc_;

    p_root_ = rhs.p_root_;
    alloc_creator_ = 0;
    n_all_ = rhs.n_all_;
                
    if (p_root_) {
        mod_nodes (reference_leaf(), reference_branch());
    }                
}

template <typename _Item, class _GetKey, u_int _MAX_FANOUT, class _Cmp>
Cowbass4<_Item, _GetKey, _MAX_FANOUT, _Cmp>&
Cowbass4<_Item, _GetKey, _MAX_FANOUT, _Cmp>::
operator= (const Cowbass4<_Item, _GetKey, _MAX_FANOUT, _Cmp>& rhs)
{
    if (&rhs == this) {  // do nothing when assigning to self
        return *this;
    }
            
    if (not_init ()) {  // just call copy-constructor
        ST_NEW_ON (this, Cowbass4, rhs);
        return *this;
    }

    if (rhs.not_init ()) {  // source is not initialized 
        this->~Cowbass4 ();  // destroy self
        return *this;
    }

    if (sp_leaf_alloc_ == rhs.sp_leaf_alloc_) {
        // shared allocators
        assert (sp_branch_alloc_ == rhs.sp_branch_alloc_);

        clear ();

        p_root_ = rhs.p_root_;
        n_all_ = rhs.n_all_;
                
        if (p_root_) {
            mod_nodes (reference_leaf(), reference_branch());
        }
    }
    else {  // separate allocators
        assert (sp_branch_alloc_ != rhs.sp_branch_alloc_);

        clear ();

        for (ConstIterator it=rhs.begin(), it_e=rhs.end();
             it != it_e; ++ it) {
            insert (*it);
        }
    }
    return *this;
}

template <typename _Item, class _GetKey, u_int _MAX_FANOUT, class _Cmp>
typename Cowbass4<_Item, _GetKey, _MAX_FANOUT, _Cmp>::Leaf*
Cowbass4<_Item, _GetKey, _MAX_FANOUT, _Cmp>::
seek_leaf (const Key& key, int* p_pos,
           ParentInfo* a_parent, int* pn_parent) const
{
    BaseCowbassNode* p_node = p_root_;
    int n_parent = 0;
    *pn_parent = 0;
    while (COWBASS_BRANCH == p_node->type_) {
        Branch* p_br = static_cast<Branch*>(p_node);
        int pos = array_binary_search<compare_key_and_dir>(
            key, p_br->a_item_+1, p_br->n_item_-1, MAX_FANOUT);
        //notice the +1 and -1
        if (pos >= 0) {
            ++ pos;
            // the item got matched, let's quit here
            p_node = p_br->a_item_[pos].p_node_;
            a_parent[n_parent].p_parent_ = p_br;
            a_parent[n_parent].pos_ = pos;
            ++ n_parent;  // don't forget
            ++ *pn_parent;
                    
            // the item matched first child/item of this node
            while (COWBASS_BRANCH == p_node->type_) {
                p_br = static_cast<Branch*>(p_node);
                p_node = p_br->a_item_[0].p_node_;

                a_parent[n_parent].p_parent_ = p_br;
                a_parent[n_parent].pos_ = 0;
                ++ n_parent;  // don't forget
                ++ *pn_parent;                        
            }
            // p_node is COWBASS_LEAF now
            *p_pos = 0;
            // number of items does not change
            // key does not change so we don't update keys in parents
            return static_cast<Leaf*>(p_node);
        }

        // the item does not match any (first) elements,
        // ~pos representing that the item is between first[pos-1]
        // and first[pos], and we should go further into child[pos]
        pos = ~pos;
                
        // push parent info, we need this to go up later
        a_parent[n_parent].p_parent_ = p_br;
        a_parent[n_parent].pos_ = pos;
        ++ n_parent;  // don't forget
        ++ *pn_parent;

        // go down
        p_node = p_br->a_item_[pos].p_node_;
    }

    // the item did not match any key, so p_node must be COWBASS_LEAF now
    Leaf* p_leaf = static_cast<Leaf*>(p_node);
    int pos = array_binary_search<compare_key_and_item>
        (key, p_leaf->a_item_, p_leaf->n_item_, MAX_FANOUT);
    *p_pos = pos;
    return p_leaf;
}


template <typename _Item, class _GetKey, u_int _MAX_FANOUT, class _Cmp>
_Item*
Cowbass4<_Item, _GetKey, _MAX_FANOUT, _Cmp>::
insert (const _Item& item)
{
    _GetKey get_key_;
            
    if (NULL == p_root_) {
        Leaf* p_leaf = Leaf::create(sp_leaf_alloc_.get());
        p_leaf->a_item_[0] = item;
        p_leaf->n_item_ = 1;
        n_all_ = 1;

        p_root_ = p_leaf;

        return p_leaf->a_item_;
    }
                
    ParentInfo a_parent_[MAX_N_PARENT];
    int n_parent = 0;
    int pos = -1;
    Leaf* p_leaf = seek_leaf(get_key_(item), &pos, a_parent_, &n_parent);
    if (pos >= 0) {  // matched
        Leaf* p_leaf2 = p_leaf->update_at (sp_leaf_alloc_.get(), pos, item);
        if (p_leaf == p_leaf2) {
            // number of items does not change
            // key does not change so we don't update keys in parents
            return p_leaf2->a_item_ + pos;
        }

        // this node is referenced by other as well, we have to copy
        Dir dir(get_key_(p_leaf2->a_item_[0]), p_leaf2);
        for (int d=n_parent-1; d>=0; --d) {
            ParentInfo& pi = a_parent_[d];
            Branch* p_parent2 = pi.p_parent_->update_at
                (sp_branch_alloc_.get(), pi.pos_, dir);
            if (p_parent2 == pi.p_parent_) {  // no split any more
                return p_leaf2->a_item_ + pos;
            }

            dir.p_node_ = p_parent2;
            dir.first_ = p_parent2->a_item_[0].first_;
        }
        // dir.p_node_ is the new root
        p_root_ = dir.p_node_;
        return p_leaf2->a_item_ + pos;
    }
            
    // key of the new item does not match any existing key,
    // we have to insert it before ~pos
    pos = ~pos;

    CowbassReqList<_Item> in_reqs;
    in_reqs.push_insert (pos, item);
    CowbassReqList<Dir> out_reqs;
    const int prnt_pos = (n_parent > 0) ? a_parent_[n_parent-1].pos_ : 0;
    _Item* p_store = p_leaf->insert
        (sp_leaf_alloc_.get(), prnt_pos, in_reqs, out_reqs);
    if (NULL == p_store) {
        ST_FATAL ("Fail to insert into p_leaf");
        return NULL;
    }
    ++ n_all_;
            
    if (out_reqs.empty()) {  // no mod requests for parents
        return p_store;
    }
            
    // go up and change parents
    for (int d=n_parent-1; d>=0; --d) {
        const int gprnt_pos = (d >= 1) ? a_parent_[d-1].pos_ : 0;
        ParentInfo& pi = a_parent_[d];
        // we have to split this parent as well
        pi.p_parent_->insert
            (sp_branch_alloc_.get(), gprnt_pos, out_reqs, out_reqs);
        if (out_reqs.empty()) {  // no mod requests for parents
            return p_store;
        }
    }  // for

    // out_reqs is not empty
    const u_int n_insert = out_reqs.ie_end() - out_reqs.ie_begin();
    if (n_insert) {
        // a new root is required
        Branch* p_new_root = Branch::create(sp_branch_alloc_.get());
        p_new_root->n_item_ = 2;
        if (COWBASS_LEAF == p_root_->type_) {
            p_new_root->a_item_[0] =
                LeafConfig::make_dir(static_cast<Leaf*>(p_root_));
        } else {  // COWBASS_BRANCH == p_root_->type_
            p_new_root->a_item_[0] =
                BranchConfig::make_dir(static_cast<Branch*>(p_root_));
        }
        p_new_root->a_item_[1] = out_reqs[out_reqs.ie_begin()].data_;
                
        for (u_int i=out_reqs.up_begin(); i<out_reqs.up_end(); ++i) {
            const typename CowbassReqList<Dir>::Req& req = out_reqs[i];
            assert (0 == req.pos_);
            p_new_root = p_new_root->update_at
                (sp_branch_alloc_.get(), 0, req.data_);
        }
                
        p_root_ = p_new_root;
    } else {  // no insert
        if (!out_reqs.empty()) {
            BaseCowbassNode* p_root_candi = out_reqs[0].data_.p_node_;
            if (p_root_candi != p_root_) {
                p_root_ = p_root_candi;
            }
        }                
    }

    return p_store;
}

template <typename _Item, class _GetKey, u_int _MAX_FANOUT, class _Cmp>
bool
Cowbass4<_Item, _GetKey, _MAX_FANOUT, _Cmp>::
erase (const Key& key)
{
    if (unlikely(NULL == p_root_)) {
        return false;
    }

    ParentInfo a_parent_[MAX_N_PARENT];
    int n_parent = 0;
    int pos = -1;
    Leaf* p_leaf = seek_leaf(key, &pos, a_parent_, &n_parent);

    // cout << "n_parent=" << n_parent;
    // for (int i=0; i<n_parent; ++i) {
    //     cout << " " << a_parent_[i].pos;
    // }
    // cout << endl;
            
    if (pos < 0) {  // did not match any item
        return false;
    }
                
    if (0 == n_parent) {
        if (1 == n_all_) {
            p_leaf->dereference(sp_leaf_alloc_.get());
            p_root_ = NULL;
            n_all_ = 0;
        } else {
            p_root_ = p_leaf->erase_at (sp_leaf_alloc_.get(), pos);
            -- n_all_;
        }
        return true;
    }

    // n_parent >= 1, root must be COWBASS_BRANCH
    CowbassReqList<_Item> in_reqs;
    in_reqs.push_erase (pos);
    CowbassReqList<Dir> out_reqs;
    p_leaf->erase
        (sp_leaf_alloc_.get(), a_parent_[n_parent-1], in_reqs, out_reqs);
    -- n_all_;

    if (out_reqs.empty()) {  // no mod requests for parent node
        return true;
    }

    if (n_parent >= 2) {
        for (int d=n_parent-1; d>=1; --d) {
            const ParentInfo& gpi = a_parent_[d-1];
            ParentInfo& pi = a_parent_[d];
            pi.p_parent_->erase
                (sp_branch_alloc_.get(), gpi, out_reqs, out_reqs);

            if (out_reqs.empty()) {
                return true;
            }
        }
    }

    const u_int n_erase = out_reqs.ie_end() - out_reqs.ie_begin();
    u_int erase_pos = INT_MAX;
    Branch* p_root_br = static_cast<Branch*>(p_root_); 
    if (n_erase) {
        const typename CowbassReqList<Dir>::Req& req = out_reqs[out_reqs.ie_begin()];
        erase_pos = req.pos_;
        p_root_br = p_root_br->erase_at (sp_branch_alloc_.get(), req.pos_);
    }
            
    for (u_int i=out_reqs.up_begin(); i<out_reqs.up_end(); ++i) {
        const typename CowbassReqList<Dir>::Req& req = out_reqs[i];
        p_root_br = p_root_br->update_at
            (sp_branch_alloc_.get(),
             req.pos_ - (req.pos_ > erase_pos), req.data_);
    }

    // leverage child if necessary
    if (1 == p_root_br->n_item_) {
        p_root_ = p_root_br->a_item_[0].p_node_;
        p_root_br->dereference (sp_branch_alloc_.get());
    } else {
        p_root_ = p_root_br;
    }
            
    return true;
}

template <typename _Item, class _GetKey, u_int _MAX_FANOUT, class _Cmp>
const _Item*
Cowbass4<_Item, _GetKey, _MAX_FANOUT, _Cmp>::
seek (const Key& key) const
{
    if (unlikely(NULL == p_root_)) {
        return NULL;
    }

    const BaseCowbassNode* p_node = p_root_;
    while (COWBASS_BRANCH == p_node->type_) {
        const Branch* p_br = static_cast<const Branch*>(p_node);
        // binary search in [key1..keyN]
        const int pos = array_binary_search<compare_key_and_dir>
            (key, p_br->a_item_+1, p_br->n_item_-1, MAX_FANOUT);
        if (pos >= 0) {
            p_node = p_br->a_item_[pos+1].p_node_;
            while (COWBASS_BRANCH == p_node->type_) {
                p_node = static_cast<const Branch*>(p_node)->a_item_[0].p_node_;
            }
            return static_cast<const Leaf*>(p_node)->a_item_;
        }
                
        p_node = p_br->a_item_[~pos].p_node_;
    }

    const Leaf* p_leaf = static_cast<const Leaf*>(p_node);
    const int pos = array_binary_search<compare_key_and_item>
        (key, p_leaf->a_item_, p_leaf->n_item_, MAX_FANOUT);
    return (pos >= 0) ? (p_leaf->a_item_ + pos) : NULL;
}

template <typename _Item, class _GetKey, u_int _MAX_FANOUT, class _Cmp>
template <class _LeafMod, class _BranchMod>
void
Cowbass4<_Item, _GetKey, _MAX_FANOUT, _Cmp>::
mod_nodes (const _LeafMod& leaf_mod, const _BranchMod& br_mod)
{
    if (NULL == p_root_)
        return;

    ParentInfo a_parent_[MAX_N_PARENT];
    u_int n_parent_ = 0;
    
    BaseCowbassNode* p_node = p_root_;
    while (COWBASS_BRANCH == p_node->type_) {
        a_parent_[n_parent_].p_parent_ = static_cast<Branch*>(p_node);
        a_parent_[n_parent_].pos_ = 0;
        ++ n_parent_;
        p_node = static_cast<Branch*>(p_node)->a_item_[0].p_node_;
    }

    leaf_mod(static_cast<Leaf*>(p_node));

    while (n_parent_ > 0) {
        ParentInfo& pi = a_parent_[n_parent_-1];
        ++ pi.pos_;
        Branch* p_br = pi.p_parent_;
        if (pi.pos_ < p_br->n_item_) {
            BaseCowbassNode* p_node2 = p_br->a_item_[pi.pos_].p_node_;
            while (COWBASS_BRANCH == p_node2->type_) {
                a_parent_[n_parent_].p_parent_ = static_cast<Branch*>(p_node2);
                a_parent_[n_parent_].pos_ = 0;
                ++ n_parent_;
                //cout << " " << n_parent_ << endl;
                p_node2 = static_cast<Branch*>(p_node2)->a_item_[0].p_node_;
            }
            leaf_mod(static_cast<Leaf*>(p_node2));
        } else {
            br_mod (p_br);
            -- n_parent_;
        }
    }
}

// Print type of Cowbass4
template <typename _Item, class _GetKey, u_int _MAX_FANOUT, class _Cmp>
struct c_show_impl<Cowbass4<_Item, _GetKey, _MAX_FANOUT, _Cmp> > {
    static void c_to_string (std::ostream& os)
    {
        os << "Cowbass4{" << c_show(TCAP(ReturnType, _GetKey, _Item))
           << "->" << _MAX_FANOUT << '*' << sizeof(_Item)
           << ':' << c_show(_Item)
           << "}";
    }
};


}  // namespace st

#endif // end _ST2

#endif  //_COWBASS4_HPP_
