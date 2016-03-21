// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Manage versions of an object
// Author: gejun@baidu.com
// Date: Jan 3 11:23:11 CST 2011
#pragma once
#ifndef _VERSION_MANAGER_HPP_
#define _VERSION_MANAGER_HPP_

#include "common.h"

namespace st {
// smalltable tries to separate reading and writing to indexes,
// one method is to redirect reading and writing into different
// versions of an index. this kind of versioning system typically
// holds following characteristics:
//   1) one writable version, generally being latest, accessed
//      and modified by one thread;
//   2) one or more read-only versions, being accessed by multiple
//      threads
// This is formally called MVCC: Multiple Version Concurrency Control
// In our system, there's another important property:
//   3) life-time of read-only versions are limited.
// This is necessary because our resources are limited. We utilize this
// property in a simple way: limit the maximum number of co-existing
// versions of a value, if all version slots ran out, the versioning
// manager replaces an oldest version with the new version under the
// assumption that the oldest version is not accessed by any threads
// anymore and safe to delete.
// So choosing the maximum number is critical, while choosing less is
// dangerous and choosing more is waste. In our current applications, 2
// is enough. We may need more versions for operations that take a
// considerable time. Underlying data structures also play important
// roles here, currently Cow* series data structures exposes a great 
// possibility between versions to share and grants us the ability to 
// have more versions.
// Versioning on one value works as follows:
//   1) One thread creates a writable version and does modifications.
//      This version is not accessible for other threads in the duration.
//      If serious issue happens, the version is rolled back, typically
//      deleted, meanwhile existing data and other threads are not affected.
//   2) After all modifications are successfully done, the thread makes
//      the writable version read-only and accessible by other threads
//   3) Since life time of read-only versions are limited, other threads
//      process these versions round by round, each time before they start
//      processing, they update their pointers to latest read-only version.
//      This is an atomic and consistent change.
// Versioning on multiple values work as follows:
//   1) One thread creates writable versions for multiple values and
//      does modifications. These versions are not accessible for other
//      threads in the duration. If serious issue happens, these versions
//      are rolled back, meanwhile existing data and other threads are not
//      affected.
//   2) After all modifications are successfully done, the thread makes
//      writable versions of values read-only and packs them into a single
//      structure and makes the structure accessible by other threads.
//   3) Same with versioning on one value, each time before other threads
//      start processing, they update their local pointers to the latest
//      packed structure. This is an atomic and consistent change.

const u_int VM_NULL_VERSION = 0;          // start version of all versions
const u_int VM_MIN_NUM_OF_VERSIONS = 2;   // minimum number of versions

class BaseVersionManager {
    // value plus version number
    struct Node {
        u_int ver_;
        bool writable_;
        // In c++11, directly dereferencing flexible array memeber
        // will break strict-aliasing rules. This warning can be
        // avoided by dereferencing via a temporary variable
        char value_[];
    };

    typedef void (*ConstructFn) (void*);
    typedef void (*DestructFn) (void*);
    typedef void (*CopyFn) (void*, const void*);
public:
    // Default constructor and destructor
    BaseVersionManager ();
    ~BaseVersionManager ();
        
    // Construct a VersionManager, must be called before using
    // Params:
    //   max_ver_num   maximum number of versions in this manager
    // Returns: GOOD/E_NULL
    int init (u_int value_size,
              u_int max_ver_num,
              std::string desc,
              ConstructFn construct,
              DestructFn destruct,
              CopyFn copy);

    // Get a read-only version with biggest version number
    // Returns: position of the slot, -1 means not found
    int find_latest_read_only () const;
                
    // Create a new version
    // Returns: position of the version, -1 means error happened
    int create_version ();

    // Make a version read-only
    // Params:
    //   pos     position of the version (returned by `create_version')
    // Returns: GOOD/E_GENERAL
    int freeze_version (u_int pos);

    // Drop a version
    // Params/Returns: Inherit freeze_version
    int drop_version (u_int pos, bool forbid_dropping_readonly);
    
    int drop_oldest_read_only ();

    Node& at (u_int pos)
    { return *reinterpret_cast<Node*>
            (a_ver_ + ((sizeof(Node) + value_size_) * pos)); }

    const Node& at (u_int pos) const
    { return *reinterpret_cast<const Node*>
            (a_ver_ + ((sizeof(Node) + value_size_) * pos)); }
    
    // Get number of existing versions
    u_int ver_num () const { return n_ver_; }
    u_int max_ver_num () const { return max_n_ver_; }


private:
    u_int n_ver_;          // number of versions
    u_int max_n_ver_;      // maximum number of versions
    u_int max_ver_;        // maximum version
    u_int value_size_;     // size of value
    char* a_ver_;          // array of versions
    std::string desc_;     // general description of value
    ConstructFn construct_;
    DestructFn destruct_;
    CopyFn copy_;
};

template <typename _VM>
class VersionManagerIterator {
public:
    typedef typename _VM::value_type value_type;

