#ifndef SHARELIB_UTIL_SESSION_H
#define SHARELIB_UTIL_SESSION_H
#include <sharelib/common.h>
#include <tr1/memory>

SHARELIB_BS;
class Session
{
public:
    Session(){}
    virtual ~Session(){}
public:
private:
};

typedef std::tr1::shared_ptr<Session> SessionPtr;


SHARELIB_ES;
#endif //CPMWIRELESS_SESSION_H
