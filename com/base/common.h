#ifndef MATCHSERVER_COMMON_H_
#define MATCHSERVER_COMMON_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*
const static int DEBUG = 1;
const static int INFO = 2;
const static int NOTICE = 3;
const static int WARN = 4;
const static int ERROR = 5;
const static int CRIT = 6;
const static int FATAL = 7;
*/
#define MATCHSERVER_BS namespace matchserver{
#define MATCHSERVER_ES  }
#define MATCHSERVER_US using namespace matchserver


#define MATCHSERVER_ALIAS_NAMESAPCE(x, y) namespace matchserver{ namespace x = y; }
/*short cut for namespace matchserver::x*/
#define MATCHSERVER_NS matchserver

#define MATCHSERVER_DELETE_AND_SET_NULL(x) do {     \
        if(x){                                  \
            delete x;                           \
            x = NULL;                           \
        }                                       \
    }while(0)

#define ARRAY_DELETE_AND_SET_NULL(x) delete [] x; x = NULL

#define TYPEDEF_PTR(x) typedef std::tr1::shared_ptr<x> x##Ptr

#define likely(x) __builtin_expect((x), 1)

#define unlikely(x) __builtin_expect((x), 0)


/* define for close assign operator and copy constructor;should not be called if not implemented */
#define COPY_CONSTRUCTOR(T) \
    T(const T &); \
    T & operator=(const T &);

#endif /*MATCHSERVER_COMMON_H_*/


