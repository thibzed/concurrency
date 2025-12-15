#include "../include/MapReduce.hpp"
#include <iostream>

int main(){
    size_t nb_threads = 4;
    MapReduce <int,int> mr(nb_threads);

    std::cout << "10 elem, 4 threads :" << std::endl;
    for (size_t i = 0 ; i < nb_threads ; i++){
        auto [start,end] = mr.get_range(i,10);
        std::cout << "Thread " << i << " : [" << start << ", " << end << ")" << std::endl;
    }

    std::cout << "7 elem, 4 threads :" << std::endl;
    for (size_t i = 0 ; i < nb_threads ; i++){
        auto [start,end] = mr.get_range(i,7);
        std::cout << "Thread " << i << " : [" << start << ", " << end << ")" << std::endl;
    }

    return 0;
}