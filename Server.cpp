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
	this->socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->socket_fd < 0)
		throw std::runtime_error("Error: Can not create socket");

	struct sockaddr_in addr = {
			.sin_family = AF_INET,
			.sin_port = htons(port)
	};
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int bind_ret = bind(this->socket_fd, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr));
	if (bind_ret < 0) {
		close(this->socket_fd);
		throw std::runtime_error("Error: Can not bind " + std::to_string(port) + " port");
	}

	int listen_ret = listen(this->socket_fd, 42);
	if (listen_ret < 0) {
		close(this->socket_fd);
		throw std::runtime_error("Error: Can not listen " + std::to_string(port) + "port");
	}

	this->accept_poll_fd = {
			.fd = this->socket_fd,
			.events = POLLIN,
			.revents = 0
	};
}

Server::~Server() {
	for (auto && fd : this->fds) {
		close(fd.fd);
	}
	close(this->socket_fd);
}

int Server::main_loop() {
	std::cout << "[DEBUG] wait a connection" << std::endl;
	char buf[2048];

	while (true) {
		if (poll(&this->accept_poll_fd, 1, 1000) > 0 && this->accept_poll_fd.revents & POLLIN) {
			struct sockaddr_in addr;
			int addr_size = sizeof (addr);
			int accept_ret = accept(this->socket_fd, reinterpret_cast<sockaddr *>(&addr),
									reinterpret_cast<socklen_t *>(&addr_size));
			char str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(addr.sin_addr), str, INET_ADDRSTRLEN);

			if (accept_ret < 0) {
				std::cout << "[DEBUG] ERROR: accept " << accept_ret << std::endl;
				return -1;
			}
			std::cout << "[" << accept_ret << "]: accepted connection from " << str << std::endl;
			struct pollfd con = {
					.fd = accept_ret,
					.events = POLLIN,
					.revents = 0
			};
			this->fds.push_back(con);
			this->accept_poll_fd.revents = 0;
		}



		if (poll(this->fds.data(), this->fds.size(), 1000) < 0)
			break;
		for (auto &fd : fds) {
			if (fd.revents & (POLLHUP | POLLERR)) {
				std::cout << "[" << fd.fd << "]: Hang up" << std::endl;
				continue;
			}
			if (fd.revents & POLLIN) {
				int recv_ret = recv(fd.fd, buf, 2047, 0);
				if (recv_ret < 0) {
					std::cout << "[ERROR]: recv";
				} else {
					buf[recv_ret] = '\0';
					this->handler(fd.fd, buf, recv_ret);
				}
			}
			fd.revents = 0;
		}
		for (auto fd = this->fds.rbegin(); fd != this->fds.rend(); fd++) {
			if (fd->revents & (POLLHUP | POLLERR)) {
				close(fd->fd);
				this->fds.erase(fd.base());
			}
		}
	}
	return 0;
}

void Server::handler(int, char*, std::size_t) {}

