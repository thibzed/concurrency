#include "safequeue.hpp"
#include <iostream>
#include <thread>

int main(){
    ThreadSafeQueue<int> queue;

    std::thread producer([&queue] {
        for (int i = 0 ; i < 5 ; ++i) {
            queue.push(i);
            std::cout << "Pushed: " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    std::thread consumer([&queue]{
        for (int i = 0 ; i < 5 ; ++i){
            int value;
            queue.pop(value);
            std::cout << "Popped: " << value << std::endl;
        }
    });

    producer.join();
    consumer.join();
}