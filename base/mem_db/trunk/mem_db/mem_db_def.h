// copyright:
//            (C) SINA Inc.
//
//      file: mem_db.h
//      desc: global header file for mem_db library
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date: 2012-10-10
//
//    change:


#ifndef MEM_DB_CONF_H_
#define MEM_DB_CONF_H_

#include <stdint.h>

#include <string>
#include <vector>

namespace mem_db {

typedef enum {
  MEM_DB_SUCCESS = 0,
  MEM_DB_FAILURE,
  MEM_DB_PARAMETER_ERR,          // error for parameter(s)
  MEM_DB_CFG_ERROR,              // invalidat format of config file
  MEM_DB_SYSTEM_ERROR,           // system error, shortage of memory
  MEM_DB_MEMORY_ERROR,
  MEM_DB_INVALID_DATA,
  MEM_DB_NO_MEMCAHCED_SERVER,
  MEM_DB_WRITE_FAILURE,
  MEM_DB_READ_FAILURE,
  MEM_DB_UNKNOWN_READ_FAILURE,
  MEM_DB_PROTOCOL_ERROR,
  MEM_DB_CLIENT_ERROR,
  MEM_DB_SERVER_ERROR,            // Server returns "SERVER_ERROR"
  MEM_DB_ERROR,                   // Server returns "ERROR"
  MEM_DB_DATA_EXISTS,
  MEM_DB_DATA_DOES_NOT_EXIST,
  MEM_DB_NOTSTORED,
  MEM_DB_STORED,
  MEM_DB_INVALID_EXPIRATION
} MemDBReturnType;

typedef struct {
  std::string               table_name;          // name of table
  std::vector<std::string>  row_keys;            // main key(s) for row(s) in table
  std::vector<std::string>  column_specifiers;   // column specified
} MutationTable;

typedef struct {
  std::string     name;          // name of column
  int32_t         time_to_live;  // time to live(in seconds)
  int32_t         family;        // family belonging to
  int32_t         bucket_id;     // bucket id
} ColumnDesp;

typedef struct {
  std::string     host_name;   // host name
  std::string     ip_addr;     // ip address
  int32_t         port;        // port
} HostInfo;

// Macro defined to avoid copy constructor and operator =
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName &); \
  void operator = (const TypeName &);

}  // namespace mem_db


#define DEPRECATED

#endif  // MEM_DB_H_
