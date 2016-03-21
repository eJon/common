// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Base class of Table
// Author: gejun@baidu.com
// Date: Mon Aug 30 00:58:44 CST 2010

#pragma once
#ifndef _BASE_TABLE_H_
#define _BASE_TABLE_H_

#include "header.hpp"
#include "common.h"

namespace st {
class BaseTable {
public:
    explicit BaseTable()
        : p_ver_(NULL)
    {}
        
    //! just make destructor virtual
    virtual ~BaseTable()
    {}

    //! header of this table
    const Header& header() const
    { return header_; }

    //! interface
    virtual int begin_update () = 0;

    //! interface
    virtual int end_update () = 0;

    //! associate an object of SimpleVersion to this table
    void create (const SimpleVersion* p_ver)
    {
        if (NULL == p_ver) {
            p_ver_ = &SimpleVersion::global();
        } else {
            p_ver_ = p_ver;
        }
    }

    int fg_no () const
    { return p_ver_->version() & 1; }
        
    int bg_no () const
    { return (p_ver_->version() + 1) & 1; }

    virtual void seek_ref_v (void* p_iterator
                             , int buf_no
                             , int index_no
                             , const ObjectHanger& oh
                             , const void* p_ref_tuple
                             , const std::vector<Predicate*>* pa_filter
        ) const = 0;
        
    virtual void all_v (void* p_iterator
                        , int buf_no
                        , int index_no
                        , const std::vector<Predicate*>* pa_filter
        ) const = 0;
        
protected:
    Header header_;
    const SimpleVersion *p_ver_;
};
}

#endif  // _BASE_TABLE_H_
