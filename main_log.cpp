#include <iostream>
#include "EchoServer.hpp"

int main(int argc, char *argv[]) {

	if (argc < 2) {
		std::cout << argv[0] << " <port>" << std::endl;
		return 0;
	}

	EchoServer log(std::stoi(argv[1]));
	log.main_loop();

	return 0;
}
