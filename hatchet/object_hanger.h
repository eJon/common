// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// (smalltable1) Hang objects together, selectors address fields inside
// objects by positions
// Author: gejun@baidu.com
// Date: Dec.1 14:36:48 CST 2010
#pragma once
#ifndef _OBJECT_HANGER_H_
#define _OBJECT_HANGER_H_

#include <vector>
#include "common.h"

namespace st {

class ObjectHanger {
public:
    struct Position {
        uint16_t idx;
        uint16_t offset;

        void to_string (StringWriter& sw) const
        { sw << "ref(" << idx << "," << offset << ")"; }
    };

    typedef std::vector<const void*>::const_iterator const_iterator;
        
    explicit ObjectHanger()
    {
        ap_obj.reserve (8);
    }

    //! set a pointer of object into this hanger
    //! if idx exceeds size(), internal vector grows to adapt the setting
    void set_object (const size_t idx, const void* p_object)
    {
        if (NULL == p_object) {
            while (ap_obj.size() <= idx) {
                ap_obj.push_back (NULL);
            }
            ap_obj[idx] = p_object;
        }
    }

    Position push_back (const void* p_object)
    {
        ap_obj.push_back (p_object);
        Position p = { idx : (uint16_t)(ap_obj.size()-1), offset : 0 };
        return p;
    }
            
    //! get pointer of the object at the position
    const void* object (const size_t idx) const
    { return ap_obj[idx]; }

    //! get field at given position
    const void* field (Position pos) const
    { return ((const char*)ap_obj[pos.idx]) + pos.offset; }

    size_t size () const
    { return ap_obj.size(); }

    void clear ()
    { ap_obj.clear(); }

    const_iterator begin() const
    { return ap_obj.begin(); }

    const_iterator end() const
    { return ap_obj.end(); }
        
private:
    std::vector<const void*> ap_obj;
};
}

#endif  //_OBJECT_HANGER_H_

