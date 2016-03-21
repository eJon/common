#ifndef __FILE_FILTER_H__
#define __FILE_FILTER_H__
#include <inttypes.h>
#include <string>
#include <vector>
#include <matchserver/mscom/base/filter.h>
using namespace std;

namespace ms {
namespace common{

class BlacklistFilter: public Filter {
public:
    BlacklistFilter(): isrunning_(true), interval_(10), last_time_stamp_(0) {}
    virtual ~BlacklistFilter() {
        isrunning_ = false;
        void *ret = NULL;
        pthread_join(checker_thread_, &ret);
    }
public:
    bool isRunning() {
        return isrunning_;
    }
    int interval() {
        return interval_;
    }
    int Load() ;
    virtual bool isUpdated() ;
    int Initialize(const string &_cfg_file, int _interval = 5) ;
    //return value: 1--id is in list
    //              0--id is not in list
    int Check(const std::string &_id) ;
    vector<int> MultiCheck(const vector<std::string> &_key);
private:
    map<std::string, int> filter__;
    vector<std::string> cfg_file_list;
    pthread_t checker_thread_;
    bool isrunning_;
    int interval_;
    uint32_t last_time_stamp_;
};

}//namespace
}//namespace
#endif
