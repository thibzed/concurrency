#ifndef READER_WRITER_HPP
#define READER_WRITER_HPP

#include <mutex>
#include <condition_variable>

class RWLock {
    public :
        void read_lock(){
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this]{return !active_writer_bool_ && waiting_writer_counter_ == 0;});
            active_reader_counter_++;
        }
        void read_unlock(){
            std::unique_lock<std::mutex> lock(mtx_);
            active_reader_counter_--;
            if(active_reader_counter_ == 0) cv_.notify_all();
        }
        void write_lock(){
            std::unique_lock<std::mutex> lock(mtx_);
            waiting_writer_counter_++;
            cv_.wait(lock, [this]{return active_reader_counter_ == 0 && !active_writer_bool_;});
            active_writer_bool_ = true;
            waiting_writer_counter_--;
            if (waiting_writer_counter_ == 0) cv_.notify_all();
        }
        void write_unlock(){
            std::unique_lock<std::mutex> lock(mtx_);
            active_writer_bool_ = false;
            cv_.notify_all();
        }

    private:
        int active_reader_counter_ = 0;
        bool active_writer_bool_ = false;
        int waiting_writer_counter_ = 0;
        std::mutex mtx_;
        std::condition_variable cv_;
};

class ReadGuard{
    public:
        ReadGuard(const ReadGuard&) = delete; 
        ReadGuard& operator=(const ReadGuard&) = delete;

        ReadGuard(RWLock& rw) : lock_(rw) {
            lock_.read_lock();
        }
        ~ReadGuard(){
            lock_.read_unlock();
        }
    private:
        RWLock& lock_;
};

class WriteGuard{
    public:
        WriteGuard(const WriteGuard&) = delete;
        WriteGuard& operator=(const WriteGuard&) = delete;

        WriteGuard(RWLock& rw) : lock_(rw) {
            lock_.write_lock();
        }
        ~WriteGuard(){
            lock_.write_unlock();
        }
    private:
        RWLock& lock_;
};

#endif