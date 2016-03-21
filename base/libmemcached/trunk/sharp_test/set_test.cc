#include <iostream>
#include <string>
#include <libmemcached/memcached.h>

#include "util.h"

using namespace std;

void print_msg(const std::string msg) {
  std::cout << msg << std::endl;
}

void print_msg(const std::string msg, int a) {
  std::cout << msg << " "<< a << std::endl;
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
  

  // The default method is MEMCACHED_DISTRIBUTION_MODULA.
  // You can enable consistent hashing by setting MEMCACHED_DISTRIBUTION_CONSISTENT.
  // Consistent hashing delivers better distribution and allows servers to be added
  // to the cluster with minimal cache losses.
  // Currently MEMCACHED_DISTRIBUTION_CONSISTENT is an alias for
  // the value MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA.
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_DISTRIBUTION, MEMCACHED_DISTRIBUTION_CONSISTENT); 


  // default is false(never remove failed servers)
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS, false); 
  // default is 5 
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT, 5); 
  // Causes libmemcached(3) to use asychronous IO.
  // This is the fastest transport available for storage functions.
  // default is false
  // 在网络连接的时候进行设置
  // 从效果上来看，使用no_block模式，故障恢复更快些，该功能是可以正常运行的，只是时间长短而已
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_NO_BLOCK, false); 






  // set behavior of the libmemcached instance
  // default is 0(only one copy of data)
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS, 0); 
  // default is 0(never timeout)
  // 在网络连接的时候通过setsockopt来设置
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_SND_TIMEOUT, 0); 
  // default is 0(never timeout)
  // 在网络连接的时候通过setsockopt来设置
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_RCV_TIMEOUT, 0); 
  // default is false(do not use binary protocol)
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, true); 
  // don't support cas(default)
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_SUPPORT_CAS, false); 
  // Enabling buffered IO causes commands to “buffer” instead of being sent.
  // Any action that gets data causes this buffer to be be sent to the remote connection.
  // Quiting the connection or closing down the connection will also cause
  // the buffered data to be pushed to the remote connection.
  // default is false
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_BUFFER_REQUESTS, false); 
  // default is false
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_TCP_KEEPIDLE, false); 
  // Find the current size of SO_SNDBUF
  // default is -1
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE, -1); 
  // Find the current size of SO_RECVBUF
  // default is -1
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE, -1); 
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_HASH_WITH_PREFIX_KEY, false); 
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_NOREPLY, false); 
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_RANDOMIZE_REPLICA_READ, false); 
  // In non-blocking mode this changes the value of the timeout during socket connection
  // in milliseconds. Specifying -1 means an infinite time‐out.
  // #define MEMCACHED_DEFAULT_CONNECT_TIMEOUT 4000 
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, 4000); 
  // When enabled a host which is problematic will only be checked for usage
  // based on the amount of time set by this behavior. The value is in seconds.
  // #define MEMCACHED_SERVER_FAILURE_RETRY_TIMEOUT 2
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, 2); 
  // #define MEMCACHED_SERVER_FAILURE_DEAD_TIMEOUT 0 
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_DEAD_TIMEOUT, 0); 
  // Modify the timeout value that is used by poll. The default value is -1.
  // An signed int must be passed to memcached_behavior_set to change this value
  // (this requires casting). For memcached_behavior_get a signed int value will be
  // cast and returned as the unsigned long long.
  // #define MEMCACHED_DEFAULT_TIMEOUT 5000 
  memcached_behavior_set(mem_instance, MEMCACHED_BEHAVIOR_POLL_TIMEOUT, 5000); 

  // set data into memcached
  std::string origin_key;
  std::string value;
  origin_key = "key";
  value = "value";

  int i = 0;
  while(true) {
    std::string key = GenerateNextKey(origin_key);
    ::usleep(10);
    ret = memcached_set(mem_instance, key.c_str(), key.length(), value.c_str(), value.length(), 0, 0);
    if (0 != ret) {
      print_msg("error in memcached_set", ret);
    } else {
      print_msg("success in memcached_set", ret);
    }
  }

#if 0
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
#endif
}


int main(int argc, char **argv) {

  classic_usage();

  return 0;
}
