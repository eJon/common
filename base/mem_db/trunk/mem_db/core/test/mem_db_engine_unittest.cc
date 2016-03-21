#include <gtest/gtest.h>
#include <limits.h>
#include <mem_db/test/test.h>

#include <mem_db/mem_db.h>
#include <mem_db/core/mem_db_engine.h>
#include <mem_db/core/mem_db_memcached_agent.h>

using namespace mem_db;

TEST (MemDBEngine, ConstructorNormal) {
  MemDBEngine db_engine;
}

TEST (MemDBEngine, DestructorNormal) {
  MemDBEngine db_engine;
  db_engine.~MemDBEngine ();

}

TEST (MemDBEngine, InitNormal) {
  MemDBEngine db_engine;

  std::string conf_file_path = std::string (TEST_DATA_PATH) + "mem_db.conf";
  int ret = db_engine.Init (conf_file_path.c_str ());

  ASSERT_TRUE (0 == ret);
}

TEST (MemDBEngine, FreeNormal) {
  MemDBEngine db_engine;

  std::string conf_file_path = std::string (TEST_DATA_PATH) + "mem_db.conf";
  int ret = db_engine.Init (conf_file_path.c_str ());

  ret = db_engine.Free ();
  ASSERT_TRUE (0 == ret);
}

TEST (MemDBEngine, ListHostsNormal) {
  MemDBEngine db_engine;

  std::string conf_file_path = std::string (TEST_DATA_PATH) + "mem_db.conf";
  int ret = db_engine.Init (conf_file_path.c_str ());
  ASSERT_TRUE (0 == ret);

  std::vector<HostInfo> hosts;
  ret = db_engine.ListHosts (hosts);
  ASSERT_TRUE (0 == ret);

  ASSERT_TRUE (0 == hosts[0].ip_addr.compare ("127.0.0.1"));
}

class MemDBEngineCommonTest : public testing::Test {
  protected:
    static void SetUpTestCase () {
      std::string conf_file_path = std::string (TEST_DATA_PATH) + "mem_db.conf";
      db_engine.Init (conf_file_path.c_str ());
    }
    static void TearDownTestCase () {
      db_engine.Free ();
    }
    static MemDBEngine db_engine;
};

MemDBEngine MemDBEngineCommonTest::db_engine;

TEST_F (MemDBEngineCommonTest, ListTablesNormal) {
  std::vector<std::string> tables;
  db_engine.memcached_agent ()->MemcachedSet ("&&", "Albania,Algeria,Afghanistan");
  int ret = db_engine.ListTables (tables);

  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (3 == tables.size ());
  ASSERT_TRUE (0 == tables[0].compare ("Albania"));
  ASSERT_TRUE (0 == tables[2].compare ("Afghanistan"));
  db_engine.memcached_agent ()->MemcachedSet ("&&", "Albania");

  ret = db_engine.ListTables (tables);
  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (1 == tables.size ());
  ASSERT_TRUE (0 == tables[0].compare ("Albania"));
}

#if 0
TEST_F (MemDBEngineCommonTest, GetColumnsNormal) {
  std::string table_info_key = "&&";
  std::string table_info_value = "Argentina";
  std::string table_column_info_key = "Argentina&&";
  std::string table_column_info_value = "loc,pop";
  db_engine.memcached_agent ()->MemcachedSet (table_info_key,
      table_info_value);
  std::vector<std::string> columns;
  int ret = db_engine.GetColumns ("Argentina", columns);
  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (0 == columns[0].compare ("loc"));
  ASSERT_TRUE (0 == columns[1].compare ("pop"));
}
#endif

