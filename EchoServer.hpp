//
// Created by ftinkere on 30.03.2022.
//

#ifndef TEST_TASK_ECHOSERVER_HPP
#define TEST_TASK_ECHOSERVER_HPP


#include <fstream>
#include "Server.hpp"

class EchoServer : public Server {
public:
	EchoServer(int port);
	virtual ~EchoServer() = default;

protected:
	void handler(int fd, char *buf, std::size_t recv_ret) override;
};


#endif //TEST_TASK_ECHOSERVER_HPP
