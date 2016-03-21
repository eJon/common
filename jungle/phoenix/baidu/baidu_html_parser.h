//@version 1.0.0.1 
//@author AndrewPD
//@date 2014-12-19
//@brief 

#ifndef _JUNGLE_RUNNER_BAIDU_HTML_PARSER_H_
#define _JUNGLE_RUNNER_BAIDU_HTML_PARSER_H_

#include <tr1/unordered_map>
#include <string>
#include "page.pb.h"

namespace jungle{
  namespace baidu{
    class HtmlParser{
    public:
      HtmlParser() {}
      virtual ~HtmlParser() {}

    public:
int parse(const std::string &html, com::page::JobDescription &jd);

    };//class HtmlParser
  };//namespace baidu
};//namespace jungle

#endif//_JUNGLE_RUNNER_BAIDU_HTML_PARSER_H_