TEST_F (MemDBEngineCommonTest, GetColumnsDescriptorsNormal) {
  std::string table_info_key = "&&";
  std::string table_info_value = "Aruba,Oman";
  std::string table_column_info_key = "Aruba&&";
  std::string table_column_info_value = "loc,pop";
  std::string table_column_attr_key_v1 = "Aruba&&loc";
  std::string table_column_attr_value_v1 = "123,3,22";
  std::string table_column_attr_key_v2 = "Aruba&&pop";
  std::string table_column_attr_value_v2 = "1,2,2";
  db_engine.memcached_agent ()->MemcachedSet (table_info_key,
      table_info_value);
  db_engine.memcached_agent ()->MemcachedSet (table_column_info_key,
      table_column_info_value);
  db_engine.memcached_agent ()->MemcachedSet (table_column_attr_key_v1,
      table_column_attr_value_v1);
  db_engine.memcached_agent ()->MemcachedSet (table_column_attr_key_v2,
      table_column_attr_value_v2);

  std::vector<ColumnDesp> column_desps;
  int ret = db_engine.GetColumnDescriptors ("Aruba", column_desps);
  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (123 == column_desps[0].time_to_live);
  ASSERT_TRUE (2 == column_desps[1].bucket_id);

  ret = db_engine.GetColumnDescriptors ("Oman", column_desps);
  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (0 == column_desps.size ());
}

TEST_F (MemDBEngineCommonTest, CreateTableNormal) {
  std::string table_info_key = "&&";
  std::string table_info_value = "Aruba,Oman";
  db_engine.memcached_agent ()->MemcachedSet (table_info_key,
      table_info_value);

  int ret = 0;
  std::string table_name = "Azerbaijan";
  ColumnDesp column_desp_v1 = {"whether", 11111111, 11, 0};
  ColumnDesp column_desp_v2 = {"loc", 0, 11, 2};
  std::vector<ColumnDesp> desp_vector;
  desp_vector.push_back (column_desp_v1);
  desp_vector.push_back (column_desp_v2);
  ret = db_engine.CreateTable (table_name, desp_vector);
  ASSERT_TRUE (0 == ret);

  std::string table_column_attr_key = "Azerbaijan&&loc";
  std::string table_column_attr_value;
  ret = db_engine.memcached_agent ()->MemcachedGet ("&&", table_info_value);
  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (0 == table_info_value.compare ("Aruba,Oman,Azerbaijan"));
  ret = db_engine.memcached_agent ()->MemcachedGet (table_column_attr_key,
        table_column_attr_value);
  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (0 == table_column_attr_value.compare ("0,11,2"));
}

TEST_F (MemDBEngineCommonTest, DeletetableNormal) {
  std::string table_info_key = "&&";
  std::string table_info_value = "Ethiopia,Egypt";
  std::string table_column_info_key = "Ethiopia&&";
  std::string table_column_info_value = "loc,pop";
  std::string table_column_attr_key_v1 = "Ethiopia&&loc";
  std::string table_column_attr_value_v1 = "123,3,22";
  std::string table_column_attr_key_v2 = "Ethiopia&&pop";
  std::string table_column_attr_value_v2 = "1,2,2";
  db_engine.memcached_agent ()->MemcachedSet (table_info_key,
      table_info_value);
  db_engine.memcached_agent ()->MemcachedSet (table_column_info_key,
      table_column_info_value);
  db_engine.memcached_agent ()->MemcachedSet (table_column_attr_key_v1,
      table_column_attr_value_v1);
  db_engine.memcached_agent ()->MemcachedSet (table_column_attr_key_v2,
      table_column_attr_value_v2);

  int ret = db_engine.DeleteTable ("Ethiopia");
  ASSERT_TRUE (0 == ret);
  db_engine.memcached_agent ()->MemcachedGet (table_info_key,
      table_info_value);
  db_engine.memcached_agent ()->MemcachedGet (table_column_attr_key_v1,
      table_column_attr_value_v1);

  ASSERT_TRUE (0 == table_info_value.compare ("Egypt"));
  ASSERT_TRUE (0 == table_column_attr_value_v1.compare (""));

  ret = db_engine.DeleteTable ("Egypt");
  ASSERT_TRUE (0 == ret);
  db_engine.memcached_agent ()->MemcachedGet (table_info_key,
      table_info_value);
  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (0 == table_info_value.compare (""));
}

