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

ProxyServer::ProxyServer(int listen_port, char* destination_addr, int destination_port): Server(listen_port), log_fd(-1) {}

ProxyServer::ProxyServer(int listen_port, char *destination_addr, int destination_port, char *log_file) : ProxyServer(listen_port, destination_addr, destination_port) {
	this->log_fd = open(log_file, O_TRUNC | O_RDWR | O_CREAT);
}

ProxyServer::~ProxyServer() {
	close(log_fd);
}


//{
//this->destination_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//if (this->destination_fd < 0) {
//perror("socket");
//throw std::runtime_error("Error: Can not create socket");
//}
//
//struct sockaddr_in destinaton_addr = {
//		.sin_family = AF_INET,
//		.sin_port = htons(destination_port)
//};
//int pton_ret = inet_pton(AF_INET, destination_addr, &destinaton_addr.sin_addr);
//if (pton_ret <= 0) {
//close(this->destination_fd);
//throw std::runtime_error("Error: Destination address");
//}
//if (connect(this->destination_fd, reinterpret_cast<const sockaddr *>(&destinaton_addr), sizeof (destinaton_addr)) < 0) {
//close(this->destination_fd);
//perror("connect");
//throw std::runtime_error("Error: Can not connect");
//}
//}

void ProxyServer::handler(int fd, char* buf, std::size_t recv_ret) {
//	send(destination_fd, buf, recv_ret, 0);
	if (log_fd >= 0) {
		send(log_fd, buf, recv_ret, 0);
	} else {
		write(1, buf, recv_ret);
	}
}

int ProxyServer::add_dest() {
	int dest_fd;

	dest_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (dest_fd < 0) {
		perror("socket");
		throw std::runtime_error("Error: Can not create socket");
	}

	struct sockaddr_in destinaton_addr = {
			.sin_family = AF_INET,
			.sin_port = htons(dest_fd)
	};
	int pton_ret = inet_pton(AF_INET, reinterpret_cast<const char *>(dest_fd), &destinaton_addr.sin_addr);
	if (pton_ret <= 0) {
		close(dest_fd);
		throw std::runtime_error("Error: Destination address");
	}
	if (connect(dest_fd, reinterpret_cast<const sockaddr *>(&destinaton_addr), sizeof (destinaton_addr)) < 0) {
		close(dest_fd);
		perror("connect");
		throw std::runtime_error("Error: Can not connect");
	}

	struct pollfd dest = {
			.fd = dest_fd,
			.events = POLLIN,
			.revents = 0
	};
	this->dest_fds.push_back(dest);
	return dest_fd;
}

void ProxyServer::pre_handler() {
	Server::pre_handler();
}

#pragma clang diagnostic pop