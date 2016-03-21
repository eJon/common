#ifndef SCRIBESERVER_COMMON_CONCURRENT_QUEUE_H_
#define SCRIBESERVER_COMMON_CONCURRENT_QUEUE_H_
#include <iostream>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp> 
#include <boost/thread/locks.hpp>
#include <boost/thread/thread.hpp>
template<typename Data>  
class CurrentQueue
{  
  private:  
    std::queue<Data> msg_queue_;  
    mutable boost::mutex mutex_;  
    boost::condition_variable condition_variable_;  
    bool is_running_;
    uint32_t max_queue_size_;
  public:
    ~CurrentQueue() {
    }
    CurrentQueue(uint32_t max_queue_size) :is_running_(true),
                 max_queue_size_(max_queue_size) {}
  public:  
    int Push(Data const& data)  
    {  
      boost::mutex::scoped_lock lock(mutex_);  
      if (msg_queue_.size() >= max_queue_size_) {
        return -1;
      }
      msg_queue_.push(data);
      // Out of range max queue size.
      lock.unlock();  
      condition_variable_.notify_one();  
      return 0;
    }

    bool Empty() const 
    {  
      boost::mutex::scoped_lock lock(mutex_);  
      return msg_queue_.empty();  
    }  
    bool TryPop(Data& popped_value)  
    {  
      boost::mutex::scoped_lock lock(mutex_);  
      if(msg_queue_.empty())  
      {  
        return false;  
      }  

      popped_value=msg_queue_.front();  
      msg_queue_.pop();  
      return true;  
    }  
    void WaitAndPop(Data& popped_value)  
    {  
      boost::mutex::scoped_lock lock(mutex_);  
      while(msg_queue_.empty())  
      {  
        if (!is_running_) {
          return;
        }
        condition_variable_.wait(lock);  
        if (!is_running_) {
          return;
        }
      }  
      popped_value=msg_queue_.front();  
      msg_queue_.pop();  
    }
    int GetCurrentQueueSize() const {
      boost::mutex::scoped_lock lock(mutex_);
      return msg_queue_.size();
    }
    void Stop() {
      is_running_ = false;
      condition_variable_.notify_all();
    }
};
#endif //end SCRIBESERVER_COMMON_CONCURRENT_QUEUE_H_
