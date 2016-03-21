#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__
#include <inttypes.h>
#include <string.h>
#include <wchar.h>
#include <iostream>
#include <string>
#include <stdlib.h>
using namespace std;
//this is only used in utf-8
namespace ms {
namespace common {
//64kb
#define MAX_STR_BUFFER_LEN 1024*64
class String {
  public:
    String() {}
    virtual ~String() {}
  public:
    static int substring(const char *str, int len, string &rest);
  private:
};
}
}
#endif
