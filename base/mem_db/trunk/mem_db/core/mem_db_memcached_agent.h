// copyright:
//            (C) SINA Inc.
//
//      file: mem_db_util.h
//      desc: utilities for mem_db
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date:
//
//    change:

#ifndef MEM_DB_MEMCACHED_AGENT_H_
#define MEM_DB_MEMCACHED_AGENT_H_

#include <libmemcached/memcached.h>

#include <mem_db/core/mem_db_common.h>
#include <mem_db/mem_db_def.h>
#include <mem_db/core/mem_db_def_internal.h>

namespace mem_db {

class MemcachedAgent {
  public:
    MemcachedAgent ();
    ~MemcachedAgent ();

    int MemcachedInit (const std::vector<HostInfo> &server_vector,
                       const FlagTokens &flags);
    int MemcachedFree (void);
    // fetch key-value from the memcached
    int MemcachedGet (const std::string &key,
                      std::string &value);
    int MemcachedGet (const std::vector<std::string> &keys,
                      std::vector<std::string> &values);
    // store values in the memcached
    int MemcachedSet (const std::string &key,
                      const std::string &value,
                      int expiration = 0);
    int MemcachedSet (const std::vector<std::string> &keys,
                      const std::vector<std::string> &values,
                      int expirations = 0);
    int MemcachedSet (const std::vector<std::string> &keys,
                      const std::vector<std::string> &values,
                      const std::vector<int> &expirations);
    // delete key-value record from
    int MemcachedDelete (const std::string &key);
    int MemcachedDelete (const std::vector<std::string> &keys);

    int max_key_length () {
      return max_key_length_;
    }
    int max_value_length () {
      return max_value_length_;
    }
    memcached_st *mem_st () {
      return mem_st_;
    }
  private:
    // set properties for libmemcached
    int SetProperities (const FlagTokens &mem_st_flags);

    // maximun length of keys allowed, the value is limited by libmmecached
    const int max_key_length_;
    const int max_value_length_;

    // the following data member is JUST for used for memcached_mget operations,
    // to avoid Memory Fragmentation by reusing the memory.
    // the memory is prealloced when calling MemcachedInit(), and do reallocing
    // if the space is not big enough.
    // The memory will be freed when calling MemcachedFree();
    char **key_container_;
    size_t *key_len_container_;

    // handler of instance for libmemcached
    memcached_st *mem_st_;

    // no copy and assignment
    DISALLOW_COPY_AND_ASSIGN (MemcachedAgent);
};

}   // namespace mem_db

#endif  // MEM_DB_MEMCACHED_AGENT_H_
