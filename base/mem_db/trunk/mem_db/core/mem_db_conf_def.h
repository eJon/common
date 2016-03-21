// copyright:
//            (C) SINA Inc.
//
//      file: reserve_config_def.h
//      desc: definitions of section names and options in configure filer
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date:
//
//    change:

#ifndef MEM_DB_CONF_DEF_H_
#define MEM_DB_CONF_DEF_H_

#include <mem_db/core/mem_db_def_internal.h>

namespace mem_db {

// load config file
int LoadConfigFile (const char *config_file,
                    std::vector<HostInfo> &server_vector,
                    FlagTokens &flag_tokens);


int LoadConfigFile (const char *config_file,
                    std::map<std::string, int> &table_expiration,
                    int &default_expiration_for_table,
                    int &default_expiration_for_general_kv);

int LoadConfigFile (const char *config_file, 
                    int &auto_load_config,
                    int &interval_for_auto_load_config);

}  // namespace

#endif  // MEM_DB_CONF_DEF_H_ 
