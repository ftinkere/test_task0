//
// Created by ftinkere on 30.03.2022.
//

#include "Server.hpp"
#include <fcntl.h>
#include <sys/socket.h>
#include <stdexcept>
#include <netinet/in.h>
#include <csignal>
#include <iostream>
#include <arpa/inet.h>


Server::Server(int port) {

}

Server::~Server() {

}



void Server::handler(int, char*, std::size_t) {}

void Server::post_accept_handler() {
