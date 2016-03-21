// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Send and receive events
// Author: gejun@baidu.com
// Date: Sun Nov  7 21:43:34 CST 2010
#pragma once
#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

#include <vector>   // std::vector

namespace st {
// This is a simple implementation of Observer Pattern. Example: we want to
// notify subscribers to an event with some data. First we should
// instantiate a `Event<int, const Foo*>' as source of events (`int' and
// `const Foo*' are types of data in this case), let's call it `foo_event';
// Next we add a class `FooObserver' inheriting BaseObserver<int, const Foo*>
// to implement `void on_event(int, const Foo*)' to handle coming events,
// instantiate as foo_ob and call `foo_event.subscribe(&foo_ob)' to
// register the observer.
// OK, foo_event.notify(int, const Foo*) should call `on_event' of the
// observer now. We may call `foo_event.unsubscribe(&foo_ob)' to stop
// receiving events.

// Base class of event handlers, at most 3 parameters
template <typename _A1 = void,
          typename _A2 = void,
          typename _A3 = void> class BaseObserver;

// Event, at most 3 parameters
template <typename _A1 = void,
          typename _A2 = void,
          typename _A3 = void> class Event;


template <> class BaseObserver<void, void, void> {
public:
    virtual ~BaseObserver () {}
    virtual void on_event () = 0;
};

template <typename _A1> class BaseObserver<_A1, void, void> {
public:
    virtual ~BaseObserver () {}
    virtual void on_event (_A1) = 0;
};

template <typename _A1, typename _A2> class BaseObserver<_A1, _A2, void> {
public:
    virtual ~BaseObserver () {}
    virtual void on_event (_A1, _A2) = 0;
};

template <typename _A1, typename _A2, typename _A3> class BaseObserver {
public:
    virtual ~BaseObserver () {}
    virtual void on_event (_A1, _A2, _A3) = 0;
};

// Manage a list of observers listening to a table
template <typename _A1, typename _A2, typename _A3> class Event {
    typedef BaseObserver<_A1, _A2, _A3> Handler;
    typedef std::vector<Handler*> HandlerSet;
    
public:
    // Add an observer, if the pointer exists, nothing happens
    int subscribe (Handler* p_ob);

    // Remove an observer, if the pointer does not exist, nothing happens
    int unsubscribe (Handler* p_ob);

    // Remove all subscribers
    void clear () { ap_ob_.clear(); }

    // Get number of observers
    size_t size () const { return ap_ob_.size(); }

    // No observer or not
    bool empty () const { return ap_ob_.empty(); }

    // Notify observers for events
    // Following `notify' methods should be called exclusively
    void notify () const
    {
        for (typename HandlerSet::const_iterator it=ap_ob_.begin();
             it!=ap_ob_.end(); ++it) {
            (*it)->on_event();
        }
    }

    template <typename _B1> void notify (const _B1& b1) const
    {
        for (typename HandlerSet::const_iterator it=ap_ob_.begin();
             it!=ap_ob_.end(); ++it) {
            (*it)->on_event(b1);
        }
    }

    template <typename _B1, typename _B2>
    void notify (const _B1& b1, const _B2& b2) const
    {
        for (typename HandlerSet::const_iterator it=ap_ob_.begin();
             it!=ap_ob_.end(); ++it) {
            (*it)->on_event(b1, b2);
        }
    }

    template <typename _B1, typename _B2, typename _B3>
    void notify (const _B1& b1, const _B2& b2, const _B3& b3) const
    {
        for (typename HandlerSet::const_iterator it=ap_ob_.begin();
             it!=ap_ob_.end(); ++it) {
            (*it)->on_event(b1, b2, b3);
        }
    }
        
private:
    HandlerSet ap_ob_;
};

template <typename _A1, typename _A2, typename _A3>
int Event<_A1, _A2, _A3>::subscribe (Handler* p_ob)
{
    if (NULL == p_ob) {
        ST_FATAL ("Param[p_ob] is NULL");
        return EINVAL;
    }
    for (size_t i = 0; i < ap_ob_.size(); ++i) {
        if (ap_ob_[i] == p_ob) {
            return EEXIST;
        }
    }
    ap_ob_.push_back(p_ob);
    return 0;
}

template <typename _A1, typename _A2, typename _A3>
int Event<_A1, _A2, _A3>::unsubscribe (Handler* p_ob)
{
    if (NULL == p_ob) {
        ST_FATAL ("param[p_ob] is NULL");
        return EINVAL;
    }
    for (size_t i=0; i<ap_ob_.size(); ++i) {
        if (ap_ob_[i] == p_ob) {
            ap_ob_[i] = ap_ob_.back();
            ap_ob_.pop_back();
            return 0;
        }
    }
    return 0;    
}

}  // namespace st

#endif  // _OBSERVER_HPP_
