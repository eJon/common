/** 
 * \file tb_hashmap.h
 * \brief  patch & commit hash map
 * \author sjcui
 * \date 2010-07-26
 * Copyright (C) Baidu Company
 */
#ifndef _TB_HASHMAP_HPP_
#define _TB_HASHMAP_HPP_

#include "ext_hash_map.hpp"
#include "common.h"

#include <vector>
#include <string>

namespace st
{
    template < class _Key,
               class _Value,
               typename _Clear = identity<_Value>,
               typename _Hash = hash<_Key>,
               typename _Equal = is_equal<_Key>,
               typename _Compare = compare<_Key>,
               typename _Spawn = identity<_Value> >
    struct tb_hashmap_t
    {
    private:  ///{{{
        struct versioned_value_t
        {
            versioned_value_t()
            {
                valid_mask = 0;
            }

            _Value a_value[2];

            inline void set_valid( uint32_t x )
            {
                assert(x < 2);
                valid_mask |= (1 << x);
            }
            inline void set_invalid( uint32_t x )
            {
                assert(x < 2);
                valid_mask &= ~(1<<x);
            }
            inline bool is_valid( uint32_t x )
            {
                assert(x < 2);
                return valid_mask & (1<<x);
            }

        private:
            char valid_mask;
        };
    
        typedef ExtHashMap<_Key,
                           versioned_value_t,
                           _Hash,
                           _Equal> hashmap_t;
        typedef typename hashmap_t::iterator hashmap_iterator_t;

        enum op_e{ op_add, op_del };
        struct op_t
        {
            op_e type;
            _Key key;
            _Value value;
        };

        struct op_cmp_f
        {
            _Compare cmp_;
            op_cmp_f(_Compare cmp)
            {
                cmp_ = cmp;
            }
            inline bool operator() (const op_t& o1, const op_t& o2) const
            {
                return cmp_(o1.key, o2.key) < 0;
            }
        };

        enum stat_e{ stat_write, stat_read, stat_freeze };
        ///}}}
    
    public:
        /** 
         * \brief iterator of tb_hashmap_t
         *   [_Key]key --- it->first()
         *   [_Value]value --- it->second()
         */
        class iterator
        {
        private:
            class value_type
            {
            public:
                const _Key& first() const
                {
                    return *first_;
                }

                const _Value& second() const
                {
                    return *second_;
                }

            private:  /// {{{
                friend class iterator;
                /// enable null value
                value_type()
                    :first_(&def_key_),
                     second_(&def_value_)
                {
                }
            
                void set( const _Key& key, _Value& val )
                {
                    first_ = &key;
                    second_ = &val;
                }

                const _Key* first_;
                _Value* second_;

                _Key def_key_;
                _Value def_value_;
                ///}}}
            };

        public:
            iterator()
                : flag_(0),
                  p_hashmap_(NULL),
                  ptr_(hashmap_iterator_t(NULL))
            {
            }

            bool operator == (const iterator & iter)
            {
                return ptr_ == iter.ptr_;
            }

            bool operator != (const iterator & iter)
            {
                return ptr_ != iter.ptr_;
            }

            iterator& operator ++()
            {
                for(; NULL != p_hashmap_ && p_hashmap_->end() != ptr_; )
                {
                    ++ ptr_;
                    if( p_hashmap_->end() != ptr_ && ptr_->second.is_valid(flag_) )
                    {
                        value_.set( ptr_->first, ptr_->second.a_value[flag_] );
                        break;
                    }
                }
                return *this;
            }

            inline value_type* operator->()
            {
                return &value_;
            }

        private:  ///{{{
            friend class tb_hashmap_t;
            iterator( uint32_t flag, hashmap_t* hashmap, hashmap_iterator_t it )
                : flag_(flag),
                  p_hashmap_(hashmap),
                  ptr_(it)
            {
                if( NULL != p_hashmap_ && p_hashmap_->end() != ptr_ )
                {
                    value_.set( ptr_->first, ptr_->second.a_value[flag_] );
                }
            }

            uint32_t flag_;
            hashmap_t* p_hashmap_;
            hashmap_iterator_t ptr_;
            value_type value_;
            ///}}}
        };

    private:///{{{
        //tb_hashmap_t( hashmap_t* p_hashmap, tb_hashmap_t* p_fa, uint32_t flag, uint32_t size )
        //    :p_hashmap_(NULL),
        //    p_fa_(NULL)
        //{
        //    init( p_hashmap, p_fa, flag, size );
        //}

