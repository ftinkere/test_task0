//
// Created by ftinkere on 30.03.2022.
//

#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <csignal>
#include <fcntl.h>
#include <sstream>
#include <unistd.h>
#include <sys/poll.h>
#include "ProxyServer.hpp"

ProxyServer::ProxyServer(int listen_port, char* destination_addr, int destination_port): log_fd(-1) {
	// Create tcp socket
	this->socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->socket_fd < 0)
		throw std::runtime_error("Error: Can not create socket");

	struct sockaddr_in addr = {
			.sin_family = AF_INET,
			.sin_port = htons(listen_port)
	};
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Bind it to given port
	int bind_ret = bind(this->socket_fd, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr));
	if (bind_ret < 0) {
		close(this->socket_fd);
		throw std::runtime_error("Error: Can not bind " + std::to_string(listen_port) + " port");
	}

	// Start listening max of 42 connections
	int listen_ret = listen(this->socket_fd, 42);
	if (listen_ret < 0) {
		close(this->socket_fd);
		throw std::runtime_error("Error: Can not listen " + std::to_string(listen_port) + "port");
	}

	// Initialize struct for checking incoming connections
	this->accept_poll_fd = {
			.fd = this->socket_fd,
			.events = POLLIN,
			.revents = 0
	};

	// Initialize struct with destination server address
	this->destination.sin_family = AF_INET;
	this->destination.sin_port = htons(destination_port);
	inet_pton(AF_INET, reinterpret_cast<const char *>(destination_addr), &this->destination.sin_addr);
}

ProxyServer::ProxyServer(int listen_port, char *destination_addr, int destination_port, char *log_file) : ProxyServer(listen_port, destination_addr, destination_port) {
	// Open file to log in it
	this->log_fd = open(log_file, O_TRUNC | O_RDWR | O_CREAT);
}

ProxyServer::~ProxyServer() {
	// Close log file
	if (log_fd > 0)
		close(log_fd);
	// Close incoming connections
	for (auto && conn : this->conns_in) {
		close(conn.fd);
	}
	// Close outcoming connections
	for (auto && conn : this->conns_out) {
		close(conn.fd);
	}
	// Close socket
	close(this->socket_fd);
}


bool work = true;
void falsify_work(int) {
	work = false;
}

// Callable function with main logic
int ProxyServer::main_loop() {
	std::cout << "[DEBUG] wait a connection" << std::endl;

	// Set stoppable handler ti signals // BUG: IF THROWN EXCEPTION, SIGNAL HANDLERS NOT RETURNED TO DEFAULT
	signal(SIGINT, &falsify_work);
	signal(SIGTERM, &falsify_work);
	while (work) {
		// Test incoming connection and accept if it is
		if (poll(&this->accept_poll_fd, 1, 50) > 0 && this->accept_poll_fd.revents & POLLIN) {
			// Call accept handler
			this->accept_handler();
			this->accept_poll_fd.revents = 0;
		}

		// Test incoming messages from clients
		if (poll(this->conns_in.data(), this->conns_in.size(), 50) < 0)
			break;
		for (size_t i = 0; i < this->conns_in.size(); i++) {
			auto& conn_in = this->conns_in[i];
			auto& conn_out = this->conns_out[i];

			// Skipped connection if closed or has error
			if ((conn_in.revents & (POLLHUP | POLLERR)) || (conn_out.revents & (POLLHUP | POLLERR))) {
				std::cout << "[" << conn_in.fd << " > " << conn_out.fd << "]: Hang up" << std::endl;
				continue;
			}
			// Call handler with logic to one incoming connection
			this->handler(conn_in, conn_out);
		}
		// Test messages from destination server to clients
		if (poll(this->conns_out.data(), this->conns_out.size(), 50) < 0)
			break;
		for (size_t i = 0; i < this->conns_out.size(); i++) {
			auto& conn_in = this->conns_in[i];
			auto& conn_out = this->conns_out[i];

			if ((conn_in.revents & (POLLHUP | POLLERR)) || (conn_out.revents & (POLLHUP | POLLERR))) {
				continue;
			}
			// Call same handler as above, but with reverse arguments
			this->handler(conn_out, conn_in);
		}

		// Close and erase hanged up and errored connections
		auto conn_in = this->conns_in.rbegin();
		auto conn_out = this->conns_out.rbegin();
		for (; conn_in != this->conns_in.rend() && conn_out != this->conns_out.rend(); conn_in++, conn_out++) {
			if ((conn_in->revents & (POLLHUP | POLLERR)) || (conn_out->revents & (POLLHUP | POLLERR))) {
				close(conn_in->fd);
				close(conn_out->fd);
				this->conns_in.erase(conn_in.base() - 1);
				this->conns_out.erase(conn_out.base() - 1);
			}
		}
	}
	// Retuen defaults signal handlers
	signal(SIGINT, SIG_DFL);
	signal(SIGTERM, SIG_DFL);
	return 0;
}

void ProxyServer::handler(struct pollfd& conn_in, struct pollfd& conn_out) {
	char buf[2048];

	if (conn_in.revents & POLLIN) {
		// Recieve from incoming connection data and write to buf
		int recv_ret = recv(conn_in.fd, buf, 2047, 0);
		if (recv_ret < 0) {
			std::cout << "[ERROR]: recv";
		} else {
			buf[recv_ret] = '\0';
			// Send data from buf to destination server
			send(conn_out.fd, buf, recv_ret, 0);

			// Log this data
			if (this->log_fd >= 0) {
				std::stringstream tmp;
				tmp << "[" << conn_in.fd << " > " << conn_out.fd << "]: ";
				send(this->log_fd, tmp.str().c_str(), tmp.str().size(), 0);
				send(this->log_fd, buf, recv_ret, 0);
			} else {
				std::stringstream tmp;
				tmp << "[" << conn_in.fd << " > " << conn_out.fd << "]: ";
				write(1, tmp.str().c_str(), tmp.str().size());
				write(1, buf, recv_ret);
			}
		}
	}
	conn_in.revents = 0;
}

int ProxyServer::accept_handler() {
	struct sockaddr_in addr;
	int addr_size = sizeof (addr);
	// Accept connection
	int in_fd = accept(this->socket_fd, reinterpret_cast<sockaddr *>(&addr),
					   reinterpret_cast<socklen_t *>(&addr_size));
	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(addr.sin_addr), str, INET_ADDRSTRLEN);

	if (in_fd < 0) {
		std::cout << "[DEBUG] ERROR: accept" << std::endl;
		perror("Accept");
		return -1;
	}
	struct pollfd conn_in = {
			.fd = in_fd,
			.events = POLLIN,
			.revents = 0
	};
	// and push it to pool of incoming connection
	this->conns_in.push_back(conn_in);

	// Create client tcp socket for destination server
	int dest_fd;
	dest_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (dest_fd < 0) {
		perror("socket");
		throw std::runtime_error("Error: Can not create socket");
	}
	// Connect to destination server
	if (connect(dest_fd, reinterpret_cast<const sockaddr *>(&this->destination), sizeof (this->destination)) < 0) {
		close(dest_fd);
		perror("connect");
		throw std::runtime_error("Error: Can not connect");
	}

	struct pollfd dest = {
			.fd = dest_fd,
			.events = POLLIN,
			.revents = 0
	};
	// And push it connection to pool of outcoming connections with same id
	this->conns_out.push_back(dest);
	std::cout << "[" << in_fd << " > " << dest_fd << "]: accepted connection from " << str << std::endl;
	return dest_fd;
}
