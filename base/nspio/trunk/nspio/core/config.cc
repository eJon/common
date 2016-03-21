// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/config.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <nspio/errno.h>
#include "proto/route.h"
#include "path/filepath.h"
#include "mem_status.h"
#include "config.h"
#include "base/ini_parser.h"


NSPIO_DECLARATION_START

extern spio_mem_status_t spio_mem_stats;

static mem_stat_t *spioconfig_mem_stats = &spio_mem_stats.spio_config;
static mem_stat_t *appconfig_mem_stats = &spio_mem_stats.app_config;

#define __nspioconf_int_parse(sec, field)	\
    do {					\
	field = parser.get_int(sec, #field);	\
	if (field <= 0)				\
	    field = default_##field;		\
    } while (0)

#define __nspioconf_string_parse(sec, field)		\
    do {						\
	if (!(p = parser.get_string(sec, #field))) {	\
	    errno = SPIO_ECONFIGURE;			\
	    return -1;					\
	}						\
	field = p;					\
    } while (0)

#define __nspioconf_filepath_parse(sec, field)	\
    do {					\
	__nspioconf_string_parse(sec, field);	\
	if (!IsAbs(field))			\
	    field = Abs(dir + field);		\
	if (0 != access(field.c_str(), F_OK)) {	\
	    errno = SPIO_ECONFIGURE;		\
	    return -1;				\
	}					\
    } while (0)


    
SpioConfig::SpioConfig() :

    epoll_timeout_msec(default_epoll_timeout_msec),
    register_timeout_msec(default_register_timeout_msec),
    register_interval_msec(default_register_interval_msec),
    iothreadpool_workers(default_iothreadpool_workers),
    appconfupdate_interval_sec(default_appconfupdate_interval_sec)
{
    spioconfig_mem_stats->alloc++;
    spioconfig_mem_stats->alloc_size += sizeof(SpioConfig);
}

SpioConfig::~SpioConfig() {
    spioconfig_mem_stats->alloc--;
    spioconfig_mem_stats->alloc_size -= sizeof(SpioConfig);
}

void SpioConfig::Reset() {
    apps_configdir = log4conf = "";
    register_timeout_msec = default_register_timeout_msec;
    register_interval_msec = default_register_interval_msec;
    epoll_timeout_msec = default_epoll_timeout_msec;
    iothreadpool_workers = default_iothreadpool_workers;
    appconfupdate_interval_sec = default_appconfupdate_interval_sec;
    regmgr_listen_addrs.clear();
}    

int SpioConfig::Init(const char *conf) {
    ini_parser_t parser;
    const char *p;
    string dir, laddr;
    char *listen;

    if (0 != access(conf, F_OK))
	return -1;
    dir = Dir(conf);
    parser.load(conf);

    if (!(p = parser.get_string("regmgr", "regmgr_listen_addrs"))) {
	errno = SPIO_ECONFIGURE;
	return -1;
    }
    listen = strdup(p);
    p = strtok(listen, ";");
    while (p) {
	laddr = p;
	regmgr_listen_addrs.insert(laddr);
	p = strtok(NULL, ";");
    }
    free(listen);
    __nspioconf_int_parse("regmgr", epoll_timeout_msec);
    __nspioconf_int_parse("regmgr", register_timeout_msec);
    __nspioconf_int_parse("regmgr", register_interval_msec);

    __nspioconf_filepath_parse("spio", log4conf);
    __nspioconf_filepath_parse("spio", monitor);
    __nspioconf_filepath_parse("spio", apps_configdir);
    __nspioconf_int_parse("spio", iothreadpool_workers);
    __nspioconf_int_parse("spio", appconfupdate_interval_sec);
    
    return 0;
}


CtxConf::CtxConf() :
    mtime(0),
    pollcycle_count(default_pollcycle_count),
    virtual_node(default_virtual_node),
    role_timeout_msec(default_role_timeout_msec),
    role_healthcheck_msec(default_role_healthcheck_msec),
    msg_max_size(default_msg_max_size),
    msg_timeout_msec(default_msg_timeout_msec),
    msg_balance_factor(default_msg_balance_factor),
    msg_balance_algo(default_msg_balance_algo),
    msg_queue_size(default_msg_queue_size),
    epoll_io_events(default_epoll_io_events),
    epoll_timeout_msec(default_epoll_timeout_msec),
    reconnect_timeout_msec(default_reconnect_timeout_msec),
    monitor_record_stats_msec(default_monitor_record_stats_msec)
{
    appconfig_mem_stats->alloc++;
    appconfig_mem_stats->alloc_size += sizeof(SpioConfig);
}


CtxConf::~CtxConf() {
    appconfig_mem_stats->alloc--;
    appconfig_mem_stats->alloc_size -= sizeof(SpioConfig);
}



int CtxConf::Init(const char *conf) {
    ini_parser_t parser;
    const char *p;
    string raddr, roleid;
    char *connect_to, *cur;
    struct stat finfo = {};

    if (stat(conf, &finfo) < 0)
	return -1;
    parser.load(conf);

    __nspioconf_string_parse("app", appid);
    __nspioconf_int_parse("app", pollcycle_count);
    __nspioconf_int_parse("app", virtual_node);
    __nspioconf_int_parse("app", role_timeout_msec);
    __nspioconf_int_parse("app", msg_max_size);
    __nspioconf_int_parse("app", msg_timeout_msec);
    __nspioconf_int_parse("app", msg_balance_factor);
    __nspioconf_int_parse("app", msg_balance_algo);
    __nspioconf_int_parse("app", msg_queue_size);
    __nspioconf_int_parse("app", epoll_io_events);    
    __nspioconf_int_parse("app", epoll_timeout_msec);
    __nspioconf_int_parse("app", reconnect_timeout_msec);
    __nspioconf_int_parse("app", monitor_record_stats_msec);
    
    if ((p = parser.get_string("app", "connect_to_apps")) != NULL) {
	connect_to = strdup(p);
	cur = strtok(connect_to, ";");
	while (cur) {
	    raddr = cur;
	    route_genid(roleid);
	    inat_apps.insert(make_pair(raddr, roleid));
	    cur = strtok(NULL, ";");
	}
	free(connect_to);
    }
    __nspioconf_string_parse("module_stat", app_trigger_level);
    __nspioconf_string_parse("module_stat", mq_trigger_level);
    __nspioconf_string_parse("module_stat", role_trigger_level);
    mtime = finfo.st_mtime;

    return 0;
}

#undef __nspioconf_int_parse
#undef __nspioconf_filepath_parse


bool CtxConf::raddr_exist(const string &raddr) {
    map<string, string>::iterator it;

    if ((it = at_apps.find(raddr)) != at_apps.end())
	return true;
    if ((it = inat_apps.find(raddr)) != inat_apps.end())
	return true;
    if ((it = wt_apps.find(raddr)) !=
	wt_apps.end())
	return true;
    return false;
}    

int CtxConf::insert_inactive(const string &raddr, const string &id) {
    inat_apps.insert(make_pair(raddr, id));
    return 0;
}

int CtxConf::delete_inactive(const string &raddr) {
    map<string, string>::iterator it;

    if ((it = inat_apps.find(raddr)) != inat_apps.end())
	inat_apps.erase(it);
    return 0;
}

int CtxConf::insert_active(const string &raddr, const string &id) {
    at_apps.insert(make_pair(raddr, id));
    return 0;
}

int CtxConf::delete_active(const string &raddr) {
    map<string, string>::iterator it;

    if ((it = at_apps.find(raddr)) != at_apps.end())
	at_apps.erase(it);
    return 0;
}

int CtxConf::insert_waiting(const string &raddr, const string &id) {
    wt_apps.insert(make_pair(raddr, id));
    return 0;
}

int CtxConf::delete_waiting(const string &raddr) {
    map<string, string>::iterator it;

    if ((it = wt_apps.find(raddr)) != wt_apps.end())
	wt_apps.erase(it);
    return 0;
}

int CtxConf::insert_removed(const string &raddr) {
    rm_apps.insert(raddr);
    return 0;
}

int CtxConf::delete_removed(const string &raddr) {
    set<string>::iterator it;

    if ((it = rm_apps.find(raddr)) != rm_apps.end())
	rm_apps.erase(it);
    return 0;
}



int CtxConf::clone(CtxConf *cp) {
    string roleid;
    map<string, string>::iterator it;
    
    *cp = *this;
    cp->inat_apps.clear();
    for (it = inat_apps.begin(); it != inat_apps.end(); ++it) {
	route_genid(roleid);
	cp->inat_apps.insert(make_pair(it->first, roleid));
    }
    return 0;
}

    
}
