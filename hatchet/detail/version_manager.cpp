// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Implement version_manager.hpp
// Author: gejun@baidu.com
// Date: Jan 3 11:23:11 CST 2011
#include "version_manager.hpp"

namespace st {
BaseVersionManager::BaseVersionManager()
    : n_ver_(0)
    , max_n_ver_(VM_MIN_NUM_OF_VERSIONS)
    , max_ver_(VM_NULL_VERSION)
    , value_size_(0)
    , a_ver_(NULL)
    , construct_(NULL)
    , destruct_(NULL)
    , copy_(NULL)
{}

BaseVersionManager::~BaseVersionManager ()
{
    if (NULL != a_ver_) {
        for (u_int i=0; i<max_n_ver_; ++i) {
            Node& vn = at(i);
            if (vn.ver_ != VM_NULL_VERSION) {
                destruct_(vn.value_);
            }
        }
        ST_DELETE_ARRAY(a_ver_);
    }
}

int BaseVersionManager::init (u_int value_size,
                              u_int max_n_ver,
                              std::string desc,
                              ConstructFn construct,
                              DestructFn destruct,
                              CopyFn copy)
{
    value_size_ = value_size;
    desc_ = desc;
    construct_ = construct;
    destruct_ = destruct;
    copy_ = copy;
    
    if (max_n_ver_ < VM_MIN_NUM_OF_VERSIONS) {
        ST_FATAL ("[%s] max_n_ver is less than %u",
                  desc_.c_str(), VM_MIN_NUM_OF_VERSIONS);
        max_n_ver_ = VM_MIN_NUM_OF_VERSIONS;
    } else {
        max_n_ver_ = max_n_ver;
    }

    a_ver_ = ST_NEW_ARRAY(char, (sizeof(Node) + value_size) * max_n_ver_);
    if (NULL == a_ver_) {
        ST_FATAL ("[%s] Fail to new a_ver_", desc_.c_str());
        return ENOMEM;
    }
    for (u_int i=0; i<max_n_ver_; ++i) {
        at(i).ver_ = VM_NULL_VERSION;
        at(i).writable_ = false;
    }
            
    return 0;
}

int BaseVersionManager::find_latest_read_only () const
{
    int pos = -1;
    for (u_int i=0; i<max_n_ver_; ++i) {
        const Node& vn = at(i);
        if (!vn.writable_ &&
            (VM_NULL_VERSION != vn.ver_) &&
            (pos < 0 || vn.ver_ > at(pos).ver_)) {
            pos = i;
        }
    }
    return pos;
}

int BaseVersionManager::create_version ()
{
    ++ max_ver_;
    int oldest_pos = -1;
    int newest_pos = -1;
    for (u_int i=0; i<max_n_ver_; ++i) {
        const Node& vn = at(i);
        if (max_ver_ <= vn.ver_) {
            ST_FATAL ("[%s] max_ver=%u is not biggest",
                      desc_.c_str(), max_ver_);
            return -1;
        }
        if (newest_pos < 0 || vn.ver_ > at(newest_pos).ver_) {
            newest_pos = i;
        }
        if (oldest_pos < 0 || vn.ver_ < at(oldest_pos).ver_) {
            oldest_pos = i;
        }
    }
    assert (oldest_pos >= 0);
    assert (newest_pos >= 0);

    Node& dest = at(oldest_pos);
    Node& newest_node = at(newest_pos);
    if (VM_NULL_VERSION != newest_node.ver_) {
        if (newest_node.writable_) {
            newest_node.writable_ = false;
            ST_TRACE("version=%u of %s is read-only"
                     " because new version=%u is created",
                     newest_node.ver_, desc_.c_str(), max_ver_);
        }

        if (VM_NULL_VERSION == dest.ver_) {
            ++ n_ver_;
            construct_(dest.value_);
        } else {
            ST_TRACE("version %u of %s is eliminated by version %u",
                     dest.ver_, desc_.c_str(), max_ver_);
        }
        copy_(dest.value_, newest_node.value_);
    } else {
        ++ n_ver_;
        construct_(dest.value_);
    }

    dest.ver_ = max_ver_;
    dest.writable_ = true;
    ST_TRACE("version %u of %s is created at %d",
             dest.ver_, desc_.c_str(), oldest_pos);
    return oldest_pos;
}

int BaseVersionManager::freeze_version(u_int pos)
{
    if (pos >= max_n_ver_) {
        ST_FATAL("[%s] pos=%u is out of range", desc_.c_str(), pos);
        return EINVAL;
    }

    Node& vn = at(pos);
    if (!vn.writable_) {
        ST_FATAL("Version %u of %s is already immutable",
                 vn.ver_, desc_.c_str());
        return EIMMUTABLE;
    }
    vn.writable_ = false;
    ST_TRACE("version %u of %s is read-only", vn.ver_, desc_.c_str());
    return 0;
}

int BaseVersionManager::drop_version(u_int pos, bool forbid_dropping_readonly)
{
    if (pos >= max_n_ver_) {
        ST_FATAL("[%s] pos=%u is out of range", desc_.c_str(), pos);
        return EINVAL;
    }
    
    Node& vn = at(pos);
    if (forbid_dropping_readonly && !vn.writable_) {
        ST_FATAL("[%s] cannot drop an immutable version=%u",
                 desc_.c_str(), vn.ver_);
        return EIMMUTABLE;
    }
    vn.ver_ = VM_NULL_VERSION;
    vn.writable_ = false;
    -- n_ver_;

    // destruct value at last
    destruct_(vn.value_);
    ST_WARN("Dropped version %u of %s", vn.ver_, desc_.c_str());
    return vn.ver_;
}

int BaseVersionManager::drop_oldest_read_only()
{
    u_int min_ver = UINT_MAX;
    int min_pos = -1;
    int n_ro = 0;
    for (u_int i = 0; i < max_n_ver_; ++i) {
        Node& vn = at(i);
        if (vn.ver_ != VM_NULL_VERSION && !vn.writable_) {
            if (vn.ver_ <= min_ver) {
                min_ver = vn.ver_;
                min_pos = i;
            }
            ++ n_ro;
        }
    }
    if (n_ro >= 2 && min_pos >= 0) {
        return drop_version(min_pos, false);
    }
    return EEMPTY;
}

}  // namespace st

