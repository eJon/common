// copyright:
//            (C) SINA Inc.
//
//           file: nspio/service/benchmark/benchmark.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_NSPIOBENCHMARK_
#define _H_NSPIOBENCHMARK_


#define buflen 10240
extern char buffer[buflen];
extern string g_spioconf;
extern string g_check;
extern string g_roles;
extern int g_mode;
extern int g_threads;
extern int g_clients;
extern int g_servers;
extern int g_size;
extern int g_pkgs;
extern int g_freq;
extern int g_time;
extern string appname;
extern string nspioclihost;
extern string nspiosvrhost;
extern int stopping;




#endif
