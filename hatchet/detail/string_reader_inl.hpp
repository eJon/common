// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Inline implementations of string_reader.hpp
// Author: gejun@baidu.com
// Date: Mon Aug  2 10:00:17 2010

namespace st {

StringReader& sr_ends(StringReader& sr)
{
    if (sr.good()) {
        sr.set_status(!*sr.buf());
    }
    return sr;
}

StringReader& sr_endl(StringReader& sr)
{
    return sr >> '\n';
}

// Note: "" (with a single \0) should never fail
StringReader& StringReader::operator>>(const char* s)
{
    if (good()) {
        const char* head = p_buf_;
        const char* s2 = s;
        for ( ; *s2 && *head == *s2 && *head; ++head, ++s2);
        
        if (*s2 == '\0') {
            p_buf_ += s2 - s;
        } else {
            set_status(false);
            if (logging_) {
                const int n = (int)strnlen(s2, 4);
                ST_WARN("Expect prefix `%*s', acutally `%*s'", n, s2, n, head);
            }
        }
    }
    return *this;            
}

StringReader& StringReader::operator>>(char c)
{
    if (good()) {
        if (*p_buf_ == c) {
            ++ p_buf_;
        } else {
            set_status(false);
        }
    }
    return *this;
}


template <typename _M0, typename _M1>
StringReader& Seq<_M0, _M1>::operator()(StringReader& sr) const
{
    if (sr.good()) {
        typename SideEffectManager<_M0>::State s;
        SideEffectManager<_M0>::get_state(m0_, &s);
        sr >> m0_;
        if (sr.good()) {
            sr >> m1_;
            if (!sr.good()) {
                SideEffectManager<_M0>::set_state(m0_, s);
            }
        }
    }
    return sr;
}

template <typename _M0, typename _M1>
struct SideEffectManager<Seq<_M0, _M1> > {
    typedef std::pair<typename SideEffectManager<_M0>::State,
                      typename SideEffectManager<_M1>::State> State;
    
    static void get_state(const Seq<_M0, _M1>& s, State* t)
    {
        SideEffectManager<_M0>::get_state(s.m0_, &t->first);
        SideEffectManager<_M1>::get_state(s.m1_, &t->second);
    }
    static void set_state(const Seq<_M0, _M1>& s, const State& t)
    {
        SideEffectManager<_M0>::set_state(s.m0_, t.first);
        SideEffectManager<_M1>::set_state(s.m1_, t.second);
    }
};


template <typename _M>
StringReader& Many<_M>::operator()(StringReader& sr) const
{
    if (sr.good()) {
        int n = 0;
        bool prev_logging = sr.turn_off_logging();
        for (; n < max_times_; ++n) {
            StringReader bak = sr;
            sr >> m0_;
            if (!sr.good()) {
                sr = bak;
                break;
            }
        }
        sr.set_logging(prev_logging);
        if (n < min_times_) {
            sr.set_status(false);
            if (sr.logging()) {
                ST_WARN("(Many) Expect at least %d times, acutally %d",
                        min_times_, n);
            }
        }
    }
    return sr;
}

template <typename _M0>
struct SideEffectManager<Many<_M0> > {
    typedef typename SideEffectManager<_M0>::State State;

    static void get_state(const Many<_M0>& s, State* t)
    { SideEffectManager<_M0>::get_state(s.m0_, t); }
    
    static void set_state(const Many<_M0>& s, const State& t)
    { SideEffectManager<_M0>::set_state(s.m0_, t); }
};

template <typename _M0>
struct SideEffectManager<Once<_M0> > {
    typedef typename SideEffectManager<_M0>::State State;

    static void get_state(const Once<_M0>& s, State* t)
    { SideEffectManager<_M0>::get_state(s.m0_, t); }
    
    static void set_state(const Once<_M0>& s, const State& t)
    { SideEffectManager<_M0>::set_state(s.m0_, t); }
};

template <typename _M0, typename _M1>
StringReader& Either<_M0, _M1>::operator()(StringReader& sr) const
{
    if (sr.good()) {
        StringReader bak = sr;
        sr.turn_off_logging();
        sr >> m0_;
        if (!sr.good()) {
            sr = bak;
            sr >> m1_;
        }
    }
    return sr;
}

template <typename _M0, typename _M1>
struct SideEffectManager<Either<_M0, _M1> > {
    typedef std::pair<typename SideEffectManager<_M0>::State,
                      typename SideEffectManager<_M1>::State> State;

