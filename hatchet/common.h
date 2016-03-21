// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Provide commonly used constants / declarations / headers
// Author: gejun@baidu.com
// Date: Fri Jul 30 13:16:06 2010
#pragma once
#ifndef _COMMON_H_
#define _COMMON_H_

#include <limits.h>
#include "debug.h"
#include "string_writer.hpp"
#include "object_hanger.h"
#include "c_common.hpp"

// TODO: this file should only contain constants and inclusions

namespace st {

enum ModType { MOD_INSERT, MOD_ERASE };

class SimpleVersion {
public:
    SimpleVersion () : ver_(0)
    {}

    static SimpleVersion& global ()
    {
        static SimpleVersion g_sv;
        return g_sv;
    }
        
    void increase () { ++ ver_; }

    int version () const { return ver_; }

private:
    volatile int ver_;
};

}

#endif  // _COMMON_H_
