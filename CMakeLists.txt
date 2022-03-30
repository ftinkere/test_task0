cmake_minimum_required(VERSION 3.21)
project(test_task)

set(CMAKE_CXX_STANDARD 14)

add_executable(proxy main_proxy.cpp Server.cpp Server.hpp)
add_executable(log_server main_server.cpp)
