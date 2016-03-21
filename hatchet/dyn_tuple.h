// Copyright(c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// A tuple with a runtime-decided layout
// Author: gejun@baidu.com
// Date: Mon. Jun. 20 11:39:31 CST 2011
#pragma once
#ifndef _DYN_TUPLE_HPP_
#define _DYN_TUPLE_HPP_

#include <limits.h>              // CHAR_BIT
#include "common.h"
#include "st_hash.h"              // compute_hash


// TODO:
// - Refine add_field(3)
// - Multi-threading issues (atomicity, memory violation)

namespace st {

// Type of fields
// Note: 
//   the INT/UINT does not necessarily stand for int/unsigned int, they're 
//   more properly the widest integer types in the system(typically 64 bits)
//   narrower types are represented by an additional bit width
enum FieldType {
    FIELD_TYPE_UNKNOWN = 0,  // 0b0000
    
    FIELD_TYPE_INT     = 1,  // 0b0001
    FIELD_TYPE_UINT    = 3,  // 0b0011
                             //      ^
    FIELD_TYPE_FLOAT   = 4,  // 0b0100
    FIELD_TYPE_DOUBLE  = 12  // 0b1100
                             //    ^
};

inline bool is_integer_type(FieldType t)           { return (t & 1); }
inline bool is_signed_integer_type(FieldType t)    { return (t & 3) == 1; }
inline bool is_unsigned_integer_type(FieldType t)  { return (t & 3) == 3; }
inline bool is_fp_type(FieldType t)                { return (t & 4); }
inline bool is_float_type(FieldType t)             { return (t & 12) == 4; }
inline bool is_double_type(FieldType t)            { return (t & 12) == 12; }
inline bool is_compatible_type(FieldType t1, FieldType t2) { return t1 & t2; }
const char* field_type_desc(FieldType);  // Get description of a type
FieldType desc_field_type(const char*);  // Get type from description
size_t field_type_width(FieldType);      // Get default width
int field_type_check(FieldType, size_t); 

struct DynTupleAccess {
    static DynTupleAccess EMPTY;
    
    DynTupleAccess() {}
    DynTupleAccess(
        FieldType type2, uint32_t byte_offset2, uint32_t shift2,
        uint32_t width2, std::string name2)
        : type(type2), byte_offset(byte_offset2), shift(shift2)
        , width(width2), name(name2)
    {
        mask = (width2 < 64) ? (((1ul << width2) - 1) << shift) : (uint64_t)-1;
    }

    //size_t width() const { return __builtin_popcountl(mask); }
    bool operator==(const DynTupleAccess&) const;
    bool operator!=(const DynTupleAccess& rhs) const { return !operator==(rhs); }
friend std::ostream& operator<<(std::ostream&, const DynTupleAccess&);
    
    FieldType type;            // type of value
    uint32_t byte_offset;        // bytes from base address
    uint32_t shift;              // bits to shift
    uint32_t width;
    uint64_t mask;
    std::string name;
};

// A class to manage layout of memory, bitwise fields are supported.
// Example: 
//   DynTupleSchema s;
//   s.add_field("apple",  FIELD_TYPE_INT, 1);
//   s.add_field("banana", FIELD_TYPE_INT, 2);
//   s.add_field("cherry", FIELD_TYPE_INT, 3);
//   s.add_field("durian", FIELD_TYPE_INT, 4);
// or
//   s.add_fields_by_string("int1 apple, int2 banana, int3 cherry, int4 durian");
class DynTupleSchema {
public:
    DynTupleSchema();
    
    // Add a field named `name' and typed `type : width'
    // Params:
    //   name          should match regexp `[0-9a-zA-Z_]+'
    // Returns:
    //   0             success
    //   EEXIST        A field with same name already exists
    //   EINVAL        `width' does not match `type'
    int add_field(const char* name, FieldType type, size_t width = 0);

    // Add fields by a string which is "type name" pairs separated by commas
    // Accepted types:
    //   int         32-bit signed integer
    //   uint        32-bit unsigned integer
    //   intN        N-bit signed integer, 1 <= N <= 64
    //   uintN       N-bit unsigned integer, 1 <= N <= 64
    //   float       32-bit floating point
    //   double      64-bit floating point
    // Example:
    //   add_fields_by_string("int16 site_id, int16 unit_id, float scale")
    // Returns:
    //   0           success
    //   EINVAL      `decl' is ill-formatted
    //   ERANGE      #fields exceeds `max_nfield'
    int add_fields_by_string(const char* decl, size_t max_nfield = UINT_MAX);

    // Get an entry by position or name
    inline const DynTupleAccess* at_n(const size_t idx) const;
    inline const DynTupleAccess* at(const char* name) const;
    
