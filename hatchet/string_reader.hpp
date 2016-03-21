// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// A combinator parser in C++
// Author: gejun@baidu.com
// Date: Mon Aug  2 10:00:17 2010
#pragma once
#ifndef _STRING_READER_HPP_
#define _STRING_READER_HPP_

#include <vector>                // std::vector
#include <ostream>               // std::ostream
#include "common.h"

namespace st {

class StringReader {
public:
    explicit StringReader(const char* p_buf)
        : p_buf_(p_buf), good_(true), logging_(true) {}
    ~StringReader() {}
        
    inline StringReader& operator>>(const char*);
    inline StringReader& operator>>(char);

    // true|TRUE|false|FALSE
    StringReader& operator>>(bool*);

    // [-+]{0,1}[0-9]+
    StringReader& operator>>(short*);
    StringReader& operator>>(int*);
    StringReader& operator>>(long*);
    StringReader& operator>>(long long*);

    // +{0,1}[0-9]+
    StringReader& operator>>(unsigned short*);
    StringReader& operator>>(unsigned int*);
    StringReader& operator>>(unsigned long*);
    StringReader& operator>>(unsigned long long*);

    StringReader& operator>>(float*);
    StringReader& operator>>(double*);
    
    StringReader& operator>>(StringReader& (*m)(StringReader&)) { return m(*this); }
    template <typename M> StringReader& operator>>(const M& m) { return m(*this); }

    const char* buf() const            { return p_buf_; }
    void set_buf(const char* buf)      { p_buf_ = buf; }
    void forward_buf(size_t steps)     { p_buf_ += steps; }

    bool good() const                  { return good_; }
    void set_status(bool g)            { good_ = g; }

    bool logging() const               { return logging_; }
    void set_logging(bool b)           { logging_ = b; }
    bool turn_off_logging()
    {
        const bool tmp = logging_;
        logging_ = false;
        return tmp;
    }

friend std::ostream& operator<<(std::ostream&, const StringReader&);
    
private:
    const char* p_buf_;
    bool good_;
    bool logging_;
};

// Require `sr' ends (with \0)
inline StringReader& sr_ends(StringReader& sr);

// Require `sr' ends with newline
inline StringReader& sr_endl(StringReader& sr);

// Denote `_expr_' with `_name_' so that `_name_' could replace
// `_expr_' in a bigger manipulator composition
#define HAVE_MANIPULATOR(_name_, _expr_)        \
    typeof(_expr_) _name_ = (_expr_);

template <typename T0, typename T1> class Seq;
template <typename _M0, typename _M1 = void> class Either;

namespace aux {
struct ZeroSized {
    char dummy[];
};
}

template <typename _M> struct SideEffectManager {
    typedef aux::ZeroSized State;
    static void get_state(const _M&, State*) {}
    static void set_state(const _M&, const State&) {}
};

template <typename _T> struct BaseManipulator {
    template <typename _M2>                             
    Seq<_T, _M2> operator>>(_M2 m) const   
    {                                                   
        return Seq<_T, _M2>(*(_T*)(this), m);        
    }

    template <typename _M2>                             
    Either<_T, _M2> operator|(_M2 m) const   
    {                                                   
        return Either<_T, _M2>(*(_T*)(this), m);        
    }
};

// Run two manipulators sequentially, fail if any manipulator failed
template <typename _M0, typename _M1>
class Seq : public BaseManipulator<Seq<_M0, _M1> > {
friend struct SideEffectManager<Seq<_M0, _M1> >;
public:
    Seq(const _M0& m0, const _M1& m1) : m0_(m0), m1_(m1) {}
    inline StringReader& operator()(StringReader& sr) const;
    
private:
    _M0 m0_;
    _M1 m1_;
};

// Run a manipulator once, fail if the manipulator failed
// It's generally used for wrapping pointers/strings to have operator<<
template <typename _M>
class Once : public BaseManipulator<Once<_M> > {
friend struct SideEffectManager<Once<_M> >;
public:
    explicit Once(_M m0) : m0_(m0) {}
    StringReader& operator()(StringReader& sr) const { return sr >> m0_; }
private:
    _M m0_;
};

template <typename _M> Once<_M> once(_M s) { return Once<_M>(s); }

// Run a manipulator for many times, fail if running times is not
// bound in [min_times, max_times]
template <typename _M>
class Many : public BaseManipulator<Many<_M> > {
friend struct SideEffectManager<Many<_M> >;
public:
    Many(_M m, int min_times, int max_times)
        : m0_(m), min_times_(min_times), max_times_(max_times) {}
    StringReader& operator()(StringReader& sr) const;
private:
    _M m0_;
    int min_times_;
    int max_times_;
};

// Do nothing or at most `max_times' times, never fail
template <typename T> Many<T> many0(T v, int max_times = INT_MAX)
{ return Many<T>(v, 0, max_times); }

template <typename T> Many<T> many1(T v, int max_times = INT_MAX)
{ return Many<T>(v, 1, max_times); }

template <typename T> Many<T> repeat(int times, T v)
{ return Many<T>(v, times, times); }

// Run _M0 first, run _M1 *only* if _M0 failed
// fail if manipulators both failed
template <typename _M0, typename _M1>
class Either : public BaseManipulator<Either<_M0, _M1> > {
friend struct SideEffectManager<Either<_M0, _M1> >;
public:
    Either(_M0 m0, _M1 m1) : m0_(m0), m1_(m1) {}
    inline StringReader& operator()(StringReader& sr) const;
private:
    _M0 m0_;
    _M1 m1_;
};

template <typename _M0>
class Either<_M0, void> : public BaseManipulator<Either<_M0, void> > {
friend struct SideEffectManager<Either<_M0, void> >;
public:
    explicit Either(_M0 m0) : m0_(m0) {}
    inline StringReader& operator()(StringReader& sr) const;
private:
    _M0 m0_;
};

template <typename T0, typename T1>
Either<T0, T1> either(T0 v0, T1 v1) { return Either<T0, T1>(v0, v1); }

template <typename T0>
Either<T0> maybe(T0 v0) { return Either<T0>(v0); }


// Simply print something, never fail
class Sentry : public BaseManipulator<Sentry> {
public:
    explicit Sentry(const char* desc) : desc_(desc) {}
    StringReader& operator()(StringReader& sr) const;
private:    
    const char* desc_;
};

inline Sentry sentry(const char* desc) { return Sentry(desc); }

// Push a _T into a list, never fail
template <typename _T>
class PushBack : public BaseManipulator<PushBack<_T> > {
friend struct SideEffectManager<PushBack<_T> >;
public:
    PushBack(_T* t, std::vector<_T>* v) : t_(t), v_(v) {}
    inline StringReader& operator()(StringReader& sr) const;
private:
    _T* t_;
    std::vector<_T>* v_;
};

template <typename T> PushBack<T> sr_push_back(T& t, std::vector<T>& v)
{ return PushBack<T>(&t, &v); }

// Read a _T and push into a list
// fail if parsing _T failed
template <typename _T>
class ReadPush : public BaseManipulator<ReadPush<_T> > {
friend struct SideEffectManager<ReadPush<_T> >;
public:
    explicit ReadPush(std::vector<_T>* v) : v_(v) {}
    inline StringReader& operator()(StringReader& sr) const;
private:
    std::vector<_T>* v_;
};

template <typename _T> ReadPush<_T> read_push(std::vector<_T>& v)
{ return ReadPush<_T>(&v); }

namespace aux {
struct DoNothing {
    bool operator()(const char*, const char*) const { return true; }
};

class CopyChars {
public:
    explicit CopyChars(char* d) : d_(d) {}
    
