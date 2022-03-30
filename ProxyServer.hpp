//
// Created by ftinkere on 30.03.2022.
//

#ifndef TEST_TASK_PROXYSERVER_HPP
#define TEST_TASK_PROXYSERVER_HPP


#include "Server.hpp"

class ProxyServer : public Server {
protected:
	std::vector<struct pollfd>	dest_fds;
	int							log_fd;

	int add_dest();
public:
	ProxyServer(int listen_port, char* destination_addr, int destination_port);
	ProxyServer(int listen_port, char* destination_addr, int destination_port, char* log_file);
	virtual ~ProxyServer();

	void handler(int fd, char* buf, std::size_t recv_ret) override;

	void pre_handler() override;
};


#endif //TEST_TASK_PROXYSERVER_HPP
