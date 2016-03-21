// copyright:
//            (C) SINA Inc.
//
//      file: mem_db_util.h
//      desc: implementation of utilities for mem_db
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date:
//
//    change:

#include <mem_db/core/mem_db_util.h>

namespace mem_db_util {

#define MAX_STRING_LENGTH_FOR_3INT32_T 64

static const std::string kAlnumTokens = "abcdefghijklmnopqrstuvwxyz"
                                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                        "1234567890_:";

bool IsAlnumString (const std::string &str) {
  size_t pos = 0;
  pos = str.find_first_not_of (kAlnumTokens);

  if (std::string::npos != pos) {
    return false;
  }

  return true;
}

bool IsAlnumString (const std::vector<std::string> &str_vector) {
  size_t pos = 0;

  for (std::vector<std::string>::const_iterator iter = str_vector.begin ();
       iter != str_vector.end (); iter++) {
    pos = (*iter).find_first_not_of (kAlnumTokens);

    if (std::string::npos != pos) {
      return false;
    }
  }

  return true;
}

bool IsAlnumString (const std::map<std::string, std::string> &str_map) {
  size_t pos = 0;

  for (std::map<std::string, std::string>::const_iterator iter = str_map.begin ();
       iter != str_map.end (); iter++) {
    pos = (*iter).first.find_first_not_of (kAlnumTokens);

    if (std::string::npos != pos) {
      return false;
    }

    pos = (*iter).second.find_first_not_of (kAlnumTokens);

    if (std::string::npos != pos) {
      return false;
    }
  }

  return true;
}

// separator for key components
// i.e. "student&alina&age"
const char kMemKeySeparator = '&';
const char kMemValueSeparator = ',';

void GenerateValueString (const std::vector<std::string> &values,
                          std::string &value_list) {
  value_list.clear ();

  if (values.empty ()) {
    return;
  }

  for (std::vector<std::string>::const_iterator iter = values.begin ();
       iter != values.end (); iter++) {
    value_list += *iter;
    value_list += kMemValueSeparator;
  }

  // remove the last char
  value_list.erase (value_list.size () - 1);
}

void GenerateColumnAttrValueString (const mem_db::ColumnDesp &column_attr,
                                    std::string &column_attr_string) {
  column_attr_string.clear ();
  char str[MAX_STRING_LENGTH_FOR_3INT32_T + 1] = {0};
  ::snprintf (str, MAX_STRING_LENGTH_FOR_3INT32_T, "%d%c%d%c%d",
              column_attr.time_to_live, kMemValueSeparator, column_attr.family,
              kMemValueSeparator, column_attr.bucket_id);
  column_attr_string = std::string (str);
}

}   // namespace mem_db_util

