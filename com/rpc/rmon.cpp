#include <iostream>

#include <mscom/rpc/rmon.h>
using namespace ms::com;
using namespace std;

ClientAgent *RMON::rmon_client__ = NULL;
map<string/*key*/, timerec_t/*time*/> RMON::timer__;
map<string/*key*/, count_t/*count*/> RMON::counter__;
map<string/*key*/, log_t/*log msg*/> RMON::logger__;
monitor_opt RMON::opt__;

int RMON::uinit() {
    if(rmon_client__) {
        delete rmon_client__;
        rmon_client__ = NULL;
    }

    return 0;
}
int RMON::init(string  _cfg_file) {
    static int init_status = 0;

    if(init_status) {
        return 0;
    }

    if(rmon_client__) {
        delete rmon_client__;
        rmon_client__ = NULL;
    }

    rmon_client__ = new ClientAgent;

    if(NULL == rmon_client__) {
        return 1;
    }

    if(rmon_client__->Init(_cfg_file)) {
        delete rmon_client__;
        rmon_client__ = NULL;
        return 2;
    }

    init_status++;
    return 0;
}

int RMON::setopt(OPT _opt, int _v) {
    switch(_opt) {
    case OPT_LOG_FLUSH_SIZE:
        opt__.log_flush_size = _v;
        break;

    case OPT_COUNT_DEF_FLUSH_SIZE:
        opt__.def_flush_size = _v;
        break;

    default:
        break;
    }

    return 0;
}

string RMON::gettimes(time_t t) {
    struct tm *ptr;
    char str[80] = {0};
    ptr = localtime(&t);
    strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S", ptr);
    return str;
}
int RMON::writelog(string _log, int _flush_size) {
    //log
    if(_flush_size <= 1) {
        flush(_log);
        return 0;
    }

    map<string, log_t>::iterator itr = logger__.find(_log);

    if(itr == logger__.end()) {
        log_t log;
        log.size = 1;
        log.btime = log.etime = time(NULL);
    } else {
        log_t log = itr->second;

        if(_flush_size <= log.size) {  //flush
            char kbuf[1024] = {0};
            snprintf(kbuf, 1024, "%ld:[%s~%s]%s",
                     log.size, gettimes(log.btime).c_str(),
                     gettimes(log.etime).c_str(), _log.c_str());
            flush(string(kbuf));
            log.size = 0;
            log.btime = 0;
            log.etime = 0;
        } else {//update
            log.etime = time(NULL);
            log.size++;
        }

        logger__[_log] = log;//update
    }

    return 0;
}
int RMON::write(string _key, int _v/*count or time*/,
                int type, int _flush_size) {
    switch(type) {
    case COUNT:
        writecnt(_key, _v, _flush_size);
        break;

    case TIME:
        writetime(_key, _v, _flush_size);
        break;

    default:
        writecnt(_key, _v, _flush_size);
        break;
    }

    return 0;
}
int RMON::writecnt(string _key, int _count, int _flush_size) {
    //write at flush size
    //counter
    if(_flush_size <= 1) {
        flush(_key, _count);
        return 0;
    }

    map<string, count_t>::iterator itr = counter__.find(_key);

    if(itr == counter__.end()) {
        count_t c;
        c.value = _count;
        c.size = 1;
        counter__[_key] = c;
    } else {
        count_t cnt = itr->second;

        if(cnt.size >= _flush_size) {
            itr->second = count_t();
            flush(_key, cnt.value);
        }

        cnt.size++;
        cnt.value = _count;
        counter__[_key] = cnt;
    }

    return 0;
}
int RMON::writetime(string _key, int _time , int _flush_size) {
    //write at flush size
    //timer
    if(_flush_size <= 1) {
        flush(_key, firsttime(_time));
        return 0;
    }

    map<string, timerec_t>::iterator itr = timer__.find(_key);

    if(itr == timer__.end()) {
        timer__[_key] = firsttime(_time);
    } else {
        timerec_t t = itr->second;

        if(t.size >= _flush_size) {
            itr->second = timerec_t();
            flush(_key, t);
        }

        if(t.max < _time) {
            t.max = _time;
        }

        if(t.min > _time) {
            t.min = _time;
        }

        t.time += _time;
        t.size++;
        t.mean = t.time / t.size;
        timer__[_key] = t;
    }

    return 0;
}
int RMON::flush(string _key, int64_t _count) {
    if(rmon_client__) {
        cout << "flush count:" << _key << ";value:" << _count << endl;
        rmon_client__->Add(_key, _count);
    }

    return 0;
}

int RMON::flush(string _key, timerec_t _t) {
    char kbuf[50] = {0};
    snprintf(kbuf, 50, "%s:min", _key.c_str());
    string msg = kbuf;

    if(rmon_client__) {
        cout << "flush time :" << msg << ";value:" << _t.min << endl;
        rmon_client__->Add(msg, _t.min);
    }

    snprintf(kbuf, 50, "%s:max", _key.c_str());
    msg = kbuf;

    if(rmon_client__) {
        cout << "flush time :" << msg << ";value:" << _t.max << endl;
        rmon_client__->Add(msg, _t.max);
    }

    snprintf(kbuf, 50, "%s:mean", _key.c_str());
    msg = kbuf;

    if(rmon_client__) {
        cout << "flush time :" << msg << ";value:" << _t.mean << endl;
        rmon_client__->Add(msg, _t.mean);
    }

    snprintf(kbuf, 50, "%s:count", _key.c_str());
    msg = kbuf;

    if(rmon_client__) {
        cout << "flush time :" << msg << ";value:" << _t.size << endl;
        rmon_client__->Add(msg, _t.size);
    }

    return 0;
}
int RMON::flush(string _log) {
    if(rmon_client__) {
        cout << "flush log :" << _log << endl;
        rmon_client__->SendSnapshot(_log);
    }

    return 0;
}
timerec_t RMON::firsttime(time_t _time) {
    timerec_t t;
    t.mean = _time;
    t.max = _time;
    t.min = _time;
    t.time = _time;
    t.size = 1;
    return t;

}

