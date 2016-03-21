// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Implement dyn_tuple.h
// Author: gejun@baidu.com
// Date: Jun 20 11:39:31 CST 2011
#include <ctype.h>                   // isblank
#include <endian.h>                  // __BYTE_ORDER
#include "dyn_tuple.h"
#include "string_reader.hpp"         // StringReader

namespace st {

DynTupleAccess DynTupleAccess::EMPTY(FIELD_TYPE_UNKNOWN, 0, 0, 0, "");

size_t align_bits(size_t nbit, size_t nalign)
{
    size_t n2 = (nbit / nalign) * nalign;
    return (n2 == nbit ? n2 : (n2 + nalign));
}

#if __BYTE_ORDER != __LITTLE_ENDIAN
#error "Only support little endian"
#endif

// Only support x86_64 now
#ifdef __x86_64__
const size_t ALIGN_WIDTH = 64ul;

// Find the proper starting bit to pack `width' bits from bit `start_nbit'
size_t align_memory(size_t start_bit, size_t width)
{
    const size_t start_bit2 = start_bit / ALIGN_WIDTH * ALIGN_WIDTH;
    if (start_bit2 == start_bit) {
        // If start_bit divides ALIGN_WIDTH, any length of memory will fit well
        return start_bit;
    }
    
    const size_t nleft = ALIGN_WIDTH + start_bit2 - start_bit;
    if (nleft >= width) {
        // left bits of a alignment block is more than width, there's a special
        // case to handle
        if (32 == width && nleft >= 32) {
            start_bit += nleft - 32;
        }
        return start_bit;
    }

    // Align to next block
    start_bit += nleft;
    return start_bit;
}

size_t field_type_width(FieldType type)
{
    switch (type) {
    case FIELD_TYPE_INT:
    case FIELD_TYPE_UINT:
    case FIELD_TYPE_FLOAT:
        return 32;
    case FIELD_TYPE_DOUBLE:
        return 64;
    case FIELD_TYPE_UNKNOWN:
        return 0;
    }
    return 0;
}

#else
#error "The arch is not supported"
#endif  // __x86_64__

const char* field_type_desc(FieldType type)
{
    switch (type) {
    case FIELD_TYPE_INT:
        return "int";
    case FIELD_TYPE_UINT:
        return "uint";
    case FIELD_TYPE_FLOAT:
        return "float";
    case FIELD_TYPE_DOUBLE:
        return "double";
    case FIELD_TYPE_UNKNOWN:
        return "UnknownType";
    }
    return "UnknownType";
}

FieldType desc_field_type(const char* name)
{
    if (0 == strcmp(name, "int")) {
        return FIELD_TYPE_INT;
    } else if (0 == strcmp(name, "uint")) {
        return FIELD_TYPE_UINT;
    } else if (0 == strcmp(name, "float")) {
        return FIELD_TYPE_FLOAT;
    } else if (0 == strcmp(name, "double")) {
        return FIELD_TYPE_DOUBLE;
    }
    return FIELD_TYPE_UNKNOWN;
}

int field_type_check(FieldType type, size_t width)
{
    if (is_integer_type(type)) {
        if (width < 1 || width > ALIGN_WIDTH) {
            ST_FATAL("Invalid width=%lu for %s", width, field_type_desc(type));
            return EINVAL;
        }
    } else if (is_fp_type(type)) {
        if (width != field_type_width(type)) {
            ST_FATAL("Invalid width=%lu for %s", width, field_type_desc(type));
            return EINVAL;
        }
    } else {
        ST_FATAL("Unknown type=%d", type);
        return EINVAL;
    }
    return 0;
}

DynTupleSchema::DynTupleSchema() : nbit_(0), nbyte_(0)
{}

int DynTupleSchema::check_name(const char* name)
{
    const char* s = name;
    for ( ; isalpha(*s) || isdigit(*s) || '_' == *s; ++s);
    return *s;
}

struct AddField {
    AddField(DynTupleSchema* s, const std::string* tn, const size_t* width)
        : schema_(s), type_name_(tn), width_(width) {}
    
