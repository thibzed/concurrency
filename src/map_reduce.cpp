#include "../include/MapReduce.hpp"
#include <numeric>

int main(){
    size_t nb_threads = 8;
    MapReduce <int,double> mr(nb_threads);

    //std::cout << "10 elem, 4 threads :" << std::endl;
    //for (size_t i = 0 ; i < nb_threads ; i++){
    //    auto [start,end] = mr.get_range(i,10);
    //    std::cout << "Thread " << i << " : [" << start << ", " << end << "[" << std::endl;
    //}
//
    //std::cout << "7 elem, 4 threads :" << std::endl;
    //for (size_t i = 0 ; i < nb_threads ; i++){
    //    auto [start,end] = mr.get_range(i,7);
    //    std::cout << "Thread " << i << " : [" << start << ", " << end << "[" << std::endl;
    //}

    size_t data_size = 10'000'000;
    std::vector<int> data (data_size);
    std::iota(data.begin(), data.end(), 0);
    auto map_fn = [](int x){return x*x;};
    auto reduce_fn = [](double x, double y){return x + y;};
    //int result = mr.compute_par(data, map_fn, reduce_fn, 0);
    //std::cout << "result : " << result;
    mr.benchmark(data, map_fn, reduce_fn, 0);
    return 0;
}