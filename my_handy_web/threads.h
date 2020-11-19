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

    template <class T>
    class SafeQueue : private std::mutex, private non_copyable {
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
        std::list<T> items_;
        std::condition_variable ready_;
        size_t capacity_;
        std::atomic<bool> exit_;

        void wait_ready(std::unique_lock<std::mutex>& lock, int waitMs);
    };


    class ThreadPool {

    };
}


#endif //WEB_SERVER_THREADS_H
