//author: Andrew(pengdong@staff.sina.com.cn)
//date:2013-06-09
//desc:this is for statistics for requesting of time and count
//     maybe something else
//warnning:thread unsafe

#ifndef COMMON_UTIL_H_
#define COMMON_UTIL_H_
#include <time.h>
#include <sys/time.h>
#include <string>
#include <fstream>
#include <sstream>
#include <jsoncpp/json.h>

using namespace std;

namespace ms {
namespace common{
typedef unsigned long long ULONG;
#define MODUAL_NAME_DEFAULT "Default"

struct RT {
    ULONG request_count;
    ULONG request_time_cost;//ms to avoid out of range
    int response_time;
    ULONG max_time;//ms
    ULONG min_time;//ms
    RT() {
        request_count = 0;
        request_time_cost = 0;
        response_time = 0;
        max_time = 0;
        min_time = 0xFFFFFFFF;
    }
};
class Statistics {
public:
    Statistics() {}
    virtual ~Statistics() {}
    virtual std::string JsonStatistics() = 0;
    virtual std::string ConsoleStatistics() = 0;
    virtual void Clean() = 0;
};
class TimerRecorder: public Statistics {
public:
    TimerRecorder() {}
    virtual ~TimerRecorder() {}
public:
    void Start(std::string modual_name = MODUAL_NAME_DEFAULT) {
        gettimeofday(&time_begin_[modual_name], NULL);
    }
    ULONG Stop(std::string modual_name = MODUAL_NAME_DEFAULT) {
        ULONG timeuse;
        gettimeofday(&time_end_[modual_name], NULL);
        timeuse = GetTime(modual_name);
        response_time_[modual_name].request_count++;
        response_time_[modual_name].request_time_cost += timeuse;
        response_time_[modual_name].max_time =
            response_time_[modual_name].max_time > timeuse ?
            response_time_[modual_name].max_time : timeuse ;
        response_time_[modual_name].min_time =
            response_time_[modual_name].min_time < timeuse ?
            response_time_[modual_name].min_time : timeuse ;

        response_time_[modual_name].response_time =
            response_time_[modual_name].request_time_cost /
            response_time_[modual_name].request_count;
        return timeuse / 1000;

    }
    void GetRt(RT &rt, std::string modual_name = MODUAL_NAME_DEFAULT) {
        if(response_time_.find(modual_name) != response_time_.end()) {
            rt = response_time_[modual_name];
        }
    }
    void Clean() {
        response_time_.clear();
        time_begin_.clear();
        time_end_.clear();
    }
    std::string JsonStatistics() {
        Json::Value root;
        Json::FastWriter writer;
        Json::Value items;

        std::map < string/*modual name*/, RT/*rt*/ >::const_iterator itr;

        for(itr = response_time_.begin(); itr != response_time_.end(); ++itr) {
            Json::Value item;
            item["name"] = itr->first;
            item["count"] = itr->second.request_count;
            item["tm(ms)"] = (itr->second.request_time_cost / 1000);
            item["RT(ms)"] = (itr->second.response_time / 1000);
            item["max(ms)"] = (itr->second.max_time / 1000);
            item["min(ms)"] = (itr->second.min_time / 1000);
            items.append(item);
        }

        root.append(items);

        return writer.write(root);
    }
    std::string ConsoleStatistics() {
        ostringstream sstr;
        int width[6] = {20, 10, 10, 10, 10, 10};
        char titles[][20] = {"notify name", "count", "tm(ms)", "RT(ms)", "max(ms)", "min(ms)"};

        for(int i = 0; i < 6; i++) {
            for(int j = 0; j < width[i]; j++) {
                sstr << "-";
            }
        }

        sstr << endl;

        for(int i = 0; i < 6; i++) {
            sstr.width(width[i]);
            sstr << titles[i];
        }

        sstr << endl;
        std::map < string/*modual name*/, RT/*rt*/ >::const_iterator itr;

        for(itr = response_time_.begin(); itr != response_time_.end(); ++itr) {
            sstr.width(width[0]);
            sstr << itr->first;
            sstr.width(width[1]);
            sstr << itr->second.request_count;
            sstr.width(width[2]);
            sstr << (itr->second.request_time_cost / 1000);
            sstr.width(width[3]);
            sstr << (itr->second.response_time / 1000);
            sstr.width(width[4]);
            sstr << (itr->second.max_time / 1000);
            sstr.width(width[5]);
            sstr << (itr->second.min_time / 1000) << endl;
        }

        for(int i = 0; i < 6; i++) {
            for(int j = 0; j < width[i]; j++) {
                sstr << "-";
            }
        }

        return sstr.str();
    }
private:
    ULONG GetTime(std::string modual_name = MODUAL_NAME_DEFAULT) {
        return 1000000 * (time_end_[modual_name].tv_sec - time_begin_[modual_name].tv_sec)
               + (time_end_[modual_name].tv_usec - time_begin_[modual_name].tv_usec);
    }
private:
    std::map < string/*modual name*/, RT/*rt*/ > response_time_;
    std::map < string/*modual name*/, struct timeval/*time_begin_*/ > time_begin_;
    std::map < string/*modual name*/, struct timeval/*time_end_*/ > time_end_;
};
class RequestRecorder: public Statistics {
public:
    void Add(std::string modual_name = MODUAL_NAME_DEFAULT) {
        if(request_recorder_.find(modual_name) != request_recorder_.end()) {
            request_recorder_[modual_name]++;
        } else {
            request_recorder_[modual_name] = 1;
        }
    }
    ULONG Get(std::string modual_name = MODUAL_NAME_DEFAULT) {
        if(request_recorder_.find(modual_name) != request_recorder_.end()) {
            return request_recorder_[modual_name];
        }

        return 0;
    }
    void Clean() {
        request_recorder_.clear();
    }
    std::string JsonStatistics() {
        Json::Value root;
        Json::FastWriter writer;
        Json::Value item;
        std::map < string/*modual name*/, ULONG/*rt*/ >::const_iterator itr;

        for(itr = request_recorder_.begin(); itr != request_recorder_.end(); ++itr) {
            item[itr->first] = itr->second;
        }

        root.append(item);

        return writer.write(root);
    }
    std::string ConsoleStatistics() {
        ostringstream sstr;
        int width[2] = {20, 10};
        char title[][20] = {"notify name", "count"};

        for(int i = 0; i < 2; i++) {
            for(int j = 0; j < width[i]; j++) {
                sstr << "-";
            }
        }

        sstr << endl;

        for(int i = 0; i < 2; i++) {
            sstr.width(width[i]);
            sstr << title[i];
        }

        sstr << endl;
        std::map < string/*modual name*/, ULONG/*rt*/ >::const_iterator itr;

        for(itr = request_recorder_.begin(); itr != request_recorder_.end(); ++itr) {
            sstr.width(width[0]);
            sstr << itr->first;
            sstr.width(width[1]);
            sstr << itr->second << endl;
        }

        for(int i = 0; i < 2; i++) {
            for(int j = 0; j < width[i]; j++) {
                sstr << "-";
            }
        }

        return sstr.str();
    }

private:
    std::map < string/*modual name*/, ULONG/*count*/ > request_recorder_;
};

class CCalTimeSpan {
  public:
    CCalTimeSpan() {
        reset();
    }
    ~CCalTimeSpan() {}
    void reset() {
        gettimeofday(&_start, NULL);
    }
    int GetTimeMilliS() {
        return get_interval() / 1000;
    }
    int GetTimeMicroS() {
        return get_interval();
    }
    int GetTimeS() {
        return get_interval() / 1000000;
    }
  private:
    int get_interval() {
        struct timeval now;
        gettimeofday(&now, NULL);
        return (now.tv_sec - _start.tv_sec) * 1000000
               + now.tv_usec - _start.tv_usec;
    }
    struct timeval _start;
};

}//namespace ms
}
#endif//COMMON_UTIL_H


