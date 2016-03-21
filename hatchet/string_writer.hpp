// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Convert objects to std::string conveniently and efficiently
// Author: gejun@baidu.com
// Date: Mon Aug  2 10:00:17 2010
#pragma once
#ifndef _STRING_WRITER_HPP_
#define _STRING_WRITER_HPP_

#include "debug.h"
#include <stdarg.h>
#include <string>
#include <bsl/string.h>
//#include <bsl/utils/bsl_memcpy.h>

namespace st {
// To write an object into a StringWriter(sw), just write down:
//     sw << obj;
//
// You can write down several objects in one line, const char* and std::string
// are objects as well:
//     std::string test = "be cool";
//     sw << obj1 << obj2 << "hello world" << test;
//
// To make an object useable by <<, implement
// "void to_string(StringWriter& sw) const" in the object:
//     class foo {
//         void to_string(StringWriter& sw) const
//         {
//             sw << "blah blah";
//         }
//     };
//
// If you can't modify the code of the object, say a primitive type
// which is not supported by StringWriter (rarely), specialize operator<<:
//     StringWriter& operator<< (StringWriter& sw, const object_t& val)
//     {
//         sw << "blah blah";
//         return sw;
//     }
// If you want to do printf alike formatting, call append_format(1, ...)
//     sw.append_format ("number[%d]=%d", i, value);
//
// Note: DO NOT send printf style format to "<<", the code generally compiles
//       but does not work as intended.
//       For example:
//       sw << "number[%d]=%d", i, number[i];
//       G++ reads this as "sw.operator<<("number[%d]=%d"), i, number[i]"
//       which writes the format into StringWriter but arguments after
//       are lost because they're just arguments of the comman expression
// If you want to re-format the StringWriter, call format(1, ...)
//    sw.format ("just from %s", "scratch");
//
// You may want to clear the StringWriter directly:
//    sw.clear();
//
// To convert an object to std::string, call show(1)
//    printf ("hello %s\n", show(obj).c_str());

class StringWriter {
public:
    static const int MAX_N_PRINT = 10;
    static const int MIN_CAPACITY = 64;
    static const int GROW_RATIO = 2;

    // Construct a StringWriter
    // Params:
    //   capacity   initial size of interal char array
    explicit StringWriter(const int capacity = MIN_CAPACITY)
    {
        capacity_ = (capacity < MIN_CAPACITY) ? MIN_CAPACITY : capacity;

        b_char_ = ST_NEW_ARRAY (char, capacity_);
        if (NULL == b_char_) {
            ST_FATAL ("Fail to new b_char_");
            capacity_ = 0;
            n_char_ = 0;
        } else {
            n_char_ = 1;
            b_char_[0] = '\0';
        }
        set_sep (',');
    }

    // Destroy this StringWriter
    ~StringWriter()
    {
        ST_DELETE_ARRAY (b_char_);
        capacity_ = 0;
        n_char_ = 0;
    }

    // Construct from another StringWriter
    StringWriter(const StringWriter& other)
    {
        b_char_ = ST_NEW_ARRAY (char, other.capacity_);
        if (NULL == b_char_) {
            ST_FATAL ("Fail to new b_char_");
            return;
        }
        memcpy (b_char_, other.b_char_, other.n_char_);            
        capacity_ = other.capacity_;
        n_char_ = other.n_char_;
    }

    // Assign from another StringWriter
    void operator= (const StringWriter& other)
    {
        if (other.n_char_ > capacity_) {
            char* b_char2 = ST_NEW_ARRAY (char, other.capacity_);
            if (NULL == b_char2) {
                ST_FATAL ("Fail to new b_char2");
                return;
            }
            memcpy (b_char2, other.b_char_, other.n_char_);
            
            if (b_char_) {
                delete [] b_char_;
            }
            b_char_ = b_char2;
            capacity_ = other.capacity_;
            n_char_ = other.n_char_;
        } else {
            memcpy (b_char_, other.b_char_, other.n_char_);
            n_char_ = other.n_char_;
        }
    }
        
    // Write from beginning
    int format (const char* const fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        int len = vformat (fmt, args);
        va_end (args);
        return len;
    }
        
    // Write from beginning, va_list version
    int vformat (const char* const fmt, va_list& args)
    {
        n_char_ = 1;
        return append_vformat (fmt, args);
    }
    
