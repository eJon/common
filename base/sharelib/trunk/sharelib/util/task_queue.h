#ifndef CPMWIRELESS_UTIL_TASK_QUEUE_H
#define CPMWIRELESS_UTIL_TASK_QUEUE_H


#include <sharelib/util/condition.h>
#include <sharelib/util/session.h>
SHARELIB_BS;
class TaskQueue
{
public:
    class Node{
    public:
        Node(): prev(NULL), next(NULL){}
        ~Node(){if(session != NULL) session.reset();}
    public:
        Node* prev;
        Node* next;
SessionPtr session;
    };
public:
    TaskQueue();
    ~TaskQueue();
public:
    ret_t Enqueue(SessionPtr& task);
    ret_t Dequeue(SessionPtr& taskp);
    uint32_t Size(){return _size;};
    void Interrupt();
public:
    void Terminate();
    void Free();
private:
    bool _terminated;
    
    volatile uint32_t _size;
    Condition _lock;
    TaskQueue::Node* _head;
    TaskQueue::Node* _tail;
private:

};

typedef std::tr1::shared_ptr<TaskQueue> TaskQueuePtr;

SHARELIB_ES;

#endif //CPMWIRELESS_TASK_QUEUE_H
