//monitor of all module
//warnning: thread unsafe
//by Andrew
#ifndef __MONITOR_MODULE_H__
#define __MONITOR_MODULE_H__
#include <map>
#include <string>
#include <monitor/client/client_agent.h>
using namespace monitor;
using namespace std;


namespace ms {
namespace com {
/************************************************************************************/
//struct
class count_t {
  public:
    int64_t value;
    int size;
    count_t() {
        value = 0;
        size = 0;
    }
    virtual ~count_t() {}
};
class timerec_t {
  public:
    int mean;
    int max;
    int min;
    int64_t time;
    int64_t size;
    timerec_t() {
        mean = 0;
        max = 0;
        min = 0;
        time = 0;
        size = 0;
    }
};
class log_t {
  public:
    int64_t size;
    time_t btime;
    time_t etime;
    log_t() {
        size = 0;
        btime = 0;
        etime = 0;
    }
};


class monitor_opt {
  public:
    int log_flush_size;
    int def_flush_size;
    bool auto_flush;
    monitor_opt() {
        log_flush_size  = 1;
        def_flush_size = 100;
        auto_flush = false;
    }
};

class RMON {
  public:
    RMON() {}
    virtual ~RMON() {}
  private:
    static ClientAgent *rmon_client__ ;
    static map<string/*key*/, timerec_t/*time*/> timer__;
    static map<string/*key*/, count_t/*count*/> counter__;
    static map<string/*key*/, log_t/*log msg*/> logger__;
    static monitor_opt opt__;
  public:
    enum OPT {
        OPT_LOG_FLUSH_SIZE = 0,
        OPT_COUNT_DEF_FLUSH_SIZE
    };
    enum WTYPE {
        COUNT = 0,
        TIME
    };
  public:
    static int uinit() ;
    static int init(string  _cfg_file) ;
    static int setopt(OPT _opt, int _v);
    static int writelog(string _log, int _flush_size = opt__.log_flush_size) ;
    static int write(string _key, int _v/*count or time*/,
                     int type, int _flush_size = opt__.def_flush_size) ;
    //        static int write (string _key, time_t _time , int _flush_size) ;
  private:
    static int writecnt(string _key, int _count, int _flush_size) ;
    static int writetime(string _key, int _time , int _flush_size) ;
    static int flush(string _key, int64_t _count) ;
    static int flush(string _key, timerec_t _t) ;
    static int flush(string _log) ;
    static timerec_t firsttime(time_t _time) ;
    static string gettimes(time_t t);

};
}
}
#endif