    static void get_state(const Either<_M0, _M1>& s, State* t)
    {
        SideEffectManager<_M0>::get_state(s.m0_, &t->first);
        SideEffectManager<_M1>::get_state(s.m1_, &t->second);
    }

    static void set_state(const Either<_M0, _M1>& s, const State& t)
    {
        SideEffectManager<_M0>::set_state(s.m0_, t.first);
        SideEffectManager<_M1>::set_state(s.m1_, t.second);
    }
};


template <typename _M0>
StringReader& Either<_M0, void>::operator()(StringReader& sr) const
{
    if (sr.good()) {
        StringReader bak = sr;
        sr.turn_off_logging();
        sr >> m0_;
        if (!sr.good()) {
            sr = bak;
        }
    }
    return sr;
}

template <typename _M0>
struct SideEffectManager<Either<_M0, void> > {
    typedef typename SideEffectManager<_M0>::State State;
    
    static void get_state(const Either<_M0, void>& s, State* t)
    { SideEffectManager<_M0>::get_state(s.m0_, t); }
    
    static void set_state(const Either<_M0, void>& s, const State& t)
    { SideEffectManager<_M0>::set_state(s.m0_, t); }
};

template <typename _T>
StringReader& PushBack<_T>::operator()(StringReader& sr) const
{
    if (sr.good()) {
        v_->push_back(*t_);
    }
    return sr;
}

template <typename _T>
struct SideEffectManager<PushBack<_T> > {
    typedef size_t State;
    static void get_state(const PushBack<_T>& s, State* t) { *t = s.v_->size(); }
    static void set_state(const PushBack<_T>& s, const State& t)
    {
        if (t < s.v_->size()) {
            s.v_->resize(t);
            return;
        }
        ST_FATAL("(PushBack) restored size(%lu) is bigger than current size(%lu)",
                 t, s.v_->size());
    }
};

template <typename _T>
StringReader& ReadPush<_T>::operator()(StringReader& sr) const
{
    if (sr.good()) {
        _T tmp;
        sr >> &tmp;
        if (sr.good()) {
            v_->push_back(tmp);
        }
    }
    return sr;
}

template <typename _T>
struct SideEffectManager<ReadPush<_T> > {
    typedef size_t State;
    static void get_state(const ReadPush<_T>& s, State* t) { *t =  s.v_->size(); }
    static void set_state(const ReadPush<_T>& s, const State& t)
    {
        if (t < s.v_->size()) {
            s.v_->resize(t);
            return;
        }
        ST_FATAL("(ReadPush) restored size(%lu) is bigger than current size(%lu)",
                 t, s.v_->size());
    }
};

template <class _P, class _U>
StringReader& ReadIf<_P, _U>::operator()(StringReader& sr) const
{
    if (sr.good()) {
        const char* b = sr.buf();
        for ( ; pred_(*b) && *b; ++b);
        if (b != sr.buf()) {
            sr.set_status(usage_(sr.buf(), b));
            sr.set_buf(b);
        } else {
            sr.set_status(false);
        }
    }
    return sr;
}

template <class _P, class _U> struct SideEffectManager<ReadIf<_P, _U> > {
    typedef typename SideEffectManager<_U>::State State;

    static void get_state(const ReadIf<_P, _U>& s, State* t)
    { SideEffectManager<_U>::get_state(s.usage_, t); }
    
    static void set_state(const ReadIf<_P, _U>& s, const State& t)
    { SideEffectManager<_U>::set_state(s.usage_, t); }
};


template <typename _T> struct SideEffectManager<IncValue<_T> > {
    typedef _T State;
    static void get_state(const IncValue<_T>& s, State* t) { *t = *s.d_; }
    static void set_state(const IncValue<_T>& s, const State& t) { *s.d_ = t; }
};

}  // namespace st