TEST_F (MemDBEngineCommonTest, GetTablesNormal) {
  // environment
  std::string data_item_key = "Ireland&Paaaa&loc";
  std::string data_item_value = "EU";
  db_engine.memcached_agent ()->MemcachedSet (data_item_key,
      data_item_value);
  std::string data_item_key_ex = "Korea&Paaaa&loc";
  std::string data_item_value_ex = "Asia";
  db_engine.memcached_agent ()->MemcachedSet (data_item_key_ex,
      data_item_value_ex);
  // input
  MutationTable mutation_table;
  mutation_table.table_name = "Ireland";
  mutation_table.row_keys.push_back ("Paaaa");
  mutation_table.row_keys.push_back ("Pbbbb");
  mutation_table.column_specifiers.push_back ("loc");
  mutation_table.column_specifiers.push_back ("pop");
  mutation_table.column_specifiers.push_back ("province");
  std::vector<MutationTable> mutation_tables;
  mutation_tables.push_back (mutation_table);

  MutationTable mutation_table_ex;
  mutation_table_ex.table_name = "Korea";
  mutation_table_ex.row_keys.push_back ("Paaaa");
  mutation_table_ex.row_keys.push_back ("Pbbbb");
  mutation_table_ex.column_specifiers.push_back ("loc");
  mutation_table_ex.column_specifiers.push_back ("pop");
  mutation_table_ex.column_specifiers.push_back ("province");
  mutation_tables.push_back (mutation_table_ex);
  // output
  int ret = 0;
  std::vector<std::vector<std::vector<std::string> > > table_contents;
  ret = db_engine.GetTables (mutation_tables, table_contents);
  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (2 == table_contents.size ()); // key number
  ASSERT_TRUE (3 == table_contents[0][0].size ()); // column num
  ASSERT_TRUE (0 == table_contents[0][0][0].compare ("EU"));
  ASSERT_TRUE (0 == table_contents[0][1][1].compare (""));
  ASSERT_TRUE (0 == table_contents[1][0][0].compare ("Asia"));
  ASSERT_TRUE (0 == table_contents[1][1][1].compare (""));
  // Assertion
}

TEST_F (MemDBEngineCommonTest, SetTablesNormal) {
  // environment
  // input
  MutationTable mutation_table;
  mutation_table.table_name = "Denmark";
  mutation_table.row_keys.push_back ("Paaaa");
  mutation_table.row_keys.push_back ("Pbbbb");
  mutation_table.column_specifiers.push_back ("loc");
  mutation_table.column_specifiers.push_back ("pop");
  mutation_table.column_specifiers.push_back ("province");
  std::vector<MutationTable> mutation_tables;
  mutation_tables.push_back (mutation_table);

  MutationTable mutation_table_ex;
  mutation_table_ex.table_name = "Equatorial";
  mutation_table_ex.row_keys.push_back ("Paaaa");
  mutation_table_ex.row_keys.push_back ("Pbbbb");
  mutation_table_ex.column_specifiers.push_back ("loc");
  mutation_table_ex.column_specifiers.push_back ("pop");
  mutation_table_ex.column_specifiers.push_back ("province");
  mutation_tables.push_back (mutation_table_ex);

  std::string value_v1 = "same value";
  std::vector<std::string> value_v2; // column values
  value_v2.push_back (value_v1);
  value_v2.push_back (value_v1);
  value_v2.push_back (value_v1);
  std::vector<std::vector<std::string> > value_v3; // row values
  value_v3.push_back (value_v2);
  value_v3.push_back (value_v2);
  std::vector<std::vector<std::vector<std::string> > > value_v4; // table values
  value_v4.push_back (value_v3);
  value_v4.push_back (value_v3);
  // output
  int ret = 0;
  ret = db_engine.SetTables (mutation_tables, value_v4);
  // Assertion
  ASSERT_TRUE (0 == ret);
  std::string value;
  db_engine.memcached_agent ()->MemcachedGet ("Denmark&Paaaa&loc",
      value);
  ASSERT_TRUE (0 == value.compare ("same value"));
  db_engine.memcached_agent ()->MemcachedGet ("Equatorial&Paaaa&loc",
      value);
  ASSERT_TRUE (0 == value.compare ("same value"));
}

