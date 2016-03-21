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

#ifndef MEM_DB_UTIL_H_
#define MEM_DB_UTIL_H_

#include <mem_db/core/mem_db_common.h>
#include <mem_db/mem_db_def.h>

namespace mem_db_util {

// check the string whether is vailable string
bool IsAlnumString (const std::string &str);
bool IsAlnumString (const std::vector<std::string> &str_vector);
bool IsAlnumString (const std::map<std::string, std::string> &str_map);

extern const char kMemKeySeparator;
extern const char kMemValueSeparator;

// generate keys for data item operator
inline void GenerateTableDataItemKey (const std::string &table,
                                      const std::string &key,
                                      const std::string &column,
                                      std::string &item_key) {
  item_key = table + std::string (1, kMemKeySeparator)
             + key + std::string (1, kMemKeySeparator)
             + column;
}
// generate keys to get table list in the memcached
inline void GenerateTableInfoKey (std::string &table_info_key) {
  table_info_key = std::string (1, kMemKeySeparator)
                   + std::string (1, kMemKeySeparator);
}
// generate keys to get column list in the specified table
inline void GenerateTableColumnInfoKey (const std::string &table_name,
                                        std::string &table_column_key) {
  table_column_key = table_name + std::string (1, kMemKeySeparator)
                     + std::string (1, kMemKeySeparator);
}
// generate keys to get column attr info(live time, family, bucket id)
// in the memcached
inline void GenerateTableColumnAttrKey (const std::string &table_name,
                                        const std::string &column_name,
                                        std::string &column_attr_key) {
  column_attr_key = table_name + std::string (1, kMemKeySeparator)
                    + std::string (1, kMemKeySeparator) + column_name;
}
void GenerateValueString (const std::vector<std::string> &values,
                          std::string &value_list);

inline void ResolveValueString (const std::string &value_list,
                                std::vector<std::string> &values) {
  values.clear ();
  ADutil::Split2 (values, value_list, kMemValueSeparator);
}

void GenerateColumnAttrValueString (const mem_db::ColumnDesp &column_attr,
                                    std::string &column_attr_string);

}   // namespace mem_db_util

// 需要对失效时间的值进行修正
// 目前, memcached接受失效时间设置时处理过程如下：
//    如果<30天，以offset开始的间隔为失效时间；如果>30天，就认为是绝对时间(1970年以来的秒数）
// 所有，需要修正如下；如果设置的时间超过30天，需要转化为绝对时间才是可以的。
inline void AmendExpirationTimeValue (time_t pre_amend_expiration_time,
                                      time_t &post_amend_expiration_time) {
  time_t threshold_time = 60 * 60 * 24 * 30;

  if (pre_amend_expiration_time <= threshold_time) {
    post_amend_expiration_time = pre_amend_expiration_time;
  } else {
    struct timeval tv;
    gettimeofday (&tv, NULL);
    post_amend_expiration_time = pre_amend_expiration_time + tv.tv_sec;
  }
}

#endif  // MEM_DB_UTIL_H_
