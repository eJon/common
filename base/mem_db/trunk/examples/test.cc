#include <mem_db.h>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace mem_db;

int main (int argc, char **argv) {
  mem_db::MemDB mem_db;
  mem_db.Init ("/home/w/conf/mem_db/mem_db.conf");

  string table = "user_info_1";
  string key = "1000000245";
  std::vector<std::string> columns;
  columns.push_back ("up:ubi:nm");
  columns.push_back ("up:uvt:nm");
  vector<std::string> values;
  values.push_back (string ("\n\0010\022\0010\032\0011\"\0011*\00102\0010d\017", 18));
  values.push_back (string ("\n\0010\022\0010\"\00112\0010?", 12));
  mem_db.PutRowWithColumns (table, key, columns, values);
  std::string column1 = values[0];
  std::string column2 = values[1];
  cout << "column1.size()=" << column1.size () << " column1=" << column1 << endl;
  cout << "column2.size()=" << column2.size () << " column2=" << column2 << endl;
  values.clear ();
  mem_db.GetRowWithColumns (table, key, columns, values);
  cout << "values[0].size()=" << values[0].size () << " values[0]=" << values[0] << endl;
  cout << "values[1].size()=" << values[1].size () << " values[1]=" << values[1] << endl;

  for (int i = 0 ; i < 1000000; i++) {
    values.clear ();
    mem_db.GetRowWithColumns (table, key, columns, values);

    if (values[0].compare (column1)) {
      cout << "Eorro for column1" << column1 << endl;
      return -1;
    }

    if (values[1].compare (column2)) {
      cout << "Eorro for column2" << column2 << endl;
      return -1;
    }
  }

  return 0;
}