TEST_F (MemDBEngineCommonTest, AddColumnsNormal) {
  std::string table_info_key = "&&";
  std::string table_info_value = "Germany,Dominica";
  db_engine.memcached_agent ()->MemcachedSet (table_info_key,
      table_info_value);

  int ret = 0;
  std::string table_name = "Russia";
  ColumnDesp column_desp_v1 = {"whether", 11111111, 11, 0};
  ColumnDesp column_desp_v2 = {"loc", 0, 11, 2};
  std::vector<ColumnDesp> desp_vector;
  desp_vector.push_back (column_desp_v1);
  desp_vector.push_back (column_desp_v2);
  ret = db_engine.CreateTable (table_name, desp_vector);
  ASSERT_TRUE (0 == ret);

  std::string table_column_attr_key = "Russia&&loc";
  std::string table_column_attr_value;
  ret = db_engine.memcached_agent ()->MemcachedGet ("&&", table_info_value);
  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (0 == table_info_value.compare ("Germany,Dominica,Russia"));
  ret = db_engine.memcached_agent ()->MemcachedGet (table_column_attr_key,
        table_column_attr_value);
  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (0 == table_column_attr_value.compare ("0,11,2"));
}

TEST_F (MemDBEngineCommonTest, GetRowWithColumnsNormal) {
  // environment
  std::string data_item_key = "Estonia&Paaaa&loc";
  std::string data_item_value = "EU";
  std::string data_item_key_ex = "Estonia&Paaaa&worker";
  std::string data_item_value_ex = "ohhhhhhhhhhhhhhh";
  db_engine.memcached_agent ()->MemcachedSet (data_item_key,
      data_item_value);
  db_engine.memcached_agent ()->MemcachedSet (data_item_key_ex,
      data_item_value_ex);

  // input
  std::string table = "Estonia";
  std::string key = "Paaaa";
  std::vector<std::string> columns;
  columns.push_back ("loc");
  columns.push_back ("pop");
  columns.push_back ("stats");
  columns.push_back ("university");
  columns.push_back ("worker");
  columns.push_back ("hospital");

  // output
  int ret = 0;
  std::vector<std::string> values;
  ret = db_engine.GetRowWithColumns (table, key, columns, values);

  // Assertion
  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (6 == values.size ()); // value number
  ASSERT_TRUE (0 == values[0].compare ("EU"));
  ASSERT_TRUE (0 == values[1].compare (""));
  ASSERT_TRUE (0 == values[2].compare (""));
  ASSERT_TRUE (0 == values[3].compare (""));
  ASSERT_TRUE (0 == values[4].compare ("ohhhhhhhhhhhhhhh"));
  ASSERT_TRUE (0 == values[5].compare (""));
}

TEST_F (MemDBEngineCommonTest, GetRowsWithColumnsNormal) {
  // environment
  std::string data_item_key = "Andorra&Paaaa&loc";
  std::string data_item_value = "EU";
  std::string data_item_key_ex = "Andorra&Paaaa&worker";
  std::string data_item_value_ex = "ohhhhhhhhhhhhhhh";
  std::string data_item_key_ex_ex = "Andorra&Pbbbb&worker";
  std::string data_item_value_ex_ex = "ohoh";
  db_engine.memcached_agent ()->MemcachedSet (data_item_key,
      data_item_value);
  db_engine.memcached_agent ()->MemcachedSet (data_item_key_ex,
      data_item_value_ex);
  db_engine.memcached_agent ()->MemcachedSet (data_item_key_ex_ex,
      data_item_value_ex_ex);

  // input
  std::string table = "Andorra";
  std::string key = "Paaaa";
  std::vector<std::string> columns;
  columns.push_back ("loc");
  columns.push_back ("pop");
  columns.push_back ("stats");
  columns.push_back ("university");
  columns.push_back ("worker");
  columns.push_back ("hospital");
  std::vector<std::string> keys;
  keys.push_back (key);
  keys.push_back ("Pbbbb");

  // output
  int ret = 0;
  std::vector<std::vector<std::string> > values;
  ret = db_engine.GetRowsWithColumns (table, keys, columns, values);

  // Assertion
  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (2 == values.size ()); // value number
  ASSERT_TRUE (6 == values[0].size ()); // value number
  ASSERT_TRUE (0 == values[0][0].compare ("EU"));
  ASSERT_TRUE (0 == values[0][1].compare (""));
  ASSERT_TRUE (0 == values[0][2].compare (""));
  ASSERT_TRUE (0 == values[0][3].compare (""));
  ASSERT_TRUE (0 == values[0][4].compare ("ohhhhhhhhhhhhhhh"));
  ASSERT_TRUE (0 == values[0][5].compare (""));
  ASSERT_TRUE (0 == values[1][4].compare ("ohoh"));
  ASSERT_TRUE (0 == values[1][5].compare (""));
}

