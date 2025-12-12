#ifndef SAFEQUEUE_HPP
#define SAFEQUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue{
    private:
        std::queue<T> queue_;
        std::mutex mutex_;
        std::condition_variable cv_;
    
    public:
        void push(T func){
            std::unique_lock<std::mutex> lock(mutex_);
            queue_.push(func);
            cv_.notify_one();
        }        
        bool pop (T& item){
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this]{return !queue_.empty();});
            item = queue_.front(); //access first element
            queue_.pop(); //remove first element 
            return true;
        }
};

#endif