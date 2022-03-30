//
// Created by ftinkere on 30.03.2022.
//

#ifndef TEST_TASK_ECHOSERVER_HPP
#define TEST_TASK_ECHOSERVER_HPP


#include <fstream>
#include "Server.hpp"

class EchoServer : protected Server {
public:
	EchoServer(int port);
	virtual ~EchoServer() = default;

	int main_loop() override;
};


#endif //TEST_TASK_ECHOSERVER_HPP