TEST_F (MemDBEngineCommonTest, GetRowNormal) {
  // environment
  std::string column_info_key = "Austria&&";
  std::string column_info_value = "loc,pop,stats,worker";
  std::string data_item_key = "Austria&Paaaa&loc";
  std::string data_item_value = "EU";
  std::string data_item_key_ex = "Austria&Paaaa&worker";
  std::string data_item_value_ex = "ohhhhhhhhhhhhhhh";
  db_engine.memcached_agent ()->MemcachedSet (column_info_key,
      column_info_value);
  db_engine.memcached_agent ()->MemcachedSet (data_item_key,
      data_item_value);
  db_engine.memcached_agent ()->MemcachedSet (data_item_key_ex,
      data_item_value_ex);

  // input
  std::string table = "Austria";
  std::string key = "Paaaa";

  // output
  int ret = 0;
  std::map<std::string, std::string> values;
  ret = db_engine.GetRow (table, key, values);

  // Assertion
  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (4 == values.size ());
  ASSERT_TRUE (0 == values.find ("worker")->second.compare ("ohhhhhhhhhhhhhhh"));
  ASSERT_TRUE (0 == values.find ("loc")->second.compare ("EU"));
  ASSERT_TRUE (0 == values.find ("stats")->second.compare (""));
}

TEST_F (MemDBEngineCommonTest, GetRowsNormal) {
  // environment
  std::string column_info_key = "Australia&&";
  std::string column_info_value = "loc,pop,stats,worker";
  std::string data_item_key = "Australia&Paaaa&loc";
  std::string data_item_value = "EU";
  std::string data_item_key_ex = "Australia&Paaaa&worker";
  std::string data_item_value_ex = "ohhhhhhhhhhhhhhh";
  db_engine.memcached_agent ()->MemcachedSet (column_info_key,
      column_info_value);
  db_engine.memcached_agent ()->MemcachedSet (data_item_key,
      data_item_value);
  db_engine.memcached_agent ()->MemcachedSet (data_item_key_ex,
      data_item_value_ex);

  // input
  std::string table = "Australia";
  std::vector<std::string> keys;
  keys.push_back ("Paaaa");
  keys.push_back ("Pbbbb");

  // output
  int ret = 0;
  std::vector<std::map<std::string, std::string> > values;
  ret = db_engine.GetRows (table, keys, values);

  // Assertion
  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (2 == values.size ());
  ASSERT_TRUE (0 == values[0].find ("worker")->second.compare ("ohhhhhhhhhhhhhhh"));
  ASSERT_TRUE (0 == values[0].find ("loc")->second.compare ("EU"));
  ASSERT_TRUE (0 == values[0].find ("stats")->second.compare (""));
  ASSERT_TRUE (0 == values[1].find ("stats")->second.compare (""));
  ASSERT_TRUE (4 == values[1].size ());
}

TEST_F (MemDBEngineCommonTest, PutRowWithColumnsNormal) {
  // environment
  // input
  std::string table = "Macau";
  std::string key = "Paaaa";
  std::vector<std::string> columns;
  columns.push_back ("loc");
  columns.push_back ("pop");
  columns.push_back ("stats");
  columns.push_back ("university");
  columns.push_back ("worker");
  std::vector<std::string> values;
  values.push_back ("South of China");
  values.push_back ("100000");
  values.push_back ("MM");
  values.push_back ("university");
  values.push_back ("1000000000000");

  // output
  int ret = 0;
  ret = db_engine.PutRowWithColumns (table, key, columns, values);

  // Assertion
  ASSERT_TRUE (0 == ret);

  std::string value;
  db_engine.memcached_agent ()->MemcachedGet ("Macau&Paaaa&worker",
      value);
  ASSERT_TRUE (0 == value.compare ("1000000000000"));

  db_engine.memcached_agent ()->MemcachedGet ("Macau&Paaaa&university",
      value);
  ASSERT_TRUE (0 == value.compare ("university"));

  db_engine.memcached_agent ()->MemcachedGet ("Macau&Paaaa&other",
      value);
  ASSERT_TRUE (0 == value.compare (""));
}

