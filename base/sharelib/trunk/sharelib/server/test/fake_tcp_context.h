#ifndef SHARELIB_SERVER_FAKE_TCP_CONTEXT_H
#define SHARELIB_SERVER_FAKE_TCP_CONTEXT_H

#include <sharelib/common.h>
#include <sharelib/server/tcp_context.h>
#include <sharelib/server/test/fake_obj.h>
SHARELIB_BS;
class FakeTcpContext : public CTcpContext
{
public:
    FakeTcpContext() : obj(NULL){
        idx = 1;}
    ~FakeTcpContext(){
        if(obj != NULL){
            delete obj;
            obj = NULL;
        }
    }
public:
    /*override*/void HandlePkg(std::string& pkg);
    /*override*/void Reset(){
        if(obj != NULL){
            delete obj;
            obj = NULL;
        }
    }
    void SetObj(FakeObj* objIn){ obj = objIn;}
private:
    FakeObj* obj;
    int idx;
};

SHARELIB_ES;

#endif //SHARELIB_FAKE_TCP_CONTEXT_H
