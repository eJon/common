// copyright:
//            (C) SINA Inc.
//
//           file: nspio/service/nspiod/opt.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <signal.h>
#include <nspio/errno.h>
#include "os/os.h"
#include "log.h"
#include "config.h"
#include "os/daemon.h"
#include "os/pid.h"
#include "runner/task_pool.h"

using namespace std;
using namespace nspio;

SpioConfig spio_conf;
string default_spio_conf_name = "spio.conf";
string g_spioconf;
string g_spioconfdir;
string g_cmd;
string g_daemon = "yes";
int32_t g_rtime = 0x7fffffff;
TaskPool iothreadpool;



static char _usage_data[] = "\n\
NAME\n\
    nspio - proxy io\n\
\n\
SYNOPSIS\n\
    nspio -c configdir -s start|stop|restart [-r runtime] [-d no]\n\
\n\
OPTIONS\n\
    -c nspio configure dir.\n\
    -s service cmd. start|stop|restart\n\
    -r service running time. default = 68years\n\
    -d daemon. default daemon = yes\n\n\
\n\
EXAMPLE:\n\
    nspio -c /home/w/share/nspio/conf -s start -d no\n\n";





static void nspio_usage(const char* exeName) {
    system("clear");
    printf("%s", _usage_data);
}

static int nspio_getoption (int argc, char* argv[]) {
    int rc;

    while ( (rc = getopt(argc, argv, "c:s:r:d:h")) != -1 ) {
        switch(rc) {
        case 'c':
	    g_spioconfdir = optarg;
	    g_spioconf = g_spioconfdir + "/" + default_spio_conf_name;
            break;
        case 's':
            if(strcmp(optarg,"start") == 0 || strcmp(optarg,"restart") == 0 ||
               strcmp(optarg,"stop") == 0) {
                g_cmd = optarg;
            } else {
                nspio_usage(argv[0]);
                return -1;
            }
            break;
	case 'r':
	    g_rtime = atoi(optarg);
	    if (g_rtime == 0)
		g_rtime = 0x7fffffff;
	    break;
	case 'd':
	    g_daemon = optarg;
	    break;
        case 'h':
	    nspio_usage(argv[0]);
	    return -1;
        }
    }
    if (g_spioconf == "" || g_cmd == "") {
	nspio_usage(argv[0]);
	exit(-1);
    }
    return 0;
}

static int nspio_ignore_signal() {
    // ignore SIGPIPE signal
    if (SIG_ERR == signal(SIGPIPE, SIG_IGN)) {
	fprintf(stderr, "signal SIGPIPE ignore\n");
	return -1;
    }
    return 0;
}

static int nspio_process_command() {
    pid_ctrl_t pc("nspio", "daemon", NULL);
    pid_ctrl_t::EPidCtrlResult reason;

    if (g_cmd == "stop" || g_cmd == "restart") {
	if (!pc.kill(SIGTERM)) {
	    fprintf(stderr, "permission denied when stop or restart\n");
	}
	if (g_cmd == "stop")
	    return -1;
    }
    if (pc.exists(reason)) {
	switch (reason) {
	case pid_ctrl_t::PCR_PERMISSION_DENIED:
	    fprintf(stderr, "permission denied\n");
	    break;
	case pid_ctrl_t::PCR_RUNNING_ALREADY:
	    fprintf(stderr, "running already\n");
	    break;
	case pid_ctrl_t::TOTAL_PCR:
	default:
	    break;
	}
	return -1;
    }
    if (g_daemon == "yes")
	init_daemon();
    sleep(1);
    pc.savePid();
    return 0;
}


int nspiod_global_init(int argc, char **argv) {
    nspio_os_init();
    nspio_ignore_signal();

    if (nspio_getoption(argc, argv) < 0)
	return -1;
    if ((spio_conf.Init(g_spioconf.c_str())) < 0) {
	fprintf(stderr, "spio conf init %s with errno %d\n", g_spioconf.c_str(), errno);
	return -1;
    }
    if (chdir(g_spioconfdir.c_str()) < 0) {
	fprintf(stderr, "spio chdir %s with errno %d\n", g_spioconfdir.c_str(), errno);
	return -1;
    }
    if (nspio_process_command() < 0)
	return -1;

    if (!spio_conf.log4conf.empty())
	NSPIOLOG_CONFIG(spio_conf.log4conf);
    iothreadpool.Setup(spio_conf.iothreadpool_workers);
    iothreadpool.Start();
    return 0;
}


int nspiod_global_exit() {
    if (!spio_conf.log4conf.empty())
	NSPIOLOG_SHUTDOWN();
    iothreadpool.Stop();
    return 0;
}