    // Get position of an entry by its name
    // Returns:
    //   -1             not found
    //   0,1...         position of the field
    inline int find_name(const char* name) const;

    size_t byte_size() const         { return nbyte_; }
    size_t field_num() const         { return list_.size(); }
    size_t width() const             { return nbit_; }
    bool empty() const               { return list_.empty(); }
    void clear()                     { list_.clear(); }
    bool operator==(const DynTupleSchema&) const;
    bool operator!=(const DynTupleSchema& rhs) const { return !operator==(rhs); }
friend std::ostream& operator<<(std::ostream&, const DynTupleSchema&);

private:
    static int check_name(const char*);
    
    std::vector<DynTupleAccess> list_;
    size_t nbit_;
    size_t nbyte_;
};

// A proxy class that reads only from a field
class ConstDynField {
public:
    ConstDynField(void* b, const DynTupleAccess* e) : addr_(b), e_(e) {}
    int32_t to_int32() const   { return cast_int(); }
    uint32_t to_uint32() const { return cast_uint(); }
    int64_t to_int64() const   { return cast_int(); }
    uint64_t to_uint64() const { return cast_uint(); }
    float to_float() const     { return cast_float(); }
    double to_double() const   { return cast_double(); }
    FieldType type() const     { return e_->type; }
    inline bool operator==(const ConstDynField&) const;
    bool operator!=(const ConstDynField& rhs) const { return !operator==(rhs); }
friend std::ostream& operator<<(std::ostream&, ConstDynField const&);
    
protected:
    inline int64_t cast_int() const;
    inline uint64_t cast_uint() const;
    inline float cast_float() const;
    inline double cast_double() const;
    
    void* addr_;
    const DynTupleAccess* e_;
};

// A proxy class that reads and writes from a field
class DynField : public ConstDynField {
public:
    DynField(void* b, const DynTupleAccess* e) : ConstDynField(b, e) {}
    const DynField& operator=(int32_t v) const  { return assign_uint(v); }
    const DynField& operator=(uint32_t v) const { return assign_uint(v); }
    const DynField& operator=(int64_t v) const  { return assign_uint(v); }
    const DynField& operator=(uint64_t v) const { return assign_uint(v); }
    const DynField& operator=(float v) const    { return assign_float(v); }
    const DynField& operator=(double v) const   { return assign_double(v); }
    inline const DynField& operator=(const ConstDynField& rhs) const;

    // Returns:
    //   0                    success
    //   ENOTINIT             type of this field is unknown
    //   EINVAL               `value' does not match type
    int set_by_string(const char* value) const;
private:
    inline const DynField& assign_uint(uint64_t) const;
    inline const DynField& assign_float(float) const;
    inline const DynField& assign_double(double) const;
};

#define SCOPED_DYN_TUPLE(_tup_, _schema_)       \
    char _tup_##_buf[(_schema_)->byte_size()];  \
    DynTuple _tup_((_schema_), _tup_##_buf);

// A tuple with a runtime-decided layout
class DynTuple {
public:
    inline DynTuple();
    inline DynTuple(const DynTupleSchema* schema, void* data);
    inline DynTuple(const DynTuple&);
    inline DynTuple& operator=(const DynTuple&);
    inline bool operator<(const DynTuple&) const;
    inline bool operator<=(const DynTuple&) const;
    inline bool operator>(const DynTuple&) const;
    inline bool operator>=(const DynTuple&) const;
    inline bool operator!=(const DynTuple&) const;
    inline bool operator==(const DynTuple&) const;
    
    // Set data with values in the string
    // Returns:
    //   0             success
    //   EINVAL        any parsing error or #values does not match #fields
    int set_by_string(const char*, char = ',', const size_t* = NULL);
    
    // Get a field by name or position
    inline const DynField at(const char* name);
    inline const DynField at_n(int idx);
    inline const ConstDynField at(const char* name) const;
    inline const ConstDynField at_n(int idx) const;

    void set_schema(const DynTupleSchema* schema) { schema_ = schema; }
    void set_buf(void* buf)                       { data_ = buf; }
    inline size_t field_num() const;              // Get # of fields
    inline bool valid() const;                    // Valid or not
    const DynTupleSchema* schema() const          { return schema_; }
    const void* buf() const                       { return data_; }
    inline size_t hash_code() const;
friend std::ostream& operator<<(std::ostream&, const DynTuple&);
    
private:
    const DynTupleSchema* schema_;    // read-only schema
    void* data_;
};

inline size_t hash(const DynTuple& t) { return t.hash_code(); }


}  // namespace st

#include "detail/dyn_tuple_inl.h"

#endif  //_DYN_TUPLE_HPP_
