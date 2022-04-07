//
// Created by ftinkere on 30.03.2022.
//

#ifndef TEST_TASK_PROXYSERVER_HPP
#define TEST_TASK_PROXYSERVER_HPP

#include <arpa/inet.h>
#include <poll.h>
#include <map>
#include <vector>

class ProxyServer {
protected:
	std::vector<struct pollfd> conns_in;
	std::vector<struct pollfd> conns_out;

//	std::vector<struct pollfd>	dest_fds;
	int							log_fd;
	int							socket_fd;
//	std::vector<struct pollfd>	fds;
	struct pollfd				accept_poll_fd;

	struct sockaddr_in destination;

	int accept_handler();
	void handler(struct pollfd& conn_in, struct pollfd& conn_out);

public:
	ProxyServer(int listen_port, char* destination_addr, int destination_port);
	ProxyServer(int listen_port, char* destination_addr, int destination_port, char* log_file);
	virtual ~ProxyServer();

	int main_loop();
};


#endif //TEST_TASK_PROXYSERVER_HPP