    // Append to StringWriter
    int append_format (const char* const fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        const int len = append_vformat (fmt, args);
        va_end (args);
        return len;
    }

    // Append to StringWriter, va_list version
    int append_vformat (const char* const fmt, va_list& args)
    {
        if (unlikely (NULL == b_char_)) {
            return 0;
        }
            
        int len = 0;
        va_list copy_of_args;
            
        for ( ; ; ) {
            va_copy(copy_of_args, args);

            len = vsnprintf(b_char_+n_char_-1, capacity_-n_char_+1,
                            fmt, copy_of_args);
            if (len >= 0 && len <= (capacity_-n_char_)) {
                n_char_ += len;
                break;
            } else if (grow_capacity()) {
                ST_FATAL ("Fail to grow_capacity, capacity=%d", capacity_);
                return 0;
            }
        }
        return len;
    }

    // Clear string buffer
    void clear ()
    {
        n_char_ = 1;
        b_char_[0] = '\0';
    }

    // Get seperator
    char sep () const { return sep_; }

    // Set seperator
    void set_sep (char sep) { sep_ = sep; }
        
    // Get the content as std::string
    std::string str () const { return std::string (b_char_); }

    // Get the content as const char*, zero overhead
    const char* c_str () const { return b_char_; }
    
    // Number of charaters, not including ending \0
    int length () const { return n_char_-1; }

    // debugging info
    std::string info () const
    {
        char buf[64];
        snprintf (buf, sizeof(buf)/sizeof(char),
                  "length=%d, capacity=%d", n_char_-1, capacity_-1);
        return std::string(buf);
    }
    
    // Overloads
    StringWriter& operator<< (bool val)
    { append_format ("%s", val ? "true" : "false"); return *this; }
        
    StringWriter& operator<< (char val)
    { append_format ("%c", val); return *this; }
    
    StringWriter& operator<< (unsigned char val)
    { append_format ("%u", (unsigned int)val); return *this; }
    
    StringWriter& operator<< (short val)
    { append_format ("%d", (int)val); return *this; }
    
    StringWriter& operator<< (unsigned short val)
    { append_format ("%u", (unsigned int)val); return *this; }
    
    StringWriter& operator<< (int val)
    { append_format ("%d", val); return *this; }
    
    StringWriter& operator<< (unsigned int val)
    { append_format ("%u", val);  return *this; }
    
    StringWriter& operator<< (long val)
    { append_format ("%ld", val); return *this; }
    
    StringWriter& operator<< (unsigned long val)
    { append_format ("%lu", val); return *this; }
    
    StringWriter& operator<< (float val)
    { append_format ("%f", val); return *this; }
    
    StringWriter& operator<< (double val)
    { append_format ("%f", val); return *this; }
    
    StringWriter& operator<< (long long val)
    { append_format ("%lld", val); return *this; }
    
    StringWriter& operator<< (unsigned long long val)
    { append_format ("%llu", val); return *this; }

    StringWriter& operator<< (const char* s)
    { append_format ("%s", s); return *this; }

    StringWriter& operator<< (char* s)
    { append_format ("%s", s); return *this; }

    StringWriter& operator<< (const void* p)
    { append_format ("%p", p); return *this; }

    StringWriter& operator<< (void* p)
    { append_format ("%p", p); return *this; }

    StringWriter& operator<< (const std::string& s)
    { append_format ("%s", s.c_str()); return *this; }
        
    StringWriter& operator<< (const bsl::string& s)
    { append_format ("%s", s.c_str()); return *this; }
    
private:
    int grow_capacity()
    {
        int new_capacity = capacity_ * GROW_RATIO;
        char* new_a_data = ST_NEW_ARRAY (char, new_capacity);
        if (NULL == new_a_data) {
            ST_FATAL ("Fail to new new_a_data");
            return ENOMEM;
        }
        // did not see performance boost when using bsl::xmemcpy
        memcpy (new_a_data, b_char_, n_char_);
        char* old_a_data = b_char_;
        b_char_ = new_a_data;
        capacity_ = new_capacity;

        if (old_a_data) {
            delete [] old_a_data;
        }
        return 0;
    }


