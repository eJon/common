// Record operations
// Author: gejun@baidu.com
// Date: Fri Aug 13 17:17:30 2010
#pragma once

#ifndef _OP_LIST_HPP_
#define _OP_LIST_HPP_

#include "common.h"
#include <vector>
#include <algorithm>

namespace st
{
    enum op_e { OP_ADD, OP_DEL };

    template <typename T>
    struct op_t {
        typedef op_t<T> Self;

        template <typename CompareFunc>
        struct cmp_f {            
            inline bool operator() (const Self& o1, const Self& o2) const
            {
                return cmp_(o1.value, o2.value) < 0;
            }

        private:
            CompareFunc cmp_;
        };

        template <typename CompareFunc>
        struct equal_f {
            inline bool operator() (const Self& o1, const Self& o2) const
            {
                return cmp_(o1.value, o2.value) == 0;
            }

        private:
            CompareFunc cmp_;
        };

        explicit op_t()
        {}

        explicit op_t (op_e type, const T& value)
        {
            this->type = type;
            this->value = value;
        }

        void to_string (StringWriter& sb) const
        {
            sb << (type==OP_ADD?'+':'-') << value;
        }

        op_e type;
        T value;
    };

    
    template <typename T>
    struct op_list_t {
        typedef op_list_t<T> Self;
        typedef op_t<T> Op;
        typedef std::vector<Op> Vector;
        typedef typename std::vector<Op>::const_iterator iterator;
        static const size_t MAX_CLEAR_CAPACITY = (1<<16);
        
        explicit op_list_t(int reserved_size=32)
            : reserved_size_(reserved_size)
        {
            pa_op_ = new Vector;
            pa_op_->reserve (reserved_size_);
            no_chg_ = true;
        }

        ~op_list_t()
        {
            if (pa_op_) {
                delete pa_op_;
                pa_op_ = NULL;
            }
        }
       
        op_list_t(const Self& other)
        {
            no_chg_ = true;
            pa_op_ = new Vector(*(other.pa_op_));
        }

        void operator= (const Self& other)
        {
            *pa_op_ = *(other.pa_op_);
        }

        void clear () {
            no_chg_ = false;
            if (pa_op_->capacity() > MAX_CLEAR_CAPACITY) {
                delete pa_op_;
                pa_op_ = new Vector;
                pa_op_->reserve (reserved_size_);
            }
            else {
                pa_op_->clear();
            }
        }
    
        void add(const T& value)
        {
            no_chg_ = false;
            pa_op_->push_back (Op(OP_ADD, value));
        }

        void del(const T& value)
        {
            no_chg_ = false;
            pa_op_->push_back (Op(OP_DEL, value));
        }

        size_t size() const
        {
            if (unlikely(!no_chg_)) {
                ST_FATAL ("change is not frozen in op_list=%p", this);
            }
            return pa_op_->size();
        }

        size_t capacity () const
        {
            return pa_op_->capacity();        
        }
        
        //sort and unique
        iterator begin() const
        {
            if (unlikely(!no_chg_)) {
                ST_FATAL ("change is not frozen in op_list=%p", this);
            }
            return pa_op_->begin();
        }
        
        iterator end() const
        {
            if (unlikely(!no_chg_)) {
                ST_FATAL ("change is not frozen in op_list=%p", this);
            }
            return pa_op_->end();
        }

        const Op& operator[] (size_t i) const
        {
            if (unlikely(!no_chg_)) {
                ST_FATAL ("change is not frozen in op_list=%p", this);
            }
            return pa_op_->at(i);
        }
        
        void to_string (StringWriter& sb) const
        {
            shows(sb, pa_op_->begin(), pa_op_->end());
        }

        void make_no_change ()
        {
            no_chg_ = true;
        }

        template <typename CompareFunc>
        inline void sort ()
        {
            std::stable_sort (pa_op_->begin(), pa_op_->end(), typename Op::template cmp_f<CompareFunc>());
        }

        template <typename CompareFunc>
        void unique ()
        {
            //LOG ("here1");
            // if (no_chg_ || pa_op_->empty())
            // {
            //     no_chg_ = true;
            //     return;
            // }
            if (pa_op_->empty()) {
                no_chg_ = true;
                return;
            }

            //LOG ("here2");
            // std::unique can't be used here!
            CompareFunc cmp;
            typename std::vector<Op>::iterator it=pa_op_->begin(), it_e=pa_op_->end();
            typename std::vector<Op>::iterator wit=it;
            //StringWriter sb;
            for (; it != it_e; ) {
                //LOG("here3");
                *wit = *it;
                ++ it;

                //sb.clear();
                //sb << "compare " << it->value_ << " and " << wit->value_ << ", ret=" << cmp(it->value_,wit->value_);
                //LOG ("%s", sb.c_str());
                while (it != it_e && (0 == cmp(it->value, wit->value))) {
                    *wit = *it;
                    ++it;
                }
                ++wit;
            }
            pa_op_->resize (wit-pa_op_->begin());
                
            /*
              typename std::vector<Op>::iterator new_end = 
              std::unique(pa_op_->begin(), pa_op_->end(), eq_);
              pa_op_->resize(pa_op_->end() - pa_op_->begin());
            */
            //LOG ("here4");

            no_chg_ = true;
        }

    private:
        std::vector<Op> *pa_op_;
        bool no_chg_;
        int reserved_size_;
    };

}

#endif /* _OP_LIST_HPP_ */
