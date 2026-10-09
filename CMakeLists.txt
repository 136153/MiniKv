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

# =========================
# GoogleTest
# =========================

enable_testing()

find_package(GTest REQUIRED)

add_executable(kv_store_test
    test/kv_store_test.cpp
    src/kv_store.cpp
)

target_include_directories(kv_store_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(kv_store_test
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(kv_store_test)