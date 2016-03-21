// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/test/config_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <gtest/gtest.h>
#include <iostream>
#include <fcntl.h>
#include <set>
#include "log.h"
#include "config.h"


using namespace std;
using namespace nspio;


char spiodata[] = "[regmgr]\n\
regmgr_listen_addrs=*:1510;*:1889;*8080\n\
epoll_timeout_msec=10\n\
register_timeout_msec=10\n\
register_interval_msec=60000\n\
\n\
[spio]\n\
apps_configdir=/tmp\n				\
log4conf=spio.conf\n				\
monitor=monitor.conf\n\
iothreadpool_workers=1\n\
appconfupdate_interval_sec=1\n";


char spiodata2[] = "[regmgr]\n\
regmgr_listen_addrs=*:1510;*:1889;*8080\n\
epoll_timeout_msec=10\n\
register_timeout_msec=10\n\
register_interval_msec=60000\n\
\n\
[spio]\n\
apps_configdir=/tmp\n				\
log4conf=/tmp/spio2.conf\n			\
monitor=/tmp/monitor2.conf\n\
appconfupdate_interval_sec=1\n";


char appdata[] = "[app]\n\
appid=testapp\n\
pollcycle_count=5\n\
virtual_node=5\n\
role_timeout_msec=500\n				\
msg_max_size=1024\n\
msg_timeout_msec=10\n				\
msg_balance_factor=500\n\
msg_queue_size=1024000\n\
epoll_io_events=1024\n\
reconnect_timeout_msec=5000\n\
monitor_record_stats_msec=60\n\
connect_to_apps=127.0.0.1:1510;127.0.0.2:1770;127.0.0.9:8080\n\
[module_stat]\n\
app_trigger_level=RRCVPKG:m:1\n\
role_trigger_level=RECONNECT:m:1;SEND_BYTES:m:1;RECV_BYTES:m:1\n	\
mq_trigger_level=DROPPED:s:1;AVG_TO_MSEC:s:50\n\n";



static int test_appconfig() {
    CtxConf config;
    int fd;
    string appconf = "/tmp/app.conf";
    map<string, string>::iterator it;

    fd = open(appconf.c_str(), O_RDWR|O_CREAT|O_TRUNC, 0644);
    if (fd < 0)
	return -1;
    write(fd, appdata, strlen(appdata));
    close(fd);
    
    if (0 != config.Init(appconf.c_str())) {
	unlink(appconf.c_str());
	return -1;
    }
    EXPECT_TRUE(config.appid == "testapp");
    EXPECT_TRUE(config.role_trigger_level == "RECONNECT:m:1;SEND_BYTES:m:1;RECV_BYTES:m:1");
    EXPECT_TRUE(config.mq_trigger_level == "DROPPED:s:1;AVG_TO_MSEC:s:50");
    EXPECT_EQ(500, config.role_timeout_msec);
    EXPECT_EQ(5, config.pollcycle_count);
    EXPECT_EQ(5, config.virtual_node);
    EXPECT_EQ(1024, config.msg_max_size);
    EXPECT_EQ(10, config.msg_timeout_msec);
    EXPECT_EQ(500, config.msg_balance_factor);
    EXPECT_EQ(1024000, config.msg_queue_size);
    EXPECT_EQ(1024, config.epoll_io_events);
    EXPECT_EQ(default_epoll_timeout_msec, config.epoll_timeout_msec);

    EXPECT_EQ(5000, config.reconnect_timeout_msec);
    EXPECT_EQ(60, config.monitor_record_stats_msec);
    
    for (it = config.inat_apps.begin(); it != config.inat_apps.end(); ++it)
	NSPIOLOG_INFO("appid %s connect_to %s", config.appid.c_str(), it->second.c_str());
    
    unlink(appconf.c_str());
    return 0;
}


static int test_spioconfig() {
    SpioConfig config;
    int fd, fd2;
    string monitor = "/tmp/monitor.conf";
    string spioconf = "/tmp/spio.conf";
    set<string>::iterator it;

    fd = open(spioconf.c_str(), O_RDWR|O_CREAT|O_TRUNC, 0644);
    if (fd < 0)
	return -1;
    fd2 = open(monitor.c_str(), O_RDWR|O_CREAT|O_TRUNC, 0644);
    if (fd2 < 0)
	return -1;
    write(fd, spiodata, strlen(spiodata));
    close(fd);
    close(fd2);
    
    if (0 != config.Init(spioconf.c_str())) {
	unlink(spioconf.c_str());
	unlink(monitor.c_str());
	return -1;
    }
    EXPECT_TRUE(config.log4conf == "/tmp/spio.conf");
    EXPECT_TRUE(config.monitor == "/tmp/monitor.conf");
    for (it = config.regmgr_listen_addrs.begin(); it != config.regmgr_listen_addrs.end(); ++it)
	NSPIOLOG_INFO("spio register %s", (*it).c_str());
    EXPECT_EQ(10, config.epoll_timeout_msec);
    EXPECT_EQ(10, config.register_timeout_msec);
    EXPECT_EQ(1, config.iothreadpool_workers);
    EXPECT_EQ(1, config.appconfupdate_interval_sec);
    EXPECT_EQ(60000, config.register_interval_msec);
    
    unlink(spioconf.c_str());
    unlink(monitor.c_str());
    return 0;
}


static int test_spioconfig2() {
    SpioConfig config;
    int fd, fd2;
    string monitor = "/tmp/monitor2.conf";
    string spioconf = "/tmp/spio2.conf";
    set<string>::iterator it;

    fd = open(spioconf.c_str(), O_RDWR|O_CREAT|O_TRUNC, 0644);
    if (fd < 0)
	return -1;
    fd2 = open(monitor.c_str(), O_RDWR|O_CREAT|O_TRUNC, 0644);
    if (fd2 < 0)
	return -1;
    write(fd, spiodata2, strlen(spiodata2));
    close(fd);
    close(fd2);
    
    if (0 != config.Init(spioconf.c_str())) {
	unlink(spioconf.c_str());
	unlink(monitor.c_str());
	return -1;
    }
    EXPECT_TRUE(config.log4conf == "/tmp/spio2.conf");
    EXPECT_TRUE(config.monitor == "/tmp/monitor2.conf");
    for (it = config.regmgr_listen_addrs.begin(); it != config.regmgr_listen_addrs.end(); ++it)
	NSPIOLOG_INFO("spio register %s", (*it).c_str());
    EXPECT_EQ(10, config.epoll_timeout_msec);
    EXPECT_EQ(10, config.register_timeout_msec);
    EXPECT_EQ(1, config.appconfupdate_interval_sec);
    
    unlink(spioconf.c_str());
    unlink(monitor.c_str());
    return 0;
}



TEST(config, app) {
    EXPECT_EQ(0, test_appconfig());
}



TEST(config, spio) {
    EXPECT_EQ(0, test_spioconfig());
    EXPECT_EQ(0, test_spioconfig2());
}
