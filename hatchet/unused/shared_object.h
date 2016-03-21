// base class of objects that want thread-safe referential counting
// Author: gejun@baidu.com
// Date: Thu Sep  9 18:08:47 CST 2010

#ifndef _SHARED_OBJECT_H_
#define _SHARED_OBJECT_H_

#include <asm/atomic.h>

namespace st {

struct SharedObject {
    // constructor
    explicit SharedObject()
    { atomic_set (&n_ref_, 0); }

    virtual ~SharedObject ()
    {}

    template <typename _SO>
    static void assign (_SO*& p_old, _SO* p_new)
    {
        if (p_old != p_new) {
            if (p_old) {
                p_old->dereference();
            }
            if (p_new) {
                p_new->reference();
            }
            p_old = p_new;
        }
    }
        
    // add a reference
    void reference() { atomic_inc (&n_ref_); }
    
    // remove a reference, if reference goes back to zero, this object is deleted.
    void dereference()
    {
        atomic_dec (&n_ref_);
        if (0 == atomic_read (&n_ref_)) {
            zero_ref_action();
        }
    }

    int ref_cnt() const { return atomic_read (&n_ref_); }

    // take the action when referential count hits 0, default is deleting this.
    virtual void zero_ref_action() { delete this; }

protected:
    // number of referencers
    atomic_t n_ref_;
};
}

#endif  // _SHARED_OBJECT_H_

