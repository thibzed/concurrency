#ifndef MAP_REDUCE_HPP
#define MAP_REDUCE_HPP

#include "threadpool.hpp"

template<typename TInput, typename TOutput>
class MapReduce{
    public:

        MapReduce(size_t nb_threads) : nb_threads_(nb_threads) {

        };
        ~MapReduce();

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

    private:
        std::vector<TOutput> partial_results_;
        std::mutex mtx_;
        size_t nb_threads_;
};




#endif