    bool operator()(const char* buf_begin, const char* buf_end) const
    {
        char* head = d_;
        for (const char* b = buf_begin; b != buf_end; *head++ = *b++);
        *head = '\0';
        return true;
    }

private:
    char* d_;
};

class CopyCharsStr {
public:
    explicit CopyCharsStr(std::string* d) : d_(d) {}
    
    bool operator()(const char* buf_begin, const char* buf_end) const
    {
        d_->assign(buf_begin, buf_end - buf_begin);
        return true;
    }

private:
    std::string* d_;
};

class CopyCharsMutable {
public:
    explicit CopyCharsMutable(char* d) : d_(d) {}
    
    bool operator()(const char* buf_begin, const char* buf_end) const
    {
        for (const char* b = buf_begin; b != buf_end; *d_++ = *b++);
        *d_++ = '\0';
        return true;
    }

private:
    mutable char* d_;
};

class OneOfChars {
public:
    explicit OneOfChars(const char* cs) : cs_(cs) {}
    
    bool operator()(char c) const
    {
        for (const char* b = cs_; *b; ++b) {
            if (c == *b) {
                return true;
            }
        }
        return false;
    }

private:
    const char* cs_;
};

struct IsChar {
    explicit IsChar(char c) : c_(c) {}
    bool operator()(const char c) const { return c == c_; }
    char c_;
};

struct IsNameChar {
    int operator()(const int c) const
    {
        // "return isalpha(c) || isdigit(c) || '_' == c;" is 17% slower
        return ((c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') ||
                '_' == c);
    }
};

template <class _P>
struct NotP {
    explicit NotP(_P pred) : pred_(pred) {}
    bool operator()(char c) const { return !pred_(c); }
private:
    _P pred_;
};

}

// Read alphabets and send to `_U' which should implement:
// "bool operator()(const char* buf_begin, const char* buf_end) const;"
template <class _P, class _U>
class ReadIf : public BaseManipulator<ReadIf<_P, _U> > {
friend struct SideEffectManager<ReadIf<_P, _U> >;
public:
    ReadIf(_P pred, const _U& usage) : pred_(pred), usage_(usage) {}
    inline StringReader& operator()(StringReader& sr) const;
private:
    _P pred_;
    _U usage_;
};

template <class _P> ReadIf<_P, aux::CopyChars>
read_if1(_P pred, char* buf)
{ return ReadIf<_P, aux::CopyChars>(pred, aux::CopyChars(buf)); }

template <class _P> ReadIf<_P, aux::CopyCharsStr>
read_if1(_P pred, std::string* buf)
{ return ReadIf<_P, aux::CopyCharsStr>(pred, aux::CopyCharsStr(buf)); }

template <class _P, class _U> ReadIf<_P, _U>
read_if1(_P pred, const _U& usage)
{ return ReadIf<_P, _U>(pred, usage); }

inline ReadIf<aux::IsNameChar, aux::CopyChars>
read_name1(char* buf)
{ return ReadIf<aux::IsNameChar, aux::CopyChars>(
        aux::IsNameChar(), aux::CopyChars(buf)); }

inline ReadIf<aux::IsNameChar, aux::CopyCharsStr>
read_name1(std::string* buf)
{ return ReadIf<aux::IsNameChar, aux::CopyCharsStr>(
        aux::IsNameChar(), aux::CopyCharsStr(buf)); }

template <class _U> ReadIf<aux::IsNameChar, _U>
read_name1(const _U& fn)
{ return ReadIf<aux::IsNameChar, _U>(aux::IsNameChar(), fn); }

inline ReadIf<aux::OneOfChars, aux::DoNothing>
skip0(const char* cs)
{ return ReadIf<aux::OneOfChars, aux::DoNothing>(
        aux::OneOfChars(cs), aux::DoNothing()); }

// Copy characters to `buf' before meeting `stop_chars'
template <class _P> ReadIf<aux::NotP<_P>, aux::CopyChars>
read_until1(_P pred, char* buf)
{ return ReadIf<aux::NotP<_P>, aux::CopyChars>(
        aux::NotP<_P>(pred), aux::CopyChars(buf)); }

template <class _P, class _U> ReadIf<aux::NotP<_P>, _U>
read_until1(_P pred, const _U& usage)
{ return ReadIf<aux::NotP<_P>, _U>(aux::NotP<_P>(pred), usage); }

inline ReadIf<aux::NotP<aux::OneOfChars>, aux::CopyChars>
read_until1(const char* cs, char* buf)
{ return ReadIf<aux::NotP<aux::OneOfChars>, aux::CopyChars>(
        aux::NotP<aux::OneOfChars>(aux::OneOfChars(cs)), aux::CopyChars(buf)); }

template <class _U> ReadIf<aux::NotP<aux::OneOfChars>, _U>
read_until1(const char* cs, const _U& usage)
{ return ReadIf<aux::NotP<aux::OneOfChars>, _U>(
        aux::NotP<aux::OneOfChars>(aux::OneOfChars(cs)), usage); }

inline ReadIf<aux::NotP<aux::IsChar>, aux::CopyChars>
read_until1(char c, char* buf)
{ return ReadIf<aux::NotP<aux::IsChar>, aux::CopyChars>(
        aux::NotP<aux::IsChar>(aux::IsChar(c)), aux::CopyChars(buf)); }

template <class _U> ReadIf<aux::NotP<aux::IsChar>, _U>
read_until1(char c, const _U& usage)
{ return ReadIf<aux::NotP<aux::IsChar>, _U>(
        aux::NotP<aux::IsChar>(aux::IsChar(c)), usage); }

// Set `v' into `*d', never fail
template <typename _T>
class SetValue : public BaseManipulator<SetValue<_T> > {
public:
    explicit SetValue(_T* d, const _T& v) : d_(d), v_(v) {}
    StringReader& operator()(StringReader& sr) const
    {
        if (sr.good()) {
            *d_ = v_;
        }
        return sr;
    }
private:
    _T* d_;
    _T v_;
};

template <typename _T> SetValue<_T> sr_set_value(_T* d, const _T& v)
{ return SetValue<_T>(d, v); }

// Increase `*d' by one, never fail
template <typename _T>
class IncValue : public BaseManipulator<IncValue<_T> > {
friend struct SideEffectManager<IncValue<_T> >;
public:
    explicit IncValue(_T* d) : d_(d) {}
    StringReader& operator()(StringReader& sr) const
    {
        if (sr.good()) {
            ++*d_;
        }
        return sr;
    }
private:
    
    _T* d_;
};

template <typename _T> IncValue<_T> sr_inc_value(_T* d)
{ return IncValue<_T>(d); }

}  // namespace st

#include "detail/string_reader_inl.hpp"

#endif // _STRING_READER_HPP_
