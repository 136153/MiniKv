#pragma once

#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <stdexcept>

using namespace std;

class ThreadPool {
public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();

    bool equeue(function<void()> task);
    void stop();
private:
    void worker_loop();
private:
    vector<thread> workers_;
    mutex mtx_;
    condition_variable cv_;
    queue<function<void()>> tasks_;
    bool stopping = false;
    int max_queue_size_ = 16;
};