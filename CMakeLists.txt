cmake_minimum_required(VERSION 3.21)
project(test_task)

set(CMAKE_CXX_STANDARD 14)

add_executable(proxy_server main_proxy.cpp Server.cpp Server.hpp ProxyServer.cpp ProxyServer.hpp)
add_executable(echo_server main_log.cpp Server.cpp Server.hpp EchoServer.cpp EchoServer.hpp)
add_executable(test test.cpp Server.cpp Server.hpp ProxyServer.cpp ProxyServer.hpp EchoServer.cpp EchoServer.hpp)
