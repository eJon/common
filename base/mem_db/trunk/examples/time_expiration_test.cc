#include <mem_db.h>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace mem_db;

int main (int argc, char **argv) {
  mem_db::MemDB mem_db;
  mem_db.Init ("/home/w/conf/mem_db/mem_db.conf");

  std::string insert_key;
  std::string insert_value;
  int exp_time = 0;  // 3 months


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

  if (argc < 4) {
    exp_time = 0;
  } else {
    exp_time = atoi (argv[3]);
  }

  int ret = 0 ;

  for (int i = 0; i < 1; i++) {
#if 1
    ret = mem_db.SetGeneralValue (insert_key, insert_value, exp_time);

    if (ret != 0) {
      std::cout << "insert error" << std::endl;
    } else {
      std::cout << "insert ok: key: " << insert_key << std::endl;
    }

#endif

    std::string stored_value;
    ret = mem_db.GetGeneralValue (insert_key, stored_value);

    if (ret != 0) {
      std::cout << "get error" << std::endl;
    } else {
      std::cout << "get ok: Value: " << stored_value << ", Key: " << insert_key << std::endl;
    }

  }

  mem_db.Free ();
  return 0;
}
