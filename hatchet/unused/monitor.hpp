// Implement multi-version concurrency control of data structures, 
// A data_holder is watched by mutiple monitors and destroyed when all
// monitors are destroyed.
// Author: gejun@baidu.com
// Date: Fri Sep 10 00:42:21 CST 2010
#pragma once
#ifndef _MONITOR_HPP_
#define _MONITOR_HPP_

#include "common.h"
#include "shared_object.h"
#include "fixed_deque.hpp"

namespace st
{
    const u_int NULL_VERSION = 0;

    template <typename T
              , int VerNum=2
              >
    struct Monitor {
        typedef T Value;
        typedef Monitor<T,VerNum> Self;

        // avoid VerNum to be less than 2
        C_ASSERT(VerNum >= 2, VerNum_is_less_than_2);

    private:
        struct VersionedData : public SharedObject {
            VersionedData(Value* p_value)
                : p_value_(p_value)
                , version_id_(NULL_VERSION)
            {}

            Value* p_value()
            {
                return p_value_;
            }

            u_int version_id() const
            {
                return version_id_;
            }

            void set_version_id(u_int version_id)
            {
                version_id_ = version_id;
            }

            void zero_ref_action()
            {
                if (p_value_) {
                    p_value_->action_on_zero_referenced();
                }
            }

            void to_string (StringWriter& sb) const
            {
                sb << "ver=" << version_id_ << "-->" << p_value_;
            }

        private:
            Value *p_value_;
            u_int version_id_;
        };

        typedef FixedDeque<VersionedData*,VerNum> VerQueue;

        struct DataHolder : public SharedObject {
            void to_string (StringWriter& sb) const
            {
                bool first = true;
                int i=0;
                for (typename VerQueue::const_iterator it=ver_queue_.begin(), it_e=ver_queue_.end(); it!=it_e; ++it,++i) {
                    if (!first) {
                        sb << ",";
                    }
                    else {
                        first = false;
                    }
                    //sb << "buffer[" << i << "]={" << *it << "}";
                    sb << *it;
                }
            }
            
            DataHolder(Value* p_value)
            {
                ver_queue_.push_back (new (std::nothrow) VersionedData(p_value));
            }

            ~DataHolder()
            {
                ST_DEBUG("Deallocating data_holder=%s",show(*this).c_str());
                for (typename VerQueue::iterator it=ver_queue_.begin(), it_e=ver_queue_.end(); it!=it_e; ++it) {
                    VersionedData *p_vd = *it;
                    if (p_vd) {
                        if (p_vd->p_value()) {
                            delete p_vd->p_value();
                        }
                        delete p_vd;
                    }
                }
                ver_queue_.clear();
            }

            VersionedData* new_VersionedData()
            {
                if (ver_queue_.empty()) {
                    return NULL;
                }
                else {
                    return new (std::nothrow) VersionedData(ver_queue_.back()->p_value()->new_shared_clone());
                }
            }

            VerQueue& queue()
            {
                return ver_queue_;
            }
            
        private:
            VerQueue ver_queue_;
        };

    public:
        //! call this to create objects of Monitor
        static Self create_mutable (Value* p_value)
        {
            return Monitor(new (std::nothrow) DataHolder(p_value));
        }

        static Self create_mutable (const Self& other)
        {
            if (!other.mutable_) {
                ST_FATAL ("Trying to create mutable Monitor from an immutable one!");
            }
            return Monitor(other.p_dh_);
        }
        
        //! copy-constructor, Monitor<..> m'=m always set m' to immutable
        Monitor (const Self& other)
        {
            if (NULL == other.p_dh_) {
                ST_FATAL ("other.p_dh_ is NULL");
                return;
            }

            mutable_ = false;
            p_dh_ = other.p_dh_;
            p_dh_->reference ();
            p_buffer_ = other.p_buffer_;
            if (p_buffer_) {
                p_buffer_->reference();
            }
        }

        //! assignment, m'=m always set m' to immutable except assigning to self
        void operator= (const Self& other)
        {
            if (this == &other) {
                ST_FATAL ("assigning self");
                return;
            }

            //! we need mutiple mutable Monitors, don't change mutable_
            //mutable_ = false;
            SharedObject::assign (p_dh_, other.p_dh_);
            SharedObject::assign (p_buffer_, other.p_buffer_);
        }

