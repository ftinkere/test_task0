//
// Created by ftinkere on 30.03.2022.
//

#ifndef TEST_TASK_PROXYSERVER_HPP
#define TEST_TASK_PROXYSERVER_HPP


#include "Server.hpp"

class ProxyServer : protected Server {
protected:
	int							destination_fd;
	int							log_fd;

	void proxy(char* data, size_t len);

public:
	ProxyServer(int listen_port, char* destination_addr, int destination_port);
	ProxyServer(int listen_port, char* destination_addr, int destination_port, char* log_file);
	virtual ~ProxyServer();

	int main_loop();
};


#endif //TEST_TASK_PROXYSERVER_HPP