TEST_F (MemDBEngineCommonTest, PutRowsWithColumnsNormal) {
  // environment
  // input
  std::string table = "Barbados";
  std::string key = "Paaaa";
  std::vector<std::string> columns;
  columns.push_back ("loc");
  columns.push_back ("pop");
  columns.push_back ("stats");
  columns.push_back ("university");
  columns.push_back ("worker");
  std::vector<std::string> values;
  values.push_back ("South of China");
  values.push_back ("100000");
  values.push_back ("MM");
  values.push_back ("university");
  values.push_back ("1000000000000");

  std::vector<std::string> keys;
  keys.push_back (key);
  keys.push_back ("Pbbbb");
  std::vector<std::vector<std::string> > input_values;
  input_values.push_back (values);
  input_values.push_back (values);

  // output
  int ret = 0;
  ret = db_engine.PutRowsWithColumns (table, keys, columns, input_values);

  // Assertion
  ASSERT_TRUE (0 == ret);

  std::string value;
  db_engine.memcached_agent ()->MemcachedGet ("Barbados&Paaaa&worker",
      value);
  ASSERT_TRUE (0 == value.compare ("1000000000000"));

  db_engine.memcached_agent ()->MemcachedGet ("Barbados&Paaaa&university",
      value);
  ASSERT_TRUE (0 == value.compare ("university"));

  db_engine.memcached_agent ()->MemcachedGet ("Barbados&Paaaa&other",
      value);
  ASSERT_TRUE (0 == value.compare (""));
  db_engine.memcached_agent ()->MemcachedGet ("Barbados&Pbbbb&worker",
      value);
  ASSERT_TRUE (0 == value.compare ("1000000000000"));

  db_engine.memcached_agent ()->MemcachedGet ("Barbados&Pbbbb&university",
      value);
  ASSERT_TRUE (0 == value.compare ("university"));

  db_engine.memcached_agent ()->MemcachedGet ("Barbados&Pbbbb&other",
      value);
  ASSERT_TRUE (0 == value.compare (""));
}

TEST_F (MemDBEngineCommonTest, PutRowNormal) {
  // environment
  // input
  std::string table = "Bahamas";
  std::string key = "Paaaa";

  std::map<std::string, std::string> row_record;
  row_record.insert (std::pair<std::string, std::string> ("loc", "South of China"));
  row_record.insert (std::pair<std::string, std::string> ("pop", "1000000000000"));

  // output
  int ret = 0;
  ret = db_engine.PutRow (table, key, row_record);

  // Assertion
  ASSERT_TRUE (0 == ret);

  std::string value;
  db_engine.memcached_agent ()->MemcachedGet ("Bahamas&Paaaa&loc",
      value);
  ASSERT_TRUE (0 == value.compare ("South of China"));

  db_engine.memcached_agent ()->MemcachedGet ("Bahamas&Paaaa&pop",
      value);
  ASSERT_TRUE (0 == value.compare ("1000000000000"));

  db_engine.memcached_agent ()->MemcachedGet ("Bahamas&Paaaa&other",
      value);
  ASSERT_TRUE (0 == value.compare (""));
}