        //! destructor, referenced buffer and data_holder are dereferenced
        ~Monitor ()
        {
            if (p_buffer_) {
                p_buffer_->dereference();
                p_buffer_ = NULL;
            }
            if (p_dh_) {
                p_dh_->dereference();
                p_dh_ = NULL;
            }
        }

        //! switch to another version
        //! @retval E_NULL internal data_holder is NULL or can't find a buffer with the version_id
        //! @retval EIMMUTABLE the Monitor is immutable and the version_id relatesto a mutable one
        //! @retval 0 everything goes fine
        int switch_version (u_int version_id)
        {
            if (NULL == p_dh_) {
                return ENOTINIT;
            }

            VersionedData* p_new_buffer = buffer(version_id);
            if (NULL == p_new_buffer) {
                ST_FATAL ("Fail to find buffer with version=%u", version_id);
                return ENOTEXIST;
            }
            if (!mutable_ && p_new_buffer->p_value()->is_mutable()) {
                ST_FATAL ("Forbidden switch_version of an immutable Monitor=%p to a mutable version=%u", this, version_id);
                return EIMMUTABLE;
            }

            // <-- nothing changed till now -->
            SharedObject::assign (p_buffer_, p_new_buffer);

            //ST_DEBUG ("Switched to version_id=%u", version_id);
            return 0;
        }
        
        //! create a new version
        //! @param version_id version to be created
        //! @retval E_NULL internal data_holder is NULL or new data_holder can't be created
        //! @retval EIMMUTABLE the Monitor is immutable
        //! @retval EBADVERSION last version_id is still mutable(not closed), or version_id is less than last version_id
        //! @retval EFULL version slots are full and least version is not zero-referenced
        //! @retval 0 everything goes fine
        int open_version (u_int version_id)
        {
            // we can do nothing without data_holder
            if (NULL == p_dh_) {
                return ENOTINIT;
            }

            // check if this Monitor has chaning permission
            if (!mutable_) {
                ST_FATAL ("Forbidden open_version(%u) of an immutable Monitor=%p", version_id, this);
                return EIMMUTABLE;
            }

            // setup common variable
            VerQueue& q = p_dh_->queue();
            VersionedData* p_back = NULL;
            if (!q.empty()) {
                p_back = q.back();
            }
            
            // check last buffer if available
            if (p_back) {
                // check if last version is closed
                if (p_back->p_value()->is_mutable()) {
                    ST_FATAL ("previous verison_id=%u is still mutable, having two mutable versions is not allowed!", p_back->version_id());
                    return EBADVERSION;
                }
                
                // version must grow incrementally
                u_int last_ver = p_back->version_id();
                if (last_ver >= version_id) {
                    ST_FATAL ("version_id=%u is not greater than last version_id=%u", version_id, last_ver);
                    return EBADVERSION;
                }
            }

            // pop least version or create a new one, in either way the version will be pushed back later
            VersionedData* p_new_buffer = NULL;
            if (q.full()) {
                p_new_buffer = q.front();
                int ref_cnt = p_new_buffer->ref_cnt();
                // dont count self in
                if (p_new_buffer == p_buffer_) {
                    --ref_cnt;
                }
                if (ref_cnt) {
                    ST_FATAL ("version_id=%u still has %d referential Monitor%s, call-site logic is broken", p_new_buffer->version_id(), ref_cnt, ref_cnt>1?"s":"");
                    return EFULL;
                }
            }
            else {
                p_new_buffer = p_dh_->new_VersionedData();
                if (NULL == p_new_buffer) {
                    ST_FATAL ("Fail to new p_new_buffer");
                    return ENOMEM;
                }
            }

            if (p_new_buffer->p_value()->is_mutable()) {
                ST_FATAL ("Can't open_version(%u) on a mutable buffer!", version_id);
                if (!q.full()) {
                    delete p_new_buffer;
                }
                return EIMMUTABLE;
            }

            // notify p_new_buffer that it's ready to close itself
            int ret = p_new_buffer->p_value()->action_on_open_version(p_back ? p_back->p_value() : NULL);
            if (0 != ret) {
                ST_FATAL ("action_on_open_version fails, ret=%d", ret);
                // release allocated resource, "&& p_new_buffer" is not required in principle
                if (!q.full()) {
                    delete p_new_buffer;
                }
                return ret;
            }

            // p_new_buffer generally depends on p_back and we must make sure p_back is zero-referenced before p_new_buffer shape its data. p_back will be dereferenced in close_version, NOTICE that p_back is probably different from p_buffer_
            if (p_back) {
                p_back->reference();
            }
            
            // make it mutable
            p_new_buffer->p_value()->set_mutable(true);
            p_new_buffer->set_version_id(version_id);
            SharedObject::assign (p_buffer_, p_new_buffer);
            
            // push the version back which is either popped from front or new created
            if (q.full()) {
                q.pop_front();
                q.push_back (p_new_buffer);
            }
            else {
                q.push_back (p_new_buffer);
            }
            //ST_DEBUG ("Opened version_id=%u", version_id);
            
            return 0;
        }

