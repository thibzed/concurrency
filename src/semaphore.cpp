#include "../include/semaphore.hpp"

#include <vector>
#include <thread>
#include <chrono>

using clk = std::chrono::steady_clock;
using ms = std::chrono::milliseconds;
 
void test_concurrency_limit(){
    Semaphore sem(1);   
    std::mutex mtx;

    auto func = [&](int id){
        SemaphoreGuard guard(sem);
        mtx.lock();
        std::cout << "Thread : " << std::this_thread::get_id() << "\n"; 
        mtx.unlock();
        std::this_thread::sleep_for(ms(3000));
    };

    std::vector<std::thread> threads;
    for (int i = 0 ; i < 5 ; i++){
        threads.emplace_back(func, i);
    }

    for (auto& t : threads){
        t.join();
    }
}

int main (){
    test_concurrency_limit();
}