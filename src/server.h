#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include "kv_store.h"
#include "thread_pool.h"
#include <atomic>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <sys/time.h>
#include <csignal>

using namespace std;


class Server {
public:
    void start(const volatile sig_atomic_t* stop_requested);
    Server(size_t worker_num);

private:
    KVStore store_;
    ThreadPool pool_;
    mutex mtx_;
    atomic<bool> stopping_{false};
    void handle_client(int client_fd);
    string execute_command(const string& line);
};
