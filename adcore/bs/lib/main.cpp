// Copyright (c) 2010, 2011 Baidu.com, Inc. All Rights Reserved
// Author: GE Jun, JIANG Rujie
#include "lib_manager.h"
//#include "nova_thread.h"
#include <gflags/gflags.h>
#include "com_log.h"
#include "mrf/mrf.h"

DEFINE_int32(sleep_interval, 1, "Sleep interval after handling inc");
DEFINE_int64(inc_rounds, 2, "Rounds of handling inc, 0 means infinite rounds");
DEFINE_bool(log_to_screen, true, "Write log on screen or not");
DEFINE_string(log_prefix_name, "mock.",
              "log file is named as <log_prefix_name>.log, "
              "the prefix cannot have underscore and don't miss the .");

DECLARE_string(flagfile);

namespace das_lib {

struct conf_t {
    das_lib::LibManager bdlib;
};

}

void* inc_run(das_lib::conf_t* conf)
{
    ul_logstat_t ls;
    ls.spec = FLAGS_log_to_screen ? UL_LOGTTY : 0;
    ls.to_syslog = 15;
    ls.events = 15;
    if (ul_openlog_r("inc_thread", &ls) < 0) {
        DL_LOG_FATAL("Fail to open log for inc_thread");

        return NULL;
    }

    do {
        int ret = 0;
        
        DL_LOG_TRACE("Begin loading base indexes");
        ret = conf->bdlib.start_service();
        if (!ret) {
            DL_LOG_FATAL("failed to load base index!");
            break;
        }
        DL_LOG_TRACE("End loading base indexes");


        system("mv data/inc/dsp.event.1 data/inc/dsp.event.1_bak");

        system("echo all > dump_req");

        for (long i = 0; 0 == FLAGS_inc_rounds || i < FLAGS_inc_rounds; ++i) {
            //each time
            DL_LOG_TRACE("Begin bdlib.handle_inc, round(%ld)", i);
            ret = conf->bdlib.handle_inc();
            if(!ret) {
                DL_LOG_WARNING("Error occurred in bdlib.handle_inc()");
            }
            DL_LOG_TRACE("End bdlib.handle_inc");

            if (FLAGS_sleep_interval > 0) {
                DL_LOG_TRACE("Sleep for %d seconds", FLAGS_sleep_interval);
                sleep(FLAGS_sleep_interval);
            }
            system("mv data/inc/dsp.event.1_bak data/dsp.event.1");
        }

    } while (0);

    ul_closelog_r(0);
    return NULL;
}

int main(int argc,char** argv)
{
    
    int ret = 0;
    das_lib::conf_t conf;

    FLAGS_flagfile="./conf/startup/gflags.conf";
    ret = google::ParseCommandLineFlags(&argc, &argv, true);
    if (ret < 0) {
        DL_LOG_FATAL("Fail to parse flags");
        return -1;
    }

    ul_logstat_t ls;
    ls.spec = FLAGS_log_to_screen ? UL_LOGTTY : 0;
    ls.to_syslog = 15;
    ls.events = 15;
    if (ul_openlog("./log", FLAGS_log_prefix_name.c_str(), &ls, 100) < 0) {
        DL_LOG_FATAL("Fail to open log");
    }

    if (ret < 0) {
        DL_LOG_FATAL ("Fail to load_conf!");
        return -1;
    }

     mrf::ConfigureService &cs = *mrf::GlobalConfigureService::instance();
    if (!cs.load("./conf/startup/bs.conf")) {
        return -1;
    }

    ret = conf.bdlib.init(cs["lib_conf"]);
    if (! ret ) {
        DL_LOG_FATAL ("Fail to init bdlib!");
        return -1;
    }
    
    DL_LOG_TRACE("Create inc thread");
    
    inc_run(&conf);
        
    ul_closelog(0);
    
    return 0;
}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