TEST_F (MemDBEngineCommonTest, PutRowsNormal) {
  // environment
  // input
  std::string table = "Pakistan";
  std::string key = "Paaaa";

  std::vector<std::string> keys;
  keys.push_back (key);
  keys.push_back ("Pbbbb");
  std::map<std::string, std::string> row_record;
  row_record.insert (std::pair<std::string, std::string> ("loc", "South of China"));
  row_record.insert (std::pair<std::string, std::string> ("pop", "1000000000000"));
  std::vector<std::map<std::string, std::string> > row_records;
  row_records.push_back (row_record);
  row_records.push_back (row_record);

  // output
  int ret = 0;
  ret = db_engine.PutRows (table, keys, row_records);

  // Assertion
  ASSERT_TRUE (0 == ret);

  std::string value;
  db_engine.memcached_agent ()->MemcachedGet ("Pakistan&Paaaa&loc",
      value);
  ASSERT_TRUE (0 == value.compare ("South of China"));

  db_engine.memcached_agent ()->MemcachedGet ("Pakistan&Paaaa&pop",
      value);
  ASSERT_TRUE (0 == value.compare ("1000000000000"));

  db_engine.memcached_agent ()->MemcachedGet ("Pakistan&Paaaa&other",
      value);
  ASSERT_TRUE (0 == value.compare (""));
  db_engine.memcached_agent ()->MemcachedGet ("Pakistan&Pbbbb&loc",
      value);
  ASSERT_TRUE (0 == value.compare ("South of China"));

  db_engine.memcached_agent ()->MemcachedGet ("Pakistan&Pbbbb&pop",
      value);
  ASSERT_TRUE (0 == value.compare ("1000000000000"));

  db_engine.memcached_agent ()->MemcachedGet ("Bahamas&Pbbbb&other",
      value);
  ASSERT_TRUE (0 == value.compare (""));
}

TEST_F (MemDBEngineCommonTest, DeleteRowNormal) {
  std::string column_info_key = "France&&";
  std::string column_info_value = "loc,pop,stats,worker";
  std::string data_item_key = "France&Paaaa&loc";
  std::string data_item_value = "EU";
  std::string data_item_key_ex = "France&Paaaa&worker";
  std::string data_item_value_ex = "ohhhhhhhhhhhhhhh";
  db_engine.memcached_agent ()->MemcachedSet (column_info_key,
      column_info_value);
  db_engine.memcached_agent ()->MemcachedSet (data_item_key,
      data_item_value);
  db_engine.memcached_agent ()->MemcachedSet (data_item_key_ex,
      data_item_value_ex);
  std::string table = "France";
  std::string key = "Paaaa";
  std::vector<std::string> columns;
  columns.push_back ("loc");
  columns.push_back ("pop");
  columns.push_back ("worker");

  int ret = 0;
  ret = db_engine.DeleteRow (table, key);

  // Assertion
  ASSERT_TRUE (0 == ret);

  std::string value;
  db_engine.memcached_agent ()->MemcachedGet ("France&Paaaa&loc",
      value);
  ASSERT_TRUE (0 == value.compare (""));

  db_engine.memcached_agent ()->MemcachedGet ("France&Paaaa&pop",
      value);
  ASSERT_TRUE (0 == value.compare (""));
  db_engine.memcached_agent ()->MemcachedGet ("France&Paaaa&other",
      value);
  ASSERT_TRUE (0 == value.compare (""));
}

TEST_F (MemDBEngineCommonTest, DeleteRowWithColumnsNormal) {
  std::string column_info_key = "Paraguay&&";
  std::string column_info_value = "loc,pop,stats,worker";
  std::string data_item_key = "Paraguay&Paaaa&loc";
  std::string data_item_value = "EU";
  std::string data_item_key_ex = "Paraguay&Paaaa&worker";
  std::string data_item_value_ex = "ohhhhhhhhhhhhhhh";
  db_engine.memcached_agent ()->MemcachedSet (column_info_key,
      column_info_value);
  db_engine.memcached_agent ()->MemcachedSet (data_item_key,
      data_item_value);
  db_engine.memcached_agent ()->MemcachedSet (data_item_key_ex,
      data_item_value_ex);
  std::string table = "Paraguay";
  std::string key = "Paaaa";
  std::vector<std::string> columns;
  columns.push_back ("loc");
  columns.push_back ("pop");
  columns.push_back ("worker");

  int ret = 0;
  ret = db_engine.DeleteRowWithColumns (table, key, columns);

  // Assertion
  ASSERT_TRUE (0 == ret);

  std::string value;
  db_engine.memcached_agent ()->MemcachedGet ("Paraguay&Paaaa&loc",
      value);
  ASSERT_TRUE (0 == value.compare (""));

  db_engine.memcached_agent ()->MemcachedGet ("Paraguay&Paaaa&pop",
      value);
  ASSERT_TRUE (0 == value.compare (""));
  db_engine.memcached_agent ()->MemcachedGet ("Paraguay&Paaaa&other",
      value);
  ASSERT_TRUE (0 == value.compare (""));
}

