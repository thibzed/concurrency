#include <iostream>

#include <vector>
#include <thread>
#include <mutex>

std::mutex mtx;
double sum = 0.0;

void compute_partial_sum(long long start_point, long long end_point){
    double partial_sum = 0.0;
    for (int i = start_point; i < end_point ; i++){
        partial_sum += 1.0 / i;
    }
    
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Thread : " << std::this_thread::get_id() << "partial sum = " << partial_sum <<
                                                              "; total sum = " << sum + partial_sum << std::endl;
    sum += partial_sum;
}

int main(int argc, char* argv []){
    auto n = std::stoll(argv[1]);
    int n_threads = std::thread::hardware_concurrency();
    //std::cout << "max : " << std::thread::hardware_concurrency() << std::endl;
    long long interval = n / n_threads;

    std::vector<std::thread> threads;

    for (int i = 0 ; i < n_threads ; i++){
        threads.emplace_back(compute_partial_sum, i * interval + 1, (i + 1) * interval + 1);
    }

    for (auto& t : threads){
        t.join();
    }
}
