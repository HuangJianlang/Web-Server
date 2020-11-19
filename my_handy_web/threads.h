//
// Created by Jianlang Huang on 11/18/20.
//

#ifndef WEB_SERVER_THREADS_H
#define WEB_SERVER_THREADS_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <limits>
#include <list>
#include <mutex>
#include <thread>
#include <vector>
#include "util.h"

namespace my_handy{
    template <typename T>
    class SafeQueue : private non_copyable {
    public:
        static const int wait_infinite = std::numeric_limits<int>::max();
        SafeQueue(size_t capacity = 0) : capacity_(capacity), exit_(false) {}

        bool push(T&& v);
        T pop_wait(int waitMs = wait_infinite);
        bool pop_wait(T* v, int waitMs = wait_infinite);

        size_t size();
        void exit();

        bool exited(){
            return exit_;
        }
    private:
        std::mutex mutex_;
        std::list<T> items_;
        std::condition_variable ready_;
        size_t capacity_;
        std::atomic<bool> exit_;

        void wait_ready(std::unique_lock<std::mutex>& lock, int waitMs);
    };

    using Task = std::function<void()>;
    extern template class SafeQueue<Task>;

    class ThreadPool : private non_copyable{
    private:
        std::vector<std::thread> threads_;
        SafeQueue<Task> tasks_;

    public:
        ThreadPool(int threads, int taskCapacity = 0, bool start = true);
        ~ThreadPool();

        void start();
        ThreadPool &exit(){
            tasks_.exit();
            return *this;
        }

        void join();

        bool addTask(Task&& task);

        bool addTask(Task &task) {
            return addTask(std::move(task));
        }

        size_t taskSize() {
            return tasks_.size();
        }
    };


    //implement for SafeQueue
    template<typename T>
    bool SafeQueue<T>::push(T &&v) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (exit_ || this->size() >= capacity_){
            //RAII
            return false;
        }
        //it is rvalue
        items_.push_back(std::move(v));
        ready_.notify_one();
        return true;
    }

    template<typename T>
    T SafeQueue<T> :: pop_wait(int waitMs) {
        std::unique_lock<std::mutex> lock(mutex_);
        wait_ready(lock, waitMs);
        if (items_.empty()){
            return T();
        }
        T r = std::move(items_.front());
        items_.pop_front();
        return r;
    }

    template<typename T>
    bool SafeQueue<T> :: pop_wait(T *v, int waitMs) {
        std::unique_lock<std::mutex> lock(mutex_);
        wait_ready(lock, waitMs);
        if (items_.empty()){
            return false;
        }
        //non-copyable
        *v = std::move(items_.front());
        items_.pop_front();
        return true;
    }

    template<typename T>
    void SafeQueue<T> :: wait_ready(std::unique_lock<std::mutex> &lock, int waitMs) {
        while(exit_ || !items_.empty()){
            return;
        }
        if (waitMs == wait_infinite){
            ready_.wait(lock, [this](){ return exit_ || !items_.empty();});
        } else if (waitMs > 0){
            auto tp = std::chrono::steady_clock::now() + std::chrono::milliseconds(waitMs);
            while (ready_.wait_until(lock, tp) != std::cv_status::timeout && items_.empty() && !exit_);
        }
    }

    template<typename T>
    size_t SafeQueue<T> :: size() {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        return items_.size();
    }

    template<typename T>
    void SafeQueue<T> :: exit() {
        exit_ = true;
        std::lock_guard<std::mutex> lockGuard(mutex_);
        ready_.notify_all();
    }
}


#endif //WEB_SERVER_THREADS_H