        /// the source tb_hashmap_t object MUST change its state, when copied or assigned
        tb_hashmap_t( const tb_hashmap_t& rhs );
        tb_hashmap_t( tb_hashmap_t& rhs );
        const tb_hashmap_t& operator =( const tb_hashmap_t& rhs );
        const tb_hashmap_t& operator =( tb_hashmap_t& rhs );

        int32_t init(
                     hashmap_t* p_hashmap,
                     tb_hashmap_t* p_fa,
                     uint32_t flag,
                     uint32_t size, 
                     _Clear clear = _Clear(),
                     _Spawn spawn = _Spawn(),
                     _Compare cmp = _Compare())
        {
            if( NULL != p_hashmap_ && p_hashmap != p_hashmap_ )
            {
                ST_WARN( "desc=init_fail p_hashmap_=%p p_hashmap=%p",
                         p_hashmap_, p_hashmap );
                return -1;
            }
        
            if( NULL != p_fa_ && p_fa != p_fa_ )
            {
                ST_WARN( "desc=init_fail p_fa_=%p p_fa=%p",
                         p_fa_, p_fa );
                return -1;
            }

            if( NULL == p_hashmap_ )
            {
                p_hashmap_ = p_hashmap;
                p_fa_ = p_fa;

                clear_ = clear;
                spawn_ = spawn;
                cmp_ = cmp;
            }

            flag_ = flag;
            size_ = size;
            stat_ = stat_write;
            return 0;
        }
    
        bool spawn_add( uint32_t flag, const _Key & key, const _Value & value)
        {
            assert( flag < 2 );

            versioned_value_t v;
            hashmap_iterator_t it = p_hashmap_->find( key );
            if( p_hashmap_->end() == it )
            {  /// create a new node
                v.a_value[flag] = value;
                v.set_valid(flag);

                (void)p_hashmap_->insert( key, v );
                return true;
            }
            else
            { 
                if( it->second.is_valid(flag) )
                {/// here is the replacement which won't sync, evil..
                    clear_( it->second.a_value[flag] );

                    /// NOTE simply do assignment 
                    it->second.a_value[flag] = value;
                    return false;
                }
                else
                {
                    it->second.a_value[flag] = value;
                    it->second.set_valid(flag);
                    return true;
                }
            }
        }

