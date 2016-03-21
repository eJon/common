//@version 1.0.0.1 
//@author AndrewPD
//@date 2014-12-18
//@brief 

#ifndef _JUNGLE_RUNNER_BAIDU_H_
#define _JUNGLE_RUNNER_BAIDU_H_

#include <tr1/unordered_map>
#include <string>
#include "runner.h"

namespace jungle{
  typedef std::tr1::unordered_map<std::string, bool> SeedTable;
  typedef std::tr1::unordered_map<std::string, bool>::iterator SeedTableIterator;
  class BaiduRunner : public Runner{
  public:
    BaiduRunner() {}
    virtual ~BaiduRunner() {}

  public:
    int Init(const char* arg);
    int Start();
  private:
    SeedTable url_seeds_;
  };//class BaiduRunner
};//namespace jungle

#endif//_JUNGLE_RUNNER_BAIDU_H_
