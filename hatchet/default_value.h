// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Define and get default value of types
// Author: gejun@baidu.com
// Date: Mon. Feb. 14 11:42:47 CST 2011
#pragma once
#ifndef _DEFAULT_VALUE_H_
#define _DEFAULT_VALUE_H_

namespace st {
template <typename> struct default_value_of;
}  // namespace st

// Get default value from a type
#define ST_DEFAULT_VALUE_OF(_type_)  st::default_value_of<_type_>::R()

// Specialize a default value for a type
#define ST_DEFINE_DEFAULT_VALUE(_type_, _value_)                        \
    namespace st {                                                      \
    template <> struct default_value_of<_type_> {                       \
        static _type_ R() { return _value_; };                          \
    };                                                                  \
    }

#define ST_INSIDE_DEFINE_DEFAULT_VALUE(_type_, _value_)                 \
    template <> struct default_value_of<_type_> {                       \
        static _type_ R() { return _value_; };                          \
    };

// Below are default values for primitive types
ST_DEFINE_DEFAULT_VALUE(int, 0);
ST_DEFINE_DEFAULT_VALUE(unsigned int, 0);
ST_DEFINE_DEFAULT_VALUE(long, 0);
ST_DEFINE_DEFAULT_VALUE(unsigned long, 0);
ST_DEFINE_DEFAULT_VALUE(char, 0);
ST_DEFINE_DEFAULT_VALUE(unsigned char, 0);
ST_DEFINE_DEFAULT_VALUE(short, 0);
ST_DEFINE_DEFAULT_VALUE(unsigned short, 0);
ST_DEFINE_DEFAULT_VALUE(long long, 0);
ST_DEFINE_DEFAULT_VALUE(unsigned long long, 0);

namespace st {                                                     
template <typename _T> struct default_value_of<_T*> {
    static _T* R() { return NULL; }; 
}; 
}


#endif  // _DEFAULT_VALUE_H_
