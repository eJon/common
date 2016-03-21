#ifndef MONITOR_COMMON_H_
#define MONITOR_COMMON_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MONITOR_BS namespace monitor {
#define MONITOR_ES  }
#define MONITOR_US using namespace monitor


#define MONITOR_NS monitor

#define MONITOR_DELETE_AND_SET_NULL(x) do {     \
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


#endif /*MONITOR_COMMON_H_*/