TEST_F (MemDBEngineCommonTest, DeleteRowsWithColumnsNormal) {
  std::string column_info_key = "Burundi&&";
  std::string column_info_value = "loc,pop,stats,worker";
  std::string data_item_key = "Burundi&Paaaa&loc";
  std::string data_item_value = "EU";
  std::string data_item_key_ex = "Burundi&Paaaa&worker";
  std::string data_item_value_ex = "ohhhhhhhhhhhhhhh";
  db_engine.memcached_agent ()->MemcachedSet (column_info_key,
      column_info_value);
  db_engine.memcached_agent ()->MemcachedSet (data_item_key,
      data_item_value);
  db_engine.memcached_agent ()->MemcachedSet (data_item_key_ex,
      data_item_value_ex);
  std::string table = "Burundi";
  std::vector<std::string> columns;
  columns.push_back ("loc");
  columns.push_back ("pop");
  columns.push_back ("worker");
  std::vector<std::string> keys;
  keys.push_back ("Paaaa");
  keys.push_back ("Pbbbb");

  int ret = 0;
  ret = db_engine.DeleteRowsWithColumns (table, keys, columns);

  // Assertion
  ASSERT_TRUE (0 == ret);

  std::string value;
  db_engine.memcached_agent ()->MemcachedGet ("Burundi&Pbbbb&loc",
      value);
  ASSERT_TRUE (0 == value.compare (""));

  db_engine.memcached_agent ()->MemcachedGet ("Burundi&Pbbbb&pop",
      value);
  ASSERT_TRUE (0 == value.compare (""));
  db_engine.memcached_agent ()->MemcachedGet ("Burundi&Pbbbb&other",
      value);
  ASSERT_TRUE (0 == value.compare (""));
  db_engine.memcached_agent ()->MemcachedGet ("Burundi&Paaaa&loc",
      value);
  ASSERT_TRUE (0 == value.compare (""));

  db_engine.memcached_agent ()->MemcachedGet ("Burundi&Paaaa&pop",
      value);
  ASSERT_TRUE (0 == value.compare (""));
  db_engine.memcached_agent ()->MemcachedGet ("Burundi&Paaaa&other",
      value);
  ASSERT_TRUE (0 == value.compare (""));
}

TEST_F (MemDBEngineCommonTest, GetColumnDespNormal) {
  std::string table_info_key = "&&";
  std::string table_info_value = "Arubaa,Omana";
  std::string table_column_info_key = "Aruba&&";
  std::string table_column_info_value = "loc,pop";
  std::string table_column_attr_key_v1 = "Arubaa&&loc";
  std::string table_column_attr_value_v1 = "123,3,22";
  std::string table_column_attr_key_v2 = "Arubaa&&pop";
  std::string table_column_attr_value_v2 = "1,2,2";
  db_engine.memcached_agent ()->MemcachedSet (table_info_key,
      table_info_value);
  db_engine.memcached_agent ()->MemcachedSet (table_column_info_key,
      table_column_info_value);
  db_engine.memcached_agent ()->MemcachedSet (table_column_attr_key_v1,
      table_column_attr_value_v1);
  db_engine.memcached_agent ()->MemcachedSet (table_column_attr_key_v2,
      table_column_attr_value_v2);

  std::vector<ColumnDesp> column_desps;
  int exp_time;
  int ret = db_engine.GetColumnExpiration ("Arubaa", "loc", exp_time);
  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (123 == exp_time);

  ret = db_engine.GetColumnExpiration ("Omana", "loc", exp_time);
  ASSERT_TRUE (0 == ret);
  ASSERT_TRUE (0 == exp_time);
}
