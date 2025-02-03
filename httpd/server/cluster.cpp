#include <../includes/cluster.hpp>
#include <../includes/server.hpp>

static const int MAX_EVENTS = 1024;

Cluster:Cluster(Configurations& config): config(config), epoll_fd(-1)
{
	try {
		createEpoll();
		initializeServers();
	} catch (const std::exception& e) {
		cleanup();
		std::cerr << "Cluster initialization faild: " << e.what() << std::endl;
	}
}

void Cluster::createEpoll() {
	epoll_fd = epoll_create1(0); /* TODO: Rak 3aref ash khassek hna */
	if (epoll_fd == -1)
		throw std::runtime_error("Error creating epoll instance: " + std::string(strerror(errno)));
}

void Cluster::initializeServers() {
	std::vector<ServerConfiguration*> serversConfigs = config.servers;

	for (long unsigned int i = 0; i < serversConfigs.size(); i++) {
		try {
			Server *server = new Server(serverConfigs[i]);
			const std::vector<int> &serverSockets = server->getSockets();

			if (serverSockets.empty())
			{
				delete server;
				throw std::runtime_error("Server: no valid sockets");
			}
			servers.push_back(server);
			for (size_t j = 0; j < serverSockets.size(); j++)
			{
				addSocketToEpoll(serverSockets[j]);
				server_fd_to_server[serverSockets[j]] = server;
			}
		} catch (const std::exception& e) {
			std::cerr << e.what() << std::endl;
			throw std::runtime_error("Server: Initialization failed");
		}
	}
}

void addSocketToEpoll(int fd) {
	struct epoll_event event;
	event.events = EPOLLIN | EPOLLOUT | EPOLLHUB | EPOLLERR;
	even.data.fd = fd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
		throw std::runtime_error("Error adding socket fd to epoll");
}

void cluster::run() {
	struct epoll_event events[MAX_EVENTS];
}
