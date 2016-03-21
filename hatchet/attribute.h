// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Define attributes used by tuple_t/NamedTuple
// Author: gejun@baidu.com
// Date: Fri Jul 30 13:16:06 2010
#pragma once
#ifndef _ATTRIBUTE_H_
#define _ATTRIBUTE_H_

namespace st {

class AttributeTag {};

// Extract Type from attribute
template <class _Attr> struct get_attr_type { typedef typename _Attr::Type R; };

}  // namespace st

// define the type of a column
// name() gives string from preprocessing macro
// id() gives unique identifier representing the column at run-time
// now it's produced by hash_value, effective but not quite efficient
#define DEFINE_COLUMN(_name_, _type_)                                   \
    struct _name_ : public st::c_show_base {                    \
        typedef _type_ Type;                                            \
        typedef st::AttributeTag Tag;                           \
        static const char* name() { return #_name_; }                   \
        static int id() { return st::hash(#_name_); }   \
        static void c_to_string (std::ostream& os) { os << #_name_; }   \
    };

// Older name
#define DEFINE_ATTRIBUTE DEFINE_COLUMN

// Define an attribute.
// Note: currently we still use upper old macro, this macro is not used.
// Example: DEFINE_ATTR (UNIT_ID, int);
//          defines the attribute "UNIT_ID" typed "int"
// Params:
//   _name_  name of the attribute
//   _type_  value type of the attribute
#define DEFINE_ATTR(_name_,_type_)                                      \
    struct _name_ : public st::c_show_base {                    \
        typedef _type_ ValueType;                                       \
        typedef st::AttributeTag Tag;                           \
        static const char* name() { return #_name_; }                   \
        static void c_to_string (std::ostream& os) { os << #_name_; }   \
    };

#endif  // _ATTRIBUTE_H_
