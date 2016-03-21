#ifndef __ERROR_DEF_H__
#define __ERROR_DEF_H__

#include <string>
#include <stdio.h>

using namespace std;
namespace ms {
namespace common {
class Error {
  public:
    Error(): last_err_no_(0) {}
    virtual ~Error() {}
  public:
    virtual void ErrorMsg(const int _err_no, const string &_msg) {
        last_err_no_ = _err_no;
        last_err_msg_ = _msg;
    }
    virtual string GetLastError() {
        char buf[10240] = {0};
        snprintf(buf, 10240, "%d:%s", last_err_no_, last_err_msg_.c_str());
        return buf;
    }
  private:
    string last_err_msg_;
    int last_err_no_;
};
}
}
#endif
