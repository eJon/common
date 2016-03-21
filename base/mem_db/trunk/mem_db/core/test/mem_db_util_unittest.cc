#include <gtest/gtest.h>
#include <limits.h>

#include <iostream>

#include <mem_db/core/mem_db_util.h>

using namespace mem_db_util;

TEST (IsAlnumStringTest, Normal) {
  std::string country = "India";
  bool ret = IsAlnumString (country);
  ASSERT_TRUE (true == ret);
  country = "Republic_of_India";
  ret = IsAlnumString (country);
  ASSERT_TRUE (true == ret);
}

TEST (IsAlnumStringTest, Invaliateparam) {
  std::string country = "Republic of India";
  bool ret = IsAlnumString (country);
  ASSERT_TRUE (false == ret);
  country = "Republic_of_India&America";
  ret = IsAlnumString (country);
  ASSERT_TRUE (false == ret);
  country = "Republic_of_India ";
  ret = IsAlnumString (country);
  ASSERT_TRUE (false == ret);
}

TEST (IsAlnumStringTest, VectorNormal) {
  std::vector<std::string> countries;

  countries.push_back ("New_Zealand");
  bool ret = IsAlnumString (countries);
  ASSERT_TRUE (true == ret);

  countries.push_back ("America");
  ret = IsAlnumString (countries);
  ASSERT_TRUE (true == ret);
}

TEST (IsAlnumStringTest, VectorInvaliateparam) {
  std::vector<std::string> countries;

  countries.push_back ("New_Zealand&");
  bool ret = IsAlnumString (countries);
  ASSERT_TRUE (false == ret);

  countries.push_back ("New Zealand");
  ret = IsAlnumString (countries);
  ASSERT_TRUE (false == ret);

  countries.push_back ("The United States of America");
  ret = IsAlnumString (countries);
  ASSERT_TRUE (false == ret);
}

TEST (IsAlnumStringTest, MapNormal) {
  std::map<std::string, std::string> countries;

  countries.insert (std::pair<std::string, std::string> (
                      "America", "The_United_States_of_America"));
  bool ret = IsAlnumString (countries);
  ASSERT_TRUE (true == ret);

  countries.insert (std::pair<std::string, std::string> (
                      "Pakista", "Islamic_Republic_of_Pakista"));
  ret = IsAlnumString (countries);
  ASSERT_TRUE (true == ret);
}

TEST (IsAlnumStringTest, MapInvaliateparam) {
  std::map<std::string, std::string> countries;

  countries.insert (std::pair<std::string, std::string> (
                      "America", "The&United&States&of&America"));
  bool ret = IsAlnumString (countries);
  ASSERT_TRUE (false == ret);

  countries.insert (std::pair<std::string, std::string> (
                      "Pakista", "Islamic Republic of Pakista"));
  ret = IsAlnumString (countries);
  ASSERT_TRUE (false == ret);
}

TEST (GenerateTableDataItemKeyTest, Normal) {
  std::string table = "table";
  std::string key = "key";
  std::string column = "column";
  std::string result;
  GenerateTableDataItemKey (table, key, column, result);
  ASSERT_TRUE (0 == result.compare ("table&key&column"));
}

TEST (GenerateTableDataItemKeyTest, Invalidate) {
}

TEST (GenerateTableInfoKeyTest, Normal) {
  std::string result;
  GenerateTableInfoKey (result);
  ASSERT_TRUE (0 == result.compare ("&&"));
}

TEST (GenerateTableColumnInfoKeyTest, Normal) {
  std::string table = "table";
  std::string result;
  GenerateTableColumnInfoKey (table, result);

  ASSERT_TRUE (0 == result.compare ("table&&"));
}

TEST (GenerateTableColumnAttrKeyTest, Normal) {
  std::string table = "table";
  std::string column = "column";
  std::string result;
  GenerateTableColumnAttrKey (table, column, result);

  ASSERT_TRUE (0 == result.compare ("table&&column"));
}

TEST (GenerateValueStringTest, Normal) {
  std::vector<std::string> countries;

  std::string value;
  countries.push_back ("New_Zealand");
  GenerateValueString (countries, value);
  ASSERT_TRUE (0 == value.compare ("New_Zealand"));

  countries.push_back ("America");
  countries.push_back ("Pakista");

  GenerateValueString (countries, value);
  ASSERT_TRUE (0 == value.compare ("New_Zealand,America,Pakista"));
}

TEST (GenerateValueStringTest, Invalidate) {
}

TEST (ResolveValueStringTest, Normal) {
  std::string country_list = "New_Zealand,America,Pakista";
  std::vector<std::string> countries;

  ResolveValueString (country_list, countries);
  ASSERT_TRUE (3 == countries.size ());
  ASSERT_TRUE (0 == countries[0].compare ("New_Zealand"));
  ASSERT_TRUE (0 == countries[1].compare ("America"));
  ASSERT_TRUE (0 == countries[2].compare ("Pakista"));

  country_list = "New_Zealand";
  ResolveValueString (country_list, countries);
  ASSERT_TRUE (0 == countries[0].compare ("New_Zealand"));
  ASSERT_TRUE (1 == countries.size ());
}

TEST (GenerateColumnAttrValueStringTest, Normal) {
  mem_db::ColumnDesp desp = {"", 1231, 2, 9};
  std::string result;
  GenerateColumnAttrValueString (desp, result);
  ASSERT_TRUE (0 == result.compare ("1231,2,9"));
}

TEST (AmendExpirationTimeValueTest, Normal) {
  time_t small_expiration_time = 10;
  time_t amend_expiration_time;
  AmendExpirationTimeValue (small_expiration_time, amend_expiration_time);
  ASSERT_TRUE (small_expiration_time == amend_expiration_time);
  std::cout << "original expriation time: " << small_expiration_time << std::endl;
  std::cout << "amended expriation time: " << amend_expiration_time << std::endl;
}

TEST (AmendExpirationTimeValueTest, NormalN) {
  time_t big_expiration_time = 60 * 60 * 24 * 60;;
  time_t amend_expiration_time;
  AmendExpirationTimeValue (big_expiration_time, amend_expiration_time);
  ASSERT_TRUE (big_expiration_time < amend_expiration_time);
  std::cout << "original expriation time: " << big_expiration_time << std::endl;
  std::cout << "amended expriation time: " << amend_expiration_time << std::endl;
}



