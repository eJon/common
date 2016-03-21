#ifndef ST_BOOST_SMART_PTR_DETAIL_SHARED_COUNT_HPP_INCLUDED
#define ST_BOOST_SMART_PTR_DETAIL_SHARED_COUNT_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  detail/shared_count.hpp
//
//  Copyright (c) 2001, 2002, 2003 Peter Dimov and Multi Media Ltd.
//  Copyright 2004-2005 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifdef __BORLANDC__
# pragma warn -8027     // Functions containing try are not expanded inline
#endif

#include "st_sp_counted_base.hpp"
#include "st_sp_counted_impl.hpp"

#include <functional>       // std::less
#include <new>              // std::bad_alloc

namespace st_boost
{

namespace detail
{

#if defined(BOOST_SP_ENABLE_DEBUG_HOOKS)

int const shared_count_id = 0x2C35F101;
int const   weak_count_id = 0x298C38A4;

#endif

struct sp_nothrow_tag {};

class weak_count;

class shared_count
{
private:

    sp_counted_base * pi_;

#if defined(BOOST_SP_ENABLE_DEBUG_HOOKS)
    int id_;
#endif

    friend class weak_count;

public:

    shared_count(): pi_(0) // nothrow
#if defined(BOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
    {
    }

    template<class Y> explicit shared_count( Y * p ): pi_( 0 )
#if defined(BOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
    {

        try
        {
            pi_ = new sp_counted_impl_p<Y>( p );
        }
        catch(...)
        {
            delete p;
        }
    }

    template<class P, class D> shared_count( P p, D d ): pi_(0)

#if defined(BOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
    {

        try
        {
            pi_ = new sp_counted_impl_pd<P, D>(p, d);
        }
        catch(...)
        {
            d(p); // delete p
        }

    }

    template<class P, class D, class A> shared_count( P p, D d, A a ): pi_( 0 )
#if defined(BOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
    {
        typedef sp_counted_impl_pda<P, D, A> impl_type;
        typedef typename A::template rebind< impl_type >::other A2;

        A2 a2( a );

        try
        {
            pi_ = a2.allocate( 1, static_cast< impl_type* >( 0 ) );
            new( static_cast< void* >( pi_ ) ) impl_type( p, d, a );
        }
        catch(...)
        {
            d( p );

            if( pi_ != 0 )
            {
                a2.deallocate( static_cast< impl_type* >( pi_ ), 1 );
            }
        }

    }

    ~shared_count() // nothrow
    {
        if( pi_ != 0 ) pi_->release();
#if defined(BOOST_SP_ENABLE_DEBUG_HOOKS)
        id_ = 0;
#endif
    }

    shared_count(shared_count const & r): pi_(r.pi_) // nothrow
#if defined(BOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
    {
        if( pi_ != 0 ) pi_->add_ref_copy();
    }


    shared_count( weak_count const & r, sp_nothrow_tag ); // constructs an empty *this when r.use_count() == 0

    shared_count & operator= (shared_count const & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;

        if( tmp != pi_ )
        {
            if( tmp != 0 ) tmp->add_ref_copy();
            if( pi_ != 0 ) pi_->release();
            pi_ = tmp;
        }

        return *this;
    }

    void swap(shared_count & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;
        r.pi_ = pi_;
        pi_ = tmp;
    }

    long use_count() const // nothrow
    {
        return pi_ != 0? pi_->use_count(): 0;
    }

    bool unique() const // nothrow
    {
        return use_count() == 1;
    }

    bool empty() const // nothrow
    {
        return pi_ == 0;
    }

    friend inline bool operator==(shared_count const & a, shared_count const & b)
    {
        return a.pi_ == b.pi_;
    }

    friend inline bool operator<(shared_count const & a, shared_count const & b)
    {
        return std::less<sp_counted_base *>()( a.pi_, b.pi_ );
    }

};


class weak_count
{
private:

    sp_counted_base * pi_;

#if defined(BOOST_SP_ENABLE_DEBUG_HOOKS)
    int id_;
#endif

    friend class shared_count;

public:

    weak_count(): pi_(0) // nothrow
#if defined(BOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(weak_count_id)
#endif
    {
    }

    weak_count(shared_count const & r): pi_(r.pi_) // nothrow
#if defined(BOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(weak_count_id)
#endif
    {
        if(pi_ != 0) pi_->weak_add_ref();
    }

    weak_count(weak_count const & r): pi_(r.pi_) // nothrow
#if defined(BOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(weak_count_id)
#endif
    {
        if(pi_ != 0) pi_->weak_add_ref();
    }

// Move support

#if defined( BOOST_HAS_RVALUE_REFS )

    weak_count(weak_count && r): pi_(r.pi_) // nothrow
#if defined(BOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(weak_count_id)
#endif
    {
        r.pi_ = 0;
    }

#endif

    ~weak_count() // nothrow
    {
        if(pi_ != 0) pi_->weak_release();
#if defined(BOOST_SP_ENABLE_DEBUG_HOOKS)
        id_ = 0;
#endif
    }

    weak_count & operator= (shared_count const & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;

        if( tmp != pi_ )
        {
            if(tmp != 0) tmp->weak_add_ref();
            if(pi_ != 0) pi_->weak_release();
            pi_ = tmp;
        }

        return *this;
    }

    weak_count & operator= (weak_count const & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;

        if( tmp != pi_ )
        {
            if(tmp != 0) tmp->weak_add_ref();
            if(pi_ != 0) pi_->weak_release();
            pi_ = tmp;
        }

        return *this;
    }

    void swap(weak_count & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;
        r.pi_ = pi_;
        pi_ = tmp;
    }

    long use_count() const // nothrow
    {
        return pi_ != 0? pi_->use_count(): 0;
    }

    bool empty() const // nothrow
    {
        return pi_ == 0;
    }

    friend inline bool operator==(weak_count const & a, weak_count const & b)
    {
        return a.pi_ == b.pi_;
    }

    friend inline bool operator<(weak_count const & a, weak_count const & b)
    {
        return std::less<sp_counted_base *>()(a.pi_, b.pi_);
    }
};

inline shared_count::shared_count( weak_count const & r, sp_nothrow_tag ): pi_( r.pi_ )
#if defined(BOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
{
    if( pi_ != 0 && !pi_->add_ref_lock() )
    {
        pi_ = 0;
    }
}

} // namespace detail

} // namespace st_boost

#ifdef __BORLANDC__
# pragma warn .8027     // Functions containing try are not expanded inline
#endif

#endif  // #ifndef ST_BOOST_SMART_PTR_DETAIL_SHARED_COUNT_HPP_INCLUDED
