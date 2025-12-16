#include "../include/reader_writer.hpp"
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>

using clk = std::chrono::steady_clock;
using ms = std::chrono::milliseconds;


void test_simulatenous_readers() {
    RWLock rw;
    constexpr int R = 5;
    constexpr int SLEEP_MS = 500;

    auto reader = [&](int id){
        ReadGuard g(rw);
        std::this_thread::sleep_for(ms(SLEEP_MS));
    };

    auto start = clk::now();

    std::vector<std::thread> threads;
    for (int i = 0 ; i < R ; i++){
        threads.emplace_back(reader, i);
    }

    for (auto& t : threads){
        t.join();
    }

    auto end = clk::now();
    auto duration = std::chrono::duration_cast<ms>(end - start).count();

    std::cout << "Readers total time: " << duration << "ms\n";
}

void test_simulatenous_writers() {
    RWLock rw;

    constexpr int W = 3;
    constexpr int SLEEP_MS = 500;

    auto writer = [&](int id){
        WriteGuard g(rw);
        std::this_thread::sleep_for(ms(SLEEP_MS));
    };

    auto start = clk::now();

    std::vector<std::thread> threads;
    for (int i = 0 ; i < W ; i++){
        threads.emplace_back(writer, i);
    }

    for (auto& t : threads){
        t.join();
    }

    auto end = clk::now();
    auto duration = std::chrono::duration_cast<ms>(end - start).count();

    std::cout << "Writers total time: " << duration << "ms\n";
}

int main(){
    test_simulatenous_readers();
    test_simulatenous_writers();
}