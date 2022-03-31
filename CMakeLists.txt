cmake_minimum_required(VERSION 3.21)
project(test_task)

set(CMAKE_CXX_STANDARD 14)

add_executable(proxy_server main_proxy.cpp ProxyServer.cpp ProxyServer.hpp)