        bool spawn_del( uint32_t flag, const _Key & key)
        {
            assert( flag < 2 );
            hashmap_iterator_t it = p_hashmap_->find( key );
            if( p_hashmap_->end() == it )
            {
                return false;
            }
            else
            { 
                if( it->second.is_valid(flag) )
                {
                    clear_( it->second.a_value[flag] );
                    it->second.set_invalid(flag);
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }

        void freeze()
        {
            if( NULL == p_hashmap_ || stat_read != stat_ )
            {
                return ;
            }
            stat_ = stat_freeze;
            ///NOTE forbid spawn without freeze if it's a public interface
            p_hashmap_->recycle_delayed();
            uint32_t new_flag = ( flag_ + 1 ) % 2;
            int32_t n_suspicion_ = suspicion_.size();
            int32_t delta = 0;
            for( int32_t i = 0; i < n_suspicion_; ++i )
            {
                hashmap_iterator_t it = p_hashmap_->find( suspicion_[i].key );
                if( suspicion_[i].type == op_add )
                {
                    assert( it != p_hashmap_->end() );
                    if( it->second.is_valid(flag_) )
                    {/// here is the replacement which won't sync, evil..
                        clear_( it->second.a_value[flag_] );
                    }
                    else
                    {
                        it->second.set_valid(flag_);
                        ++ delta;
                    }
                    it->second.a_value[flag_] = it->second.a_value[new_flag];
                }
                else if( it != p_hashmap_->end() )
                {
                    if( it->second.is_valid(flag_) )
                    {
                        clear_( it->second.a_value[flag_] );
                        //it->second.set_invalid(flag);
                        -- delta;
                    }
                    p_hashmap_->delayed_erase( suspicion_[i].key );
                }
            }
            size_ += delta;
            suspicion_.clear();
            stat_ = stat_write;
        }

        void erase()
        {
            if( NULL != p_hashmap_ )
            {
                delete p_hashmap_;
                p_hashmap_ = NULL;
            }
        }

        bool is_dual( tb_hashmap_t& tb )
        {
            return p_fa_ == &tb;
        }
        ///}}}

    public:
        tb_hashmap_t()
            :p_hashmap_(NULL),
             p_fa_(NULL)
        {
            init( NULL, NULL, 0, 0 );
        }
    
        //tb_hashmap_t( tb_hashmap_t& rhs )
        //{
        //    rhs.spawn( *this );
        //}

        //const tb_hashmap_t& operator =( tb_hashmap_t& rhs )
        //{
        //    rhs.spawn( *this );
        //    return *this;
        //}

        ~tb_hashmap_t()
        {
            p_hashmap_ = NULL;
        }
    
        /** 
         * \brief begin
         *   get the beginning iterator of current version tb_hashmap_t object
         *   NOTE: this method will be very SLOW if invalid data is too much
         * 
         * \return   the iterator points to the data
         *           end() --- found no (valid) data in tb_hashmap_t
         */
        iterator begin() const
        {
            hashmap_iterator_t ptr;
            /// initialize with first valid value
            for( ptr = p_hashmap_->begin();
                 ptr != p_hashmap_->end() && !ptr->second.is_valid(flag_);
                 ++ ptr );

            return iterator( flag_, p_hashmap_, ptr );
        }

        /** 
         * \brief end
         *   get the ending iterator of current version tb_hashmap_t object
         *   NOTE: use consistent value for comparing between iterators
         * 
         * \return   always the last position of hashmap
         */
        iterator end() const
        {
            hashmap_iterator_t ptr = p_hashmap_->end();
            return iterator( flag_, p_hashmap_, ptr );
        }
    
        /** 
         * \brief create
         *   create an hashmap
         *   MUST be called ONCE, before use tb_hashmap_t
         * 
         * \param clear [in] the clear function of type V
         */
        static int32_t create(
                              tb_hashmap_t& tb1,
                              tb_hashmap_t& tb2,
                              int n_hint = 10,
                              int n_block_size = 0,
                              _Clear clear = _Clear(),
                              _Spawn spawn = _Spawn(),
                              _Compare cmp = _Compare())
        {
            hashmap_t* p_hashmap = new(std::nothrow)hashmap_t(
                                                              n_hint,
                                                              n_block_size,
                                                              80);
            if( NULL == p_hashmap )
            {
                ST_FATAL( "desc=fail_malloc p_hashmap" );
                return -1;
            }

            int32_t ret = 0;
            ret = tb1.init(
                           p_hashmap,
                           &tb2,
                           0,
                           0,
                           clear,
                           spawn,
                           cmp);
            if( 0 != ret )
            {
                ST_WARN( "desc=fail_init tb1=%p ret=%d", &tb1, ret );
                delete p_hashmap;
                p_hashmap = NULL;
                return -1;
            }

            ret = tb2.init(
                           p_hashmap,
                           &tb1,
                           1,
                           0,
                           clear,
                           spawn,
                           cmp);
            if( 0 != ret )
            {
                ST_FATAL( "desc=fail_init tb2=%p ret=%d", &tb2, ret );
                /// don't know how to handle p_hashmap, fatal
                return -1;
            }

            return 0;
        }

        /** 
         * \brief destroy
         *   destroy the hashmap, the dual-operation of create()
         * MUST be called ONCE
         */
        static int32_t destroy( tb_hashmap_t& tb1, tb_hashmap_t& tb2 )
        {
            if( !tb1.is_dual( tb2 ) || !tb2.is_dual( tb1 ) )
            {
                ST_WARN( "desc=destroy_non_dual_object tb1=%p tb2=%p", &tb1, &tb2 );
                return -1;
            }
            tb1.erase();
            return 0;
        }

        /** 
         * \brief spawn
         *   user controls the creation of tb_hashmap_t.
         * the spawning object need no create()
         * 
         * \param tb 
         */
        int32_t spawn( tb_hashmap_t& tb )
        {
            if( stat_write != stat_ )
            {
                ST_WARN( "desc=forbidden_spawn state=%d", stat_ );
                return -1;
            }
            if( !is_dual( tb ) )
            {
                ST_WARN( "desc=invalid_spawn_target expect=%p get=%p", p_fa_, &tb );
                return -1;
            }
            tb.freeze();

            stat_ = stat_read;
            uint32_t new_flag = ( flag_ + 1 ) % 2;
            std::stable_sort( suspicion_.begin(), suspicion_.end(), op_cmp_f(cmp_) );
        
            int32_t n_suspicion_ = suspicion_.size();
            int32_t p = n_suspicion_ - 1;
            for( int32_t i = n_suspicion_- 2; i >= 0; --i )
            {
                if( !eq_( suspicion_[i].key, suspicion_[p].key ) )
                {
                    suspicion_[--p] = suspicion_[i];
                }
            }
            if( p >= 0 )
            {
                vector<op_t> vo( suspicion_.begin() + p, suspicion_.end() );
                suspicion_ = vo;
            }

            n_suspicion_ = suspicion_.size();
            int32_t delta = 0;
            for( int32_t i = 0; i < n_suspicion_; ++i )
            {
                if( suspicion_[i].type == op_add )
                {
                    if( spawn_add( new_flag, suspicion_[i].key, spawn_( suspicion_[i].value ) ) )
                    {
                        ++ delta;
                    }
                }
                else
                {
                    if( spawn_del( new_flag, suspicion_[i].key ) )
                    {
                        -- delta;
                    }
                }
            }

            return tb.init(
                           p_hashmap_,
                           this,
                           new_flag,
                           size_ + delta,
                           clear_,
                           spawn_,
                           cmp_);
        }

        /** 
         * \brief empty
         *   check if current version is empty
         * 
         * \return  true --- empty
         *          false--- not empty
         */
        bool empty() const
        {
            return 0 == size_;
        }

        /** 
         * \brief size
         *   get the number of item in current version
         * 
         * \return the size of valid item
         */
        uint32_t size() const
        {
            return size_;
        }

        /** 
         * \brief set
         *   set (_Key, V) into current version of hashmap
         * 
         * \param key       [in] the key
         * \param value     [in] the value
         * 
         * \return      true --- set success, call _Clear
         *                      when replacement happens
         *              false --- operation fail
         */
        bool set(const _Key & key, const _Value & value)
        {
            if( stat_write != stat_ )
            {
                ST_WARN( "desc=set_fail state=%d", stat_ );
                return false;
            }
            op_t op = { op_add, key, value };
            suspicion_.push_back( op );
            return true;
        }

        /** 
         * \brief del
         *   del item from hashmap with the key
         * 
         * \param key       [in] the key
         * 
         * \return      true --- del success, call _Clear
         *                      duplicated operation is ALLOWed
         *              false --- operation fail
         */
        bool del(const _Key & key)
        {
            if( stat_write != stat_ )
            {
                ST_WARN( "desc=set_fail state=%d", stat_ );
                return false;
            }
            op_t op = { op_del, key, _Value() };
            suspicion_.push_back( op );
            return true;
        }

        /**
         * Destroy all entries in the table
         *
         */
        void clear()
        {
            suspicion_.clear();
            p_hashmap_->clear();
            size_ = 0;
            stat_ = stat_write;
        }
        /** 
         * \brief find
         *   find the key in hashmap to get associated value
         * 
         * \param key 
         * 
         * \return      end() --- not exists
         *              the pointer to associated value --- otherwise
         */
        inline iterator find(const _Key & key)
        {
            hashmap_iterator_t it = p_hashmap_->find( key );
            if( p_hashmap_->end() == it )
            {  /// not exists
                return end();
            }
            /// search the key from statable sorted suspicion_ 
            if(it->second.is_valid(flag_))
            {  /// MOSTLY TRUE
                return iterator( flag_, p_hashmap_, it );
            }

            return end();
        }

        /** 
         * \brief dump
         *   dump the data into string, debug ONLY
         * 
         * \return  string of content
         */
        void to_string(StringWriter& sb) const
        {
            sb << "[";
            for(iterator it = begin(); it != end(); ++it )
            {
                if( it != begin() )
                {
                    sb << ",";
                }
                sb << "(" << it->first() << "," << it->second() << ")";
            }
            sb << "]";
        }

    private:///{{{
        hashmap_t *p_hashmap_;
        uint32_t flag_;
        std::vector< op_t > suspicion_;
        uint32_t size_;
        stat_e stat_;
        _Clear clear_;
        _Spawn spawn_;
        _Compare cmp_;
        _Equal eq_;
        tb_hashmap_t *p_fa_;
        ///}}}
    };
}
#endif  //_TB_HASHMAP_HPP_

