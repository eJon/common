// copyright:
//            (C) SINA Inc.
//
//           file: nspio/api/ed.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_ERRORDATA_
#define _H_ERRORDATA_


#include "async_api.h"

NSPIO_DECLARATION_START

class deicmp_ed : public Error {
 public:
    deicmp_ed();
    ~deicmp_ed();

    int parsefrom(struct appmsg *__msg);
    string Str();

 private:
    struct appmsg *msg;
};

class simple_timeout_ed : public Error {
 public:
    simple_timeout_ed() {
	hid = 0;
    }
    ~simple_timeout_ed() {}
    int parsefrom(int64_t _hid, int64_t _stime);
    string Str();

 private:
    int64_t hid, stime;
};



}
#endif  // _H_APPICMP_
