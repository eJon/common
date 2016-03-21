#include <inttypes.h>
#include <sys/stat.h>
#include <unistd.h>
#include <mscom/base/checklist.h>

using namespace ms::common;
typedef void * (*thread_func)(void *);
void *thread_reloader_checker(void *_pdata) {
    CheckList *pthis = (CheckList *) _pdata;

    if(NULL == pthis) {
        return NULL;
    }

    while(pthis->isRunning()) {
        if(pthis->isUpdated()) {
            pthis->Load();
        }

        sleep(pthis->interval());
    }

    return NULL;
}

int CheckList::Load() {
    filter__.clear();
    ifstream reader(cfg_file__.c_str());

    if(!reader.is_open()) {
        return 1;
    }

    string line;

    while(reader >> line) {
        filter__[line] = 1;
    }

    reader.close();
    //TBCARD_LOG_INFO ("reload file ok");
    return 0;

}
bool CheckList::isUpdated() {
    int ret = 0;
    struct stat res;
    ret = stat(cfg_file__.c_str(), &res);

    if(ret < 0) {
        return false;
    }

    if((uint32_t)res.st_mtime == last_time_stamp_) {
        return false;
    } else {
        last_time_stamp_ = (uint32_t)res.st_mtime;
        return true; //changed
    }

    return false;
}

int CheckList::Initialize(const string &_cfg_file, int _interval) {
    cfg_file__ = _cfg_file;
    interval_ = _interval;
    Load();

    if(pthread_create(&checker_thread_, NULL, (thread_func)thread_reloader_checker, this)) {
        //TBCARD_LOG_ERROR ("file checker init failed");
        return 1;
    }

    //TBCARD_LOG_DEBUG ("file checker init ok");
    return 0;
}

