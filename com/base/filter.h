#ifndef __FILTER_H__
#define __FILTER_H__
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <pthread.h>

using namespace std;
namespace ms {
namespace common{

#define NOTIN_FILTER_LIST 0
#define ISIN_FILTER_LIST 1
#define ERR_FILTER_LIST 2
#define FILTER_ON 1
#define FILTER_OFF 0

class Filter {
public:
    Filter(): filter_mask_(FILTER_ON) {}
    virtual ~Filter() {}
public:
    virtual int Initialize(const string &_cfg_file, int _interval = 60) = 0;
    //return value: true--id is in list
    //              false--id is not in list
    virtual int Check(const std::string &_key) = 0;
    virtual vector<int> MultiCheck(const vector<std::string> &_key) = 0;
    void Mask(int _mask) {
        filter_mask_ = _mask;
    }
    int Mask() {
        return filter_mask_;
    }
private:
    int filter_mask_;

};

}
}
#endif
