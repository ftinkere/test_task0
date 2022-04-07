#include <iostream>
#include "ProxyServer.hpp"

/*
 * No check for correctly of destination address not yet being first connection to this server
 */

int main(int argc, char* argv[]) {
	/*
	 * Checking for required arguments
	 * NO CHECK FOR CORRECTLY OF DATA
	 */
	if (argc < 4) {
		std::cout << argv[0] << " <listen port> <dest ip> <dest port> [file to log]" << std::endl;
		return 0;
	}
	try {
		if (argc >= 5) {
			// With log file
			ProxyServer server(std::stoi(argv[1]), argv[2], std::stoi(argv[3]), argv[4]);
			server.main_loop();
		} else {
			// Just log in stout
			ProxyServer server(std::stoi(argv[1]), argv[2], std::stoi(argv[3]));
			server.main_loop();
		}
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
