#include <gtest/gtest.h>
#include <limits.h>

#include <mem_db/core/mem_db_memcached_agent.h>
#include <mem_db/core/mem_db_def_internal.h>
#include <mem_db/mem_db_def.h>

using mem_db::MemcachedAgent;
using mem_db::FlagTokens;
using mem_db::HostInfo;

TEST (MemcachedAgentTest, ConstructorNormal) {
  MemcachedAgent memcached_agent;

  ASSERT_TRUE (250 == memcached_agent.max_key_length ());
  ASSERT_TRUE ((1024 * 1024) == memcached_agent.max_value_length ());
}

TEST (MemcachedAgentTest, DestructorNormal) {
  MemcachedAgent memcached_agent;
  memcached_agent.~MemcachedAgent ();

}

TEST (MemcachedAgentTest, MemcachedInitNormal) {
  MemcachedAgent memcached_agent;

  FlagTokens flag_tokens = {0};

  std::vector<HostInfo> server_vector;
  HostInfo host_v1 = {"", "127.0.0.1", 20401};
  HostInfo host_v2 = {"", "127.0.0.1", 20402};
  // MAKE SURE that the above 2 servers are working
  server_vector.push_back (host_v1);
  server_vector.push_back (host_v2);

  int ret = 0;
  ret = memcached_agent.MemcachedInit (server_vector, flag_tokens);

  ASSERT_TRUE (0 == ret);
}


TEST (MemcachedAgentTest, MemcachedFreeNormal) {
  MemcachedAgent memcached_agent;

  FlagTokens flag_tokens = {0};

  std::vector<HostInfo> server_vector;
  HostInfo host_v1 = {"", "127.0.0.1", 20401};
  HostInfo host_v2 = {"", "127.0.0.1", 20402};
  // MAKE SURE that the above 2 servers are working
  server_vector.push_back (host_v1);
  server_vector.push_back (host_v2);

  int ret = 0;
  ret = memcached_agent.MemcachedInit (server_vector, flag_tokens);
  ASSERT_TRUE (0 == ret);
  memcached_agent.MemcachedFree ();

}

TEST (MemcachedAgentTest, MemcachedSetNormal) {
  MemcachedAgent memcached_agent;

  FlagTokens flag_tokens = {0};

  std::vector<HostInfo> server_vector;
  HostInfo host_v1 = {"", "127.0.0.1", 20401};
  HostInfo host_v2 = {"", "127.0.0.1", 20402};
  // MAKE SURE that the above 2 servers are working
  server_vector.push_back (host_v1);
  server_vector.push_back (host_v2);

  int ret = 0;
  ret = memcached_agent.MemcachedInit (server_vector, flag_tokens);

  std::string key = "bass";
  std::string value = "walking bass";

  ret = memcached_agent.MemcachedSet ("bass", "walking bass");
  ASSERT_TRUE (0 == ret);
  size_t result_length = 0;
  memcached_return_t mem_ret;
  char *result = memcached_get (memcached_agent.mem_st (), key.c_str (),
                                key.size (), &result_length, 0, &mem_ret);

  std::string result_value;

  if (NULL == result) {
    result_value = "";
  } else {
    result_value = std::string (result);
  }

  ASSERT_TRUE (0 == result_value.compare (value));
  memcached_agent.MemcachedFree ();
}

TEST (MemcachedAgentTest, MemcachedSetVectorNormal) {
  MemcachedAgent memcached_agent;

  FlagTokens flag_tokens = {0};

  std::vector<HostInfo> server_vector;
  HostInfo host_v1 = {"", "127.0.0.1", 20401};
  HostInfo host_v2 = {"", "127.0.0.1", 20402};
  // MAKE SURE that the above 2 servers are working
  server_vector.push_back (host_v1);
  server_vector.push_back (host_v2);

  int ret = 0;
  ret = memcached_agent.MemcachedInit (server_vector, flag_tokens);

  std::vector<std::string> keys;
  std::vector<std::string> values;

  keys.push_back ("buffalofish");
  values.push_back ("Iotiobus");
  keys.push_back ("carp");
  values.push_back ("yprinus");
  ret = memcached_agent.MemcachedSet (keys, values);
  ASSERT_TRUE (0 == ret);

  memcached_return_t mem_ret;
  size_t result_length = 0;
  char *result = memcached_get (memcached_agent.mem_st (), keys[0].c_str (),
                                keys[0].size (), &result_length, 0, &mem_ret);
  std::string value;

  if (NULL == result) {
    value = "";
  } else {
    value = std::string (result);
  }

  ASSERT_TRUE (0 == value.compare ("Iotiobus"));
  result = memcached_get (memcached_agent.mem_st (), keys[1].c_str (),
                          keys[1].size (), &result_length, 0, &mem_ret);

  if (NULL == result) {
    value = "";
  } else {
    value = std::string (result);
  }

  ASSERT_TRUE (0 == value.compare ("yprinus"));

  memcached_agent.MemcachedFree ();
}