    char* b_char_;
    int n_char_;
    int capacity_;
    char sep_;
};
    
template <typename T>
inline StringWriter& operator<< (StringWriter& sw, const T& x)
{
    x.to_string (sw);
    return sw;
}

template <typename T>
inline StringWriter& operator<< (StringWriter& sw, T* x)
{
    if (NULL != x) {
        sw << "&" << *x;
    } else {
        sw << "(void)";
    }
    return sw;
}

template <typename T>
inline StringWriter& operator<< (StringWriter& sw, const T* x)
{
    if (NULL != x) {
        sw << "&" << *x;
    } else {
        sw << "(void)";
    }
    return sw;
}

template <typename K, typename V>
inline StringWriter& operator<< (StringWriter& sw, const std::pair<K,V>& p)
{
    sw << "(" << p.first << "," << p.second << ")";
    return sw;
}


// template <typename T> struct shows_f {
//     shows_f (char sep=' ')
//     { sep_ = sep; }

//     inline StringWriter& operator() (StringWriter& sw, const T& x) const
//     {
//         sw << x << sep_;
//         return sw;
//     }

//     char sep_;
// };


template <typename T> inline std::string show (const T& x)
{
    StringWriter sw;
    sw << x;
    return sw.str();
}

template <typename _InputIterator>
inline int shows_range (StringWriter& sw,
                        const _InputIterator& it_b,
                        const _InputIterator& it_e,
                        int max_n_print = StringWriter::MAX_N_PRINT)
{
    if (max_n_print < 2) {
        max_n_print = 2;
    }
        
    bool need_sep = false;
    int cnt = 0;
    sw << "[";
    for (_InputIterator it=it_b; it!=it_e; ++it, ++cnt) {
        if (cnt >= max_n_print) {
            sw << "...";
            for (++it, ++cnt; it!=it_e; ++it, ++cnt);
            break;
        }

        if (need_sep) {
            sw << ' ';
        } else {
            need_sep = true;
        }
                
        sw << *it;
    }
    sw << "]:" << cnt;
    return cnt;
}

template <typename _InputIterator, typename _T>
inline int shows_range_compact (StringWriter& sw,
                                const _InputIterator& it_b,
                                const _InputIterator& it_e,
                                const _T& maj_val,
                                int max_n_print=StringWriter::MAX_N_PRINT)
{
    if (max_n_print < 2) {
        max_n_print = 2;
    }
    bool need_sep = false;
    int nn_cnt = 0;
    int cnt = 0;
    sw << "[";
    for (_InputIterator it=it_b; it != it_e;) {
        if (need_sep) { 
            sw << ' '; 
        } else { 
            need_sep = true; 
        }

        if (maj_val == *it) {
            _InputIterator it2 = it;
            int c = 0;
            for (++it2; it2 != it_e && maj_val == *it2; ++it2, ++c);
            if (c > 0) {
                sw << maj_val << "{" << c+1 << "}";
            } else {
                sw << maj_val;
            }
            it = it2;
            cnt += c+1;
        } else {
            if (nn_cnt >= max_n_print) {
                sw << "...";
                for (++it, ++cnt; it != it_e; ++it, ++cnt);
                break;
            }
            sw << *it;
            ++ nn_cnt;
            ++ it;
            ++ cnt;
        }
    }
    sw << "]:" << cnt;
    return cnt;
}

    
template <typename _InputIterator>
inline std::string show (const _InputIterator& it_b,
                         const _InputIterator& it_e,
                         int max_n_print=StringWriter::MAX_N_PRINT)
{
    StringWriter sw;
    shows_range (sw, it_b, it_e, max_n_print);
    return sw.str();
}

template <typename T>
inline std::string show_container
(const T& c, int max_n_print=StringWriter::MAX_N_PRINT)
{ return show (c.begin(), c.end(), max_n_print); }


// helper FO to print an attribute into a _Stream
template <typename _Stream> struct show_with_pos {
    explicit show_with_pos (_Stream* p_sw) : p_sw_(p_sw) {}
    
    template <typename _T> int operator() (int pos, const _T& a) const
    {
        if (0 != pos) {
            *p_sw_ << ' ';
        }
        *p_sw_ << a;
        return pos+1;
    }
        
private:
    _Stream* p_sw_;
};

}
#endif /* _STRING_WRITER_HPP_ */
