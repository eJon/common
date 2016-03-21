#ifndef SHARELIB_RUNNABLE_H
#define SHARELIB_RUNNABLE_H

#include <sharelib/common.h>
#include <tr1/memory>
SHARELIB_BS;
class Runnable {
public:
    Runnable() { }
    virtual ~Runnable() { }
public:
    void SetThreadId(uint32_t id){
        threadId = id;
    }
    uint32_t GetThreadId(){
        return threadId;
    }
protected:
    uint32_t threadId;
public:
    virtual ret_t Run() = 0;
};

typedef std::tr1::shared_ptr<Runnable> RunnablePtr;


SHARELIB_ES;

#endif //SHARELIB_RUNNABLE_H
