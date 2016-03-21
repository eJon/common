#include <matchserver/mscom/base/blacklist_filter.h>
#include <sharelib/util/string_util.h>
#include <sharelib/util/str_util.h>
#include <sys/stat.h>
#include <unistd.h>
#include "log.h"

namespace ms {
namespace common{
typedef void * (*thread_func)(void *);
void *thread_reloader_checker(void *_pdata) {
    BlacklistFilter *pthis = (BlacklistFilter *) _pdata;

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

int BlacklistFilter::Load() {
    vector<string>::iterator it;
    map<string, int>::iterator it2;
    filter__.clear();

    for (it = cfg_file_list.begin(); it != cfg_file_list.end(); ++it) {
	    string cfg_file = *it;
	    ifstream reader(cfg_file.c_str());
	    DEBUG(LOGROOT, "update blacklist filter file %s", cfg_file.c_str());

	    if(!reader.is_open()) {
	    	continue;
	    }

	    string line;

	    while(reader >> line) {
		    filter__[line] = 1;
	    }

	    reader.close();
    }
    for (it2 = filter__.begin(); it2 != filter__.end(); ++it2) {
	//DEBUG(LOGROOT, "%s", it2->first.c_str());
    }
    return 0;

}
bool BlacklistFilter::isUpdated() {
    int ret = 0;
    struct stat res;
    bool update(false);
    vector<string>::iterator it;

    for (it = cfg_file_list.begin(); it != cfg_file_list.end(); ++it) {
	    string cfg_file = *it;
	    if ((ret = stat(cfg_file.c_str(), &res)) < 0)
		    continue;
	    if ((uint32_t)res.st_mtime <= last_time_stamp_) {
		    continue;
	    }
	    last_time_stamp_ = (uint32_t)res.st_mtime;
	    update = true;
    }
    return update;
}

int BlacklistFilter::Initialize(const string &_cfg_file, int _interval) {
    vector<string>::iterator it;
    interval_ = _interval;

    //文件列表
    cfg_file_list.clear();
    utility::ADutil::Split3(cfg_file_list, _cfg_file, ';');

    DEBUG(LOGROOT, "file list %s", _cfg_file.c_str());
    for (it = cfg_file_list.begin(); it != cfg_file_list.end(); ++it) {
	DEBUG(LOGROOT, "single file %s", (*it).c_str());
    }

    Load();
    if(pthread_create(&checker_thread_, NULL, (thread_func)thread_reloader_checker, this)) {
        //TBCARD_LOG_ERROR("file checker init failed");
        return 1;
    }

    //TBCARD_LOG_DEBUG("file checker init ok");
    return 0;
}
int BlacklistFilter::Check(const string &_id) {
    //not lock,maybe lose some request while reloading configure file
    return filter__.find(_id) == filter__.end() ? NOTIN_FILTER_LIST : ISIN_FILTER_LIST;
}
vector<int> BlacklistFilter::MultiCheck(const vector<string> &_key) {
    return vector<int>();

}
}
}

