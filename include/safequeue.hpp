#ifndef SAFEQUEUE_HPP
#define SAFEQUEUE_HPP

#include <functional>
#include <vector>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue{
    private:
        std::queue<T> queue_;
        std::mutex mutex_;
        std::condition_variable cv_;
        bool done_ = false;
    
    public:
        void push(T func){
            std::unique_lock<std::mutex> lock(mutex_);
            queue_.push(func);
            cv_.notify_one();
        }        
        bool pop (T& item){
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this]{ return !queue_.empty() || done_; });
            if(queue_.empty()) return false;
            item = queue_.front(); //access first element
            queue_.pop(); //remove first element 
            return true;
        }
        void close(){
            std::unique_lock<std::mutex> lock(mutex_);
            done_ = true;
            cv_.notify_all();
        }
};

#endif