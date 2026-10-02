#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include "kv_store.h"
#include "thread_pool.h"


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

using namespace std;


class Server {
public:
    void start();
    Server(size_t worker_num);

private:
    KVStore store_;
    ThreadPool pool_;
    mutex mtx_;

    void handle_client(int client_fd);
};
