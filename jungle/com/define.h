//@version 1.0.0.1 
//@author AndrewPD
//@date 2014-12-19
//@brief 

#ifndef _JUNGLE_COM_DEFINE_H_
#define _JUNGLE_COM_DEFINE_H_

#include <string>

namespace jungle{
  namespace com{
    struct job_profile_t{
      uint64_t id;
      std::string url;
std::string title;
std::string require;
std::string duty;
std::string des;
std::string about;
uint64_t pub_tm;
uint64_t expire_tm;
uint64_t update_tm;
int salary;
int work_age;
    };//job_profile_t
    struct lecture_profile_t{
    };//lecture_profile_t
  };//namespace com
};//namespace jungle
#endif//_JUNGLE_COM_DEFINE_H_
