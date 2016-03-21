// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Know if one type inherits another at compile-time
// Author: gejun@baidu.com
// Date: Dec 4 15:08:43 CST 2010
#pragma once
#ifndef _IS_BASE_OF_HPP_
#define _IS_BASE_OF_HPP_


namespace st {
// Test if _D inherits _B
// Returns: yes or no
template<typename _B, typename _D> struct is_base_of;


// ----------- Implementation --------------
// modified from boost/type_traits/is_base_and_derived.hpp, very tricky

// sizeof(bd_yes_type) must be different from sizeof(bd_no_type)
struct bd_yes_type { char b_[ 16 * sizeof(int) ]; };
struct bd_no_type { char b_[ sizeof(int) ]; };

template <typename _B, typename _D> struct bd_helper {
    template <typename T> static bd_yes_type check_sig(_D const volatile *, T);
    static bd_no_type check_sig(_B const volatile *, int);
};

template<typename _B, typename _D> struct is_base_of {
    struct Host {
        operator _B const volatile *() const;
        operator _D const volatile *();
    };

    static const bool R =
        sizeof(bd_helper<_B, _D>::check_sig(Host(), 0)) == sizeof(bd_yes_type);
};

}
#endif  //_IS_BASE_OF_HPP_
