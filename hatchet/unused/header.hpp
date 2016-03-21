// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// (smalltable1) Header of table
// Author: gejun@baidu.com
// Date: Mon Aug 16 17:40:15 2010
#pragma once
#ifndef _HEADER_HPP_
#define _HEADER_HPP_

#include <vector>
#include <set>
#include "tuple.hpp"

namespace st {

struct Header {
    
    // if key1 contains key2, both are sorted ascently
    bool contains (const Header& other) const
    {
        for (std::set<int>::const_iterator
                 it=other.s_key_.begin()
                 , it_e=other.s_key_.end()
                 ; it!=it_e; ++it) {
            if (s_key_.find (*it) == s_key_.end()) {
                return false;
            }
        }
        return true;
    }

    bool contains (int attr_id) const
    { return s_key_.find (attr_id) != s_key_.end(); }

    void clear ()
    { s_key_.clear(); }

    bool insert (const int attr_id, const char* desc=NULL)
    {
        if (desc) {
            a_desc_.push_back (desc);
        } else {
            char a_buf[32];
            snprintf (a_buf, sizeof(a_buf), "unknown%lu", a_desc_.size());
            a_desc_.push_back (a_buf);
        }
        return s_key_.insert (attr_id).second;
    }
    
    template <typename _Tuple>
    bool record()
    {
        clear();
        bool ret = true;
        
        if (! c_same<typename _Tuple::Repacked::Type0,void>::R) {
            typedef de_a<typename _Tuple::Repacked::Type0> d;
            ret = ret && insert (d::id(), d::name());
        }
        if (! c_same<typename _Tuple::Repacked::Type1,void>::R) {
            typedef de_a<typename _Tuple::Repacked::Type1> d;
            ret = ret && insert (d::id(), d::name());
        }
        if (! c_same<typename _Tuple::Repacked::Type2,void>::R) {
            typedef de_a<typename _Tuple::Repacked::Type2> d;
            ret = ret && insert (d::id(), d::name());
        }
        if (! c_same<typename _Tuple::Repacked::Type3,void>::R) {
            typedef de_a<typename _Tuple::Repacked::Type3> d;
            ret = ret && insert (d::id(), d::name());
        }
        if (! c_same<typename _Tuple::Repacked::Type4,void>::R) {
            typedef de_a<typename _Tuple::Repacked::Type4> d;
            ret = ret && insert (d::id(), d::name());
        }
        if (! c_same<typename _Tuple::Repacked::Type5,void>::R) {
            typedef de_a<typename _Tuple::Repacked::Type5> d;
            ret = ret && insert (d::id(), d::name());
        }
        if (! c_same<typename _Tuple::Repacked::Type6,void>::R) {
            typedef de_a<typename _Tuple::Repacked::Type6> d;
            ret = ret && insert (d::id(), d::name());
        }
        if (! c_same<typename _Tuple::Repacked::Type7,void>::R) {
            typedef de_a<typename _Tuple::Repacked::Type7> d;
            ret = ret && insert (d::id(), d::name());
        }
        if (! c_same<typename _Tuple::Repacked::Type8,void>::R) {
            typedef de_a<typename _Tuple::Repacked::Type8> d;
            ret = ret && insert (d::id(), d::name());
        }
        if (! c_same<typename _Tuple::Repacked::Type9,void>::R) {
            typedef de_a<typename _Tuple::Repacked::Type9> d;
            ret = ret && insert (d::id(), d::name());
        }

        if (!ret) {
            ST_FATAL ("%s has same-named attributes! righter one"
                      " will be inaccessable", show(*this).c_str());
        }
        return ret;
    }

    void to_string (StringWriter& sb) const
    {
        bool first = true;
        sb << "[";
        for (size_t i=0; i<a_desc_.size(); ++i) {
            if (!first) {
                sb << ",";
            }
                
            sb << a_desc_[i];
            first = false;
        }
        sb << "]";
    }

private:
    std::set<int> s_key_;
    std::vector<std::string> a_desc_;
};
}

#endif /* _HEADER_HPP_ */
