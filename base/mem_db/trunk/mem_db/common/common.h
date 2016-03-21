#ifndef MEM_DB__COMMON_H_
#define MEM_DB__COMMON_H_

#include <string>
#include <tr1/unordered_map>
#include <tr1/memory>
#define MEM_DB_BS namespace mem_db {
#define MEM_DB_ES  }
#define MEM_DB_US using namespace mem_db;
#define DELETE_AND_SET_NULL(x)              \
  do {                                    \
    if(x){                              \
      delete x;                       \
      x = NULL;                       \
    }                                   \
  }while(0)


#endif //end MEM_DB_COMMON_H_
