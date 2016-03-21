/*
 * =====================================================================================
 *
 *       Filename:  get_set_loop_test.cc
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  11/02/2012 10:50:00 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <mem_db.h>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace mem_db;

#define MAX_TABLE_NAME_LENGTH 15
#define MAX_KEY_LENGTH 50
#define MAX_COLUMN_LENGTH 75
#define MAX_VALUE_LENGTH 10000

#define KEY_PAIR_NUM 100000000
#define LOOP_NUM_FOR_PUT 5
#define LOOP_NUM_FOR_GET 5

int GenerateRandomTextString (int max_length, std::string &text_string) {
  const char chs[] = "abcdefghijklmnopqrstuvwzyzABCDEJFHIJKLMNOPQRSTUVWXYZ0123456789";
  int length = random () % max_length + 1;

  text_string = "";

  for (int i = 0; i < length; i++) {
    text_string += chs[random () % strlen (chs)];
  }

  return 0;
}

int GenerateRandomBinaryString (int max_length, std::string &binary_string) {
  binary_string = "";

  int length = random () % max_length + 1;

  for (int i = 0; i < length; i++) {
    binary_string += char (random () % 128);
  }

  return 0;
}

#define COLUMN_NUM 50
int main (int argc, char **argv) {
  srand (time (0));
  mem_db::MemDB mem_db;
  mem_db.Init ("/home/w/conf/mem_db/mem_db.conf");

  std::string table;
  std::string key;
  std::vector<std::string> columns;
  std::vector<std::string> values;
  std::vector<std::string> values_fetched;

  int ret = 0;
AAA:

  for (int i = 0; i < KEY_PAIR_NUM; i++) {
    // number of different key-values pqirs
    table.clear ();
    key.clear ();
    columns.clear ();
    values.clear ();
    GenerateRandomTextString (MAX_TABLE_NAME_LENGTH, table);
    GenerateRandomTextString (MAX_KEY_LENGTH, key);

    for (int j = 0; j < COLUMN_NUM; j++) {
      if (1) {
        // random to choose this branch
        std::string column;
        column.clear ();
        GenerateRandomTextString (MAX_COLUMN_LENGTH, column);
        columns.push_back (column);

        std::string value;
        value.clear ();
        GenerateRandomBinaryString (MAX_VALUE_LENGTH, value);
        values.push_back (value);
      }
    }

    // check that all columns are of diffenence
    for (int m = 0; m < COLUMN_NUM; m++) {
      for (int n = m + 1; n < COLUMN_NUM; n++) {
        if (0 ==  columns[m].compare (columns[n])) {
          goto AAA;
        }
      }
    }

    // put into mem_db
    for (int j = 0; j < LOOP_NUM_FOR_PUT; j++) {
      // number of loops of same key-value pair
      ret = mem_db.PutRowWithColumns (table, key, columns, values);

      if (0 != ret) {
        std::cout << "Putting failed: " << i << "/" << j << std::endl;
        return 0;
      }

      std::cout << "ok for putting:" << i << table << "&" << key << std::endl;
    }

#if 1

    for (int j = 0; j < LOOP_NUM_FOR_GET; j++) {
      // number of loops of same key-value pair
      ret = mem_db.GetRowWithColumns (table, key, columns, values_fetched);

      if (0 != ret) {
        std::cout << "Getting failed: " << i << "/" << j << std::endl;
        return 0;
      }

      std::cout << "ok for getting:" << i << table << "&" << key << std::endl;

      // number of loops of same key-value pair
      // do value checking
      for (int k = 0; k < COLUMN_NUM; k++) {
        if (0 != values[k].compare (values_fetched[k])) {
          std::cout << "Checking failed: " << i << "/" << j << "/" << k << std::endl;
          return 0;
        }

        std::cout << "Checking Ok: " << i << "/" << j << "/" << k << std::endl;
        //          << "----"<< "value: " <<values_fetched[k]<<std::endl;
      }
    }

    std::cout << "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOK for loop:" << i << std::endl;
#endif
  }

  return 0;

}
