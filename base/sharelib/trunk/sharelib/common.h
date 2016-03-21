#ifndef SHARELIB_COMMON_H_
#define SHARELIB_COMMON_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SHARELIB_BS namespace sharelib {
#define SHARELIB_ES  }
#define SHARELIB_US using namespace sharelib


#define SHARELIB_NS sharelib

#define SHARELIB_DELETE_AND_SET_NULL(x) do {     \
        if(x){                                  \
            delete x;                           \
            x = NULL;                           \
        }                                       \
    }while(0)

#define ARRAY_DELETE_AND_SET_NULL(x) delete [] x; x = NULL


#define likely(x) __builtin_expect((x), 1)

#define unlikely(x) __builtin_expect((x), 0)


/* define for close assign operator and copy constructor;should not be called if not implemented */
#define COPY_CONSTRUCTOR(T) \
    T(const T &); \
    T & operator=(const T &);

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);               \
    void operator=(const TypeName&)  

enum _ret_t {
    r_succeed = 0,
    r_failed = 1,
    r_eagain,
    r_eof,
    r_unknown
};
typedef enum _ret_t ret_t;

#endif /*SHARELIB_COMMON_H_*/