TEST (MemcachedAgentTest, MemcachedGetNormal) {
  MemcachedAgent memcached_agent;

  FlagTokens flag_tokens = {0};

  std::vector<HostInfo> server_vector;
  HostInfo host_v1 = {"", "127.0.0.1", 20401};
  HostInfo host_v2 = {"", "127.0.0.1", 20402};
  // MAKE SURE that the above 2 servers are working
  server_vector.push_back (host_v1);
  server_vector.push_back (host_v2);

  int ret = 0;
  ret = memcached_agent.MemcachedInit (server_vector, flag_tokens);

  ret = memcached_agent.MemcachedSet ("bass", "walking bass");
  ASSERT_TRUE (0 == ret);

  std::string result;
  ret = memcached_agent.MemcachedGet ("bass", result);
  ASSERT_TRUE (0 == ret);

  ASSERT_TRUE (0 == result.compare ("walking bass"));

  ret = memcached_agent.MemcachedSet ("&&", "&&&&&&&&&");
  ret = memcached_agent.MemcachedGet ("&&", result);
  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (0 == result.compare ("&&&&&&&&&"));

  memcached_agent.MemcachedFree ();
}

TEST (MemcachedAgentTest, MemcachedGetVectorNormal) {
  MemcachedAgent memcached_agent;

  FlagTokens flag_tokens = {0};

  std::vector<HostInfo> server_vector;
  HostInfo host_v1 = {"", "127.0.0.1", 20401};
  HostInfo host_v2 = {"", "127.0.0.1", 20402};
  // MAKE SURE that the above 2 servers are working
  server_vector.push_back (host_v1);
  server_vector.push_back (host_v2);

  int ret = 0;
  ret = memcached_agent.MemcachedInit (server_vector, flag_tokens);

  std::vector<std::string> keys;
  std::vector<std::string> values;

  keys.push_back ("buffalofish");
  values.push_back ("Iotiobus");
  keys.push_back ("carp");
  values.push_back ("yprinus");
  ret = memcached_agent.MemcachedSet (keys, values);
  ASSERT_TRUE (0 == ret);

  std::vector<std::string> results;
  ret = memcached_agent.MemcachedGet (keys, results);
  ASSERT_TRUE (0 == ret);

  ASSERT_TRUE (0 == std::string (results[0]).compare (values[0]));
  ASSERT_TRUE (0 == std::string (results[1]).compare (values[1]));
  memcached_agent.MemcachedFree ();
}

TEST (MemcachedAgentTest, MemcachedDeleteNormal) {
  MemcachedAgent memcached_agent;

  FlagTokens flag_tokens = {0};

  std::vector<HostInfo> server_vector;
  HostInfo host_v1 = {"", "127.0.0.1", 20401};
  HostInfo host_v2 = {"", "127.0.0.1", 20402};
  // MAKE SURE that the above 2 servers are working
  server_vector.push_back (host_v1);
  server_vector.push_back (host_v2);

  int ret = 0;
  ret = memcached_agent.MemcachedInit (server_vector, flag_tokens);

  std::string key = "bass";
  ret = memcached_agent.MemcachedSet (key, "walking bass");
  ASSERT_TRUE (0 == ret);

  ret = memcached_agent.MemcachedDelete (key);
  ASSERT_TRUE (0 == ret);

  memcached_return_t mem_ret;
  size_t result_length = 0;
  char *result = memcached_get (memcached_agent.mem_st (), key.c_str (),
                                key.size (), &result_length, 0, &mem_ret);

  ASSERT_TRUE (NULL == result);

  memcached_agent.MemcachedFree ();
}

TEST (MemcachedAgentTest, MemcachedDeleteVectorNormal) {
  MemcachedAgent memcached_agent;

  FlagTokens flag_tokens = {0};

  std::vector<HostInfo> server_vector;
  HostInfo host_v1 = {"", "127.0.0.1", 20401};
  HostInfo host_v2 = {"", "127.0.0.1", 20402};
  // MAKE SURE that the above 2 servers are working
  server_vector.push_back (host_v1);
  server_vector.push_back (host_v2);

  int ret = 0;
  ret = memcached_agent.MemcachedInit (server_vector, flag_tokens);

  std::vector<std::string> keys;
  std::vector<std::string> values;

  keys.push_back ("buffalofish");
  values.push_back ("Iotiobus");
  keys.push_back ("carp");
  values.push_back ("yprinus");
  ret = memcached_agent.MemcachedSet (keys, values);
  ASSERT_TRUE (0 == ret);
  ret = memcached_agent.MemcachedDelete (keys);
  ASSERT_TRUE (0 == ret);

  memcached_return_t mem_ret;
  size_t result_length = 0;
  char *result = memcached_get (memcached_agent.mem_st (), keys[0].c_str (),
                                keys[0].size (), &result_length, 0, &mem_ret);
  std::string value;

  if (NULL == result) {
    value = "";
  } else {
    value = std::string (result);
  }

  ASSERT_TRUE (0 == value.compare (""));
  result = memcached_get (memcached_agent.mem_st (), keys[1].c_str (),
                          keys[1].size (), &result_length, 0, &mem_ret);

  if (NULL == result) {
    value = "";
  } else {
    value = std::string (result);
  }

  ASSERT_TRUE (0 == value.compare (""));
  memcached_agent.MemcachedFree ();
}

