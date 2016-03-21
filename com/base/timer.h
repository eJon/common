//by Andrew,2013-10-08
//for record timeout info

#ifndef __TIME_PRINTER_H__
#define __TIME_PRINTER_H__

#include <sys/time.h>

namespace ms {
namespace com {
#define PRINT_THRESHOLD 10000
class Timer {
  public:
    Timer() {
        Reset();
    }
    ~Timer() {}
    void Reset() {
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
typedef void                * (*time_print_call_back)(string &, void *);
class TimePrinter {
    struct RecordInfo {
        Timer timer;
        bool flag;
    };
  public:
    TimePrinter(time_print_call_back _call, void *_this): call_back(_call), this__(_this) {}
    virtual ~TimePrinter() {
        Print();
    }
    int On(std::string desc) {
        std::map < std::string /*desc*/, RecordInfo >::iterator itr;
        itr = recorder_info_.find(desc);

        if(itr == recorder_info_.end()) {
            recorder_info_[desc] = RecordInfo();
        }

        recorder_info_[desc].timer.Reset();
        recorder_info_[desc].flag = false;
        return 0;
    }
    int Off(std::string desc) {
        std::map < std::string /*desc*/, RecordInfo >::iterator itr;
        itr = recorder_info_.find(desc);

        if(itr == recorder_info_.end()) {
            return 1;
        }

        int microsecond = recorder_info_[desc].timer.GetTimeMicroS();
        char buff[50] = {0};
        snprintf(buff, 50, "%d", microsecond);
        recorder_[desc] = buff;
        recorder_info_[desc].flag = true;
        return 0;
    }

  protected:
    void Notify() {
        std::map < std::string /*desc*/, RecordInfo >::iterator itr;

        for(itr = recorder_info_.begin(); itr != recorder_info_.end(); ++itr) {
            if(!itr->second.flag) {  //false off
                Off(itr->first);
            }
        }
    }
    void Print() {
        Notify();
        std::map<std::string, std::string>::const_iterator itr;
        string print_info = "\n[";

        for(itr = recorder_.begin(); itr != recorder_.end(); ++itr) {
            if(itr->first.find("product") != itr->first.npos) {
                if(atoi(itr->second.c_str()) < PRINT_THRESHOLD) {
                    return;
                }
            }

            print_info += "\"";
            print_info += itr->first;
            print_info += "\":";
            print_info += itr->second;
            print_info += ",";
        }

        if(print_info[print_info.length() - 1] == ',') {
            print_info[print_info.length() - 1] = ']';
            print_info += "\n";
        } else {
            print_info += "]\n";
        }

        if(NULL == call_back) {
            std::cout << print_info << std::endl;
        } else {
            call_back(print_info, this__);
        }
    }
  private:
    std::map < std::string /*desc*/, std::string/*microsecond*/ > recorder_;
    std::map < std::string /*desc*/, RecordInfo > recorder_info_;
    time_print_call_back call_back;
    void *this__;

};
}//end namespace
}//end namespace
#endif

