// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// A reference couting B+ tree
// Author: jiangrujie@baidu.com
// Date: Fri July 1 11:52:37 CST 2011

#include "base_cowbass4.h"
#include <iostream>
using namespace std;

namespace st {
    
struct CowbassReqList {
    static const u_int MAX_N_REQ = 2;  // maximum length

    struct Req {
        u_int pos_;
        const void* data_;
    };

    
    CowbassReqList () : n_ie_(0), n_update_(0) {}

    void push_insert (int pos, const void* d)
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

    void push_update (int pos, const void* d)
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
    
template <class _Config>
CowbassNode<_Config>*
CowbassNode<_Config>::resize (Alloc* p_alloc, int n, CopyFn copy_fn)
{
    if (local()) {
        n_item_ = n;
        return this;
    } else {  // shared
        CowbassNode* p_gn = CowbassNode::create (p_alloc, item_size_);
        array_copy_far (p_gn->begin(), begin(), 
                        begin() + n * item_size_, 
                        item_size_, copy_fn);
        p_gn->n_item_ = n;
        this->dereference(p_alloc);
        return p_gn;
    }
}

template <class _Config>
CowbassNode<_Config>*
CowbassNode<_Config>::
insert_before (Alloc* p_alloc, const int pos, const void* item, CopyFn copy_fn)
{
    if (local()) {
        array_insert (begin(), end(), begin() + pos * item_size_, item, 
                      item_size_, copy_fn);
        ++ n_item_;
        return this;
    } else {  // shared
        CowbassNode* p_gn = CowbassNode::create(p_alloc, item_size_);
        array_nip_insert_far (p_gn->begin(), 
                              begin(), end(), begin() + pos * item_size_, 
                              item, item_size_, copy_fn);
        p_gn->n_item_ = n_item_ + 1;
        this->dereference(p_alloc);
        return p_gn;
    }
}

template <class _Config>
CowbassNode<_Config>*
CowbassNode<_Config>::
erase_at (Alloc* p_alloc, const int pos, CopyFn copy_fn)
{
    if (local()) {
        array_erase (begin(), end(), begin() + pos * item_size_, 
                     item_size_, copy_fn);
        -- n_item_;
        return this;
    } else {  // shared
        CowbassNode* p_gn = CowbassNode::create(p_alloc, item_size_);
        array_nip_erase_far (p_gn->begin(), begin(), end(), 
                             begin() + pos * item_size_, 
                             item_size_, copy_fn);
        p_gn->n_item_ = n_item_ - 1;
        this->dereference(p_alloc);
        return p_gn;
    }
}

template <class _Config>
CowbassNode<_Config>*
CowbassNode<_Config>::
update_at (Alloc* p_alloc, int pos, const void* item, CopyFn copy_fn)
{
    if (local()) {
        copy_fn(begin() + item_size_ * pos, item);
        return this;
    } else {  // shared
        CowbassNode* p_gn = create (p_alloc, item_size_);
        array_copy_far (p_gn->begin(), 
                        begin(), end(), 
                        item_size_, copy_fn);
        p_gn->n_item_ = n_item_;
        copy_fn(p_gn->begin() + item_size_ * pos, item);
        this->dereference (p_alloc);
        return p_gn;
    }
}

template <class _Config>
CowbassNode<_Config>*
CowbassNode<_Config>::
move (Alloc* p_alloc, const int d, const int b, const int e, CopyFn copy_fn)
{
    if (local()) {
        array_copy (begin() + d * item_size_, 
                    begin() + b * item_size_,
                    begin() + e * item_size_, 
                    item_size_, copy_fn);
        n_item_ = d + e - b;
        return this;
    } else {  // shared
        CowbassNode* p_node = create (p_alloc, item_size_);
        array_copy_far (p_node->begin() + d * item_size_, 
                        begin() + b * item_size_, 
                        begin() + e * item_size_, 
                        item_size_, copy_fn);
        p_node->n_item_ = d + e - b;
        this->dereference (p_alloc);
        return p_node;
    }
}


template <class _Config>
void* CowbassNode<_Config>::
insert (Alloc* p_alloc, const int prnt_pos, 
        void* dir_buf, u_int dir_size, u_int max_fanout,
        CopyFn copy_fn, CopyFn get_key_fn, EqFn dir_eq_fn,
        const CowbassReqList& in_reqs,
        CowbassReqList& out_reqs /*OUT*/)
{
    const u_int n_insert_req = in_reqs.ie_end() - in_reqs.ie_begin();
    assert (n_insert_req <= 1);

    const u_int n_item2 = n_insert_req + n_item_;
    u_int insert_pos = UINT_MAX;
    CowbassNode* p_node = this;
    CopyFn dir_fn = (this->type_ == COWBASS_LEAF? get_key_fn: copy_fn);
    Dir* p_orig_dir = _Config::make_dir (p_node, dir_buf, dir_fn);
    dir_buf = static_cast<char*>(dir_buf) + dir_size;
                
    if (n_item2 <= max_fanout) {
        // no need to split
        if (n_insert_req) {
            const CowbassReqList::Req& req =
                in_reqs[in_reqs.ie_begin()];
            insert_pos = req.pos_;
            p_node = p_node->insert_before
                (p_alloc, insert_pos, req.data_, copy_fn);
        }
        for (u_int i=in_reqs.up_begin(); i<in_reqs.up_end(); ++i) {
            const CowbassReqList::Req& req = in_reqs[i];
            p_node = p_node->update_at
                (p_alloc, req.pos_ + (req.pos_ > insert_pos), 
                 req.data_, copy_fn);
        }
                    
        out_reqs.clear();
        Dir* p_curr_dir = _Config::make_dir (p_node, dir_buf, dir_fn);
        dir_buf = static_cast<char*>(dir_buf) + dir_size;
        if (!dir_eq_fn (p_curr_dir, p_orig_dir)) {
            out_reqs.push_update (prnt_pos, p_curr_dir);
        }
        return p_node->begin() + insert_pos * item_size_;
    }

    // n_insert_req >= 1
    // (n_item2 > MAX_FANOUT)
    // need to split, we only support splitting into two leaves now
    CowbassNode* p_node2 = create (p_alloc, item_size_);
    const CowbassReqList::Req& req = in_reqs[0];
    insert_pos = req.pos_;
    const u_int HALF_MAX_FANOUT = ((max_fanout + 1) >> 1);
    void* p_store = NULL;
    
    if (req.pos_ < HALF_MAX_FANOUT) {
        // assignment to p_node2 MUST happen before changing old node
        array_copy_far (p_node2->begin(),
                        p_node->end() - HALF_MAX_FANOUT * item_size_, 
                        p_node->end(), item_size_, copy_fn);
        p_node2->n_item_ = HALF_MAX_FANOUT;

        // make local
        p_node = p_node->resize (p_alloc, 
                                 p_node->n_item_ - HALF_MAX_FANOUT, copy_fn);
        p_node = p_node->insert_before (p_alloc, insert_pos, req.data_, copy_fn);
        p_store = p_node->begin() + insert_pos * item_size_;
    } else {
        array_nip_insert_far (p_node2->begin(),
                              p_node->begin() + HALF_MAX_FANOUT * item_size_,
                              p_node->end(),
                              p_node->begin() + insert_pos * item_size_,
                              req.data_,
                              item_size_, copy_fn);
        p_node2->n_item_ = p_node->n_item_ - HALF_MAX_FANOUT + 1;
        p_store = p_node2->begin() + 
            (insert_pos - HALF_MAX_FANOUT) * item_size_;
                    
        p_node = p_node->resize (p_alloc, HALF_MAX_FANOUT, copy_fn);
    }

    for (u_int i=in_reqs.up_begin(); i<in_reqs.up_end(); ++i) {
        const CowbassReqList::Req& req2 = in_reqs[i];
        const u_int pos = req2.pos_ + (req2.pos_ > insert_pos);
        if (pos < p_node->n_item_) {
            p_node = p_node->update_at (p_alloc, pos, req2.data_, copy_fn);
        } else {
            p_node2 = p_node2->update_at (p_alloc, pos-p_node->n_item_, 
                                          req2.data_, copy_fn);
        }
    }

    out_reqs.clear();
    if (p_node2) {
        Dir* p_ins_dir = _Config::make_dir (p_node2, dir_buf, dir_fn);
        dir_buf = static_cast<char*>(dir_buf) + dir_size;
        out_reqs.push_insert (prnt_pos+1, p_ins_dir);
    }
    Dir* p_curr_dir = _Config::make_dir (p_node, dir_buf, dir_fn);
    dir_buf = static_cast<char*>(dir_buf) + dir_size;
    if (!dir_eq_fn (p_curr_dir, p_orig_dir)) {
        out_reqs.push_update (prnt_pos, p_curr_dir);
    }
    return p_store;
}


template <class _Config>
int CowbassNode<_Config>::choose_neighbor (const ParentInfo& pi)
{
    Branch* p_parent = pi.p_parent_;
    //static const int DOMINATE_SCORE = MAX_FANOUT << 2;
    int sum1 = INT_MAX;
    int sum2 = INT_MAX;
    u_int dir_size = p_parent->item_size_;
                
    if (pi.pos_ < p_parent->n_item_-1) {  // not the most right one
        Dir* p_dir = reinterpret_cast<Dir*>
            (p_parent->begin() + (pi.pos_ + 1) * dir_size);
        BaseCowbassNode* p_right_node = p_dir->p_node_;
        if (_Config::TYPE != p_right_node->type_) {
            ST_FATAL ("Bad structure");
            return pi.pos_;
        }
        sum1 = n_item_ + static_cast<CowbassNode*>(p_right_node)->n_item_;
        // sum1 -= Alloc::is_local(p_right_node)
        //     * (sum1 <= MAX_FANOUT+1) * DOMINATE_SCORE;
    }
                
    if (pi.pos_ > 0) {  // not the most left one
        Dir* p_dir = reinterpret_cast<Dir*>
            (p_parent->begin() + (pi.pos_ - 1) * dir_size);
        BaseCowbassNode* p_left_node = p_dir->p_node_;
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

template <class _Config>
bool CowbassNode<_Config>::
erase (Alloc* p_alloc, const ParentInfo& pi,
       void* dir_buf, u_int dir_size, u_int max_fanout,
       CopyFn copy_fn, CopyFn get_key_fn, EqFn dir_eq_fn,
       const CowbassReqList& in_reqs,
       CowbassReqList& out_reqs /*OUT*/)
{
    const u_int n_erase_req = in_reqs.ie_end() - in_reqs.ie_begin();
    assert (n_erase_req <= 1);

    const u_int n_item2 = n_item_ - n_erase_req;                
    CowbassNode* p_node = this;
    u_int erase_pos = INT_MAX;
    const u_int HALF_MAX_FANOUT = ((max_fanout + 1) >> 1);
    CopyFn dir_fn = (this->type_ == COWBASS_LEAF? get_key_fn: copy_fn);

    Dir* p_orig_dir = _Config::make_dir (p_node, dir_buf, dir_fn);
    dir_buf = static_cast<char*>(dir_buf) + dir_size;
                
    if (n_item2 >= HALF_MAX_FANOUT) {
        // this node does not collapse
        if (n_erase_req) {
            const CowbassReqList::Req& req = in_reqs[in_reqs.ie_begin()];
            erase_pos = req.pos_;
            p_node = p_node->erase_at (p_alloc, erase_pos, copy_fn);
        }

        for (u_int i=in_reqs.up_begin(); i<in_reqs.up_end(); ++i) {
            const CowbassReqList::Req& req = in_reqs[i];
            p_node = p_node->update_at
                (p_alloc, req.pos_ - (req.pos_ > erase_pos), req.data_, copy_fn);
        }

        out_reqs.clear();
        Dir* p_curr_dir = _Config::make_dir (p_node, dir_buf, dir_fn);
        dir_buf = static_cast<char*>(dir_buf) + dir_size;
        if (!dir_eq_fn (p_curr_dir, p_orig_dir)) {
            out_reqs.push_update (pi.pos_, p_curr_dir);
        }
        return true;
    }
                
    // n_item2 < HALF_MAX_FANOUT and n_erase_req >= 1
    const CowbassReqList::Req& req = in_reqs[in_reqs.ie_begin()];
    erase_pos = req.pos_;

    const u_int neighbor_pos = choose_neighbor (pi);
    if (pi.pos_ == neighbor_pos) {
        ST_FATAL ("Fail to find neighbor");
        return false;
    }

    Dir* p_dir = reinterpret_cast<Dir*>
        (pi.p_parent_->begin() + neighbor_pos * dir_size);
    CowbassNode* p_neighbor = static_cast<CowbassNode*>(p_dir->p_node_);
    
    Dir* p_orig_neighbor_dir = _Config::make_dir (p_neighbor, dir_buf, dir_fn);
    dir_buf = static_cast<char*>(dir_buf) + dir_size;
    
    u_int sum = p_node->n_item_ + p_neighbor->n_item_ - 1;
    bool LHS = (pi.pos_ < neighbor_pos);
    
    if (sum <= max_fanout) {
        // merge with neighbor
        CowbassNode* p_merged = NULL;
        int update_offset = 0;
        if (LHS) {
            if (p_node->local()) {
                array_erase (p_node->begin(), p_node->end(),
                             p_node->begin() + erase_pos * item_size_,
                             item_size_, copy_fn);
                -- p_node->n_item_;
                array_copy_far (end(), p_neighbor->begin(), 
                                p_neighbor->end(),
                                item_size_, copy_fn);
                p_node->n_item_ += p_neighbor->n_item_;

                p_neighbor->dereference (p_alloc);
                p_merged = p_node;
            } else {  // p_node is shared
                p_neighbor = p_neighbor->move
                    (p_alloc, p_node->n_item_ - 1, 0, p_neighbor->n_item_, copy_fn);
                // p_neighbor is local now, and has correct n_item_
                array_nip_erase_far (p_neighbor->begin(),
                                     p_node->begin(), p_node->end(),
                                     p_node->begin() + erase_pos * item_size_,
                                     item_size_, copy_fn);

                p_node->dereference (p_alloc);
                p_merged = p_neighbor;
            }                            
        } else {  // RHS
            update_offset = p_neighbor->n_item_;
                        
            if (p_node->local()) {
                array_nip_erase (p_node->begin() + p_neighbor->n_item_ * item_size_,
                                 p_node->begin(), p_node->end(),
                                 p_node->begin() + erase_pos * item_size_,
                                 item_size_, copy_fn);
                array_copy_far (p_node->begin(),
                                p_neighbor->begin(), 
                                p_neighbor->end(),
                                item_size_, copy_fn);
                p_node->n_item_ += p_neighbor->n_item_ - 1;
                            
                p_neighbor->dereference (p_alloc);
                p_merged = p_node;
            } else {  // p_node is shared
                // make local
                p_neighbor = p_neighbor->resize (p_alloc, p_neighbor->n_item_, copy_fn);
                array_nip_erase_far (p_neighbor->end(),
                                     p_node->begin(), p_node->end(),
                                     p_node->begin() + erase_pos * item_size_,
                                     item_size_, copy_fn);
                p_neighbor->n_item_ += p_node->n_item_ - 1;

                p_node->dereference (p_alloc);
                p_merged = p_neighbor;
            }
        }

        for (u_int i=in_reqs.up_begin(); i<in_reqs.up_end(); ++i) {
            const CowbassReqList::Req& req = in_reqs[i];
            p_merged = p_merged->update_at
                (p_alloc,
                 update_offset + req.pos_ - (req.pos_ > erase_pos),
                 req.data_, copy_fn);
        }

        out_reqs.clear();
        if (p_merged == p_node) {
            out_reqs.push_erase (neighbor_pos);
            Dir* p_curr_dir = _Config::make_dir (p_node, dir_buf, dir_fn);
            dir_buf = static_cast<char*>(dir_buf) + dir_size;
            if (!dir_eq_fn (p_curr_dir, p_orig_dir)) {
                out_reqs.push_update (pi.pos_, p_curr_dir);
            }
        } else {  // p_merged == p_neighbor
            out_reqs.push_erase (pi.pos_);
            Dir* p_curr_neighbor_dir = _Config::
                make_dir (p_neighbor, dir_buf, dir_fn);
            dir_buf = static_cast<char*>(dir_buf) + dir_size;
            if (!dir_eq_fn (p_curr_neighbor_dir, p_orig_neighbor_dir)) {
                out_reqs.push_update (neighbor_pos, p_curr_neighbor_dir);
            }
        }

        return true;
    }

    // sum > MAX_FANOUT, average p_node and p_neighbor
    const int n_move = p_neighbor->n_item_ - (sum >> 1);
    int update_offset = 0;
    if (LHS) {
        // make local
        p_node = p_node->erase_at (p_alloc, erase_pos, copy_fn); 
        array_copy_far(p_node->end(), p_neighbor->begin(), 
                       p_neighbor->begin() + n_move * item_size_,
                       item_size_, copy_fn);
        p_node->n_item_ += n_move;
        p_neighbor = p_neighbor->move (p_alloc, 0, n_move, 
                                       p_neighbor->n_item_, copy_fn);
    } else{  // RHS
        update_offset = n_move;
        // make local
        p_node = p_node->resize (p_alloc, p_node->n_item_, copy_fn); 
        
        array_nip_erase (p_node->begin() + n_move * item_size_,
                         p_node->begin(), p_node->end(),
                         p_node->begin() + erase_pos * item_size_,
                         item_size_, copy_fn);
        p_node->n_item_ += n_move - 1;
        array_copy_far (p_node->begin(), 
                        p_neighbor->end() - n_move * item_size_, 
                        p_neighbor->end(), item_size_, copy_fn);
                    
        p_neighbor = p_neighbor->resize (p_alloc,
                                         p_neighbor->n_item_ - n_move,
                                         copy_fn);
    }

    for (u_int i=in_reqs.up_begin(); i<in_reqs.up_end(); ++i) {
        const CowbassReqList::Req& req = in_reqs[i];
        p_node = p_node->update_at
            (p_alloc,
             update_offset + req.pos_ - (req.pos_ > erase_pos),
             req.data_, copy_fn);
    }

    out_reqs.clear();
    Dir* p_curr_dir = _Config::make_dir (p_node, dir_buf, dir_fn);
    dir_buf = static_cast<char*>(dir_buf) + dir_size;
    if (!dir_eq_fn (p_curr_dir, p_orig_dir)) {
        out_reqs.push_update (pi.pos_, p_curr_dir);
    }
    Dir* p_curr_neighbor_dir = _Config::make_dir (p_neighbor, dir_buf, dir_fn);
    dir_buf = static_cast<char*>(dir_buf) + dir_size;
    if (!dir_eq_fn (p_curr_neighbor_dir, p_orig_neighbor_dir)) {
        out_reqs.push_update (neighbor_pos, p_curr_neighbor_dir);
    }
                 
    return true;
}


BaseCowbass4::BaseIterator::BaseIterator() 
    : p_item_(NULL)
    , p_item_end_(NULL)
    , item_size_(0)
    , dir_size_(0)
    {}

BaseCowbass4::BaseIterator::BaseIterator (BaseCowbassNode* p_root, 
                                     u_int item_size, u_int dir_size)
{
    if (NULL == p_root) {
        p_item_ = NULL;
    } else {
        n_parent_ = 0;
        item_size_ = item_size;
        dir_size_ = dir_size;

        while (COWBASS_BRANCH == p_root->type_) {
            Branch* p_br = static_cast<Branch*>(p_root);
            a_parent_[n_parent_].p_parent_ = p_br;
            a_parent_[n_parent_].pos_ = 0;
            ++ n_parent_;
            p_root = reinterpret_cast<Dir*>(p_br->begin())->p_node_;
        }
        const Leaf* p_leaf = static_cast<const Leaf*>(p_root);
        p_item_ = p_leaf->begin();
        p_item_end_ = p_leaf->begin() + 
            p_leaf->n_item_ * item_size_;
    }
}

BaseCowbass4::BaseCowbass4()
    : p_root_(NULL)
    , alloc_creator_(0)
    , n_all_(0)
    , item_size_(0)
    , dir_size_(0)
    , key_size_(0)
    , max_fanout_(0)
    , max_n_parent_(0)
    , item_copy_fn_(NULL)
    , dir_copy_fn_(NULL)
    , get_key_fn_(NULL)
    , dir_eq_fn_(NULL)
    , arr_search_dir_fn_(NULL) 
    , arr_search_item_fn_(NULL) 
{}
    
int BaseCowbass4::
init (u_int item_size, u_int key_size, u_int max_fanout,
      CopyFn item_copy_fn, CopyFn dir_copy_fn, 
      CopyFn get_key_fn, EqFn dir_eq_fn, 
      ArraySearchFn arr_search_dir_fn, 
      ArraySearchFn arr_search_item_fn)
{
    if (!not_init ()) {
        ST_FATAL ("This BaseCowbass4 is already initialized!");
        return ECONFLICT;
    }
            
    p_root_ = NULL;
    alloc_creator_ = 1;
    n_all_ = 0;

    item_size_ = item_size;
    key_size_ = key_size;
    dir_size_ = sizeof (Dir) + key_size;
    max_fanout_ = (max_fanout >> 1) << 1;

    // bit_num = ceiling (log2(max_fanout));
    u_int bit_num = 1;
    while ((max_fanout - 1) >> bit_num) {
        bit_num++;
    }
    // Maximum depth, We may store max_fanout_ ^ max_n_parent_ = 2^30
    // (1 billion) in this cowbass
    max_n_parent_ = 30 / bit_num;        

    item_copy_fn_ = item_copy_fn;
    dir_copy_fn_ = dir_copy_fn;
    get_key_fn_ = get_key_fn;
    dir_eq_fn_ = dir_eq_fn;
    arr_search_dir_fn_ = arr_search_dir_fn; 
    arr_search_item_fn_ = arr_search_item_fn;
    
    sp_leaf_alloc_.reset
        (ST_NEW (LeafAlloc, 
                 sizeof (Leaf) + item_size_ * max_fanout_));
    if (NULL == sp_leaf_alloc_.get()) {
        ST_FATAL ("Fail to new sp_leaf_alloc_");
        return ENOMEM;
    }

    sp_branch_alloc_.reset
        (ST_NEW (BranchAlloc, 
                 sizeof (Branch) + dir_size_ * max_fanout_));
    if (NULL == sp_branch_alloc_.get()) {
        ST_FATAL ("Fail to new sp_branch_alloc_");
        return ENOMEM;
    }

    return 0;
}

BaseCowbass4::BaseCowbass4 (const BaseCowbass4& rhs)
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
                
    item_size_ = rhs.item_size_;
    key_size_ = rhs.key_size_;
    dir_size_ = rhs.dir_size_;
    max_fanout_ = rhs.max_fanout_;
    max_n_parent_ = rhs.max_n_parent_;

    item_copy_fn_ = rhs.item_copy_fn_;
    dir_copy_fn_ = rhs.dir_copy_fn_;
    get_key_fn_ = rhs.get_key_fn_;
    dir_eq_fn_ = rhs.dir_eq_fn_;
    arr_search_dir_fn_ = rhs.arr_search_dir_fn_; 
    arr_search_item_fn_ = rhs.arr_search_item_fn_;

    
    if (p_root_) {
        mod_nodes (reference_leaf(), reference_branch());
    }                
}

BaseCowbass4& BaseCowbass4::operator= (const BaseCowbass4& rhs)
{
    if (&rhs == this) {  // do nothing when assigning to self
        return *this;
    }
            
    if (not_init ()) {  // just call copy-constructor
        ST_NEW_ON (this, BaseCowbass4, rhs);
        return *this;
    }

    if (rhs.not_init ()) {  // source is not initialized 
        this->~BaseCowbass4 ();  // destroy self
        return *this;
    }

    if (sp_leaf_alloc_ == rhs.sp_leaf_alloc_) {
        // shared allocators
        assert (sp_branch_alloc_ == rhs.sp_branch_alloc_);

        clear ();

        p_root_ = rhs.p_root_;
        n_all_ = rhs.n_all_;
                
        item_size_ = rhs.item_size_;
        key_size_ = rhs.key_size_;
        dir_size_ = rhs.dir_size_;
        max_fanout_ = rhs.max_fanout_;
        max_n_parent_ = rhs.max_n_parent_;

        item_copy_fn_ = rhs.item_copy_fn_;
        dir_copy_fn_ = rhs.dir_copy_fn_;
        get_key_fn_ = rhs.get_key_fn_;
        dir_eq_fn_ = rhs.dir_eq_fn_;
        arr_search_dir_fn_ = rhs.arr_search_dir_fn_; 
        arr_search_item_fn_ = rhs.arr_search_item_fn_;

        if (p_root_) {
            mod_nodes (reference_leaf(), reference_branch());
        }
    }
    else {  // separate allocators
        assert (sp_branch_alloc_ != rhs.sp_branch_alloc_);

        clear ();

        for (BaseIterator it=rhs.begin(), it_e=rhs.end(); 
                it != it_e; ++ it) {
            insert (it.operator-> ());
        }
    }
    return *this;
}

BaseCowbass4::Leaf* BaseCowbass4::
seek_leaf (const void* key, int* p_pos,
           ParentInfo* a_parent, int* pn_parent) const
{
    BaseCowbassNode* p_node = p_root_;
    int n_parent = 0;
    *pn_parent = 0;
    Dir* p_dir = NULL;
    while (COWBASS_BRANCH == p_node->type_) {
        Branch* p_br = static_cast<Branch*>(p_node);
        int pos = arr_search_dir_fn_ (key, 
                                      p_br->begin() + dir_size_, 
                                      dir_size_,
                                      p_br->n_item_ - 1, 
                                      max_fanout_);
        //notice the +1 and -1
        if (pos >= 0) {
            ++ pos;
            // the item got matched, let's quit here
            p_dir = reinterpret_cast<Dir*>
                (p_br->begin() + pos * dir_size_);
            p_node = p_dir->p_node_;
            a_parent[n_parent].p_parent_ = p_br;
            a_parent[n_parent].pos_ = pos;
            ++ n_parent;  // don't forget
            ++ *pn_parent;
                    
            // the item matched first child/item of this node
            while (COWBASS_BRANCH == p_node->type_) {
                p_br = static_cast<Branch*>(p_node);
                p_dir = reinterpret_cast<Dir*>(p_br->begin());
                p_node = p_dir->p_node_;

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
        p_node = reinterpret_cast<Dir*>
            (p_br->begin() + pos * dir_size_)->p_node_;
    }

    // the item did not match any key, so p_node must be COWBASS_LEAF now
    Leaf* p_leaf = static_cast<Leaf*>(p_node);
    int pos = arr_search_item_fn_ (key, 
                                   p_leaf->begin(), 
                                   item_size_,
                                   p_leaf->n_item_, 
                                   max_fanout_);
    *p_pos = pos;
    return p_leaf;
}


void* BaseCowbass4::insert (const void* item)
{
    if (NULL == p_root_) {
        Leaf* p_leaf = Leaf::create(sp_leaf_alloc_.get(), 
                                    item_size_);
        item_copy_fn_ (p_leaf->begin(), item);
        p_leaf->n_item_ = 1;
        n_all_ = 1;

        p_root_ = p_leaf;

        return p_leaf->begin();
    }
                
    // VLA supported by gcc
    char key_buf[key_size_];
    char dir_buf[dir_size_ * MAX_N_DIR];
    ParentInfo a_parent_[max_n_parent_];
    int n_parent = 0;
    int pos = -1;
    
    get_key_fn_ (key_buf, item);
    Leaf* p_leaf = seek_leaf (key_buf, &pos, a_parent_, &n_parent);
    if (pos >= 0) {  // matched
        Leaf* p_leaf2 = p_leaf->update_at (sp_leaf_alloc_.get(), pos, 
                                           item, item_copy_fn_);
        if (p_leaf == p_leaf2) {
            // number of items does not change
            // key does not change so we don't update keys in parents
            return p_leaf2->begin() + pos * item_size_;
        }

        // this node is referenced by other as well, we have to copy
        Dir* p_dir = LeafConfig::make_dir (p_leaf2, dir_buf, get_key_fn_);
        for (int d=n_parent-1; d>=0; --d) {
            ParentInfo& pi = a_parent_[d];
            Branch* p_parent2 = pi.p_parent_->update_at
                (sp_branch_alloc_.get(), pi.pos_, p_dir, dir_copy_fn_);
            if (p_parent2 == pi.p_parent_) {  // no split any more
                return p_leaf2->begin() + pos * item_size_;
            }

            dir_copy_fn_ (p_dir, p_parent2->begin());
            p_dir->p_node_ = p_parent2;
        }
        // dir.p_node_ is the new root
        p_root_ = p_dir->p_node_;
        return p_leaf2->begin() + pos * item_size_;
    }
            
    // key of the new item does not match any existing key,
    // we have to insert it before ~pos
    pos = ~pos;

    CowbassReqList in_reqs;
    in_reqs.push_insert (pos, item);
    CowbassReqList out_reqs;
    const int prnt_pos = (n_parent > 0) ? a_parent_[n_parent-1].pos_ : 0;
    void* p_store = p_leaf->insert
        (sp_leaf_alloc_.get(), prnt_pos, dir_buf, dir_size_, max_fanout_,
         item_copy_fn_, get_key_fn_, dir_eq_fn_,
         in_reqs, out_reqs);
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
            (sp_branch_alloc_.get(), gprnt_pos, dir_buf, dir_size_, max_fanout_,
             dir_copy_fn_, get_key_fn_, dir_eq_fn_, 
             out_reqs, out_reqs);
        if (out_reqs.empty()) {  // no mod requests for parents
            return p_store;
        }
    }  // for

    // out_reqs is not empty
    const u_int n_insert = out_reqs.ie_end() - out_reqs.ie_begin();
    if (n_insert) {
        // a new root is required
        Branch* p_new_root = Branch::create (sp_branch_alloc_.get(),
                                             dir_size_);
        p_new_root->n_item_ = 2;
        if (COWBASS_LEAF == p_root_->type_) {
            LeafConfig::make_dir (static_cast<Leaf*>(p_root_), 
                                  p_new_root->begin(), 
                                  get_key_fn_);
        } else {  // COWBASS_BRANCH == p_root_->type_
            BranchConfig::make_dir (static_cast<Branch*>(p_root_),
                                    p_new_root->begin(),
                                    dir_copy_fn_);
        }
        
        dir_copy_fn_ (p_new_root->begin() + dir_size_, 
                      out_reqs[out_reqs.ie_begin()].data_);
                
        for (u_int i=out_reqs.up_begin(); i<out_reqs.up_end(); ++i) {
            const CowbassReqList::Req& req = out_reqs[i];
            assert (0 == req.pos_);
            p_new_root = p_new_root->update_at
                (sp_branch_alloc_.get(), 0, req.data_, dir_copy_fn_);
        }
                
        p_root_ = p_new_root;
    } else {  // no insert
        if (!out_reqs.empty()) {
            BaseCowbassNode* p_root_candi = static_cast<const Dir*>
                (out_reqs[0].data_)->p_node_;
            if (p_root_candi != p_root_) {
                p_root_ = p_root_candi;
            }
        }                
    }

    return p_store;
}

bool BaseCowbass4::erase (const void* key)
{
    if (unlikely(NULL == p_root_)) {
        return false;
    }

    // VLA supported by gcc
    char dir_buf[dir_size_ * MAX_N_DIR];
    ParentInfo a_parent_[max_n_parent_];
    int n_parent = 0;
    int pos = -1;
    Leaf* p_leaf = seek_leaf (key, &pos, a_parent_, &n_parent);

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
            p_leaf->dereference (sp_leaf_alloc_.get());
            p_root_ = NULL;
            n_all_ = 0;
        } else {
            p_root_ = p_leaf->erase_at (sp_leaf_alloc_.get(), pos, 
                                        item_copy_fn_);
            -- n_all_;
        }
        return true;
    }

    // n_parent >= 1, root must be COWBASS_BRANCH
    CowbassReqList in_reqs;
    in_reqs.push_erase (pos);
    CowbassReqList out_reqs;
    p_leaf->erase (sp_leaf_alloc_.get(), a_parent_[n_parent-1], 
                   dir_buf, dir_size_, max_fanout_,
                   item_copy_fn_, get_key_fn_, dir_eq_fn_,
                   in_reqs, out_reqs);
    -- n_all_;

    if (out_reqs.empty()) {  // no mod requests for parent node
        return true;
    }

    if (n_parent >= 2) {
        for (int d=n_parent-1; d>=1; --d) {
            const ParentInfo& gpi = a_parent_[d-1];
            ParentInfo& pi = a_parent_[d];
            pi.p_parent_->erase(sp_branch_alloc_.get(), gpi, 
                                dir_buf, dir_size_, max_fanout_,
                                dir_copy_fn_, get_key_fn_, dir_eq_fn_,
                                out_reqs, out_reqs);

            if (out_reqs.empty()) {
                return true;
            }
        }
    }

    const u_int n_erase = out_reqs.ie_end() - out_reqs.ie_begin();
    u_int erase_pos = INT_MAX;
    Branch* p_root_br = static_cast<Branch*>(p_root_); 
    if (n_erase) {
        const CowbassReqList::Req& req = out_reqs[out_reqs.ie_begin()];
        erase_pos = req.pos_;
        p_root_br = p_root_br->erase_at (sp_branch_alloc_.get(), req.pos_,
                                         dir_copy_fn_);
    }
            
    for (u_int i=out_reqs.up_begin(); i<out_reqs.up_end(); ++i) {
        const CowbassReqList::Req& req = out_reqs[i];
        p_root_br = p_root_br->update_at (sp_branch_alloc_.get(),
                                          req.pos_ - (req.pos_ > erase_pos), 
                                          req.data_,
                                          dir_copy_fn_);
    }

    // leverage child if necessary
    if (1 == p_root_br->n_item_) {
        p_root_ = reinterpret_cast<Dir*>(p_root_br->begin())->p_node_;
        p_root_br->dereference (sp_branch_alloc_.get());
    } else {
        p_root_ = p_root_br;
    }
            
    return true;
}


void BaseCowbass4::shows_with_indent (StringWriter& sw, const int indent,
                                      const Dir* dir, 
                                      PrntFn prnt_key_fn,
                                      PrntFn prnt_item_fn)
{
    for (int i=0; i<indent; ++i) { sw << ' '; }

    sw << "k=";
    prnt_key_fn (sw, dir->first_);
    sw << " ";
    shows_with_indent (sw, indent, dir->p_node_, 
            prnt_key_fn, prnt_item_fn);
}

void BaseCowbass4::shows_with_indent (StringWriter& sw, const int indent,
        const void* item, PrntFn prnt_fn)
{
    for (int i=0; i<indent; ++i) { sw << ' '; }

    prnt_fn (sw, item);
}

// call show_with_indent of concrete type
void BaseCowbass4::shows_with_indent (StringWriter& sw, int indent,
                                      const BaseCowbassNode* p_node,
                                      PrntFn prnt_key_fn,
                                      PrntFn prnt_item_fn)
{
    if (p_node != NULL) {
        switch (p_node->type_) {
            case COWBASS_LEAF:
                shows_node_with_indent (sw, indent, 
                        static_cast<const Leaf*>(p_node),
                        prnt_key_fn,
                        prnt_item_fn);
                break;
            case COWBASS_BRANCH:
                shows_node_with_indent (sw, indent, 
                        static_cast<const Branch*>(p_node),
                        prnt_key_fn,
                        prnt_item_fn);
                break;
        }
    }
}

template <class _Node>
void BaseCowbass4::shows_node_with_indent (StringWriter& sw, int indent,
                                           const _Node* p_node, 
                                           PrntFn prnt_key_fn,
                                           PrntFn prnt_item_fn)
{
    sw << ((COWBASS_LEAF==p_node->type_) ? "LEAF":"BR") 
        << "(nr=" << (_Node::Alloc::get_rc_data(p_node)) << "):{\n"; 
    if (COWBASS_LEAF == p_node->type_) {
        for (u_int i=0; i < p_node->n_item_; ++i) {
            shows_with_indent(sw, indent+2, 
                    p_node->begin() + i * p_node->item_size_,
                    prnt_item_fn);
            sw << "\n";
        }
    } else {
        for (u_int i=0; i < p_node->n_item_; ++i) {
            shows_with_indent(sw, indent+2, 
                    reinterpret_cast<const Dir*>
                    (p_node->begin() + i * p_node->item_size_),
                     prnt_key_fn, prnt_item_fn);
            sw << "\n";
        }
    }
    for (int j=0; j<indent;++j) {
        sw << ' ';
    }
    sw << "}";
}

// print important information
void BaseCowbass4::to_string (StringWriter& sw) const
{
    sw << "{leaf_alloc=" << sp_leaf_alloc_.use_count()
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



    template <class _LeafMod, class _BranchMod>
void BaseCowbass4::mod_nodes (const _LeafMod& leaf_mod, 
        const _BranchMod& br_mod)
{
    if (NULL == p_root_)
        return;

    ParentInfo a_parent_[max_n_parent_];
    u_int n_parent_ = 0;

    BaseCowbassNode* p_node = p_root_;
    while (COWBASS_BRANCH == p_node->type_) {
        Branch* p_br = static_cast<Branch*>(p_node);
        a_parent_[n_parent_].p_parent_ = p_br;
        a_parent_[n_parent_].pos_ = 0;
        ++ n_parent_;
        p_node = reinterpret_cast<Dir*>(p_br->begin())->p_node_;
    }

    leaf_mod(static_cast<Leaf*>(p_node));

    while (n_parent_ > 0) {
        ParentInfo& pi = a_parent_[n_parent_-1];
        ++ pi.pos_;
        Branch* p_br = pi.p_parent_;
        if (pi.pos_ < p_br->n_item_) {
            BaseCowbassNode* p_node2 = reinterpret_cast<Dir*>
                (p_br->begin() + pi.pos_ * dir_size_)->p_node_;
            while (COWBASS_BRANCH == p_node2->type_) {
                Branch* p_br2 = static_cast<Branch*>(p_node2);
                a_parent_[n_parent_].p_parent_ = p_br2;
                a_parent_[n_parent_].pos_ = 0;
                ++ n_parent_;
                //cout << " " << n_parent_ << endl;
                p_node2 = reinterpret_cast<Dir*>(p_br2->begin())->p_node_;
            }
            leaf_mod (static_cast<Leaf*>(p_node2));
        } else {
            br_mod (p_br);
            -- n_parent_;
        }
    }
}

}  // namespace st
