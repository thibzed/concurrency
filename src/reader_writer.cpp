#include "../include/reader_writer.hpp"
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>
#include <atomic>

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

void test_starvation() {
    RWLock rw;
    std::atomic<bool> stop{false};
    std::atomic<int> writer_done{0};

    constexpr int READERS = 5;
    constexpr int SLEEP_MS = 10;

    auto continuous_reader = [&](int id){
        while (!stop){
            ReadGuard g(rw);
            std::cout << "Reader : " << id << "\n";
            std::this_thread::sleep_for(ms(SLEEP_MS));
        }
        std::cout << "Reader " << id << " finished\n";
    };

    auto writer = [&](int id){
        WriteGuard g(rw);
        std::cout << "Writer " << id << " finished\n";
        writer_done++;
    };

    std::vector<std::thread> readers;
    for (int i = 0 ; i < READERS ; i++){
        readers.emplace_back(continuous_reader, i);
    }
    std::this_thread::sleep_for(ms(50));

    std::thread writer_thread(writer, 0);
    writer_thread.join();

    stop = true;
    for (auto& t : readers){
        t.join();
    }
    if (writer_done == 1) {
        std::cout << "Success\n";
    } else {
        std::cout << "Fail\n";
    }
}

int main(){
    //test_simulatenous_readers();
    //test_simulatenous_writers();
    test_starvation();
    return 0;
}