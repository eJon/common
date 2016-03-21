/**

// Author: gejun@baidu.com
// Date: Sat Jul 31 17:56:09 2010
// Brief: an implementation of tuple in C++ which supports variadic number of types and user-assigned column number and maximum 10 columns
 */
#ifndef _OLD_TUPLE_HPP_
#define _OLD_TUPLE_HPP_

#include "string_writer.hpp"
#include "fun/st_hash.h"
#include "fun/compare.hpp"

#include <typeinfo>
namespace st
{

    /**
       @brief following lists internal macros, do not touch them outside this header. if you want to add a new method into each specialization, you'd better define a macro here and invoke it inside tuples
    */

#define SHOW_COLUMN_INFO(_col_)                                         \
    LOG("column[%s]={real_Idx=%d,offset=%d,typeid=%s}", #_col_, field_t<_col_>::REAL_IDX, field_t<_col_>::OFFSET, typeid(typename field_t<_col_>::value_type).name())

 //   const int NULL_COLUMN = 0xFFFF;
 //   const int MAX_COLUMN_NUM = 10;
    
    /**
       @brief statically mapping indices
    */
#define IDX_MAP_SECTION_(_boolean_)                                     \
    template <int N, typename Dummy=void>                             \
    struct field_t                                                      \
    {                                                                   \
        enum {REAL_IDX = (_boolean_)};                                  \
        typedef typename direct_field_t<REAL_IDX>::value_type value_type; \
        enum {OFFSET=direct_field_t<REAL_IDX>::OFFSET};                 \
        void to_string (StringWriter& sb) const                     \
        {                                                               \
            sb << "{REAL_IDX=" << REAL_IDX                              \
               << ",OFFSET=" << OFFSET                                  \
               << ",value_type=" << typeid(value_type).name()           \
               <<"}";                                                   \
        }                                                               \
    };                                                                  \
    

#define FIELD_TYPE(_F_) typename field_t<_F_>::value_type

    
    /**
       @brief declare direct_field_t
    */
#define FIELD0_SECTION_()                                               \
    template <int Idx, typename Dummy=void>                           \
    struct direct_field_t {                                             \
        typedef void value_type;                                      \
        enum {VIRT_IDX=NULL_COLUMN, IDX=NULL_COLUMN, OFFSET=NULL_COLUMN}; \
    };                                                                  \


#define FIELD_NAME(_idx_) value##_idx_

    /**
       @brief add specialization of direct_field_t, use -Wno-invalid-offsetof to turn off g++ complains
    */
#define FIELD_SECTION_(_idx_,_type_)                                    \
    _type_ FIELD_NAME(_idx_);                                           \
    template <typename Dummy>                                           \
    struct direct_field_t <_idx_,Dummy>                                 \
    {                                                                   \
        typedef _type_ value_type;                                      \
        enum {VIRT_IDX=Idx##_idx_, IDX=(_idx_), OFFSET=offsetof(this_type, FIELD_NAME(_idx_))}; \
    };                                                                  \

    
    /**
       @brief add get/set function
    */
#define GETSET_SECTION_()                                               \
    template <int I>                                               \
    inline typename field_t<I>::value_type& get () const                 \
    {                                                                   \
        return (*((typename field_t<I>::value_type*)((char*)(this) + field_t<I>::OFFSET))); \
    };                                                                  \
    template <int I>                                                  \
    inline typename direct_field_t<I>::value_type get_at () const       \
    {                                                                   \
        return (*((typename direct_field_t<I>::value_type*)((char*)(this) + direct_field_t<I>::OFFSET))); \
    };                                                                  \
    template <int I>                                                    \
    inline void set (const typename field_t<I>::value_type& value)      \
    {                                                                   \
        (*((typename field_t<I>::value_type*)((char*)(this) + field_t<I>::OFFSET))) = value; \
    };                                                                  \
    template <int I>                                                    \
    inline int offset () const                                          \
    {                                                                   \
        return field_t<I>::OFFSET;                                      \
    };                                                                  \
    template <int I>                                                    \
    inline int idx () const                                             \
    {                                                                   \
        return field_t<I>::IDX;                                         \
    };                                                                  \


    //#define sub_tuple_t(...) sub_tuple_f<__VA_ARGS__>::value_type

#define DECLARE_SUB_SECTION_()                  \
    template <int I0=NULL_COLUMN                \
              , int I1=NULL_COLUMN              \
              , int I2=NULL_COLUMN              \
              , int I3=NULL_COLUMN              \
              , int I4=NULL_COLUMN              \
              , int I5=NULL_COLUMN              \
              , int I6=NULL_COLUMN              \
              , int I7=NULL_COLUMN              \
              , int I8=NULL_COLUMN              \
              , int I9=NULL_COLUMN              \
              >                                 \
    struct sub_tuple_f;                         \
    
#define SUB0_SECTION_()                                 \
    template <int I0>                                   \
    struct sub_tuple_f<I0>                              \
    {                                                   \
        typedef Tuple1<FIELD_TYPE(I0),I0> value_type;          \
        inline value_type operator()(const this_type& t) const  \
        {                                               \
            return value_type (t.get<I0>());                  \
        }                                               \
    };                                                  \
    template <int I0>                                   \
    Tuple1<FIELD_TYPE(I0),I0> sub_tuple() const        \
    {                                                   \
        return Tuple1<FIELD_TYPE(I0),I0> (get<I0>());  \
    }                                                   \


#define SUB1_SECTION_()                                                 \
    template <int I0                                                    \
              , int I1                                                  \
              >                                                         \
    struct sub_tuple_f<I0,I1>                                           \
    {                                                                   \
        typedef Tuple1<FIELD_TYPE(I0),I0                               \
            , FIELD_TYPE(I1),I1> value_type;                                  \
        value_type operator()(const this_type& t) const                       \
        {                                                               \
            return value_type (t.get<I0>()                                    \
                         , t.get<I1>()                                  \
                         );                                             \
        }                                                               \
};                                                                      \
    template <int I0,int I1>                                            \
    Tuple1<FIELD_TYPE(I0),I0,FIELD_TYPE(I1),I1> sub_tuple() const      \
    {                                                                   \
        return Tuple1<FIELD_TYPE(I0),I0,FIELD_TYPE(I1),I1> (get<I0>(), get<I1>()); \
    }                                                                   \


#define SUB2_SECTION_()                             \
    template <int I0                                \
              , int I1                              \
              , int I2                              \
              >                                     \
    struct sub_tuple_f<I0,I1,I2>                    \
    {                                               \
        typedef Tuple1<FIELD_TYPE(I0),I0           \
            , FIELD_TYPE(I1),I1                     \
            , FIELD_TYPE(I2),I2                     \
            > value_type;                                 \
        value_type operator()(const this_type& t) const   \
        {                                           \
            return value_type (t.get<I0>()                \
                         , t.get<I1>()              \
                         , t.get<I2>()              \
                         );                         \
        }                                           \
};                                                  \
    template <int I0,int I1,int I2>                 \
    Tuple1<FIELD_TYPE(I0),I0                       \
            ,FIELD_TYPE(I1),I1                      \
            ,FIELD_TYPE(I2),I2                      \
            > sub_tuple() const                     \
    {                                               \
        return Tuple1<FIELD_TYPE(I0),I0            \
            ,FIELD_TYPE(I1),I1                      \
            ,FIELD_TYPE(I2),I2                      \
            > (get<I0>()                            \
               , get<I1>()                          \
               , get<I2>()                          \
               );                                   \
}                                                   \


#define SUB3_SECTION_()                             \
    template <int I0                                \
              , int I1                              \
              , int I2                              \
              , int I3                              \
              >                                     \
    struct sub_tuple_f<I0,I1,I2,I3>                 \
    {                                               \
        typedef Tuple1<FIELD_TYPE(I0),I0           \
            , FIELD_TYPE(I1),I1                     \
            , FIELD_TYPE(I2),I2                     \
            , FIELD_TYPE(I3),I3                     \
            > value_type;                                 \
        value_type operator()(const this_type& t) const   \
        {                                           \
            return value_type (t.get<I0>()                \
                         , t.get<I1>()              \
                         , t.get<I2>()              \
                         , t.get<I3>()              \
                         );                         \
        }                                           \
    };                                              \
    template <int I0,int I1,int I2,int I3>          \
    Tuple1<FIELD_TYPE(I0),I0                       \
            ,FIELD_TYPE(I1),I1                      \
            ,FIELD_TYPE(I2),I2                      \
            ,FIELD_TYPE(I3),I3                      \
            > sub_tuple() const                     \
            {                                       \
                return Tuple1<FIELD_TYPE(I0),I0    \
                    ,FIELD_TYPE(I1),I1              \
                    ,FIELD_TYPE(I2),I2              \
                    ,FIELD_TYPE(I3),I3              \
                    > (get<I0>()                    \
                       , get<I1>()                  \
                       , get<I2>()                  \
                       , get<I3>()                  \
                       );                           \
                }                                   \
    

#define SUB4_SECTION_()                             \
    template <int I0                                \
              , int I1                              \
              , int I2                              \
              , int I3                              \
              , int I4                              \
              >                                     \
    struct sub_tuple_f<I0,I1,I2,I3,I4>              \
    {                                               \
        typedef Tuple1<FIELD_TYPE(I0),I0           \
            , FIELD_TYPE(I1),I1                     \
            , FIELD_TYPE(I2),I2                     \
            , FIELD_TYPE(I3),I3                     \
            , FIELD_TYPE(I4),I4                     \
            > value_type;                                 \
        value_type operator()(const this_type& t) const   \
        {                                           \
            return value_type (t.get<I0>()                \
                         , t.get<I1>()              \
                         , t.get<I2>()              \
                         , t.get<I3>()              \
                         , t.get<I4>()              \
                         );                         \
        }                                           \
    };                                              \
    template <int I0,int I1,int I2,int I3,int I4>   \
    Tuple1<FIELD_TYPE(I0),I0                       \
            , FIELD_TYPE(I1),I1                     \
            , FIELD_TYPE(I2),I2                     \
            , FIELD_TYPE(I3),I3                     \
            , FIELD_TYPE(I4),I4                     \
            > sub_tuple() const                     \
            {                                       \
                return Tuple1<FIELD_TYPE(I0),I0    \
                    , FIELD_TYPE(I1),I1             \
                    , FIELD_TYPE(I2),I2             \
                    , FIELD_TYPE(I3),I3             \
                    , FIELD_TYPE(I4),I4             \
                    > (get<I0>()                    \
                       , get<I1>()                  \
                       , get<I2>()                  \
                       , get<I3>()                  \
                       , get<I4>()                  \
                       );                           \
                }                                   \



#define SUB5_SECTION_()                                     \
    template <int I0                                        \
              , int I1                                      \
              , int I2                                      \
              , int I3                                      \
              , int I4                                      \
              , int I5                                      \
              >                                             \
    struct sub_tuple_f<I0,I1,I2,I3,I4,I5>                   \
    {                                                       \
        typedef Tuple1<FIELD_TYPE(I0),I0                   \
            , FIELD_TYPE(I1),I1                             \
            , FIELD_TYPE(I2),I2                             \
            , FIELD_TYPE(I3),I3                             \
            , FIELD_TYPE(I4),I4                             \
            , FIELD_TYPE(I5),I5                             \
            > value_type;                                         \
        value_type operator()(const this_type& t) const           \
        {                                                   \
            return value_type (t.get<I0>()                        \
                         , t.get<I1>()                      \
                         , t.get<I2>()                      \
                         , t.get<I3>()                      \
                         , t.get<I4>()                      \
                         , t.get<I5>()                      \
                         );                                 \
        }                                                   \
    };                                                      \
    template <int I0,int I1,int I2,int I3,int I4,int I5>    \
    Tuple1<FIELD_TYPE(I0),I0                               \
            , FIELD_TYPE(I1),I1                             \
            , FIELD_TYPE(I2),I2                             \
            , FIELD_TYPE(I3),I3                             \
            , FIELD_TYPE(I4),I4                             \
            , FIELD_TYPE(I5),I5                             \
            > sub_tuple() const                             \
    {                                                       \
        return Tuple1<FIELD_TYPE(I0),I0                    \
            , FIELD_TYPE(I1),I1                             \
            , FIELD_TYPE(I2),I2                             \
            , FIELD_TYPE(I3),I3                             \
            , FIELD_TYPE(I4),I4                             \
            , FIELD_TYPE(I5),I5                             \
            > (get<I0>()                                    \
               , get<I1>()                                  \
               , get<I2>()                                  \
               , get<I3>()                                  \
               , get<I4>()                                  \
               , get<I5>()                                  \
               );                                           \
}                                                           \


#define SUB6_SECTION_()                                         \
    template <int I0                                            \
              , int I1                                          \
              , int I2                                          \
              , int I3                                          \
              , int I4                                          \
              , int I5                                          \
              , int I6                                          \
              >                                                 \
    struct sub_tuple_f<I0,I1,I2,I3,I4,I5,I6>                    \
    {                                                           \
        typedef Tuple1<FIELD_TYPE(I0),I0                       \
            , FIELD_TYPE(I1),I1                                 \
            , FIELD_TYPE(I2),I2                                 \
            , FIELD_TYPE(I3),I3                                 \
            , FIELD_TYPE(I4),I4                                 \
            , FIELD_TYPE(I5),I5                                 \
            , FIELD_TYPE(I6),I6                                 \
            > value_type;                                             \
        value_type operator()(const this_type& t) const               \
        {                                                       \
            return value_type (t.get<I0>()                            \
                         , t.get<I1>()                          \
                         , t.get<I2>()                          \
                         , t.get<I3>()                          \
                         , t.get<I4>()                          \
                         , t.get<I5>()                          \
                         , t.get<I6>()                          \
                         );                                     \
        }                                                       \
    };                                                          \
    template <int I0,int I1,int I2,int I3,int I4,int I5,int I6> \
    Tuple1<FIELD_TYPE(I0),I0                                   \
            , FIELD_TYPE(I1),I1                                 \
            , FIELD_TYPE(I2),I2                                 \
            , FIELD_TYPE(I3),I3                                 \
            , FIELD_TYPE(I4),I4                                 \
            , FIELD_TYPE(I5),I5                                 \
            , FIELD_TYPE(I6),I6                                 \
            > sub_tuple() const                                 \
    {                                                           \
        return Tuple1<FIELD_TYPE(I0),I0                        \
            , FIELD_TYPE(I1),I1                                 \
            , FIELD_TYPE(I2),I2                                 \
            , FIELD_TYPE(I3),I3                                 \
            , FIELD_TYPE(I4),I4                                 \
            , FIELD_TYPE(I5),I5                                 \
            , FIELD_TYPE(I6),I6                                 \
            > (get<I0>()                                        \
               , get<I1>()                                      \
               , get<I2>()                                      \
               , get<I3>()                                      \
               , get<I4>()                                      \
               , get<I5>()                                      \
               , get<I6>()                                      \
               );                                               \
}                                                               \


#define SUB7_SECTION_()                                                 \
    template <int I0                                                    \
              , int I1                                                  \
              , int I2                                                  \
              , int I3                                                  \
              , int I4                                                  \
              , int I5                                                  \
              , int I6                                                  \
              , int I7                                                  \
              >                                                         \
    struct sub_tuple_f<I0,I1,I2,I3,I4,I5,I6,I7>                         \
    {                                                                   \
        typedef Tuple1<FIELD_TYPE(I0),I0                               \
            , FIELD_TYPE(I1),I1                                         \
            , FIELD_TYPE(I2),I2                                         \
            , FIELD_TYPE(I3),I3                                         \
            , FIELD_TYPE(I4),I4                                         \
            , FIELD_TYPE(I5),I5                                         \
            , FIELD_TYPE(I6),I6                                         \
            , FIELD_TYPE(I7),I7                                         \
            > value_type;                                                     \
        value_type operator()(const this_type& t) const                       \
        {                                                               \
            return value_type (t.get<I0>()                                    \
                         , t.get<I1>()                                  \
                         , t.get<I2>()                                  \
                         , t.get<I3>()                                  \
                         , t.get<I4>()                                  \
                         , t.get<I5>()                                  \
                         , t.get<I6>()                                  \
                         , t.get<I7>()                                  \
                         );                                             \
        }                                                               \
    };                                                                  \
    template <int I0,int I1,int I2,int I3,int I4,int I5,int I6,int I7>  \
    Tuple1<FIELD_TYPE(I0),I0                                           \
            , FIELD_TYPE(I1),I1                                         \
            , FIELD_TYPE(I2),I2                                         \
            , FIELD_TYPE(I3),I3                                         \
            , FIELD_TYPE(I4),I4                                         \
            , FIELD_TYPE(I5),I5                                         \
            , FIELD_TYPE(I6),I6                                         \
            , FIELD_TYPE(I7),I7                                         \
            > sub_tuple() const                                         \
    {                                                                   \
        return Tuple1<FIELD_TYPE(I0),I0                                \
            , FIELD_TYPE(I1),I1                                         \
            , FIELD_TYPE(I2),I2                                         \
            , FIELD_TYPE(I3),I3                                         \
            , FIELD_TYPE(I4),I4                                         \
            , FIELD_TYPE(I5),I5                                         \
            , FIELD_TYPE(I6),I6                                         \
            , FIELD_TYPE(I7),I7                                         \
            > (get<I0>()                                                \
               , get<I1>()                                              \
               , get<I2>()                                              \
               , get<I3>()                                              \
               , get<I4>()                                              \
               , get<I5>()                                              \
               , get<I6>()                                              \
               , get<I7>()                                              \
               );                                                       \
}                                                                       \


#define SUB8_SECTION_()                                                 \
    template <int I0                                                    \
              , int I1                                                  \
              , int I2                                                  \
              , int I3                                                  \
              , int I4                                                  \
              , int I5                                                  \
              , int I6                                                  \
              , int I7                                                  \
              , int I8                                                  \
              >                                                         \
    struct sub_tuple_f<I0,I1,I2,I3,I4,I5,I6,I7,I8>                      \
    {                                                                   \
        typedef Tuple1<FIELD_TYPE(I0),I0                               \
            , FIELD_TYPE(I1),I1                                         \
            , FIELD_TYPE(I2),I2                                         \
            , FIELD_TYPE(I3),I3                                         \
            , FIELD_TYPE(I4),I4                                         \
            , FIELD_TYPE(I5),I5                                         \
            , FIELD_TYPE(I6),I6                                         \
            , FIELD_TYPE(I7),I7                                         \
            , FIELD_TYPE(I8),I8                                         \
            > value_type;                                                     \
        value_type operator()(const this_type& t) const                       \
        {                                                               \
            return value_type (t.get<I0>()                                    \
                         , t.get<I1>()                                  \
                         , t.get<I2>()                                  \
                         , t.get<I3>()                                  \
                         , t.get<I4>()                                  \
                         , t.get<I5>()                                  \
                         , t.get<I6>()                                  \
                         , t.get<I7>()                                  \
                         , t.get<I8>()                                  \
                         );                                             \
        }                                                               \
    };                                                                  \
    template <int I0,int I1,int I2,int I3,int I4,int I5,int I6,int I7,int I8> \
    Tuple1<FIELD_TYPE(I0),I0                                           \
            , FIELD_TYPE(I1),I1                                         \
            , FIELD_TYPE(I2),I2                                         \
            , FIELD_TYPE(I3),I3                                         \
            , FIELD_TYPE(I4),I4                                         \
            , FIELD_TYPE(I5),I5                                         \
            , FIELD_TYPE(I6),I6                                         \
            , FIELD_TYPE(I7),I7                                         \
            , FIELD_TYPE(I8),I8                                         \
            > sub_tuple() const                                         \
    {                                                                   \
        return Tuple1<FIELD_TYPE(I0),I0                                \
            , FIELD_TYPE(I1),I1                                         \
            , FIELD_TYPE(I2),I2                                         \
            , FIELD_TYPE(I3),I3                                         \
            , FIELD_TYPE(I4),I4                                         \
            , FIELD_TYPE(I5),I5                                         \
            , FIELD_TYPE(I6),I6                                         \
            , FIELD_TYPE(I7),I7                                         \
            , FIELD_TYPE(I8),I8                                         \
            > (get<I0>()                                                \
               , get<I1>()                                              \
               , get<I2>()                                              \
               , get<I3>()                                              \
               , get<I4>()                                              \
               , get<I5>()                                              \
               , get<I6>()                                              \
               , get<I7>()                                              \
               , get<I8>()                                              \
               );                                                       \
}                                                                       \


#define SUB9_SECTION_()                                                 \
    template <int I0                                                    \
              , int I1                                                  \
              , int I2                                                  \
              , int I3                                                  \
              , int I4                                                  \
              , int I5                                                  \
              , int I6                                                  \
              , int I7                                                  \
              , int I8                                                  \
              , int I9                                                  \
              >                                                         \
    struct sub_tuple_f                                                  \
    {                                                                   \
        typedef this_type value_type;                                   \
        value_type operator()(const this_type& t) const                 \
        {                                                               \
            return t;                                                   \
        }                                                               \
    };                                                                  \
    template <int I0,int I1,int I2,int I3,int I4,int I5,int I6,int I7,int I8, int I9> \
    Tuple1<FIELD_TYPE(I0),I0                                           \
            , FIELD_TYPE(I1),I1                                         \
            , FIELD_TYPE(I2),I2                                         \
            , FIELD_TYPE(I3),I3                                         \
            , FIELD_TYPE(I4),I4                                         \
            , FIELD_TYPE(I5),I5                                         \
            , FIELD_TYPE(I6),I6                                         \
            , FIELD_TYPE(I7),I7                                         \
            , FIELD_TYPE(I8),I8                                         \
            , FIELD_TYPE(I9),I9                                         \
            > sub_tuple() const                                         \
    {                                                                   \
        return Tuple1<FIELD_TYPE(I0),I0                                \
            , FIELD_TYPE(I1),I1                                         \
            , FIELD_TYPE(I2),I2                                         \
            , FIELD_TYPE(I3),I3                                         \
            , FIELD_TYPE(I4),I4                                         \
            , FIELD_TYPE(I5),I5                                         \
            , FIELD_TYPE(I6),I6                                         \
            , FIELD_TYPE(I7),I7                                         \
            , FIELD_TYPE(I8),I8                                         \
            , FIELD_TYPE(I9),I9                                         \
            > (get<I0>()                                                \
               , get<I1>()                                              \
               , get<I2>()                                              \
               , get<I3>()                                              \
               , get<I4>()                                              \
               , get<I5>()                                              \
               , get<I6>()                                              \
               , get<I7>()                                              \
               , get<I8>()                                              \
               , get<I9>()                                              \
               );                                                       \
}                                                                       \
    
    

#define DECLARE_TUPLE_F(_tuplename_,...)                                \
    template <typename Func>                                            \
    struct tuple_f                                                      \
    {                                                                   \
        tuple_f (Func f=Func())                                         \
        {                                                               \
            f_ = f;                                                     \
        }                                                               \
        typedef typename Func::return_type return_type;                 \
        typename Func::return_type operator() (const this_type& _tuplename_) const \
        {                                                               \
            return f_ (__VA_ARGS__);                                    \
        }                                                               \
    private:                                                            \
        Func f_;                                                        \
    };                                                                  \
    

    /**
       @brief declare Tuple1
    */
    template <typename T0=void, int Idx0=NULL_COLUMN
              , typename T1=void, int Idx1=NULL_COLUMN
              , typename T2=void, int Idx2=NULL_COLUMN
              , typename T3=void, int Idx3=NULL_COLUMN
              , typename T4=void, int Idx4=NULL_COLUMN
              , typename T5=void, int Idx5=NULL_COLUMN
              , typename T6=void, int Idx6=NULL_COLUMN
              , typename T7=void, int Idx7=NULL_COLUMN
              , typename T8=void, int Idx8=NULL_COLUMN
              , typename T9=void, int Idx9=NULL_COLUMN
              >
    struct Tuple1;

    /**
       @brief 1 column specialization
    */
    template <typename T0, int Idx0>
    struct Tuple1<T0,Idx0>
    {
        typedef Tuple1<T0,Idx0> this_type;
        FIELD0_SECTION_ ();
        FIELD_SECTION_ (0, T0);
        IDX_MAP_SECTION_ (N==Idx0?0:NULL_COLUMN);
        GETSET_SECTION_ ();
        DECLARE_SUB_SECTION_();
        SUB0_SECTION_();
        DECLARE_TUPLE_F(t, t.get<Idx0>());

        void to_string(StringWriter& sb) const
        {
            sb << "(" << get<Idx0>() << ")";
        }

        bool operator== (const this_type& t) const
        {
            return get<Idx0>() == t.get<Idx0>();
        }

        friend int value_cmp (const this_type& t1, const this_type& t2)
        {
            return value_cmp(t1.get<Idx0>(), t2.get<Idx0>());
        }

        inline friend int operator< (const this_type& t1, const this_type& t2)
        {
            return value_cmp(t1,t2) < 0;
        }
        
        explicit Tuple1() {} explicit Tuple1 (T0 v0)
        {
            FIELD_NAME(0) = v0;
        }
    };

    /**
       @brief 2 column specialization
    */
    template <typename T0, int Idx0
              , typename T1, int Idx1
              >
    struct Tuple1<T0,Idx0,T1,Idx1>
    {
        typedef Tuple1<T0,Idx0,T1,Idx1> this_type;
        FIELD0_SECTION_ ();
        FIELD_SECTION_ (0, T0);
        FIELD_SECTION_ (1, T1);
        IDX_MAP_SECTION_ (N==Idx0?0:N==Idx1?1:NULL_COLUMN);
        GETSET_SECTION_ ();
        DECLARE_SUB_SECTION_();
        SUB0_SECTION_();
        SUB1_SECTION_();
        DECLARE_TUPLE_F(t, t.get<Idx0>(), t.get<Idx1>());

        void to_string(StringWriter& sb) const
        {
            sb << "(" << get<Idx0>()
               <<  "," << get<Idx1>()
               << ")";
        }

        bool operator== (const this_type& t) const
        {
            return
                get<Idx0>() == t.get<Idx0>()
                && get<Idx1>() == t.get<Idx1>()
                ;
        }

        friend int value_cmp (const this_type& t1, const this_type& t2)
        {
            int ret;
            return (ret = value_cmp(t1.get<Idx0>(), t2.get<Idx0>())) ? ret
                :  value_cmp(t1.get<Idx1>(), t2.get<Idx1>());
        }

        inline friend int operator< (const this_type& t1, const this_type& t2)
        {
            return value_cmp(t1,t2) < 0;
        }        
        
        explicit Tuple1() {} explicit Tuple1 (T0 v0, T1 v1)
        {
            FIELD_NAME(0) = v0;
            FIELD_NAME(1) = v1;
        }
    };

    /**
       @brief 3 column specialization
    */
    template <typename T0, int Idx0
              , typename T1, int Idx1
              , typename T2, int Idx2
              >
    struct Tuple1<T0,Idx0,T1,Idx1,T2,Idx2>
    {
        typedef Tuple1<T0,Idx0,T1,Idx1,T2,Idx2> this_type;
        FIELD0_SECTION_ ();
        FIELD_SECTION_ (0, T0);
        FIELD_SECTION_ (1, T1);
        FIELD_SECTION_ (2, T2);
        IDX_MAP_SECTION_ (N==Idx0?0:N==Idx1?1:N==Idx2?2:NULL_COLUMN);
        GETSET_SECTION_ ();
        DECLARE_SUB_SECTION_();
        SUB0_SECTION_ ();
        SUB1_SECTION_ ();
        SUB2_SECTION_ ();
        DECLARE_TUPLE_F(t, t.get<Idx0>(), t.get<Idx1>(), t.get<Idx2>());

        void to_string(StringWriter& sb) const
        {
            sb << "(" << get<Idx0>()
               << "," << get<Idx1>()
               << "," << get<Idx2>()
               << ")";
        }

        bool operator== (const this_type& t) const
        {
            return
                get<Idx0>() == t.get<Idx0>()
                && get<Idx1>() == t.get<Idx1>()
                && get<Idx2>() == t.get<Idx2>()
                ;
        }

        friend int value_cmp (const this_type& t1, const this_type& t2)
        {
            int ret;
            return (ret = value_cmp(t1.get<Idx0>(), t2.get<Idx0>()))
                ? ret : (ret = value_cmp(t1.get<Idx1>(), t2.get<Idx1>()))
                ? ret : (ret = value_cmp(t1.get<Idx2>(), t2.get<Idx2>()))
                ;
        }

        inline friend int operator< (const this_type& t1, const this_type& t2)
        {
            return value_cmp(t1,t2) < 0;
        }
        
        explicit Tuple1() {} explicit Tuple1 (T0 v0, T1 v1, T2 v2)
        {
            FIELD_NAME(0) = v0;
            FIELD_NAME(1) = v1;
            FIELD_NAME(2) = v2;
        }
    };

    /**
       @brief 4 column specialization
    */
    template <typename T0, int Idx0
              , typename T1, int Idx1
              , typename T2, int Idx2
              , typename T3, int Idx3
              >
    struct Tuple1<T0,Idx0,T1,Idx1,T2,Idx2,T3,Idx3>
    {
        typedef Tuple1<T0,Idx0,T1,Idx1,T2,Idx2,T3,Idx3> this_type;
        FIELD0_SECTION_ ();
        FIELD_SECTION_ (0, T0);
        FIELD_SECTION_ (1, T1);
        FIELD_SECTION_ (2, T2);
        FIELD_SECTION_ (3, T3);
        IDX_MAP_SECTION_ (N==Idx0?0:N==Idx1?1:N==Idx2?2:N==Idx3?3:NULL_COLUMN);
        GETSET_SECTION_ ();
        DECLARE_SUB_SECTION_();
        SUB0_SECTION_ ();
        SUB1_SECTION_ ();
        SUB2_SECTION_ ();
        SUB3_SECTION_ ();
        DECLARE_TUPLE_F(t, t.get<Idx0>(), t.get<Idx1>(), t.get<Idx2>(), t.get<Idx3>());

        void show_schema () const
        {
            LOG ("showing schema of %s, size=%d: ", typeid(*this).name(), (int)sizeof(*this));
            SHOW_COLUMN_INFO(Idx0);
            SHOW_COLUMN_INFO(Idx1);
            SHOW_COLUMN_INFO(Idx2);
            SHOW_COLUMN_INFO(Idx3);
        }

        void to_string(StringWriter& sb) const
        {
            sb << "(" << get<Idx0>()
               << "," << get<Idx1>()
               << "," << get<Idx2>()
               << "," << get<Idx3>()
               << ")";
        }

        bool operator== (const this_type& t) const
        {
            return
                value0 == t.value0
                && value1 == t.value1
                && value2 == t.value2
                && value3 == t.value3
                ;
        }

        friend int value_cmp (const this_type& t1, const this_type& t2)
        {
            int ret;
            return (ret = value_cmp(t1.get<Idx0>(), t2.get<Idx0>()))
                ? ret : (ret = value_cmp(t1.get<Idx1>(), t2.get<Idx1>()))
                ? ret : (ret = value_cmp(t1.get<Idx2>(), t2.get<Idx2>()))
                ? ret : (ret = value_cmp(t1.get<Idx3>(), t2.get<Idx3>()))
                ;
        }

        inline friend int operator< (const this_type& t1, const this_type& t2)
        {
            return value_cmp(t1,t2) < 0;
        }
        
        explicit Tuple1() {} explicit Tuple1 (T0 v0, T1 v1, T2 v2, T3 v3)
        {
            FIELD_NAME(0) = v0;
            FIELD_NAME(1) = v1;
            FIELD_NAME(2) = v2;
            FIELD_NAME(3) = v3;
        }
    };


    /**
       @brief 5 column specialization
    */
    template <typename T0, int Idx0
              , typename T1, int Idx1
              , typename T2, int Idx2
              , typename T3, int Idx3
              , typename T4, int Idx4
              >
    struct Tuple1<T0,Idx0,T1,Idx1,T2,Idx2,T3,Idx3,T4,Idx4>
    {
        typedef Tuple1<T0,Idx0,T1,Idx1,T2,Idx2,T3,Idx3,T4,Idx4> this_type;
        FIELD0_SECTION_ ();
        FIELD_SECTION_ (0, T0);
        FIELD_SECTION_ (1, T1);
        FIELD_SECTION_ (2, T2);
        FIELD_SECTION_ (3, T3);
        FIELD_SECTION_ (4, T4);
        IDX_MAP_SECTION_ (N==Idx0?0:N==Idx1?1:N==Idx2?2:N==Idx3?3:N==Idx4?4:NULL_COLUMN);
        GETSET_SECTION_ ();
        DECLARE_SUB_SECTION_();
        SUB0_SECTION_ ();
        SUB1_SECTION_ ();
        SUB2_SECTION_ ();
        SUB3_SECTION_ ();
        SUB4_SECTION_ ();
        DECLARE_TUPLE_F(t, t.get<Idx0>(), t.get<Idx1>(), t.get<Idx2>(), t.get<Idx3>(), t.get<Idx4>());

        void to_string(StringWriter& sb) const
        {
            sb << "(" << get<Idx0>()
               << "," << get<Idx1>()
               << "," << get<Idx2>()
               << "," << get<Idx3>()
               << "," << get<Idx4>()
               << ")";
        }

        bool operator== (const this_type& t) const
        {
            return
                get<Idx0>() == t.get<Idx0>()
                && get<Idx1>() == t.get<Idx1>()
                && get<Idx2>() == t.get<Idx2>()
                && get<Idx3>() == t.get<Idx3>()
                && get<Idx4>() == t.get<Idx4>()
                ;
        }

        friend int value_cmp (const this_type& t1, const this_type& t2)
        {
            int ret;
            return (ret = value_cmp(t1.get<Idx0>(), t2.get<Idx0>()))
                ? ret : (ret = value_cmp(t1.get<Idx1>(), t2.get<Idx1>()))
                ? ret : (ret = value_cmp(t1.get<Idx2>(), t2.get<Idx2>()))
                ? ret : (ret = value_cmp(t1.get<Idx3>(), t2.get<Idx3>()))
                ? ret : (ret = value_cmp(t1.get<Idx4>(), t2.get<Idx4>()))
                ;
        }

        inline friend int operator< (const this_type& t1, const this_type& t2)
        {
            return value_cmp(t1,t2) < 0;
        }
        
        explicit Tuple1() {} explicit Tuple1 (T0 v0, T1 v1, T2 v2, T3 v3, T4 v4)
        {
            FIELD_NAME(0) = v0;
            FIELD_NAME(1) = v1;
            FIELD_NAME(2) = v2;
            FIELD_NAME(3) = v3;
            FIELD_NAME(4) = v4;
        }
    };

    /**
       @brief 6 column specialization
    */
    template <typename T0, int Idx0
              , typename T1, int Idx1
              , typename T2, int Idx2
              , typename T3, int Idx3
              , typename T4, int Idx4
              , typename T5, int Idx5
              >
    struct Tuple1<T0,Idx0,T1,Idx1,T2,Idx2,T3,Idx3,T4,Idx4,T5,Idx5>
    {
        typedef Tuple1<T0,Idx0,T1,Idx1,T2,Idx2,T3,Idx3,T4,Idx4,T5,Idx5> this_type;
        FIELD0_SECTION_ ();
        FIELD_SECTION_ (0, T0);
        FIELD_SECTION_ (1, T1);
        FIELD_SECTION_ (2, T2);
        FIELD_SECTION_ (3, T3);
        FIELD_SECTION_ (4, T4);
        FIELD_SECTION_ (5, T5);
        IDX_MAP_SECTION_ (N==Idx0?0:N==Idx1?1:N==Idx2?2:N==Idx3?3:N==Idx4?4:N==Idx5?5:NULL_COLUMN);
        GETSET_SECTION_ ();
        DECLARE_SUB_SECTION_();
        SUB0_SECTION_ ();
        SUB1_SECTION_ ();
        SUB2_SECTION_ ();
        SUB3_SECTION_ ();
        SUB4_SECTION_ ();
        SUB5_SECTION_ ();
        DECLARE_TUPLE_F(t, t.get<Idx0>(), t.get<Idx1>(), t.get<Idx2>(), t.get<Idx3>(), t.get<Idx4>(), t.get<Idx5>());

        void to_string(StringWriter& sb) const
        {
            sb << "(" << get<Idx0>()
               << "," << get<Idx1>()
               << "," << get<Idx2>()
               << "," << get<Idx3>()
               << "," << get<Idx4>()
               << "," << get<Idx5>()
               << ")";
        }

        bool operator== (const this_type& t) const
        {
            return
                get<Idx0>() == t.get<Idx0>()
                && get<Idx1>() == t.get<Idx1>()
                && get<Idx2>() == t.get<Idx2>()
                && get<Idx3>() == t.get<Idx3>()
                && get<Idx4>() == t.get<Idx4>()
                && get<Idx5>() == t.get<Idx5>()
                ;
        }

        friend int value_cmp (const this_type& t1, const this_type& t2)
        {
            int ret;
            return (ret = value_cmp(t1.get<Idx0>(), t2.get<Idx0>()))
                ? ret : (ret = value_cmp(t1.get<Idx1>(), t2.get<Idx1>()))
                ? ret : (ret = value_cmp(t1.get<Idx2>(), t2.get<Idx2>()))
                ? ret : (ret = value_cmp(t1.get<Idx3>(), t2.get<Idx3>()))
                ? ret : (ret = value_cmp(t1.get<Idx4>(), t2.get<Idx4>()))
                ? ret : (ret = value_cmp(t1.get<Idx5>(), t2.get<Idx5>()))
                ;
        }

        inline friend int operator< (const this_type& t1, const this_type& t2)
        {
            return value_cmp(t1,t2) < 0;
        }
        
        explicit Tuple1() {} explicit Tuple1 (T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5)
        {
            FIELD_NAME(0) = v0;
            FIELD_NAME(1) = v1;
            FIELD_NAME(2) = v2;
            FIELD_NAME(3) = v3;
            FIELD_NAME(4) = v4;
            FIELD_NAME(5) = v5;
        }
    };

    /**
       @brief 7 column specialization
    */
    template <typename T0, int Idx0
              , typename T1, int Idx1
              , typename T2, int Idx2
              , typename T3, int Idx3
              , typename T4, int Idx4
              , typename T5, int Idx5
              , typename T6, int Idx6
              >
    struct Tuple1<T0,Idx0,T1,Idx1,T2,Idx2,T3,Idx3,T4,Idx4,T5,Idx5,T6,Idx6>
    {
        typedef Tuple1<T0,Idx0,T1,Idx1,T2,Idx2,T3,Idx3,T4,Idx4,T5,Idx5,T6,Idx6> this_type;
        FIELD0_SECTION_ ();
        FIELD_SECTION_ (0, T0);
        FIELD_SECTION_ (1, T1);
        FIELD_SECTION_ (2, T2);
        FIELD_SECTION_ (3, T3);
        FIELD_SECTION_ (4, T4);
        FIELD_SECTION_ (5, T5);
        FIELD_SECTION_ (6, T6);
        IDX_MAP_SECTION_ (N==Idx0?0:N==Idx1?1:N==Idx2?2:N==Idx3?3:N==Idx4?4:N==Idx5?5:N==Idx6?6:NULL_COLUMN);
        GETSET_SECTION_ ();
        DECLARE_SUB_SECTION_();
        SUB0_SECTION_ ();
        SUB1_SECTION_ ();
        SUB2_SECTION_ ();
        SUB3_SECTION_ ();
        SUB4_SECTION_ ();
        SUB5_SECTION_ ();
        SUB6_SECTION_ ();
        DECLARE_TUPLE_F(t, t.get<Idx0>(), t.get<Idx1>(), t.get<Idx2>(), t.get<Idx3>(), t.get<Idx4>(), t.get<Idx5>(), t.get<Idx6>());

        void to_string(StringWriter& sb) const
        {
            sb << "(" << get<Idx0>()
               << "," << get<Idx1>()
               << "," << get<Idx2>()
               << "," << get<Idx3>()
               << "," << get<Idx4>()
               << "," << get<Idx5>()
               << "," << get<Idx6>()
               << ")";
        }

        bool operator== (const this_type& t) const
        {
            return
                get<Idx0>() == t.get<Idx0>()
                && get<Idx1>() == t.get<Idx1>()
                && get<Idx2>() == t.get<Idx2>()
                && get<Idx3>() == t.get<Idx3>()
                && get<Idx4>() == t.get<Idx4>()
                && get<Idx5>() == t.get<Idx5>()
                && get<Idx6>() == t.get<Idx6>()
                ;
        }

        friend int value_cmp (const this_type& t1, const this_type& t2)
        {
            int ret;
            return (ret = value_cmp(t1.get<Idx0>(), t2.get<Idx0>()))
                ? ret : (ret = value_cmp(t1.get<Idx1>(), t2.get<Idx1>()))
                ? ret : (ret = value_cmp(t1.get<Idx2>(), t2.get<Idx2>()))
                ? ret : (ret = value_cmp(t1.get<Idx3>(), t2.get<Idx3>()))
                ? ret : (ret = value_cmp(t1.get<Idx4>(), t2.get<Idx4>()))
                ? ret : (ret = value_cmp(t1.get<Idx5>(), t2.get<Idx5>()))
                ? ret : (ret = value_cmp(t1.get<Idx6>(), t2.get<Idx6>()))
                ;
        }
        inline friend int operator< (const this_type& t1, const this_type& t2)
        {
            return value_cmp(t1,t2) < 0;
        }


        explicit Tuple1() {} explicit Tuple1 (T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6)
        {
            FIELD_NAME(0) = v0;
            FIELD_NAME(1) = v1;
            FIELD_NAME(2) = v2;
            FIELD_NAME(3) = v3;
            FIELD_NAME(4) = v4;
            FIELD_NAME(5) = v5;
            FIELD_NAME(6) = v6;
        }
    };

    /**
       @brief 8 column specialization
    */
    template <typename T0, int Idx0
              , typename T1, int Idx1
              , typename T2, int Idx2
              , typename T3, int Idx3
              , typename T4, int Idx4
              , typename T5, int Idx5
              , typename T6, int Idx6
              , typename T7, int Idx7
              >
    struct Tuple1<T0,Idx0,T1,Idx1,T2,Idx2,T3,Idx3,T4,Idx4,T5,Idx5,T6,Idx6,T7,Idx7>
    {
        typedef Tuple1<T0,Idx0,T1,Idx1,T2,Idx2,T3,Idx3,T4,Idx4,T5,Idx5,T6,Idx6,T7,Idx7> this_type;
        FIELD0_SECTION_ ();
        FIELD_SECTION_ (0, T0);
        FIELD_SECTION_ (1, T1);
        FIELD_SECTION_ (2, T2);
        FIELD_SECTION_ (3, T3);
        FIELD_SECTION_ (4, T4);
        FIELD_SECTION_ (5, T5);
        FIELD_SECTION_ (6, T6);
        FIELD_SECTION_ (7, T7);
        IDX_MAP_SECTION_ (N==Idx0?0:N==Idx1?1:N==Idx2?2:N==Idx3?3:N==Idx4?4:N==Idx5?5:N==Idx6?6:N==Idx7?7:NULL_COLUMN);
        GETSET_SECTION_ ();
        DECLARE_SUB_SECTION_();
        SUB0_SECTION_ ();
        SUB1_SECTION_ ();
        SUB2_SECTION_ ();
        SUB3_SECTION_ ();
        SUB4_SECTION_ ();
        SUB5_SECTION_ ();
        SUB6_SECTION_ ();
        SUB7_SECTION_ ();
        DECLARE_TUPLE_F(t, t.get<Idx0>(), t.get<Idx1>(), t.get<Idx2>(), t.get<Idx3>(), t.get<Idx4>(), t.get<Idx5>(), t.get<Idx6>(), t.get<Idx7>());


        void to_string(StringWriter& sb) const
        {
            sb << "(" << get<Idx0>()
               << "," << get<Idx1>()
               << "," << get<Idx2>()
               << "," << get<Idx3>()
               << "," << get<Idx4>()
               << "," << get<Idx5>()
               << "," << get<Idx6>()
               << "," << get<Idx7>()
               << ")";
        }

        bool operator== (const this_type& t) const
        {
            return
                get<Idx0>() == t.get<Idx0>()
                && get<Idx1>() == t.get<Idx1>()
                && get<Idx2>() == t.get<Idx2>()
                && get<Idx3>() == t.get<Idx3>()
                && get<Idx4>() == t.get<Idx4>()
                && get<Idx5>() == t.get<Idx5>()
                && get<Idx6>() == t.get<Idx6>()
                && get<Idx7>() == t.get<Idx7>()
                ;
        }
        
        friend int value_cmp (const this_type& t1, const this_type& t2)
        {
            int ret;
            return (ret = value_cmp(t1.get<Idx0>(), t2.get<Idx0>()))
                ? ret : (ret = value_cmp(t1.get<Idx1>(), t2.get<Idx1>()))
                ? ret : (ret = value_cmp(t1.get<Idx2>(), t2.get<Idx2>()))
                ? ret : (ret = value_cmp(t1.get<Idx3>(), t2.get<Idx3>()))
                ? ret : (ret = value_cmp(t1.get<Idx4>(), t2.get<Idx4>()))
                ? ret : (ret = value_cmp(t1.get<Idx5>(), t2.get<Idx5>()))
                ? ret : (ret = value_cmp(t1.get<Idx6>(), t2.get<Idx6>()))
                ? ret : (ret = value_cmp(t1.get<Idx7>(), t2.get<Idx7>()))
                ;
        }
        inline friend int operator< (const this_type& t1, const this_type& t2)
        {
            return value_cmp(t1,t2) < 0;
        }


        explicit Tuple1() {} explicit Tuple1 (T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7)
        {
            FIELD_NAME(0) = v0;
            FIELD_NAME(1) = v1;
            FIELD_NAME(2) = v2;
            FIELD_NAME(3) = v3;
            FIELD_NAME(4) = v4;
            FIELD_NAME(5) = v5;
            FIELD_NAME(6) = v6;
            FIELD_NAME(7) = v7;
        }
    };

    /**
       @brief 9 column specialization
    */
    template <typename T0, int Idx0
              , typename T1, int Idx1
              , typename T2, int Idx2
              , typename T3, int Idx3
              , typename T4, int Idx4
              , typename T5, int Idx5
              , typename T6, int Idx6
              , typename T7, int Idx7
              , typename T8, int Idx8
              >
    struct Tuple1<T0,Idx0,T1,Idx1,T2,Idx2,T3,Idx3,T4,Idx4,T5,Idx5,T6,Idx6,T7,Idx7,T8,Idx8>
    {
        typedef Tuple1<T0,Idx0,T1,Idx1,T2,Idx2,T3,Idx3,T4,Idx4,T5,Idx5,T6,Idx6,T7,Idx7,T8,Idx8> this_type;
        FIELD0_SECTION_ ();
        FIELD_SECTION_ (0, T0);
        FIELD_SECTION_ (1, T1);
        FIELD_SECTION_ (2, T2);
        FIELD_SECTION_ (3, T3);
        FIELD_SECTION_ (4, T4);
        FIELD_SECTION_ (5, T5);
        FIELD_SECTION_ (6, T6);
        FIELD_SECTION_ (7, T7);
        FIELD_SECTION_ (8, T8);
        IDX_MAP_SECTION_ (N==Idx0?0:N==Idx1?1:N==Idx2?2:N==Idx3?3:N==Idx4?4:N==Idx5?5:N==Idx6?6:N==Idx7?7:N==Idx8?8:NULL_COLUMN);
        GETSET_SECTION_ ();
        DECLARE_SUB_SECTION_();
        SUB0_SECTION_ ();
        SUB1_SECTION_ ();
        SUB2_SECTION_ ();
        SUB3_SECTION_ ();
        SUB4_SECTION_ ();
        SUB5_SECTION_ ();
        SUB6_SECTION_ ();
        SUB7_SECTION_ ();
        SUB8_SECTION_ ();
        DECLARE_TUPLE_F(t, t.get<Idx0>(), t.get<Idx1>(), t.get<Idx2>(), t.get<Idx3>(), t.get<Idx4>(), t.get<Idx5>(), t.get<Idx6>(), t.get<Idx7>(), t.get<Idx8>());

        void to_string(StringWriter& sb) const
        {
            sb << "(" << get<Idx0>()
               << "," << get<Idx1>()
               << "," << get<Idx2>()
               << "," << get<Idx3>()
               << "," << get<Idx4>()
               << "," << get<Idx5>()
               << "," << get<Idx6>()
               << "," << get<Idx7>()
               << "," << get<Idx8>()
               << ")";
        }
    
        bool operator== (const this_type& t) const
        {
            return
                get<Idx0>() == t.get<Idx0>()
                && get<Idx1>() == t.get<Idx1>()
                && get<Idx2>() == t.get<Idx2>()
                && get<Idx3>() == t.get<Idx3>()
                && get<Idx4>() == t.get<Idx4>()
                && get<Idx5>() == t.get<Idx5>()
                && get<Idx6>() == t.get<Idx6>()
                && get<Idx7>() == t.get<Idx7>()
                && get<Idx8>() == t.get<Idx8>()
                ;
        }

        friend int value_cmp (const this_type& t1, const this_type& t2)
        {
            int ret;
            return (ret = value_cmp(t1.get<Idx0>(), t2.get<Idx0>()))
                ? ret : (ret = value_cmp(t1.get<Idx1>(), t2.get<Idx1>()))
                ? ret : (ret = value_cmp(t1.get<Idx2>(), t2.get<Idx2>()))
                ? ret : (ret = value_cmp(t1.get<Idx3>(), t2.get<Idx3>()))
                ? ret : (ret = value_cmp(t1.get<Idx4>(), t2.get<Idx4>()))
                ? ret : (ret = value_cmp(t1.get<Idx5>(), t2.get<Idx5>()))
                ? ret : (ret = value_cmp(t1.get<Idx6>(), t2.get<Idx6>()))
                ? ret : (ret = value_cmp(t1.get<Idx7>(), t2.get<Idx7>()))
                ? ret : (ret = value_cmp(t1.get<Idx8>(), t2.get<Idx8>()))
                ;
        }
        inline friend int operator< (const this_type& t1, const this_type& t2)
        {
            return value_cmp(t1,t2) < 0;
        }        

        explicit Tuple1() {} explicit Tuple1 (T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8)
        {
            FIELD_NAME(0) = v0;
            FIELD_NAME(1) = v1;
            FIELD_NAME(2) = v2;
            FIELD_NAME(3) = v3;
            FIELD_NAME(4) = v4;
            FIELD_NAME(5) = v5;
            FIELD_NAME(6) = v6;
            FIELD_NAME(7) = v7;
            FIELD_NAME(8) = v8;
        }

    };

    /**
       @brief 10 column specialization
    */
    template <typename T0, int Idx0
              , typename T1, int Idx1
              , typename T2, int Idx2
              , typename T3, int Idx3
              , typename T4, int Idx4
              , typename T5, int Idx5
              , typename T6, int Idx6
              , typename T7, int Idx7
              , typename T8, int Idx8
              , typename T9, int Idx9
              >
    struct Tuple1
    {
        typedef Tuple1<T0,Idx0,T1,Idx1,T2,Idx2,T3,Idx3,T4,Idx4,T5,Idx5,T6,Idx6,T7,Idx7,T8,Idx8,T9,Idx9> this_type;
        FIELD0_SECTION_ ();
        FIELD_SECTION_ (0, T0);
        FIELD_SECTION_ (1, T1);
        FIELD_SECTION_ (2, T2);
        FIELD_SECTION_ (3, T3);
        FIELD_SECTION_ (4, T4);
        FIELD_SECTION_ (5, T5);
        FIELD_SECTION_ (6, T6);
        FIELD_SECTION_ (7, T7);
        FIELD_SECTION_ (8, T8);
        FIELD_SECTION_ (9, T9);    
        IDX_MAP_SECTION_ (N==Idx0?0:N==Idx1?1:N==Idx2?2:N==Idx3?3:N==Idx4?4:N==Idx5?5:N==Idx6?6:N==Idx7?7:N==Idx8?8:N==Idx9?9:NULL_COLUMN);
        GETSET_SECTION_ ();
        DECLARE_SUB_SECTION_();
        SUB0_SECTION_ ();
        SUB1_SECTION_ ();
        SUB2_SECTION_ ();
        SUB3_SECTION_ ();
        SUB4_SECTION_ ();
        SUB5_SECTION_ ();
        SUB6_SECTION_ ();
        SUB7_SECTION_ ();
        SUB8_SECTION_ ();
        SUB9_SECTION_ ();
        
        DECLARE_TUPLE_F(t, t.get<Idx0>(), t.get<Idx1>(), t.get<Idx2>(), t.get<Idx3>(), t.get<Idx4>(), t.get<Idx5>(), t.get<Idx6>(), t.get<Idx7>(), t.get<Idx8>(), t.get<Idx9>());
        
        void to_string(StringWriter& sb) const
        {
            sb << "(" << get<Idx0>()
               << "," << get<Idx1>()
               << "," << get<Idx2>()
               << "," << get<Idx3>()
               << "," << get<Idx4>()
               << "," << get<Idx5>()
               << "," << get<Idx6>()
               << "," << get<Idx7>()
               << "," << get<Idx8>()
               << "," << get<Idx9>()
               << ")";
        }

        bool operator== (const this_type& t) const
        {
            return
                get<Idx0>() == t.get<Idx0>()
                && get<Idx1>() == t.get<Idx1>()
                && get<Idx2>() == t.get<Idx2>()
                && get<Idx3>() == t.get<Idx3>()
                && get<Idx4>() == t.get<Idx4>()
                && get<Idx5>() == t.get<Idx5>()
                && get<Idx6>() == t.get<Idx6>()
                && get<Idx7>() == t.get<Idx7>()
                && get<Idx8>() == t.get<Idx8>()
                && get<Idx9>() == t.get<Idx9>()
                ;
        }

        friend int value_cmp (const this_type& t1, const this_type& t2)
        {
            int ret;
            return (ret = value_cmp(t1.get<Idx0>(), t2.get<Idx0>()))
                ? ret : (ret = value_cmp(t1.get<Idx1>(), t2.get<Idx1>()))
                ? ret : (ret = value_cmp(t1.get<Idx2>(), t2.get<Idx2>()))
                ? ret : (ret = value_cmp(t1.get<Idx3>(), t2.get<Idx3>()))
                ? ret : (ret = value_cmp(t1.get<Idx4>(), t2.get<Idx4>()))
                ? ret : (ret = value_cmp(t1.get<Idx5>(), t2.get<Idx5>()))
                ? ret : (ret = value_cmp(t1.get<Idx6>(), t2.get<Idx6>()))
                ? ret : (ret = value_cmp(t1.get<Idx7>(), t2.get<Idx7>()))
                ? ret : (ret = value_cmp(t1.get<Idx8>(), t2.get<Idx8>()))
                ? ret : (ret = value_cmp(t1.get<Idx9>(), t2.get<Idx9>()))
                ;
        }
        inline friend int operator< (const this_type& t1, const this_type& t2)
        {
            return value_cmp(t1,t2) < 0;
        }

        
        explicit Tuple1() {} explicit Tuple1 (T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8, T9 v9)
        {
            FIELD_NAME(0) = v0;
            FIELD_NAME(1) = v1;
            FIELD_NAME(2) = v2;
            FIELD_NAME(3) = v3;
            FIELD_NAME(4) = v4;
            FIELD_NAME(5) = v5;
            FIELD_NAME(6) = v6;
            FIELD_NAME(7) = v7;
            FIELD_NAME(8) = v8;
            FIELD_NAME(9) = v9;
        }

    };

}
#endif /* _OLD_TUPLE_HPP_ */
