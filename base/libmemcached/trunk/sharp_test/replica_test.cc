#include <iostream>
#include <string>
#include <libmemcached/memcached.h>

using namespace std;

void print_msg(const std::string msg) {
  std::cout << msg << std::endl;
}

void classic_usage() {

  memcached_return_t ret = MEMCACHED_SUCCESS;
  // create instance
  memcached_st *mem_instance = memcached_create(NULL);
  if (NULL == mem_instance) {
    print_msg("create mem instance error"); 
  }

  // add server list info
  memcached_server_add(mem_instance, "127.0.0.1", 11211);
  memcached_server_add(mem_instance, "127.0.0.1", 11212);
  memcached_server_add(mem_instance, "127.0.0.1", 11213);
  memcached_server_add(mem_instance, "127.0.0.1", 11214);
  memcached_server_add(mem_instance, "127.0.0.1", 11215);
  
  // set behavior of the libmemcached instance
  // default is 0(only one copy of data)
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS, 1); 
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_RANDOMIZE_REPLICA_READ, false); 
  // default is false(never remove failed servers)
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS, false); 
  // The default method is MEMCACHED_DISTRIBUTION_MODULA.
  // You can enable consistent hashing by setting MEMCACHED_DISTRIBUTION_CONSISTENT.
  // Consistent hashing delivers better distribution and allows servers to be added
  // to the cluster with minimal cache losses.
  // Currently MEMCACHED_DISTRIBUTION_CONSISTENT is an alias for
  // the value MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA.
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_DISTRIBUTION, MEMCACHED_DISTRIBUTION_CONSISTENT); 

  // set data into memcached
  std::string key;
  std::string value;
  key = "key";
  value = "value";
  ret = memcached_set(mem_instance, key.c_str(), key.length(), value.c_str(), value.length(), 0, 0);
  if (0 != ret) {
    print_msg("error in memcached_set");
  } else {
    print_msg("success in memcached_set");
  }

  // get data from memcached
  char *store_value = NULL;
  size_t value_length = 0;
  uint32_t flags = 0;
  store_value = memcached_get(mem_instance, key.c_str(), key.length(), &value_length, &flags, &ret); 
  if (NULL != store_value) {
    print_msg(store_value);
  } else {
    print_msg("NULL");
  }

  // delete data in the  memcached cluster
  ret = memcached_delete(mem_instance, key.c_str(), key.length(), 0);
  if (0 != ret) {
    print_msg("error in memcached_delete");
  } else {
    print_msg("success in memcached_delete");
  }

  store_value = memcached_get(mem_instance, key.c_str(), key.length(), &value_length, &flags, &ret); 
  if (NULL != store_value) {
    print_msg(store_value);
  } else {
    print_msg("NULL");
  }
}


int main(int argc, char **argv) {

  classic_usage();

  return 0;
}
