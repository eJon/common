#ifndef SESSION__COMMON_H_
#define SESSION__COMMON_H_

#include <string>
#include <tr1/unordered_map>
#include <tr1/memory>
#define SESSION_BS namespace session {
#define SESSION_ES  } 
#define SESSION_US using namespace session;
#define DELETE_AND_SET_NULL(x)              \
    do {                                    \
        if(x){                              \
            delete x;                       \
            x = NULL;                       \
        }                                   \
    }while(0)


#endif //end SESSION_COMMON_H_
