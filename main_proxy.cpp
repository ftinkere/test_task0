#include <iostream>
#include "ProxyServer.hpp"

int main(int argc, char* argv[]) {
	if (argc < 4) {
		std::cout << argv[0] << " <listen port> <dest ip> <dest port> [file to log]" << std::endl;
		return 0;
	}
	if (argc == 4) {
		ProxyServer server(std::stoi(argv[1]), argv[2], std::stoi(argv[3]));
		server.main_loop();
	} else {
		ProxyServer server(std::stoi(argv[1]), argv[2], std::stoi(argv[3]));
		server.main_loop();
	}
	return 0;
}