    bool operator()(const char* buf_begin, const char* buf_end) const
    {
        char name_buf[buf_end - buf_begin + 1];
        memcpy(name_buf, buf_begin, buf_end - buf_begin);
        name_buf[buf_end - buf_begin] = '\0';

        FieldType type = desc_field_type(type_name_->c_str());
        
        if (FIELD_TYPE_UNKNOWN == type) {
            return false;
        }
        return 0 == schema_->add_field(name_buf, type, *width_);
    }

private:
    DynTupleSchema* schema_;
    const std::string* type_name_;
    const size_t* width_;
};

int DynTupleSchema::add_field(const char* name, FieldType type, size_t width)
{
    int rc = 0;

    // Check validity of `name'
    if ((rc = check_name(name)) != 0) {
        ST_FATAL("Field name `%s' contains invalid character", name);
        return rc;
    }

    // Check if `name' exists
    for (std::vector<DynTupleAccess>::const_iterator
             it = list_.begin(); it != list_.end(); ++it) {
        if (0 == strcmp(name, it->name.c_str())) {
            ST_FATAL("Field `%s' already exists", name);
            return EEXIST;
        }
    }

    if (0 == width) {
        width = field_type_width(type);
    }

    if ((rc = field_type_check(type, width)) != 0) {
        return rc; 
    }

    // TODO: Refine this block
    size_t start_nbit = align_memory(nbit_, width);
    size_t block_nbit = 0;
    if (start_nbit / 32 == (start_nbit + width - 1) / 32) {
        block_nbit = 32;
    } else if (start_nbit / 64 == (start_nbit + width - 1) / 64) {
        block_nbit = 64;
    } else {
        ST_FATAL("Incorrect alignment: start_nbit=%zu width=%zu",
                 start_nbit, width);
        return ECONFLICT;
    }

    size_t start_nbyte = (type == FIELD_TYPE_FLOAT ?
                          start_nbit / 32 * 32 / CHAR_BIT :
                          start_nbit / 64 * 64 / CHAR_BIT);
    
    list_.push_back(DynTupleAccess(type,
                                   start_nbyte,
                                   start_nbit - start_nbyte * CHAR_BIT,
                                   width,
                                   name));
        
    nbit_ = start_nbit + width;

    const size_t ratio = nbit_ / block_nbit * block_nbit;
    nbyte_ = (ratio == nbit_ ? ratio : (ratio + block_nbit)) / CHAR_BIT;

    return 0;
}

int DynTupleSchema::add_fields_by_string(const char* str, size_t max_nfield)
{
    size_t width = 0;
    std::string type_name;
    size_t last_size = list_.size();
    int rc = 0;
    StringReader sr(str);
    
    HAVE_MANIPULATOR(read_type_and_name,
                     read_if1(isalpha, &type_name)
                     >> (once(&width) | sr_set_value(&width, 0ul))
                     >> many1(' ')
                     >> read_name1(AddField(this, &type_name, &width)));
    sr >> many0(' ') >> read_type_and_name >> many0(' ')
       >> many0(once(',') >> many0(' ') >> read_type_and_name >> many0(' '))
       >> sr_ends;

    if (sr.good() && (list_.size() - last_size) <= max_nfield) {
        return 0;
    }

    if (!sr.good()) {
        ST_FATAL("Invalid format `%s', stopped at `%s'", str, sr.buf());
        rc = EINVAL;
    }
    if ((list_.size() - last_size) > max_nfield) {
        ST_FATAL("Number of fields(%zu) exceeds max_nfield(%zu)",
                 list_.size() - last_size, max_nfield);
        rc = ERANGE;
    }
    // recovery
    while (list_.size() > last_size) {
        list_.pop_back();
    }
    return rc;
}

bool DynTupleAccess::operator==(const DynTupleAccess& rhs) const
{
    return byte_offset == rhs.byte_offset &&
        shift == rhs.shift &&
        mask == rhs.mask &&
        type == rhs.type;
}

bool DynTupleSchema::operator==(const DynTupleSchema& rhs) const
{
    if (this == &rhs) {
        return true;
    }
    
    if (nbit_ != rhs.nbit_ ||
        list_.size() != rhs.list_.size()) {
        return false;
    }

    for (size_t i = 0; i < list_.size(); ++i) {
        if (list_[i] != rhs.list_[i]) {
            return false;
        }
    }
    
    return true;
}

std::ostream& operator<< (std::ostream& os, const DynTupleAccess& e)
{
    return os << field_type_desc(e.type) << '_'
              << e.byte_offset << 'B'
              << e.width << 'w'
              << e.shift << 's'
              << ' ' << e.name;
}

std::ostream& operator<<(std::ostream& os, const DynTupleSchema& s)
{
    bool first = true;

    os << s.nbit_ << "b" << s.nbyte_ << "B[";
    for (std::vector<DynTupleAccess>::const_iterator it = s.list_.begin();
         it != s.list_.end(); ++it) {
        if (!first) {
            os << ',';
        } else {
            first = false;
        }
        os << *it;
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const ConstDynField& f)
{
    switch (f.type()) {
    case FIELD_TYPE_INT:
        return os << f.to_int64();
    case FIELD_TYPE_UINT:
        return os << f.to_uint64();
    case FIELD_TYPE_FLOAT:
        return os << f.to_float();
    case FIELD_TYPE_DOUBLE:
        return os << f.to_double();
    case FIELD_TYPE_UNKNOWN:
        return os << "Unknown";
    }
    return os;
            
}

int DynField::set_by_string(const char* value) const
{
    char* endptr = const_cast<char*>(value);
    switch (type()) {
    case FIELD_TYPE_INT:
        *this = strtol(value, &endptr, 10);
        break;
    case FIELD_TYPE_UINT:
        *this = strtoul(value, &endptr, 10);
        break;
    case FIELD_TYPE_FLOAT:
        *this = strtof(value, &endptr);
        break;
    case FIELD_TYPE_DOUBLE:
        *this = strtod(value, &endptr);
        break;
    case FIELD_TYPE_UNKNOWN:
        ST_FATAL("Unknown type");
        return ENOTINIT;
    }
    if (endptr == value || *endptr != '\0') {
        ST_FATAL("Value(%s) does not match the type(%s)",
                 value, field_type_desc(type()));
        return EINVAL;
    }
    return 0;
}


struct ReadSetField : public BaseManipulator<ReadSetField> {
    ReadSetField(DynTuple* tup, const size_t* i, const size_t* pos)
        : tup_(tup), i_(i), pos_(pos) {}

    StringReader& operator()(StringReader& sr) const
    {
        if (sr.good()) {
            const size_t idx = pos_ ? pos_[*i_] : *i_;
            if (idx < tup_->field_num()) {
                DynField f = tup_->at_n(idx);
                unsigned long tmp2;
                double tmp3;

                FieldType t = f.type();
                if (is_integer_type(t)) {
                    sr >> &tmp2;
                    f = tmp2;
                } else if (is_fp_type(t)) {
                    sr >> &tmp3;
                    f = tmp3;
                } else {
                    sr.set_status(false);
                }
            } else {
                ST_FATAL("Only %lu fields, #%lu is out of range",
                         tup_->field_num(), idx);
                sr.set_status(false);
            }
        }
        return sr;
    }

private:
    DynTuple* tup_;
    const size_t* i_;
    const size_t* pos_;
};

int DynTuple::set_by_string(const char* values, char sep, const size_t* pos)
{
    // " a , b , c "
    StringReader sr(values);
    size_t num = 0;

    HAVE_MANIPULATOR(read_set_field,
                     ReadSetField(this, &num, pos) >> sr_inc_value(&num));
    
    if (' ' != sep) {
        sr >> many0(' ') >> read_set_field >> many0(' ')
           >> many0(once(sep) >> many0(' ') >> read_set_field >> many0(' '))
           >> sr_ends;
    } else {  // space-as-sep should be handled separately
        sr >> many0(' ') >> read_set_field
           >> many0(many1(' ') >> read_set_field)
           >> many0(' ')
           >> sr_ends;
    }

    if (!sr.good()) {
        ST_FATAL("Invalid format `%s', stopped at `%s'", values, sr.buf());
        return EINVAL;
    }
    
    if (num != field_num()) {
        ST_FATAL("Number of values(%lu) does not match fields(%lu)",
                 num, field_num());
        return EINVAL;
    }
    return 0;
}

std::ostream& operator<< (std::ostream& os, const DynTuple& dt)
{
    if (!dt.valid()) {
        os << "{Nil}";
        return os;
    }
    
    os << '{';
    for (size_t i = 0; i < dt.field_num(); ++i) {
        os << dt.schema_->at_n(i)->name << '=' << dt.at_n(i) << ' ';
    }
    
    const size_t len = dt.schema_->byte_size();
    os << "| " << len << ":";
    for (size_t i = 0; i < len; ++i) {
        int c = ((char*)dt.data_)[i];
        os << std::hex << (c & 0xF) << ((c >> 4) & 0xF) << std::dec;
    }
    os << "}";
    
    return os;
}


}  // namespace st
