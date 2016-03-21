#include <sharelib/util/task_queue.h>
#include <assert.h>
SHARELIB_BS;

TaskQueue::TaskQueue()
    : _terminated(false)
    , _size(0)
    , _head(NULL)
    , _tail(NULL)
{
}

TaskQueue::~TaskQueue() {
    Free();
}
void TaskQueue::Free(){
    while(_head) {
        _tail = _head->next;
        delete _head;
        _head = _tail;
    }
}

void TaskQueue::Interrupt() {
    _lock.Lock();
    _lock.Broadcast();
    _lock.Unlock();
}

ret_t TaskQueue::Enqueue(SessionPtr& task) {
    _lock.Lock();
    if( unlikely(_terminated) ) {
        _lock.Unlock();
        return r_eof;
    }
    TaskQueue::Node* node = new TaskQueue::Node;
    node->session = task;
    if( _size == 0 ) {
        assert(_head == NULL);
        _head = node;
        _tail = node;
    } else {
        assert(_head != NULL);
        node->next = _head;
        _head->prev = node;
        _head = node;
        
    }
    _size++;
    if( _size == 1 ) _lock.Signal();
    _lock.Unlock();
    return r_succeed;
}

ret_t TaskQueue::Dequeue(SessionPtr& taskp) {
    SessionPtr task;
    _lock.Lock();
    if( unlikely(_terminated) ) {
        _lock.Unlock();
        return r_eof;
    }
    while( _size == 0 ) {
        _lock.Wait();
        if( unlikely(_size == 0) ) {
            if( _terminated ) {
                _lock.Unlock();
                return r_eof;
            } else {
                _lock.Unlock();
                return r_eagain;
            }
        }
    }
    if( _size == 1 ) {
        assert(_head == _tail);
        task = _tail->session;
        delete _tail;
        _head = NULL;
        _tail = NULL;
    } else {
        task = _tail->session;
        _tail = _tail->prev;
        delete _tail->next;
        _tail->next = NULL;
    }
    _size--;
    _lock.Unlock();

    taskp = task;
    return r_succeed;
}

void TaskQueue::Terminate() {
    _lock.Lock();
    _terminated = true;
    _lock.Broadcast();
    _lock.Unlock();
}

SHARELIB_ES;
