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

void cluster::run() {
	struct epoll_event events[MAX_EVENTS];
}
