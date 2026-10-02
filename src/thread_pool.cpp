#include "thread_pool.h"
using namespace std;



ThreadPool::ThreadPool(size_t num_threads) {
    if (num_threads == 0) {
        num_threads = 1;
    }

    for (size_t i = 0; i < num_threads; i++) {
        workers_.emplace_back([this] {
            worker_loop();
        });
    }
}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::equeue(function<void()> task) {
    {
    lock_guard<mutex> lock(mtx_);

    if (stopping) {
        throw runtime_error("this pool is stopped");
    }
    tasks_.push(move(task));
    }
    cv_.notify_one();
}

void ThreadPool::worker_loop() {
    while(true) {

        function<void()> task;
    {
        unique_lock<mutex> lock(mtx_);

        cv_.wait(lock, [this]{
            return stopping || !tasks_.empty();
        });

        if (stopping && tasks_.empty()) {
            return;
        }
        task = move(tasks_.front());
        tasks_.pop();
    }
        task();
    }
}

void ThreadPool::stop() {
     {
        lock_guard<mutex> lock(mtx_);

        if (stopping) {
            return;
        }
        stopping = true;
     }

     cv_.notify_all();
     for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
     }
}