        //! close the version previously created by open_version
        //! @retval E_NULL internal data_holder is NULL
        //! @retval EIMMUTABLE the Monitor is immutable or p_buffer_ != p_back
        //! @retval EEMPTY version slots are empty
        //! @retval E_GENERAL action_on_close_function fails
        //! @retval 0 everything goes fine
        int close_version ()
        {
            if (NULL == p_dh_) {
                return ENOTINIT;
            }

            if (!mutable_) {
                ST_FATAL ("Forbidden close_version of an immutable Monitor=%p", this);
                return EIMMUTABLE;
            }
            
            if (p_dh_->queue().empty()) {
                ST_FATAL ("p_dh_->queue() is empty");
                return EEMPTY;
            }

            // p_back is what we want to close
            VersionedData* p_back = p_dh_->queue().back();
            if (p_buffer_ != p_back) {
                ST_FATAL ("internal buffer is not latest (generally caused by failed open_version)");
                return EIMMUTABLE;
            }

            if (!p_back->p_value()->is_mutable()) {
                ST_FATAL ("Fail to close an immutable version");
                return EIMMUTABLE;
            }


            // find the buffer with maximum version_id amongst the buffers with smaller version_id than the input one.
            VersionedData* p_prev_buffer = NULL;
            if (p_dh_->queue().size() > 1) {
                p_prev_buffer = p_dh_->queue().back(1);
            }
            
            int ret = p_back->p_value()->action_on_close_version(p_prev_buffer ? p_prev_buffer->p_value() : NULL);
            if (ret) {
                ST_FATAL ("action_on_close_version fails, ret=%d", ret);
                return ret;
            }

            if (p_prev_buffer) {
                p_prev_buffer->dereference();
            }

            p_back->p_value()->set_mutable(false);

            //ST_DEBUG ("Closed version_id=%u", p_back->version_id());

            return 0;
        }

        //! returns pointer to the versioned object, Monitor->foo(...) generally invokes this method
        Value* operator-> ()
        {
            return p_buffer_->p_value();
        }

        Value& operator* ()
        {
            return *(p_buffer_->p_value());
        }

        //! debugging info
        void to_string (StringWriter& sb) const
        {
            sb << "{mutable=" << mutable_
               << ", version_id=";
            if (p_buffer_) {
                sb << p_buffer_->version_id();
            }
            else {
                sb << "(NULL)";
            }
            sb << ", data_holder={" << p_dh_ << "}";
            sb << "}";
        }
            
    private:
        Monitor (DataHolder* p_dh)
        {
            if (NULL == p_dh) {
                ST_FATAL ("param[p_dh] is NULL");
                return;
            }
                
            mutable_ = true;
            p_dh_ = p_dh;
            p_dh_->reference();
            p_buffer_ = NULL;

            // since constructor does not have return value, we don't check return value of subroutines, create default empty version so that operator-> doesn't core
            if (buffer(NULL_VERSION)) {
                switch_version (NULL_VERSION);
            }
            else {
                open_version(NULL_VERSION);
                close_version();
            }
        }

        VersionedData* buffer (u_int version_id)
        {
            for (typename VerQueue::iterator it=p_dh_->queue().begin(), it_e=p_dh_->queue().end(); it!=it_e; ++it) {
                if ((*it)->version_id() == version_id) {
                    return *it;
                }
            }
            return NULL;
        }
        
        DataHolder *p_dh_;
        VersionedData* p_buffer_;
        bool mutable_;
    };
}

#endif /* _MONITOR_HPP_ */
