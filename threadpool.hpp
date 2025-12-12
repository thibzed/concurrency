#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include "safequeue.hpp"

class ThreadPool{
    private:
        ThreadSafeQueue<std::function<void()>> tasks_;
        std::vector<std::thread> workers_;
        //std::atomic<bool> stop_; 

    public:
        ThreadPool(std::size_t nb_threads) {
            for(std::size_t i = 0 ; i < nb_threads ; i++){
                workers_.emplace_back([this] {worker_loop();});
            }
        };
        ~ThreadPool(){
            shutdown();
        };
        void enqueue(std::function<void()> task){
            tasks_.push(task);
        };
        void shutdown(){
            //if (stop_) return;
            //stop_ = true;
            tasks_.close();
            for (auto& t : workers_){
                if(t.joinable()) t.join();
            }
        };
        void worker_loop(){
            while(true){
                std::function<void()> task;
                if (!tasks_.pop(task)) break;
                task();
            }
        }
};

#endif