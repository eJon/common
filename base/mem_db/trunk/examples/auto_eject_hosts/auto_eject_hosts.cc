#include <mem_db/mem_db.h>
#include <cstdlib>
#include <iostream>
#include <sstream>

using namespace std;
using namespace mem_db;

int main (int argc, char **argv) {
  mem_db::MemDB mem_db;
  mem_db.Init ("./mem_db.conf");



  std::string insert_key;
  std::string insert_value;


  if (argc < 2) {
    insert_key = "insert_key";
  } else {
    insert_key = argv[1];
  }

  if (argc < 3) {
    insert_value = "insert_value";
  } else {
    insert_value = argv[2];
  }

  int ret = 0 ;
  int total_errors = 0;

  for (int i = 0; i < 10000000; i++) {
    std::stringstream ss_key;
    ss_key << insert_key;
    ss_key << i;
    std::string key = ss_key.str ();

    ret = mem_db.SetGeneralValue (key, insert_value);

    if (ret != 0) {
      std::cout << "..............................................insert error. total_errors: " << ++total_errors << "ret: " << ret << std::endl;
    } else {
      std::cout << "..............................................insert ok. total_errors: " << total_errors << std::endl;
    }

    std::string stored_value;
    ret = mem_db.GetGeneralValue (key, stored_value);

    if (ret != 0) {
      std::cout << "get error. key: " << key << std::endl;
    } else {
      std::cout << "get ok. key: " << key << "; value: " << stored_value << std::endl;
    }
  }


  mem_db.Free ();
  return 0;
}
