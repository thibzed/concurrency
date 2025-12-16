#include "../include/reader_writer.hpp"
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>
#include <atomic>
#include <map>
#include <string>
#include <random>

using clk = std::chrono::steady_clock;
using ms = std::chrono::milliseconds;

struct SharedDatabase {
    std::map<std::string, int> map;
    RWLock& rw;

    SharedDatabase(RWLock& lock) : rw(lock) {}

    int get(const std::string& key){
        ReadGuard g(rw);
        return map[key];
    }
    void set(const std::string& key, int value){
        WriteGuard g(rw);
        map[key] = value;
    }
};

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

void test_database(){
    RWLock rw;
    SharedDatabase db(rw);  
    std::mutex mtx;

    constexpr int DB_SIZE = 10;
    constexpr int WRITERS_COUNT = 2;
    constexpr int READERS_COUNT = DB_SIZE;
    constexpr int MS_SLEEP = 5;

    std::atomic<bool> end{false};
    std::atomic<int> writed{0};

    for (int i = 0 ; i < DB_SIZE ; i++){
        std::string s = "KEY_" + std::to_string(i);
        db.map[s] = 0;
    }

    auto reader = [&] (int id){
        while(!end){
            ReadGuard g(rw);
            try{
                std::lock_guard<std::mutex> lock(mtx);
                std::cout << db.map.at("KEY_" + std::to_string(id)) << std::endl;
            }
            catch (...) {
                std::cout << "Key out of range." << std::endl;
            }
        }
    };

    auto writer = [&] (int id){
        std::mt19937 rng(id);
        std::uniform_int_distribution<int> dist(0, DB_SIZE - 1);
        while(!end){
            int key_id = dist(rng);
            WriteGuard g(rw);
            db.map["KEY_" + std::to_string(key_id)]++;
            writed++;
            if(writed.fetch_add(1) >= 999) end = true;
        }
    };

    std::vector<std::thread> readers;
    for (int i = 0 ; i < READERS_COUNT ; i++){
        readers.emplace_back(reader, i);
    }

    std::vector<std::thread> writers;
    for (int i = 0 ; i < WRITERS_COUNT ; i++){
        writers.emplace_back(writer, i);
    }

    for (auto& t : writers){
        t.join();
    }
    for (auto& t : readers){
        t.join();
    }
}

int main(){
    //test_simulatenous_readers();
    //test_simulatenous_writers();
    //test_starvation();
    test_database();
    return 0;
}