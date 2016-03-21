//@version 1.0.0.1 
//@author AndrewPD
//@date 2014-12-18
//@brief 

#include <iostream>
#include <fstream>
#include "baidu_runner.h"
#include "easy_curl.h"

namespace jungle{

  int BaiduRunner::Init(const char* arg) {
    std::ifstream reader(arg);

    if (!reader.is_open()) {
      //open file failed,log here                                                                                                                   
      cerr<<"[ERR]open file failed:"<<arg<<endl;
      return 1;
    }
    url_seeds_.clear();
    int line_count = 0;
    string line = "";
    while (getline(reader, line)) {
      line_count++;
      url_seeds_[line] = true;
    }
    reader.close();
    return 0;
  }
  int BaiduRunner::Start() {
    //step 1,read url,and get the web page
    com::EasyCurl curl;
    com::curl_opt_t opt;
    curl.Initialize(opt);
    SeedTableIterator itr;
    string buff;
    for(itr = url_seeds_.begin(); itr != url_seeds_.end();++itr) {
      if (curl.Request(itr->first,buff)) {
	cerr<<"[ERR]curl failed code:"<<curl.GetLastError()<<endl;
	continue;
      }
      cout<<"response: "<<buff<<endl;
    }
    return 0;
  }

};//namespace jungle

