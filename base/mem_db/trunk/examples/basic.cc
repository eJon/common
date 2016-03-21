#include <mem_db.h>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace mem_db;

int main (int argc, char **argv) {
  mem_db::MemDB mem_db;
  mem_db.Init ("/home/w/conf/mem_db/mem_db.conf");

  std::vector<mem_db::HostInfo> hosts;
  mem_db.ListHosts (hosts);

  size_t len = hosts.size ();

  for (int i = 0; i < len; ++i) {
    std::cout << hosts[i].ip_addr << "\t" << hosts[1].port << std::endl;
  }

  std::vector<std::string> tables;
  mem_db.ListTables (tables);
  cout << "tables.size() = " << tables.size () << endl;

  for (int i = 0; i < tables.size (); i++) {
    cout << "table = " << endl;
    std::vector<ColumnDesp> columns;
    mem_db.GetColumnDescriptors (tables[i], columns);

    for (int j = 0; j < columns.size (); j++) {
      cout << "column = " << columns[j].name << endl;
    }
  }

  int error_count = 0;
  string table = "user_info";
  string key = "1000000245";
  std::vector<std::string> columns;
  columns.push_back ("up:ubi:nm");
  columns.push_back ("up:uvt:nm");
  vector<std::string> values;
#if 1
  mem_db.GetRowWithColumns (table, key, columns, values);
  std::string column1 = values[0];
  std::string column2 = values[1];

  for (int j = 0; j < 1000; j++) {
    mem_db.GetRowWithColumns (table, key, columns, values);
    cout << "c=========================================" << j  << endl;

    //  cout << "table=" << table << endl;
    //  for (int i=0; i<values.size(); i++) {
    //    cout << "colomn[" << i << "]=" << columns[i] << endl;
    //   cout << "value[" << i << "]=" << values[i] << endl;
    //   }
    if (0 != values[0].compare (column1)) {
      error_count ++;
      cout << "failed " << column1 << endl;
      break;
    }

    if (0 != values[1].compare (column2)) {
      error_count ++;
      cout << "failed " << column2 << endl;
      break;
      continue;
    }

    //cout << "c=========================================OK"  << endl;
  }

  cout << "error count: " << error_count << endl;
#endif
  //  get tieniu&tieniu&1
  table = "tieniu";
  key = "tieniu";
  columns.clear ();
  columns.push_back ("11");
  columns.push_back ("22");
  values.clear ();
  values.push_back ("cccccccccccccccccccc");
  values.push_back ("bbbddddddddddddddddddd");
  mem_db.PutRowWithColumns (table, key, columns, values);
#if 1
  table = "ad_info";
  key = "4047";
  columns.clear ();
  columns.push_back ("ap:fadi:nm");
  values.clear ();
  mem_db.GetRowWithColumns (table, key, columns, values);
  column1 = values[0];

  for (int j = 0; j < 1000; j++) {
    cout << "c=========================================" << j << endl;
    mem_db.GetRowWithColumns (table, key, columns, values);

    if (0 != values[0].compare (column1)) {
      error_count ++;
      cout << "failed " << column1 << endl;
      break;
    }
  }

  cout << "c=========================================" << column1 << endl;
  cout << "error count: " << error_count << endl;

#endif

  table = "tieniu";
  key = "tieniu";
  columns.clear ();
  columns.push_back ("33");
  columns.push_back ("44");
  values.clear ();
  values.push_back ("cccccccccccccccccccc");
  values.push_back ("bbbddddddddddddddddddd");
  mem_db.PutRowWithColumns (table, key, columns, values);
#if 1
  table = "enterprise_info";
  key = "1001994244";
  columns.clear ();
  columns.push_back ("ep:ei:nm");
  values.clear ();
  mem_db.GetRowWithColumns (table, key, columns, values);
  column1 = values[0];

  for (int j = 0; j < 1000; j++) {
    cout << "c=========================================" << j << endl;
    mem_db.GetRowWithColumns (table, key, columns, values);

    if (0 != values[0].compare (column1)) {
      error_count ++;
      cout << "failed for " << column1 << endl;
      continue;
    }

    cout << "c=========================================OK"  << endl;
    cout << "table=" << table << endl;

    for (int i = 0; i < values.size (); i++) {
      //	  cout << "colomn[" << i << "]=" << columns[i] << endl;
      //	  cout << "value[" << i << "]=" << values[i] << endl;
    }
  }

  //get enterprise_info&1001994244&ep:ei:nm

  cout << "error count: " << error_count << endl;
#endif

#if 1
  std::vector<MutationTable> mutation;
  MutationTable user_table;
  user_table.table_name = "user_info";
  user_table.row_keys = vector<string> (1, "1000000245");
  user_table.column_specifiers.push_back ("up:ubi:nm2222222222222222222222");
  user_table.column_specifiers.push_back ("up:uvt:nm");
  mutation.push_back (user_table);
  MutationTable ad_table;
  ad_table.table_name = "ad_info";
  ad_table.row_keys.push_back ("4741");
  ad_table.row_keys.push_back ("47432333333333333333");
  ad_table.row_keys.push_back ("4750");
  ad_table.column_specifiers.push_back ("ap:fadi:nm");
  mutation.push_back (ad_table);
  MutationTable cust_table;
  cust_table.table_name = "enterprise_info";
  cust_table.row_keys.push_back ("2890437171");
  cust_table.row_keys.push_back ("237635838000000000000000005");
  cust_table.row_keys.push_back ("1002611880");
  cust_table.column_specifiers.push_back ("ep:ei:nm");
  mutation.push_back (cust_table);
  cout << "\n test GetTables..." << endl;
  std::vector<std::vector<std::vector<std::string> > > table_contents_begin;
  std::vector<std::vector<std::vector<std::string> > > table_contents_iter;
  cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
  mem_db.GetTables (mutation, table_contents_begin);

  for (int j = 0; j < 1000; j++) {
    cout << "table fetching: " << j << endl;
    mem_db.GetTables (mutation, table_contents_iter);

    if (table_contents_iter[2][2][0].compare (table_contents_begin[2][2][0]) != 0) {
      error_count ++;
      break;
    }

    if (table_contents_iter[0][0][0].compare (table_contents_begin[0][0][0]) != 0) {
      error_count ++;
      break;
    }

    if (table_contents_iter[2][1][0].compare (table_contents_begin[2][1][0]) != 0) {
      error_count ++;
      break;
    }

    //  for (int i=0; i<table_contents.size(); i++) {
    //	  cout << ">>>>>>>>>>>>table=" << mutation[i].table_name << endl;
    //	  for (int j=0; j<table_contents[i].size(); j++) {
    //		  cout << ">>>>>>>>>>>>>>>>>>>>>id=" << mutation[i].row_keys[j] << endl;
    //		  for (int k=0; k<table_contents[i][j].size(); k++) {
    //			  cout << "colomn>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>=" << mutation[i].column_specifiers[k] << endl;
    //			  cout << "value>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << table_contents[i][j][k] << endl;
    //		  }
    //	  }
    //  }
  }

  std::cout << "error count: " << error_count << std::endl;
#endif

  mem_db.Free ();
  return 0;
}
