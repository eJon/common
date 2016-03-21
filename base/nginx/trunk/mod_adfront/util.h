// copyright:
//            (C) SINA Inc.
//
//      file: util.h 
//      desc: utility function for RequestHandler
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date: 2012-10-10
//
//    change: 
#ifndef _NGINX_UTIL_H_
#define _NGINX_UTIL_H_

#include <ctime>

#include <string>
#include <vector>
#include <sstream>


#if 0
static void Split3(std::vector<std::string> &vs, const std::string &line, char dmt = '\t') {
    if (line.empty()) {
        return;
    }

    std::string::size_type p = 0;
    std::string::size_type q = 0;
    vs.clear();

    for (;;) {
        q = line.find(dmt, p);
        std::string str = line.substr(p, q - p);
        vs.push_back(str);

        if (q == std::string::npos) {
            break;
        }

        p = q + 1;
    }
}


static std::string Trim(std::string str) {
    std::string::size_type p = str.find_first_not_of(" \t\r\n\"");

    if (p == std::string::npos) {
        str = "";
        return str;
    }

    std::string::size_type q = str.find_last_not_of(" \t\r\n\"");
    str = str.substr(p, q - p + 1);
    return str;
}

static int ParseQueryByField(const std::string &queryString, const std::string &field, std::string &reqtype) {
    std::vector<std::string> params;
    Split3(params, queryString, '&');
    std::vector<std::string>::iterator vitem;

    for (vitem = params.begin(); vitem != params.end(); vitem++) {
        std::string::size_type pos;
        pos = (*vitem).find(field);

        if (pos != std::string::npos) {
            reqtype = (*vitem).substr(field.length());
        }
    }

    if (reqtype.empty()) {
        return -1;
    }

    return 0;
}

static int CreateThread(void * (*thread_fun)(void *),
                        void *arg,
                        pthread_t *thread_id,
                        int stack_size = 1 * 1024 * 1024) {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    // set the thread to be detached
    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, stack_size);
    int ret = pthread_create(thread_id, &attr, thread_fun, arg);
    pthread_attr_destroy(&attr);
    return ret;
}

// input
//      date_begin: like "2012-12-14"
//      date_end: like "2012-12-29"
// output
//      date_vectors, vector member like "2012-12-23"
//      weekday_vector, vector member like: 0, 1, 2, 3, 4, 5, 6
//      NOTE: use 0 to represent Sunday.
// return
//      0: true; not 0: false
static int GenerateDateString(const std::string &date_begin,
                              const std::string &date_end,
                              std::vector<std::string> &date_vector,
                              std::vector<int> &weekday_vector) {
    if (0 == date_begin.size()) {
        return -1;
    }

    if (0 == date_end.size()) {
        return -1;
    }

    date_vector.clear();
		weekday_vector.clear();

    struct tm tm_begin;

    struct tm tm_end;

    struct tm *tm_temp = NULL;

    time_t time_begin;

    time_t time_end;

    time_t time_temp;

    char date_str[11] = {'\0'};

    // read date details from the input date string
    ::sscanf(date_begin.c_str(), "%4d-%2d-%2d",
             &tm_begin.tm_year, &tm_begin.tm_mon, &tm_begin.tm_mday);

    ::sscanf(date_end.c_str(), "%4d-%2d-%2d",
             &tm_end.tm_year, &tm_end.tm_mon, &tm_end.tm_mday);

    // for time-relevant consideration
    tm_begin.tm_year -= 1900;

    tm_begin.tm_mon -= 1;

    tm_begin.tm_hour = 0;

    tm_begin.tm_min = 0;

    tm_begin.tm_sec = 0;

    tm_begin.tm_isdst = -1;

    tm_end.tm_year -= 1900;

    tm_end.tm_mon -= 1;

    tm_end.tm_hour = 0;

    tm_end.tm_min = 0;

    tm_end.tm_sec = 0;

    tm_end.tm_isdst = -1;

    int weekday = -1;

    time_begin = mktime(&tm_begin);

    time_end = mktime(&tm_end);

    const int sec_per_day = 60 * 60 * 24; // seconds per day

    time_temp = time_begin;

    while (time_temp <= time_end) {
        tm_temp = localtime(&time_temp);
        weekday = tm_temp->tm_wday;
        snprintf(date_str, 11, "%04d-%02d-%02d",
                 tm_temp->tm_year + 1900, tm_temp->tm_mon + 1, tm_temp->tm_mday);
        // push date string into vector
        date_vector.push_back(std::string(date_str));
        // push weekday into vector
        weekday_vector.push_back(weekday);
        // next day
        time_temp += sec_per_day;
    }

    return 0;
}

// input
//      date_begin: like "2012-12-14 00:00:00,2012-12-15 00:00:00,2012-12-16 00:00:00"
//      date_end: like "2012-12-14 23:59:59,2012-12-15 23:59:59,2012-12-16 23:59:59"
// output
//      date_vectors, vector member like "2012-12-23"
//      weekday_vector, vector member like: 0, 1, 2, 3, 4, 5, 6
//      NOTE: use 0 to represent Sunday.
// return
//      0: true; not 0: false
static int GenerateDateString_v2(const std::string &date_begin,
                                 const std::string &date_end,
                                 std::vector<std::string> &date_vector,
                                 std::vector<int> &weekday_vector) {
		std::vector<std::string> date_begin_vector;
		std::vector<std::string> date_end_vector;

    Split3(date_begin_vector, date_begin, ',');
    Split3(date_end_vector, date_end, ',');
		
		size_t len_date = date_begin_vector.size();
		if (len_date != date_end_vector.size()) {
		  return -1;
		}
		
		int ret = 0;
		std::vector<std::string> single_date_vector;
		std::vector<int> single_weekday_vector;
		for (size_t i = 0; i < len_date; ++i) {
	    if (0 != date_begin_vector[i].substr(0, 10).compare(date_end_vector[i].substr(0,10))) {
			  return -1;
			} 
			ret = GenerateDateString(date_begin_vector[i].substr(0, 10),
			                         date_end_vector[i].substr(0, 10),
															 single_date_vector,
															 single_weekday_vector);
			if (0 != ret) return -1;
			
			if (1 != single_date_vector.size()) return -1;
			if (1 != single_weekday_vector.size()) return -1;

			date_vector.push_back(single_date_vector[0]);
			weekday_vector.push_back(single_weekday_vector[0]);
		}

    return 0;
}
#endif

int UrlDecode(const std::string& in, std::string& out)
{
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i) {
    if (in[i] == '%') {
      if (i + 3 <= in.size()) {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value) {
          out += static_cast<char>(value);
          i += 2;
        } else {
          return 1;
        }
      } else {
        return 2;
      }
    }
    else if (in[i] == '+') {
      out += ' ';
    } else {
      out += in[i];
    }
  }
  return 0;
}

#endif  // _NGINX_UTIL_H_
