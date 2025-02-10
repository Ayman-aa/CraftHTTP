/* -- server.cpp -- */

#include "../includes/server.hpp"

using namespace std;

Server::Server(ServerConfiguration *config): config(config), main_socket(-1) {
	try {
		createSockets();
		std::cout << "Server: Initialized successfuly" << std::endl;;
	}
	catch (const std::exception& e) {
		std::cerr << "Server: Initialized failed" << std::endl;
		cleanup();
	}
}

void Server::createSockets() {
	const int LISTEN_BACKLOG = 128;

	for (size_t i = 0; i < config->ports.size(); i++)
	{
		const std::string &port = config->ports[i];
		struct addrinfo* infoList = serverInfo(port); /* Get address info for this port */
		if (!infoList) {
			std::cerr << "Failed to get address info for this port: " + port << std::endl;
			continue;
		}
		bool success = false;
		for (struct addrinfo* info = infoList; info != NULL; info = info->ai_next)
		{
			int newSocket = socket(info->ai_family, info->ai_socktype | SOCK_NONBLOCK, info->ai_protocol);
			if (newSocket == -1) continue;

			try {
				setSocketOptions(newSocket);
				bindSocket(newSocket, info, port);
				listenSocket(newSocket, port, LISTEN_BACKLOG);

				sockets.push_back(newSocket);
				fd_to_port[newSocket] = port;

				if (main_socket == -1)
					main_socket = newSocket;

				success = true;
				break;
			}
			catch (const std::exception& e) {
				close(newSocket);
				std::cerr << "Faild to set up socket for port: " << port << ": " << e.what() << std::endl;
			}
		}
		freeaddrinfo(infoList);
		if (!success)
			throw std::runtime_error("Faild to bind to port: " + port + ": " + " (tried all available addresses)");
		if (sockets.empty())
			throw std::runtime_error("No sockets were created succussefuly");
	}
}

struct addrinfo* Server::serverInfo(const string& port) {
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *res;
	int status = getaddrinfo(config->host.c_str(), port.c_str(), &hints, &res);
	
	if (status != 0)
		throw std::runtime_error("getaddrinfo() error: " + std::string(gai_strerror(status)));
	return res;
}

void Server::setSocketOptions(int sockFD) {
	int on = 1;

	if (setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) == -1)
		throw std::runtime_error("setsockopt SO_REUSEADDR error: " + std::string(strerror(errno)));
	if (setsockopt(sockFD, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(int)) == -1)
		throw std::runtime_error("setsockopt SO_KEEPALIVE error: " + std::string(strerror(errno)));
}

void Server::bindSocket(int sockFD, struct addrinfo *info, const std::string &port) {
	if (bind(sockFD, info->ai_addr, info->ai_addrlen) == -1)
		throw std::runtime_error("bind() error on port: " + port + ": " + std::string(strerror(errno)));
}

void Server::listenSocket(int sockFD, const string& port, int backlog) {
	if (listen(sockFD, backlog) == -1) 
		throw std::runtime_error("listen() error on port: " + port + ": " + std::string(strerror(errno)));
	std::cout << "Server is listening on port " << port << endl;
}

void Server::cleanup() {
	if (!sockets.empty()) {
		for (size_t i = 0; i < sockets.size(); i++) {
			if (sockets[i] >= 0)
				close(sockets[i]);
		}
		sockets.clear();
	}
	if (!fd_to_port.empty())
		fd_to_port.clear();
	if (main_socket > -1)
		main_socket = -1;
}

Server::~Server() {
	cleanup();
	std::cout << "Server: Cleaned up resources" << std::endl;
}

int Server::getSocket() const { return main_socket; }
const map<int, string>& Server::getFdToPort() const { return fd_to_port; }
//const ServerConfiguration& Server::getConfig() const { return config; }
const vector<int>& Server::getSockets() const { return sockets; }
