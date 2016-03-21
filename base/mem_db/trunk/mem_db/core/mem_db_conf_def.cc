// copyright:
//            (C) SINA Inc.
//
//      file: mem_db_conf_def.cc
//      desc:
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date:
//
//    change:

#include <mem_db/core/mem_db_conf_def.h>
#include <mem_db/core/mem_db_common.h>

namespace mem_db {

// default number of copies per data item
#define DEFAULT_NUM_OF_COPIES_PER_DATA_ITEM (1)

// fileds in config file
static const char *kAreaMemcached = "memcached";
static const char *kAreaLibmemcached = "libmemcached";

// records in config file
static const char *kMemcachedServerGroup = "server_group";
static const char *kLibmemcachedCopiesPerDataItem = "copies_per_data_item";
static const char *kLibmemcachedBinaryProtocal = "binary_protocal";
static const char *kLibmemcachedConsistentHashing = "consistent_hashing";
static const char *kLibmemcachedNoreply = "noreply";
static const char *kLibmemcachedRandomizeReplicasReads = "randomize_replicas_reads";
static const char *kLibmemcachedAutoRemoveFailedServers = "auto_remove_failed_servers";
static const char *kLibmemcachedFailedServerDeadTimeout = "failed_server_dead_timeout";
static const char *kLibmemcachedServerFailureLimit = "server_failure_limit";

static const char *kAreaExpiration = "expiration";
static const char *kExpirationTable = "table";
static const char *kExpirationDefault = "default";
static const char *kExpirationDefaultGeneralKv= "default_expiration_for_general_kv";

static const char *kAreaGlobal= "global";
static const char *kGlobalAutoLoadConfig = "auto_load_config";
static const char *kGlobalIntervalForAutoLoadConfig = "interval_for_auto_load_config";


// load configure file ainitialize flags for libmmecached
int LoadConfigFile (const char *config_file, std::vector<HostInfo> &server_vector,
                    FlagTokens &flag_tokens) {
  if (NULL == config_file) {
    return MEM_DB_PARAMETER_ERR;
  }
  
  server_vector.clear();
  memset (&flag_tokens, 0, sizeof (flag_tokens));

  mem_db_util::INIParser init_parser (config_file);
  std::string server_str =
    init_parser.get_string (kAreaMemcached, // area
                            kMemcachedServerGroup, // key
                            ""); // default value
  std::set<std::string> server_set;
  mem_db_util::ADutil::Split3WithoutNull (server_set, server_str, ';');

  for ( std::set<std::string>::iterator iter = server_set.begin ();
        iter != server_set.end (); iter++) {
    // example for server_str
    // 10.210.208.39:11211;10.210.208.39:11222
    // first, split the string into std::set by charactor ';'
    // then, split the sub_string into std::vector by charactor ':'
    std::vector<std::string> hostent;
    mem_db_util::ADutil::Split2 (hostent, *iter, ':');

    if (2 != hostent.size ()) {
      return MEM_DB_CFG_ERROR;
    }

    HostInfo host_info = {"",          // empty by default
                          hostent[0],  // ip address
                          atoi (hostent[1].c_str ())
                         }; // port
    server_vector.push_back (host_info);
  }

  if (0 == server_vector.size ()) {
    return MEM_DB_CFG_ERROR;
  }

  flag_tokens.num_of_replicas =
    init_parser.get_int (kAreaLibmemcached, // area
                         kLibmemcachedCopiesPerDataItem, // key
                         DEFAULT_NUM_OF_COPIES_PER_DATA_ITEM); // value by default
  flag_tokens.binary_protocol =
    init_parser.get_int (kAreaLibmemcached, // area
                         kLibmemcachedBinaryProtocal, // key
                         0); // value by default
  flag_tokens.consistent_hashing =
    init_parser.get_int (kAreaLibmemcached, // area
                         kLibmemcachedConsistentHashing, // key
                         0); // value by default
  flag_tokens.noreply =
    init_parser.get_int (kAreaLibmemcached, // area
                         kLibmemcachedNoreply, // key
                         0); // value by default
  flag_tokens.randomize_replicas_reads =
    init_parser.get_int (kAreaLibmemcached, // area
                         kLibmemcachedRandomizeReplicasReads, // key
                         0); // value by default
  flag_tokens.auto_remove_failed_servers =
    init_parser.get_int (kAreaLibmemcached, // area
                         kLibmemcachedAutoRemoveFailedServers, // key
                         0); // value by default
  flag_tokens.failed_server_dead_timeout =
    init_parser.get_int (kAreaLibmemcached, // area
                         kLibmemcachedFailedServerDeadTimeout, // key
                         0); // value by default
  flag_tokens.server_failure_limit=
    init_parser.get_int (kAreaLibmemcached, // area
                         kLibmemcachedServerFailureLimit, // key
                         0); // value by default

  return MEM_DB_SUCCESS;
}

int LoadConfigFile (const char *config_file,
                    std::map<std::string, int> &table_expiration,
                    int &default_expiration_for_table,
                    int &default_expiration_for_general_kv) {
  if (NULL == config_file) {
    return MEM_DB_PARAMETER_ERR;
  }

  table_expiration.clear();

  mem_db_util::INIParser init_parser (config_file);
  std::string server_str =
    init_parser.get_string (kAreaExpiration, // area
                            kExpirationTable, // key
                            ""); // default value
  std::set<std::string> server_set;
  mem_db_util::ADutil::Split3WithoutNull (server_set, server_str, ';');

  for ( std::set<std::string>::iterator iter = server_set.begin ();
        iter != server_set.end (); iter++) {
    // example for expiration
    // user_info:691200;enterprise_info:691200;ad_info=691200
    // first, split the string into std::set by charactor ';'
    // then, split the sub_string into std::vector by charactor ':'
    std::vector<std::string> hostent;
    mem_db_util::ADutil::Split2 (hostent, *iter, ':');

    if (2 != hostent.size ()) {
      return MEM_DB_CFG_ERROR;
    }

    table_expiration[hostent[0]] = atoi (hostent[1].c_str ());
  }

  default_expiration_for_table = init_parser.get_int (kAreaExpiration, // area
                                                      kExpirationDefault, // key
                                                      0); // value by default
  default_expiration_for_general_kv = init_parser.get_int (kAreaExpiration, // area
                                                           kExpirationDefaultGeneralKv, // key
                                                           0); // value by default
  return MEM_DB_SUCCESS;
}

int LoadConfigFile (const char *config_file, 
                    int &auto_load_config,
                    int &interval_for_auto_load_config) {
  mem_db_util::INIParser init_parser (config_file);

  auto_load_config = init_parser.get_int (kAreaGlobal, // area
                                          kGlobalAutoLoadConfig, // key
                                          0); // value by default
  interval_for_auto_load_config = init_parser.get_int (kAreaGlobal, // area
                                                       kGlobalIntervalForAutoLoadConfig, // key
                                                       0); // value by default
  return MEM_DB_SUCCESS;
}

}  // namespace mem_db