    VersionManagerIterator (const _VM* p_vm, u_int pos)
        : p_vm_(p_vm), pos_(pos)
    {
        find_valid_pos();
    }
    
    bool operator== (const VersionManagerIterator& rhs) const
    { return pos_ == rhs.pos_ && p_vm_ != rhs.p_vm_; }
    
    bool operator!= (const VersionManagerIterator& rhs) const
    { return pos_ != rhs.pos_ || p_vm_ != rhs.p_vm_; }
    
    VersionManagerIterator& operator++ ()
    {
        ++ pos_;
        find_valid_pos();
        return *this;
    }
    
    VersionManagerIterator operator++ (int)
    {
        VersionManagerIterator tmp = *this;
        ++ *this;
        return tmp;
    }

    const value_type& operator* () const { return (*p_vm_)[pos_];}

    const value_type* operator-> () const { return &((*p_vm_)[pos_]);}
    
private:
    void find_valid_pos ()
    {
        if (pos_ < p_vm_->max_ver_num() &&
            p_vm_->at(pos_).ver_ == VM_NULL_VERSION) {
            ++ pos_;
        }
    }
                       
    const _VM* p_vm_;
    u_int pos_;
};

template <typename _T> class VersionManager {
public:
    typedef BaseVersionManager::Node Node;
    typedef _T value_type;
    typedef VersionManagerIterator<VersionManager> iterator;
    
    // Default constructor/destructor
    VersionManager () {}
    ~VersionManager () {}
        
    // Construct a VersionManager, must be called before using
    // Params:
    //   max_n_ver maximum number of versions in this manager
    // Returns: GOOD/E_NULL
    int init (u_int max_n_ver, std::string desc)
    { return base_.init(sizeof(_T), max_n_ver, desc,
                        construct_fn_, destruct_fn_, copy_fn_); }

    // Get a read-only version with biggest version number
    // Returns: position of the slot, -1 means not found
    int find_latest_read_only () const
    { return base_.find_latest_read_only(); }
        
    // Make a version read-only
    // Params:
    //   pos     position of the version
    // Returns: GOOD/E_GENERAL
    int freeze_version (u_int pos)
    { return base_.freeze_version(pos); }
        
    // Drop a version
    // Params:
    //   pos     position of the version
    // Returns: E_GENERAL(negative) or dropped version number(positive)
    int drop_version (u_int pos) { return base_.drop_version(pos, true); }
    
    int drop_oldest_read_only () { return base_.drop_oldest_read_only(); }
    
    // Create a new version
    // Returns: position of the version, -1 means error happened
    int create_version () { return base_.create_version(); }

    Node& at (u_int pos) { return base_.at(pos); }
    const Node& at (u_int pos) const { return base_.at(pos); }    
    
    // Get version data by position
    _T& operator[] (u_int idx )
    { 
        _T* ret = reinterpret_cast<_T*>(base_.at(idx).value_); 
        return *ret;
    }
    
    const _T& operator[] (u_int idx) const
    { return *reinterpret_cast<const _T*>(base_.at(idx).value_); }

    iterator begin () const { return iterator(this, 0); }
    iterator end () const { return iterator(this, max_ver_num()); }

    // Get number of existing versions
    u_int ver_num () const { return base_.ver_num(); }
    u_int max_ver_num () const { return base_.max_ver_num(); }

    // print info into StringWriter
    void to_string (StringWriter& sw) const {
        sw << "{ver_num=" << base_.ver_num()
           << " max_ver_num=" << base_.max_ver_num();
        for (u_int i=0; i<base_.max_ver_num(); ++i) {
            const BaseVersionManager::Node& vn = base_.at(i);
            if (VM_NULL_VERSION == vn.ver_) {
                sw << " (empty)";
            } else {
                const _T* tvalue = reinterpret_cast<const _T*>(vn.value_);
                sw << ' ' << vn.ver_ << (vn.writable_ ? 'W' : 'R')
                   << *tvalue;
            }
        }
        sw << '}';
    }
            
private:
    static void construct_fn_(void* value) { ST_NEW_ON(value, _T); }
    static void destruct_fn_(void* value) { static_cast<_T*>(value)->~_T(); }
    static void copy_fn_(void* dst, const void* src)
    { *static_cast<_T*>(dst) = *static_cast<const _T*>(src); }
    
    BaseVersionManager base_;
};


}  // namespace st

#endif  // _VERSION_MANAGER_HPP_
