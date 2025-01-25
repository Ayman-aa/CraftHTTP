#include "../includes/Server.hpp"

Server::Server(ServerConfiguration *config):main_socket(-1),config(config)
{
	try
	{
		createSockets();
		std::cout << "Server: Initialized successfully" << std::endl;
	} catch(const std::exception &e){
		std::cerr << "Server: Initialization failed" << std::endl;
		cleanup();
	}
}

void Server::createSockets()
{
	const int LISTEN_BACKLOG = 128;

	for (size_t i = 0; i < config.ports.size(); ++i)
	{
		const std::string &port = config.ports[i];
		struct addrinfo *infoList = serverInfo(port);
		if (!infoList)
		{
			std::cerr << "Failed to get address info for port " + port << std::endl;
			continue;
		}

		bool success = false;

		for (struct addrinfo *info = infoList; info != NULL; info = info->ai_next)
		{
			int newSocket = socket(info->ai_family, info->ai_socktype | SOCK_NONBLOCK, info->ai_protocol);
			if (newSocket == -1)
				continue;

			try
			{
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
			catch (const std::exception &e)
			{
				close(newSocket);
				std::cerr << "Failed to set up socket for port " << port << ": " << e.what() << std::endl;
			}
		}

		freeaddrinfo(infoList);
		if (!success)
			throw std::runtime_error("Failed to bind to port " + port + " (tried all available addresses)");
	}
	if (sockets.empty())
		throw std::runtime_error("No sockets were created successfully");
}

struct addrinfo* Server::serverInfo(const std::string &port)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *result;
	int status = getaddrinfo(config.host.c_str(), port.c_str(), &hints, &result);

	if(status != 0)
		throw std::runtime_error("getaddrinfo error: " + std::string(gai_strerror(status)));
	
	return result;
}

void Server::setSocketOptions(int sockFd)
{
	int yes = 1;
	if(setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		throw std::runtime_error("setsockopt SO_REUSEADDR error: " + std::string(strerror(errno)));
	if (setsockopt(sockFd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) == -1)
		throw std::runtime_error("setsockopt SO_KEEPALIVE error: " + std::string(strerror(errno)));
}

void Server::bindSocket(int sockFd, struct addrinfo *info, const std::string &port)
{
	if (bind(sockFd, info->ai_addr, info->ai_addrlen) == -1)
		throw std::runtime_error("bind error on port " + port + ": " + std::string(strerror(errno)));
}

void Server::listenSocket(int sockFd, const string& port, int backlog)
{
	if (listen(sockFd, backlog) == -1)
		throw std::runtime_error("listen error on port " + port + ": " + std::string(strerror(errno)));
	std::cout << "Server is listening on port " << port << std::endl;
}

Server::~Server()
{
	cleanup();
	std::cout << "Server: Cleaned up resources" << std::endl;
}

void Server::cleanup()
{
	//edging on this one
	if(!sockets.empty())
	{
		for(size_t i = 0; i < sockets.size(); ++i)
		{
			if(sockets[i] >= 0)
				close(sockets[i]);
		}
		sockets.clear();
	}
	if(!fd_to_port.empty())
		fd_to_port.clear();
	if(main_socket >= 0)
		main_socket = -1;
}

int Server::getSocket() const { return main_socket; }
const std::map<int, std::string>& Server::getFdToPort() const { return fd_to_port; }
ServerConfiguration& Server::getConfig() { return config;  }
const std::vector<int>& Server::getSockets() const { return sockets;  }
