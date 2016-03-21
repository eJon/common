// Author: gejun@baidu.com
// Date: Fri Jul 30 13:16:06 2010
#pragma once
#ifndef _MONITEE_HPP_
#define _MONITEE_HPP_

namespace st
{
    template <typename T>
    class Monitee
    {
    public:
        explicit Monitee()
            : mutable_(false)
        {}
        
        virtual ~Monitee() {}
        
        virtual int action_on_open_version (const T* p_prev) = 0;
        virtual int action_on_close_version (const T* p_prev) = 0;
        virtual int action_on_zero_referenced () = 0;
        virtual T* new_shared_clone () const = 0;
        
        bool is_mutable () const
        { return mutable_; }
        
        void set_mutable (bool m)
        { mutable_ = m; }

    protected:
        bool mutable_;
    };
}

#endif /* _MONITEE_HPP_ */
