//
// Created by ftinkere on 30.03.2022.
//

#ifndef TEST_TASK_SERVER_HPP
#define TEST_TASK_SERVER_HPP

#include <sys/poll.h>
#include <vector>

class Server {
protected:


public:

	explicit Server(int port);
	virtual ~Server();

	Server(Server const& other) = delete;
	Server& operator=(Server const&) = delete;

	virtual int main_loop();
	virtual void handler(int fd, char* buf, std::size_t recv_ret);
	virtual void post_accepthandler();
};


#endif //TEST_TASK_SERVER_HPP
