//
// Created by ftinkere on 30.03.2022.
//

#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <csignal>
#include <linux/if_ether.h>
#include <fcntl.h>
#include "ProxyServer.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

ProxyServer::ProxyServer(int listen_port, char* destination_addr, int destination_port): Server(listen_port), log_fd(-1) {
		this->destination_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->destination_fd < 0) {
		perror("socket");
		throw std::runtime_error("Error: Can not create socket");
	}

	struct sockaddr_in destinaton_addr = {
			.sin_family = AF_INET,
			.sin_port = htons(destination_port)
	};
	int pton_ret = inet_pton(AF_INET, destination_addr, &destinaton_addr.sin_addr);
	if (pton_ret <= 0) {
		close(this->destination_fd);
		throw std::runtime_error("Error: Destination address");
	}
	if (connect(this->destination_fd, reinterpret_cast<const sockaddr *>(&destinaton_addr), sizeof (destinaton_addr)) < 0) {
		close(this->destination_fd);
		perror("connect");
		throw std::runtime_error("Error: Can not connect");
	}
}

ProxyServer::ProxyServer(int listen_port, char *destination_addr, int destination_port, char *log_file) : ProxyServer(listen_port, destination_addr, destination_port) {
	this->log_fd = open(log_file, O_TRUNC | O_RDWR | O_CREAT);
}

ProxyServer::~ProxyServer() {
	close(log_fd);
}

int ProxyServer::main_loop() {
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
			send(accept_ret, "Hello\n", 7, 0);
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
					proxy(buf, recv_ret);
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

void ProxyServer::proxy(char *data, size_t len) {
	send(destination_fd, data, len, 0);
	if (log_fd >= 0) {
		send(log_fd, data, len, 0);
	} else {
		write(1, data, len);
	}
}



#pragma clang diagnostic pop