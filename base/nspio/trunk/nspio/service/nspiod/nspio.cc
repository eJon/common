// copyright:
//            (C) SINA Inc.
//
//           file: nspio/service/nspiod/nspio.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <vector>
#include "os/time.h"
#include "path/filepath.h"
#include "config.h"

using namespace std;
using namespace nspio;

extern SpioConfig spio_conf;
extern string default_spio_conf_name;
extern string g_spioconfdir;
extern int32_t g_rtime;


class _appsconf_vector {
public:
    vector<string> vec;
};

static void _walk_appsconf(const string &path, void *data) {
    _appsconf_vector *apps_conf = (_appsconf_vector *)data;
    if (HasSuffix(path, ".conf"))
	apps_conf->vec.push_back(path);
}


extern int nspiod_start_apps(vector<string> &apps_conf);
extern int nspiod_stop_apps();
extern int nspiod_start_regmgr(SpioConfig *spio_conf);
extern int nspiod_stop_regmgr();
extern int nspiod_start_monitor();
extern int nspiod_stop_monitor();
extern int nspiod_global_init(int argc, char **argv);
extern int nspiod_global_exit();

// All error will fast exit here ...

int nspio_main(int argc, char **argv) {
    int64_t start_tt = 0, up_interval = 0;
    FilePath fp;
    _appsconf_vector apps_conf;

    if (nspiod_global_init(argc, argv) < 0 || nspiod_start_regmgr(&spio_conf) < 0)
	return -1;
    nspiod_start_monitor();

    up_interval = spio_conf.appconfupdate_interval_sec * 1000;
    fp.Setup(spio_conf.apps_configdir);
    while (g_rtime > 0) {
	if (!start_tt || rt_mstime() - start_tt > up_interval) {
	    apps_conf.vec.clear();
	    fp.WalkFile(_walk_appsconf, &apps_conf);
	    if (apps_conf.vec.size())
		nspiod_start_apps(apps_conf.vec);
	    start_tt = rt_mstime();
	}
	sleep(1);
	g_rtime--;
    }

    nspiod_stop_monitor();
    nspiod_stop_apps();
    nspiod_stop_regmgr();

    nspiod_global_exit();
    return 0;
}


int main(int argc, char **argv) {
    return nspio_main(argc, argv);
}
