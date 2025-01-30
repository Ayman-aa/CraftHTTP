/* -- server.cpp -- */

#include <../includes/server.hpp>

Server::Server(ServerConfiguration *config): config(config), main_socket(-1) 
{
	try {
		createSockets();
		std::cout << "Server: Initialized successfuly";
	}
	catch (const std::exception& e) {
		std::cerr << "Server: Initialized failed";
		cleanup();
	}
}

void Server::createSockets() {

}
