#ifndef __CSVERSION_FILTER_H__
#define __CSVERSION_FILTER_H__

#include <bitset>
#include <vector>
#include <string>
#include <mem_db/mem_db.h>
#include <matchserver/mscom/base/filter.h>

using namespace ms::common;

namespace ms {
namespace common {
struct Version {
    Version(int v, int p) {
        version = v;
        platform = p;
    }
    int version;
    int platform;
};
class CSVersionFilter : public Filter {
public:
    CSVersionFilter();
    ~CSVersionFilter();
public:
    int Initialize(const string &_cfg_file, int _a = 0) ;
    int Check(const string &_id) ;
    virtual vector<int> MultiCheck(const vector<std::string> &_key) {
        return vector<int>();
    }

private:
    Version Parse(const string &_vstr);
private:
    map<int, int> min_version_;

};
}
}
#endif //define

