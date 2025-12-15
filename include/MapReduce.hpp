#ifndef MAP_REDUCE_HPP
#define MAP_REDUCE_HPP

#include "threadpool.hpp"
#include <iostream>
#include <chrono>

template<typename TInput, typename TOutput>
class MapReduce{
    public:

        MapReduce(size_t nb_threads) : nb_threads_(nb_threads) {
        };
        ~MapReduce(){};

        std::pair<std::size_t, std::size_t> get_range(size_t thread_id, size_t total_size){
            size_t chunk_size = total_size / nb_threads_;
            size_t remainder = total_size % nb_threads_;

            size_t start = chunk_size * thread_id + std::min(thread_id, remainder);
            size_t end;
            if(thread_id < remainder) end = start + chunk_size + 1;
            else end = start + chunk_size;

            return {start, end};
        }

        void map_worker(size_t thread_id, const std::vector<TInput>& data, std::function<TOutput(TInput)> map_fn, std::function<TOutput(TOutput, TOutput)> reduce_fn, TOutput initial_value){
            auto [start,end] = get_range(thread_id, data.size());
            TOutput local_result = initial_value;

            for (size_t i = start; i < end ; i++){
                TOutput mapped = map_fn(data[i]);
                local_result = reduce_fn(local_result, mapped);
            }
            partial_results_[thread_id] = local_result;
        }

        TOutput compute_par(const std::vector<TInput>& data, std::function<TOutput(TInput)> map_fn, std::function<TOutput(TOutput, TOutput)> reduce_fn, TOutput initial_value){
            
            if (data.empty()){
                std::cout << "Data vector is empty.\n";
                return initial_value;
            }
            if(nb_threads_ > data.size()){
                nb_threads_ = data.size();
            }

            partial_results_.resize(nb_threads_);
            std::vector<std::thread> threads;
            for (size_t i = 0 ; i < nb_threads_ ; i++){
                threads.emplace_back([this, i , &data, map_fn, reduce_fn, initial_value] 
                                     {map_worker(i, data, map_fn, reduce_fn, initial_value);});
            }
            for (auto& t : threads){
                if (t.joinable()) t.join();
            }
            TOutput final_result = initial_value;
            for (const auto& partial : partial_results_){
                final_result = reduce_fn(final_result, partial);
            }
            return final_result;
        }

        TOutput compute_seq(const std::vector<TInput>& data, std::function<TOutput(TInput)> map_fn, std::function <TOutput(TOutput, TOutput)> reduce_fn, TOutput initial_value){
            TOutput local_result = initial_value;
            for (auto& d : data){
                TOutput mapped = map_fn(d);
                local_result = reduce_fn(local_result, mapped);
            }
            return local_result;
        }

        void benchmark(const std::vector<TInput>& data, std::function<TOutput(TInput)> map_fn, std::function<TOutput(TOutput,TOutput)> reduce_fn, TOutput initial_value){

            auto start_seq = std::chrono::high_resolution_clock::now();
            TOutput res_seq = compute_seq(data, map_fn, reduce_fn, initial_value);
            auto end_seq = std::chrono::high_resolution_clock::now();

            auto start_par = std::chrono::high_resolution_clock::now();
            TOutput res_par = compute_par(data, map_fn, reduce_fn, initial_value);
            auto end_par = std::chrono::high_resolution_clock::now();

            //if (res_seq = res_par){
            //    std::cout << "Seq :" << res_seq << std::endl;
            //    std::cout << "Par :" << res_par << std::endl;
            //}
            std::cout << "Seq : " << std::chrono::duration_cast<std::chrono::nanoseconds>(end_seq - start_seq).count() << " ns, " << 
            std::chrono::duration_cast<std::chrono::milliseconds>(end_seq - start_seq).count() << " ms." << std::endl;

            std::cout << "Par : " << std::chrono::duration_cast<std::chrono::nanoseconds>(end_par - start_par).count() << " ns, " << 
            std::chrono::duration_cast<std::chrono::milliseconds>(end_par - start_par).count() << " ms." << std::endl;

            std::cout << "SpeedUp factor = " << static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end_seq - start_seq).count()) / std::chrono::duration_cast<std::chrono::milliseconds>(end_par - start_par).count() << std::endl;
        }

    private:
        std::vector<TOutput> partial_results_;
        std::mutex mtx_;
        size_t nb_threads_;
};




#endif