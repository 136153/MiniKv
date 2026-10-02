cmake_minimum_required(VERSION 3.16)
project(MiNiKv)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
    -Wall
    -Wextra
    
)
add_executable(mini-kv
    src/main.cpp
    src/kv_store.cpp
    src/server.cpp
    src/thread_pool.cpp
)

