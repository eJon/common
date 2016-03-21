/**
 * @brief 读取数据，必要时对字段进行Base64解码，调用API插入DataServing
 *
 * @input format: table_name [\t] row_key [\t] column_spec [\t] value(Base64 encoded)
 *
 * @auth philoc.jing@hotmail.com
 * @date 2012.10.24
 *
 * @info gerenally according to Google C++ Style & Java Style
 */

#include <string>
#include <vector>
#include <iostream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
using namespace std;

#include "Base64.h"
using namespace helptool;

#include "mem_db/mem_db.h"
using namespace mem_db;


// 最大允许错误率
const double MAX_ERR_RATIO = 1e-4;
const int REC_PER_DOT = 10000;
const int REC_PER_LINE = 80 * REC_PER_DOT;


int main (int argc, char **argv) {
  const string err_str = string ("")
                         + "Desc:  read out data from mem_db, decode and check.\n"
                         + "Usage: " + argv[0] + " db_conf < [STDIN]\n";

  if (argc < 2) {
    cerr << err_str << endl;
    exit (1);
  }

  // 与data serving服务建立连接
  MemDB db;

  if (0 != db.Init (argv[1])) {
    cerr << "connect db server error using " << argv[1] << endl;
    exit (2);
  }

  // 逐行读取数据
  string line;
  vector<string> raw_tokens;
  uint64_t total_rec = 0;
  uint64_t err_rec = 0;

  while (getline (cin, line)) {
    ++total_rec;
    std::cout << "line " << total_rec << ": ";

    boost::split (raw_tokens, line,
                  boost::is_any_of ("\t"), boost::token_compress_on);

    if (4 != raw_tokens.size ()) {
      cout << "Invalid format." << endl;
      continue;
    }

    if (!Base64::isValid (raw_tokens.at (3))) {
      cout << "Invalid format(not available base64 code)." << endl;
      continue;
    }

    const vector<string> raw_column_spec (1, raw_tokens.at (2));
    const vector<string> raw_value (1, Base64::decode (raw_tokens.at (3)));
    cout << "table/row/column: "
         << raw_tokens.at (0) << "/"
         << raw_tokens.at (1) << "/"
         << raw_column_spec.at (0) << "\t";

    // 插入数据服务
    vector<string> value_stored;
    int db_ret = db.GetRowWithColumns (raw_tokens.at (0), raw_tokens.at (1),
                                       raw_column_spec, value_stored);

    if (0 != db_ret) {
      cerr << "Get data error" << endl;
      err_rec++;
      continue;
    }

    //cout << "raw value: " << raw_value[0]<< endl;
    //cout << "value_stored: " << value_stored[0]<< endl;
    if (0 == value_stored[0].compare (raw_value[0])) {
      cout << "pass." << endl;
    } else {
      cout << "fails." << endl;
      err_rec++;
    }
  }

  cout << "Totol: " << total_rec << endl;
  cout << "Pass : " << total_rec - err_rec << endl;
  cout << "Error: " << err_rec << endl;

  db.Free ();
}
