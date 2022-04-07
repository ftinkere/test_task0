cmake_minimum_required(VERSION 3.21)
project(test_task1)

set(CMAKE_CXX_STANDARD 14)

add_executable(proxy main_proxy.cpp ProxyServer.cpp ProxyServer.hpp)
