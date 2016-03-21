// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Implement of operator>>of string_reader_t
// Author: gejun@baidu.com

#include "string_reader.hpp"
#include <errno.h>

namespace st {

std::ostream& operator<<(std::ostream& os, const StringReader& sr)
{
    return os << "StringReader_" << sr.good_ << '_' << sr.logging_ << "{"
              << sr.p_buf_ << '}';
}

StringReader& Sentry::operator()(StringReader& sr) const
{
    std::cout << "Sentry(" << desc_ << ")=" << sr << std::endl;
    return sr;
}

template <typename _T>
inline StringReader& read_integer_from_reader(
    StringReader& sr, _T* val)
{
    if (sr.good()) {
        const char* head = sr.buf();
        _T sign = 1;
        
        if (*head == '-') {
            sign = -1;
            ++ head;
        } else if (*head == '+') {
            ++ head;
        }

        const char* start = head;
        _T r = 0;
        for ( ; *head >= '0' && *head <= '9'; ++head) {
            r = r * 10 + static_cast<_T>(*head - '0');
        }
        
        if (head != start) {
            *val = sign * r;
            sr.set_buf(head);
        } else {
            sr.set_status(false);
        }
    }
    return sr;
}

template <typename _T>
inline StringReader& read_unsigned_integer_from_reader(
    StringReader& sr, _T* val)
{
    if (sr.good()) {
        const char* head = sr.buf();
        
        if (*head == '+') {
            ++ head;
        }

        const char* start = head;
        _T r = 0;
        for ( ; *head >= '0' && *head <= '9'; ++head) {
            r = r * 10 + static_cast<_T>(*head - '0');
        }
        
        if (head != start) {
            *val = r;
            sr.set_buf(head);
        } else {
            sr.set_status(false);
        }
    }
    return sr;
}


StringReader& StringReader::operator>>(bool* val)
{
    if (good()) {
        bool prev_logging = logging_;
        logging_ = false;
        
        *this >> "true";
        if (good()) {
            *val = true;
            return *this;
        }

        set_status(true);
        *this >> "TRUE";
        if (good()) {
            *val = true;
            return *this;
        }

        set_status(true);
        *this >> "false";
        if (good()) {
            *val = false;
            return *this;
        }

        set_status(true);
        *this >> "FALSE";
        if (good()) {
            *val = false;
            return *this;
        }

        logging_ = prev_logging;
        if (logging_) {
            ST_WARN("Expect `true' or `TRUE' or `false' or `FALSE', Actually `%*s'",
                    (int)strnlen(p_buf_, 4), p_buf_);
        }
    }
    return *this;
}
        
StringReader& StringReader::operator>>(short* val)
{
    return read_integer_from_reader(*this, val);
}

StringReader& StringReader::operator>>(unsigned short* val)
{
    return read_unsigned_integer_from_reader(*this, val);
}
        
StringReader& StringReader::operator>>(int* val)
{
    return read_integer_from_reader(*this, val);
}

StringReader& StringReader::operator>>(unsigned int* val)
{
    return read_unsigned_integer_from_reader(*this, val);
}

StringReader& StringReader::operator>>(long* val)
{
    return read_integer_from_reader(*this, val);
}

StringReader& StringReader::operator>>(unsigned long* val)
{
    return read_unsigned_integer_from_reader(*this, val);
}

StringReader& StringReader::operator>>(long long* val)   
{
    return read_integer_from_reader(*this, val);
}
        
StringReader& StringReader::operator>>(unsigned long long* val)
{
    return read_unsigned_integer_from_reader(*this, val);
}

StringReader& StringReader::operator>>(float* val)
{
    if (good()) {
        const char first = *p_buf_;
        if (first && !isspace(first)) {
            char* endptr = NULL;
                
            errno = 0;
            const float f = strtof(p_buf_, &endptr);
            if (0 == errno) {
                *val = f;
                p_buf_ = endptr;
                return *this;
            }
        }
        set_status(false);
    }
    return *this;
}
        
StringReader& StringReader::operator>>(double* val)
{
    if (good()) {
        const char first = *p_buf_;
        if (first && !isspace(first)) {
            char* endptr = NULL;
                
            errno = 0;
            const double f = strtod(p_buf_, &endptr);
            if (0 == errno) {
                *val = f;
                p_buf_ = endptr;
                return *this;
            }
        }
        set_status(false);
    }
    return *this;
}

}
