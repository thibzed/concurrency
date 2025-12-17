#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>

class Semaphore{
    public:
        explicit Semaphore(size_t capacity) : capacity_(capacity) , max_capacity_(capacity){}

        void acquire(){
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this]{return capacity_ > 0;});
            capacity_--;
        }
        void release(){
            std::unique_lock<std::mutex> lock(mtx_);
            if(capacity_ >= max_capacity_){
                throw std::logic_error("Semaphore released too many times");
            }
            capacity_++;
            cv_.notify_one();
        }
        bool try_acquire(){
            if (capacity_ > 0) return true;
            return false;
        }

    private:
        size_t capacity_;
        size_t max_capacity_;
        std::mutex mtx_;
        std::condition_variable cv_;
};

class SemaphoreGuard{
    public :
        explicit SemaphoreGuard(Semaphore& sem) : sem_(sem) , owns_(true) {
            sem_.acquire();
        };
        ~SemaphoreGuard(){
            if(owns_){
                sem_.release();
            }
        }
        void release(){
            if(owns_){
                sem_.release();
                owns_ = false;
            }
        }

        SemaphoreGuard(const SemaphoreGuard&) = delete;
        SemaphoreGuard& operator=(const SemaphoreGuard&) = delete;
        SemaphoreGuard& operator=(SemaphoreGuard&& other) = delete;

        SemaphoreGuard(SemaphoreGuard&& other) noexcept 
            :   sem_(other.sem_), owns_(other.owns_) {
            other.owns_ = false;
        };

    private:
        Semaphore& sem_;
        bool owns_;
};

#endif