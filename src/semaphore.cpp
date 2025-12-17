#include "../include/semaphore.hpp"

#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <cassert>

using clk = std::chrono::steady_clock;
using ms = std::chrono::milliseconds;
 
void test_concurrency_limit(){
    Semaphore sem(std::thread::hardware_concurrency());   
    std::atomic<int> active{0};
    std::atomic<int> max_active{0};
    std::mutex mtx;

    auto func = [&](int id){
        SemaphoreGuard guard(sem);
        int current = ++active;
        mtx.lock();
        std::cout << "Thread : " << std::this_thread::get_id() << "\n"; 
        mtx.unlock();
        max_active = std::max(max_active.load(), current);
        std::this_thread::sleep_for(ms(300));
        active --;
    };

    std::vector<std::thread> threads;
    for (int i = 0 ; i < 50 ; i++){
        threads.emplace_back(func, i);
    }

    for (auto& t : threads){
        t.join();
    }
    assert(max_active <= std::thread::hardware_concurrency());
}

void test_RAII(){
    Semaphore sem(1);

    try {
        SemaphoreGuard guard(sem);
        throw std::runtime_error("Error");
    } catch (...) {}

    bool acquired = sem.try_acquire();
    assert(acquired == true);
}

int main (){
    //test_concurrency_limit();
    test_RAII();
}