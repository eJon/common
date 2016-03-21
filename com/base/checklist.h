#ifndef __CHECKLIST_H__
#define __CHECKLIST_H__
#include <mscom/base/filter.h>
namespace ms {
namespace common {

class CheckList: public Filter {
  public:
    CheckList(): isrunning_(true), interval_(10) {}
    virtual ~CheckList() {
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
    virtual int Check(const string &_id) {
        //not lock,maybe lose some request while reloading configure file
        return filter__.find(_id) == filter__.end() ? NOTIN_FILTER_LIST : ISIN_FILTER_LIST;
    }
  private:
    map<string, int> filter__;
    string cfg_file__;
    pthread_t checker_thread_;
    bool isrunning_;
    int interval_;
    uint32_t last_time_stamp_;
};

}//namespace
}
#endif
