// copyright:
//            (C) SINA Inc.
//
//      file: mem_db_def_internal.h
//      desc: definition of structures  for internal usage
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date:
//
//    change:

#ifndef MEM_DB_DEF_INTERNAL_H_
#define MEM_DB_DEF_INTERNAL_H_

#include <mem_db/core/mem_db_common.h>
#include <mem_db/mem_db_def.h>

namespace mem_db {

typedef struct {
  int num_of_replicas; // number of copies for data item
  int consistent_hashing; // enable consistent hashing or not
  int binary_protocol;    // binary protocol or text
  int noreply;            // enable noreply
  int randomize_replicas_reads; // enable randomizing the replica reads
  int auto_remove_failed_servers;  // enable MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS or not
  int failed_server_dead_timeout; // 
  int server_failure_limit;
} FlagTokens;

// definition of table
typedef struct {
  // table name
  std::string name;
  // attributes for table
  std::vector<std::string> attrs;
  // attributes that build up the main key
  // if number of attributes that build up the main key is more than one,
  // then the attributes are sparated by charactor ";"
  std::vector<std::string> key_attrs;
  // keys that record rows
  std::vector<std::string> keys;  // keys for

} BaseTable;

}  // namespace mem_db

#endif  //  MEM_DB_DEF_INTERNAL_H_
