#include <sharelib/util/time_utility.h>
#include <string>

SHARELIB_BS;
using namespace std;

int64_t TimeUtility::GetSecondsOfDayBegin(){
    tm tmNow;
    time_t tNow = time(NULL);
    localtime_r(&tNow, &tmNow);
    tmNow.tm_hour = 0;
    tmNow.tm_min = 0;
    tmNow.tm_sec =0 ;
    time_t begin = mktime(&tmNow);
    return begin;
}
int64_t TimeUtility::GetSecondsOfDayEnd(){
    return GetSecondsOfDayBegin() + 86400;
}
int64_t TimeUtility::CurrentTime() {
    struct timeval tval;
    gettimeofday(&tval, NULL);
    return (tval.tv_sec * 1000000LL + tval.tv_usec);
}

int64_t TimeUtility::CurrentTimeInNanoSeconds() {
    return CurrentTime() * 1000;
}

std::string TimeUtility::CurrentTimeDateReadable(){
    tm tmNow;
    time_t tNow = time(NULL);
    localtime_r(&tNow, &tmNow);

    char szNow[32];
    sprintf(szNow,
            "%04d-%02d-%02d",
            tmNow.tm_year + 1900,
            tmNow.tm_mon + 1,
            tmNow.tm_mday);
    return string(szNow);
}
string TimeUtility::CurrentTimeInSecondsReadable(){
    tm tmNow;
    time_t tNow = time(NULL);
    localtime_r(&tNow, &tmNow);

    char szNow[32];
    sprintf(szNow,
            "%04d%02d%02d%02d%02d%02d",
            tmNow.tm_year + 1900,
            tmNow.tm_mon + 1,
            tmNow.tm_mday,
            tmNow.tm_hour,
            tmNow.tm_min,
            tmNow.tm_sec);
    return string(szNow);
}
int64_t TimeUtility::CurrentTimeInSeconds() {
    return CurrentTime() / 1000000;
}

int64_t TimeUtility::CurrentTimeInMs(){
    return CurrentTime() / 1000;
}
int64_t TimeUtility::CurrentTimeInMicroSeconds() {
    return CurrentTime();
}

int64_t TimeUtility::GetTime(int64_t usecOffset) {
    return CurrentTime() + usecOffset;
}

timeval TimeUtility::GetTimeval(int64_t usecOffset) {
    timeval tval;
    int64_t uTime = GetTime(usecOffset);
    tval.tv_sec = uTime / 1000000;
    tval.tv_usec = uTime % 1000000;
    return tval;
}

timespec TimeUtility::GetTimespec(int64_t usecOffset) {
    timespec tspec;
    int64_t uTime = GetTime(usecOffset);
    tspec.tv_sec = uTime / 1000000;
    tspec.tv_nsec = (uTime % 1000000) * 1000;
    return tspec;
}

string TimeUtility::CurrentTimeString() {
    time_t lt = time(NULL);
    struct tm *ptr = localtime(&lt);
    char str[200];
    strftime(str, sizeof(str), "%Y%m%d%H%M%S", ptr);
    return string(str);
}

int64_t TimeUtility::GetSecondFromString(const std::string& stime)
{
  tm ptime;
  if (NULL == strptime(stime.c_str(), "%Y-%m-%d %H:%M:%S", &ptime)) {
    return 0; 
  }
  return mktime(&ptime);
}

SHARELIB_ES;
