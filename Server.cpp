//
// Created by ftinkere on 30.03.2022.
//

#include "Server.hpp"
#include <fcntl.h>
#include <sys/socket.h>
#include <stdexcept>
#include <netinet/in.h>


Server::Server(int port) {
	this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->socket_fd < 0)
		throw std::runtime_error("Error: Can not create socket");

	struct sockaddr_in addr = {
			.sin_family = AF_INET,
			.sin_port = htons(port),
			.sin_addr.s_addr = htonl(INADDR_ANY)
	};

	int bind_ret = bind(this->socket_fd, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr));
	if (bind_ret < 0)
		throw std::runtime_error("Error: Can not bind " + std::to_string(port) + "port");

	int listen_ret = listen(this->socket_fd, 42);
	if (listen_ret < 0)
		throw std::runtime_error("Error: Can not listen " + std::to_string(port) + "port");


}

Server::~Server() {
	// TODO
}
