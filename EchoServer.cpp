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

int EchoServer::main_loop() {
	std::cout << "[DEBUG] wait a connection" << std::endl;
	char buf[1024];
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
				int recv_ret = recv(fd.fd, buf, 1023, 0);
				if (recv_ret < 0) {
					std::cout << "[ERROR]: recv";
				} else {
					buf[recv_ret] = '\0';
					std::cout << buf;
					send(fd.fd, buf, recv_ret, 0);
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



