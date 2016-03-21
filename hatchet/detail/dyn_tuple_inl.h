// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Inline parts of dyn_tuple.h
// Author: gejun@baidu.com
// Date: Mon. Jun. 20 11:39:31 CST 2011

#include "compare.hpp"                  // valcmp

namespace st {

const DynTupleAccess* DynTupleSchema::at_n(const size_t idx) const
{
    return (idx < list_.size() ? &list_[idx] : &DynTupleAccess::EMPTY);
}
    
const DynTupleAccess* DynTupleSchema::at(const char* name) const
{
    for (std::vector<DynTupleAccess>::const_iterator
             it = list_.begin();  it != list_.end(); ++it) {
        const char* s1 = it->name.c_str();
        const char* s2 = name;
        for ( ; *s1 == *s2 && *s2 && *s1; ++s1, ++s2);
        if (!*s2) {
            return &(*it);
        }
    }
    return &DynTupleAccess::EMPTY;
}

int DynTupleSchema::find_name(const char* name) const
{
    for (int i = 0;  i < (int)list_.size(); ++i) {
        const char* s1 = list_[i].name.c_str();
        const char* s2 = name;
        for ( ; *s1 == *s2 && *s2 && *s1; ++s1, ++s2);
        if (!*s2) {
            return i;
        }
    }
    return -1;
}

int64_t ConstDynField::cast_int() const
{
    if (is_integer_type(e_->type)) {
        register int64_t target = *(int64_t*)((char*)addr_ + e_->byte_offset);
        if (e_->width ^ 64) {
            // Complement MSB correctly
            return target << (64 - e_->width - e_->shift) >> (64 - e_->width);
        }
        return target;
    } else {
        ST_FATAL("Can't cast `%s %s' to integer",
                 field_type_desc(e_->type), e_->name.c_str());
        return 0;
    }
}

uint64_t ConstDynField::cast_uint() const
{
    if (is_integer_type(e_->type)) {
        register uint64_t target = *(uint64_t*)((char*)addr_ + e_->byte_offset);
        if (((uint64_t)-1) ^ e_->mask) {
            return (target & e_->mask) >> e_->shift;
        }
        return target;
    } else {
        ST_FATAL("Can't cast `%s %s' to integer",
                 field_type_desc(e_->type), e_->name.c_str());
        return 0;
    }
}

float ConstDynField::cast_float() const
{
    if (is_float_type(e_->type)) {
        return *(float*)((char*)addr_ + e_->byte_offset);
    } else if (is_double_type(e_->type)) {
        return *(double*)((char*)addr_ + e_->byte_offset);
    } else {
        ST_FATAL("Can't cast `%s %s' to float",
                 field_type_desc(e_->type), e_->name.c_str());
        return 0;
    }
}

double ConstDynField::cast_double() const
{
    if (is_float_type(e_->type)) {
        return *(float*)((char*)addr_ + e_->byte_offset);
    } else if (is_double_type(e_->type)) {
        return *(double*)((char*)addr_ + e_->byte_offset);
    } else {
        ST_FATAL("Can't cast `%s %s' to double",
                 field_type_desc(e_->type), e_->name.c_str());
        return 0;
    }
}
    
const DynField& DynField::assign_uint(uint64_t v) const
{
    if (is_integer_type(e_->type)) {
        uint64_t* const target = (uint64_t*)((char*)addr_ + e_->byte_offset);
        if (((uint64_t)-1) ^ e_->mask) {
            *target = (*target & ~e_->mask) | ((v << e_->shift) & e_->mask);
        } else {
            *target = v;
        }
    } else {
        ST_FATAL("Can't write integer into `%s %s'",
                 field_type_desc(e_->type),
                 e_->name.c_str());
    }
    return *this;
}

const DynField& DynField::assign_float(float v) const
{
    if (is_float_type(e_->type)) {
        *(float*)((char*)addr_ + e_->byte_offset) = v;
    } else if (is_double_type(e_->type)) {
        *(double*)((char*)addr_ + e_->byte_offset) = v;
    } else {
        ST_FATAL("Can't write float into `%s %s'",
                 field_type_desc(e_->type),
                 e_->name.c_str());
    }
    return *this;
}

const DynField& DynField::assign_double(double v) const
{
    if (is_float_type(e_->type)) {
        *(float*)((char*)addr_ + e_->byte_offset) = v;
    } else if (is_double_type(e_->type)) {
        *(double*)((char*)addr_ + e_->byte_offset) = v;
    } else {
        ST_FATAL("Can't write double into `%s %s'",
                 field_type_desc(e_->type),
                 e_->name.c_str());
    }
    return *this;
}

bool ConstDynField::operator==(const ConstDynField& rhs) const
{
    const FieldType t = e_->type;
    return (is_compatible_type(t, rhs.e_->type) &&
            (is_integer_type(t) && to_int64() == rhs.to_int64())) ||
        (is_fp_type(t) && to_double() == rhs.to_double());
}

const DynField& DynField::operator=(const ConstDynField& rhs) const
{
    if (is_compatible_type(type(), rhs.type())) {
        if (is_integer_type(e_->type)) {
            return (*this = rhs.to_int64());
        } else if (is_fp_type(e_->type)) {
            return (*this = rhs.to_double());
        }
    }
    return *this;
}

DynTuple::DynTuple() : schema_(NULL), data_(NULL) {}

DynTuple::DynTuple(const DynTupleSchema* schema, void* data)
    : schema_(schema), data_(data) {}

DynTuple::DynTuple(const DynTuple& rhs)
    : schema_(rhs.schema_), data_(rhs.data_) {}

DynTuple& DynTuple::operator=(const DynTuple& rhs)
{
    if (valid()) {
        const size_t n_field = field_num();
        for (size_t i = 0; i < n_field; ++i) {
            at_n(i) = rhs.at_n(i);
        }
    } else {
        schema_ = rhs.schema_;
        data_ = rhs.data_;
    }
    return *this;
}

const DynField DynTuple::at(const char* name)
{
    return DynField(data_, schema_->at(name));
}

const ConstDynField DynTuple::at(const char* name) const
{
    return ConstDynField(data_, schema_->at(name));
}

const DynField DynTuple::at_n(int idx)
{
    return DynField(data_, schema_->at_n(idx));
}

const ConstDynField DynTuple::at_n(int idx) const
{
    return ConstDynField(data_, schema_->at_n(idx));
}
    
size_t DynTuple::field_num() const
{
    return schema_ ? schema_->field_num() : 0;
}

bool DynTuple::valid() const
{
    return ((intptr_t)schema_ & (intptr_t)data_);
}

bool DynTuple::operator<(const DynTuple& rhs) const
{
    const size_t n_field = field_num();
    for (size_t i = 0; i < n_field; ++i) {
        ConstDynField f1 = at_n(i);
        ConstDynField f2 = rhs.at_n(i);
        if (f1.type() != f2.type()) {
            return (f1.type() < f2.type());
        }
        int rc = 0;
        switch (f1.type()) {
        case FIELD_TYPE_INT:
            if ((rc = valcmp(f1.to_int64(), f2.to_int64())) != 0) {
                return rc < 0;
            }
            continue;
        case FIELD_TYPE_UINT:
            if ((rc = valcmp(f1.to_uint64(), f2.to_uint64())) != 0) {
                return rc < 0;
            }
            continue;
        case FIELD_TYPE_FLOAT:
            if ((rc = valcmp(f1.to_float(), f2.to_float())) != 0) {
                return rc < 0;
            }
            continue;
        case FIELD_TYPE_DOUBLE:
            if ((rc = valcmp(f1.to_double(), f2.to_double())) != 0) {
                return rc < 0;
            }
            continue;
        case FIELD_TYPE_UNKNOWN:
            return false;
        }
    }
    return n_field != rhs.field_num();
}

bool DynTuple::operator>(const DynTuple& rhs) const
{
    const size_t n_field = field_num();
    for (size_t i = 0; i < n_field; ++i) {
        ConstDynField f1 = at_n(i);
        ConstDynField f2 = rhs.at_n(i);
        if (f1.type() != f2.type()) {
            return (f1.type() > f2.type());
        }
        int rc = 0;
        switch (f2.type()) {
        case FIELD_TYPE_INT:
            if ((rc = valcmp(f1.to_int64(), f2.to_int64())) != 0) {
                return rc > 0;
            }
            continue;
        case FIELD_TYPE_UINT:
            if ((rc = valcmp(f1.to_uint64(), f2.to_uint64())) != 0) {
                return rc > 0;
            }
            continue;
        case FIELD_TYPE_FLOAT:
            if ((rc = valcmp(f1.to_float(), f2.to_float())) != 0) {
                return rc > 0;
            }
            continue;
        case FIELD_TYPE_DOUBLE:
            if ((rc = valcmp(f1.to_double(), f2.to_double())) != 0) {
                return rc > 0;
            }
            continue;
        case FIELD_TYPE_UNKNOWN:
            return false;
        }
    }
    return false;
}

bool DynTuple::operator==(const DynTuple& rhs) const
{
    const size_t n_field = field_num();
    for (size_t i = 0; i < n_field; ++i) {
        if (at_n(i) != rhs.at_n(i)) {
            return false;
        }
    }
    return true;
}

bool DynTuple::operator<= (const DynTuple& rhs) const { return !operator>(rhs); }
bool DynTuple::operator>= (const DynTuple& rhs) const { return !operator<(rhs); }
bool DynTuple::operator!= (const DynTuple& rhs) const { return !operator==(rhs); }

size_t DynTuple::hash_code() const
{
    const size_t nfield = field_num();
    register size_t rc = 0;
    for (size_t i = 0; i < nfield; ++i) {
        ConstDynField f1 = at_n(i);
        if (likely(is_integer_type(f1.type()))) {
            rc = rc * 257ul + hash(f1.to_uint64());
        }
    }
    return rc;
}


}  // namespace st
