//
// Created by ftinkere on 30.03.2022.
//

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <csignal>
#include "EchoServer.hpp"

EchoServer::EchoServer(int port) : Server(port) {}

void EchoServer::handler(int fd, char *buf, std::size_t recv_ret) {
	std::cout << buf;
	send(fd, buf, recv_ret, 0);
}



