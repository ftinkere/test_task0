//
// Created by ftinkere on 30.03.2022.
//

#ifndef TEST_TASK_SERVER_HPP
#define TEST_TASK_SERVER_HPP

#include <sys/poll.h>
#include <vector>

class Server {
private:
	int							socket_fd;
	std::vector<struct pollfd>	fds;

public:
	explicit Server(int port);
	virtual ~Server();

	Server(Server const& other) = delete;
	Server& operator=(Server const&) = delete;

};


#endif //TEST_TASK_SERVER_HPP
