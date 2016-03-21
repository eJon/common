// copyright:
//            (C) SINA Inc.
//
//      file: mem_db.h
//      desc:
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date:
//
//    change:

#include <mem_db/core/mem_db_memcached_agent.h>
#include <mem_db/core/mem_db_util.h>

namespace mem_db {

// maximnm length of keys allowed in libmemcached(Byte)
#define MAX_KEY_LENGTH_IN_LIBMEMCACHED 250
// maximnm length of values allowed in libmemcached(Byte)
#define MAX_VALUE_LENGTH_IN_LIBMEMCACHED 1024 * 1024
// alive forever for data in mem_db by default
#define DEFAULT_EXPIRATION_TIME_FOR_MEM_DB 0

// prealloced memory size for storing vectors of keys when calling
// memcached_mget to avoid Memory Fragmentation
#define PREALLOCAED_MEM_LENGTH_FOR_KEY_CONTAINER 1024



MemcachedAgent::MemcachedAgent ()
  :  max_key_length_ (MAX_KEY_LENGTH_IN_LIBMEMCACHED),
     max_value_length_ (MAX_VALUE_LENGTH_IN_LIBMEMCACHED),
     key_container_ (NULL),
     key_len_container_ (NULL),
     mem_st_ (NULL) {
}

MemcachedAgent::~MemcachedAgent () {
  this->MemcachedFree ();
}

int MemcachedAgent::MemcachedInit (const std::vector<HostInfo> &server_vector,
                                   const FlagTokens &flags) {
  // create libmemcached
  mem_st_ = memcached_create (NULL);

  if (NULL == mem_st_) {
    return MEM_DB_SYSTEM_ERROR;
  }

  int ret = 0;
  // set properties for libmemcached
  ret = this->SetProperities (flags);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  // initialize memcached server list
  size_t len = server_vector.size ();

  if (len <= 0) {
    return MEM_DB_NO_MEMCAHCED_SERVER;
  }

  for (size_t i = 0; i < len; ++i) {
    ret = memcached_server_add (mem_st_, server_vector[i].ip_addr.c_str (),
                                server_vector[i].port);

    if (MEMCACHED_SUCCESS != ret) {
      return MEM_DB_SYSTEM_ERROR;
    }
  }

  //allocate memory
  while (! (key_container_ = (char **)malloc (sizeof (char *) * PREALLOCAED_MEM_LENGTH_FOR_KEY_CONTAINER))) {
    ::usleep (1000);
  }

  while (! (key_len_container_ = (size_t *)malloc (sizeof (size_t) * PREALLOCAED_MEM_LENGTH_FOR_KEY_CONTAINER))) {
    ::usleep (1000);
  }

  return MEM_DB_SUCCESS;
}

int MemcachedAgent::SetProperities (const FlagTokens &mem_st_flags) {
  if (NULL == mem_st_) {
    return MEM_DB_INVALID_DATA;
  }

  int ret = 0;

  // consistent hashing
  if (mem_st_flags.consistent_hashing) {
    ret = memcached_behavior_set (mem_st_, MEMCACHED_BEHAVIOR_DISTRIBUTION,
                                  MEMCACHED_DISTRIBUTION_CONSISTENT);

    if (MEMCACHED_SUCCESS != ret) {
      return MEM_DB_SYSTEM_ERROR;
    }
  }

  if (mem_st_flags.auto_remove_failed_servers) {
    // Enable MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS behavior
    ret = memcached_behavior_set (mem_st_, MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS, true);

    if (MEMCACHED_SUCCESS != ret) {
      return MEM_DB_SYSTEM_ERROR;
    }
  }

  if (mem_st_flags.failed_server_dead_timeout) {
    // Enable MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS behavior
    ret = memcached_behavior_set (mem_st_, MEMCACHED_BEHAVIOR_DEAD_TIMEOUT,
                                  mem_st_flags.failed_server_dead_timeout);
    if (MEMCACHED_SUCCESS != ret) {
      return MEM_DB_SYSTEM_ERROR;
    }
  }

  if (mem_st_flags.server_failure_limit) {
    // Enable MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS behavior
    ret = memcached_behavior_set (mem_st_, MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT,
                                  mem_st_flags.server_failure_limit);
    if (MEMCACHED_SUCCESS != ret) {
      return MEM_DB_SYSTEM_ERROR;
    }
  }

  if (mem_st_flags.noreply) {
    // Set this value to specify that the result from your storage commands (set,
    // add, replace, append, prepend) is not cared about.
    ret = memcached_behavior_set (mem_st_, MEMCACHED_BEHAVIOR_NOREPLY, true);

    if (MEMCACHED_SUCCESS != ret) {
      return MEM_DB_SYSTEM_ERROR;
    }
  }

  if (0 < mem_st_flags.num_of_replicas) {
    // specify the numbers of replicas libmemcached should store of each item
    // THERE IS DOES SOME UGLY AND AWEFUL FOR SUAGE OF OPTION
    // MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS. if I want do a double data item
    // storing, I have to set MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS, to be 1;
    // if I want to do a triple storage of a data item, I have to set option
    // MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS to be 2. I DO NOT know why.
    ret = memcached_behavior_set (mem_st_, MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS,
                                  mem_st_flags.num_of_replicas - 1);

    if (MEMCACHED_SUCCESS != ret) {
      return MEM_DB_SYSTEM_ERROR;
    }
  }

  if (0 < mem_st_flags.randomize_replicas_reads) {
    // Allows randomizing the replica reads from copies of item
    ret = memcached_behavior_set (mem_st_, MEMCACHED_BEHAVIOR_RANDOMIZE_REPLICA_READ,
                                  mem_st_flags.randomize_replicas_reads);

    if (MEMCACHED_SUCCESS != ret) {
      return MEM_DB_SYSTEM_ERROR;
    }
  }

  if (0 < mem_st_flags.binary_protocol) {
    // Enable the use of the binary protocol. Please note that you cannot toggle
    // this flag on an open connection
    int ret = memcached_behavior_set (mem_st_, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL,
                                      true);

    if (MEMCACHED_SUCCESS != ret) {
      return MEM_DB_SYSTEM_ERROR;
    }
  }

  return MEM_DB_SUCCESS;
}


int MemcachedAgent::MemcachedFree () {
  if (NULL != mem_st_) {
    memcached_free (mem_st_);
    mem_st_ = NULL;
  }

  free (key_container_);
  key_container_ = NULL;
  key_len_container_ = NULL;

  return MEM_DB_SUCCESS;
}

int MemcachedAgent::MemcachedGet (const std::string &key, std::string &value) {
  if (NULL == mem_st_) {
    return MEM_DB_INVALID_DATA;
  }

  if (static_cast<size_t> (this->max_key_length_) < key.size ()) {
    return MEM_DB_PARAMETER_ERR;
  }

  value.clear ();

  int ret = MEM_DB_SUCCESS;
  size_t result_length = 0;
  // call memcached API to store the value
  memcached_return_t rett;
  char *result = memcached_get (mem_st_, key.c_str (), key.size (),
                                &result_length, 0, &rett);

  // NOTE!!!---> NULL cannot be converted to string directly
  switch (rett) {
    case MEMCACHED_NOTFOUND:
      value = "";
      break;

    case MEMCACHED_BUFFERED:
    case MEMCACHED_SUCCESS:
      if (result) {
        value = std::string (result, result_length);
      } else {
        value = "";
      }

      break;

    default:
      value = "";
      ret = rett;
  }

  free (result);

  return ret;
}

int MemcachedAgent::MemcachedGet (const std::vector<std::string> &keys,
                                  std::vector<std::string> &values) {
  if (NULL == mem_st_) {
    return MEM_DB_INVALID_DATA;
  }

  values.clear ();

  size_t num_of_keys = keys.size ();

  for (size_t i = 0; i < num_of_keys; ++i) {
    if (keys[i].size () > static_cast<size_t> (this->max_key_length_)) {
      return MEM_DB_PARAMETER_ERR;
    }
  }

  int ret = 0;

#define MULTI_GET
#ifdef MULTI_GET
  key_container_ = (char **)::realloc (key_container_,
                                       num_of_keys * sizeof (char *));
  key_len_container_ = (size_t *)::realloc (key_len_container_,
                       num_of_keys * sizeof (size_t));

  for (size_t i = 0; i < num_of_keys; ++i) {
    key_len_container_[i] = keys[i].size ();
    // do a const cast
    key_container_[i] = const_cast<char *> (keys[i].c_str ());
  }

  // call libmemcached API to multi-get values
  ret = memcached_mget (mem_st_, key_container_,
                        key_len_container_, num_of_keys);

  switch (ret) {
    case MEMCACHED_NOTFOUND:
    case MEMCACHED_SUCCESS:
    case MEMCACHED_BUFFERED:
      break;

    default:
      return ret;
  }

  memcached_result_st *mem_result = memcached_result_create (mem_st_, NULL);

  if (NULL == mem_result) {
    return MEM_DB_SYSTEM_ERROR;
  }

  std::string result_value;
  std::string result_key;
  std::map<std::string, std::string> temp_result_map;
  memcached_return_t mem_ret;

  while (true) {
    mem_result = memcached_fetch_result (mem_st_, mem_result, &mem_ret);

    // if (MEMCACHED_NOTFOUND  == mem_ret) continue;
    if (MEMCACHED_IN_PROGRESS  == mem_ret) {
      continue;
    }

    if (NULL == mem_result) {
      break;
    }

    if ((mem_ret != MEMCACHED_SUCCESS) && (mem_ret != MEMCACHED_NOTFOUND)
        && (mem_ret != MEMCACHED_END)) {
      break;
    }

    // get key and value
    result_key.assign (memcached_result_key_value (mem_result),
                       memcached_result_key_length (mem_result));
    result_value.assign (memcached_result_value (mem_result),
                         memcached_result_length (mem_result));
    //if ((0 == result_key.size()) || (0 == result_value.size())) {
    //  break;
    //}
    // insert the key-value in the temporary map for later sorting based on
    // input keys
    temp_result_map.insert (std::map<std::string, std::string>::value_type (
                              result_key, result_value));
  }

  for (size_t i = 0; i < num_of_keys; ++i) {
    // push values into output parameter based on input keys
    std::map<std::string, std::string>::iterator iter =
      temp_result_map.find (keys[i]);

    if (iter != temp_result_map.end ()) {
      values.push_back (iter->second);
    } else {
      values.push_back ("");
    }
  }

  if (NULL != mem_result) {
    // free the memory
    memcached_result_free (mem_result);
    mem_result = NULL;
  }

#else
  std::string value;

  for (size_t i = 0; i < num_of_keys; ++i) {
    ret = this->MemcachedGet (keys[i], value);

    if (MEM_DB_SUCCESS != ret) {
      return ret;
    }

    values.push_back (value);
  }

#endif

  return MEM_DB_SUCCESS;
}

int MemcachedAgent::MemcachedSet (const std::string &key,
                                  const std::string &value,
                                  int expiration) {
  if (NULL == mem_st_) {
    return MEM_DB_INVALID_DATA;
  }

  if ((static_cast<size_t> (this->max_key_length_) < key.size ())
      || (static_cast<size_t> (this->max_value_length_) < value.size ())) {
    return MEM_DB_PARAMETER_ERR;
  }

  if (expiration < 0) {
    return MEM_DB_INVALID_EXPIRATION;
  }

  int ret = 0;
  // call memcached API to store the value
  time_t amend_expiration;
  AmendExpirationTimeValue ((time_t)expiration, amend_expiration);
  int retry_times = 3;

  for (int i = 0; i < retry_times; ++i) {
    ret = memcached_set (mem_st_, key.c_str (), key.size (),
                         value.c_str (), value.size (), amend_expiration,
                         0);

    if (0 == ret) {
      //if (MEMCACHED_SERVER_MARKED_DEAD != ret) {
      break;
    }
  }

  switch (ret) {
    case MEMCACHED_NOTFOUND:
    case MEMCACHED_SUCCESS:
    case MEMCACHED_BUFFERED:
      break;

    default:
      return ret;
  }

  return MEM_DB_SUCCESS;
}

int MemcachedAgent::MemcachedSet (const std::vector<std::string> &keys,
                                  const std::vector<std::string> &values,
                                  const std::vector<int> &expirations) {
  if (NULL == mem_st_) {
    return MEM_DB_INVALID_DATA;
  }

  int ret = 0;
  size_t key_vector_size = keys.size ();
  size_t value_vector_size = values.size ();

  if (key_vector_size != value_vector_size) {
    return MEM_DB_PARAMETER_ERR;
  }

  for (size_t i = 0; i < key_vector_size; ++i) {
    ret = this->MemcachedSet (keys[i], values[i], expirations[i]);

    if (MEM_DB_SUCCESS != ret) {
      return ret;
    }
  }

  return MEM_DB_SUCCESS;
}

int MemcachedAgent::MemcachedSet (const std::vector<std::string> &keys,
                                  const std::vector<std::string> &values,
                                  int expirations) {
  if (NULL == mem_st_) {
    return MEM_DB_INVALID_DATA;
  }

  int ret = 0;
  size_t key_vector_size = keys.size ();
  size_t value_vector_size = values.size ();

  if (key_vector_size != value_vector_size) {
    return MEM_DB_PARAMETER_ERR;
  }

  for (size_t i = 0; i < key_vector_size; ++i) {
    ret = this->MemcachedSet (keys[i], values[i], expirations);

    if (MEM_DB_SUCCESS != ret) {
      return ret;
    }
  }

  return MEM_DB_SUCCESS;
}

int MemcachedAgent::MemcachedDelete (const std::string &key) {
  if (NULL == mem_st_) {
    return MEM_DB_INVALID_DATA;
  }

  int ret = 0;

  // delete all key-value pairs in memcached key
  ret = memcached_delete (mem_st_, key.c_str (), key.size (), 0);

  switch (ret) {
    case MEMCACHED_NOTFOUND:
    case MEMCACHED_SUCCESS:
    case MEMCACHED_BUFFERED:
      break;

    default:
      return ret;
  }

  return MEM_DB_SUCCESS;
}

int MemcachedAgent::MemcachedDelete (const std::vector<std::string> &keys) {
  if (NULL == mem_st_) {
    return MEM_DB_INVALID_DATA;
  }

  int ret = 0;

  for (std::vector<std::string>::const_iterator iter = keys.begin ();
       iter != keys.end (); iter++) {
    // delete all key-value pairs in memcached by each key in keys
    ret = this->MemcachedDelete (*iter);

    if (MEM_DB_SUCCESS != ret) {
      return ret;
    }
  }

  return MEM_DB_SUCCESS;
}

}   // namespace mem_db
