//@version 1.0.0.1 
//@author AndrewPD
//@date 2014-12-18
//@brief 

#ifndef _JUNGLE_RUNNER_H_
#define _JUNGLE_RUNNER_H_

namespace jungle{
  class Runner{
  public:
    Runner() {}
    virtual ~Runner() {}

  public:
    virtual int Init(const char* arg) = 0;
    virtual int Start() = 0;
  };//class Runner
};//namespace jungle

#endif//_JUNGLE_RUNNER_H_
