#include "../include/threadpool.hpp"
#include <iostream>
#include <thread>

std::mutex cout_mutex;

int main(){
    ThreadPool pool(4);

    for (int i = 0; i < 20 ; i++){
        pool.enqueue([i] {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Tache " << i << " par thread "
                      << std::this_thread::get_id() << std::endl;
                      //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });
    }

    pool.shutdown();

    return 0;

}