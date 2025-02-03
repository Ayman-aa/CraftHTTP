/* -- cluster.hpp -- */

#include "../includes/cluster.hpp"
#include "../includes/server.hpp"
using namespace std;

static const int MAX_EVENTS = 1024;

Cluster::Cluster(Configuration& config): config(config), epoll_fd(-1)
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
			Server *server = new Server(serversConfigs[i]);
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

void Cluster::addSocketToEpoll(int fd) { /* TODO: we don't need to say it anymore */
	struct epoll_event event;
	event.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR;
	event.data.fd = fd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
		throw std::runtime_error("Error adding socket fd to epoll");
}

void Cluster::run() {
	struct epoll_event events[MAX_EVENTS];

	while (true) {
		try {
			processEvents(events);
		} catch (const std::exception& e) {
			std::cerr << "Error in event loop: " << e.what() << std::endl;
		}
	}
}

void Cluster::processEvents(struct epoll_event *events) {
	int numEvents = epoll_wait(epoll_fd, events, MAX_EVENTS, -1); /* TODO: again, rak 3aref ash kain */
	if (numEvents == -1) 
	{
		if (errno == EINTR) return;
		throw std::runtime_error("Error on epoll_wait(): " + std::string(strerror(errno)));
	}
	for (int i = 0; i < numEvents; i++)
	{
		int eventFD = events[i].data.fd; /* TODO: bayna ashaybi */
		try {
			if (isServerFd(eventFD)) acceptConnections(eventFD);
			else handleClient(eventFD);
		} catch (const std::exception& e) {
			std::cerr << "Error handling event for FD: " << eventFD << ": " << e.what() << std::endl;
		}
	}
}

bool Cluster::isServerFd(int fd) const {
	return server_fd_to_server.find(fd) != server_fd_to_server.end();
} 

void Cluster::acceptConnections(int serverSocket)
{
	int clientSocket = accept(serverSocket, NULL, NULL);
	if (clientSocket == -1) throw std::runtime_error("Error accepting connection: " + std::string(strerror(errno)));

	try {
		addSocketToEpoll(clientSocket);
		client_to_server[clientSocket] = serverSocket;
		std::cout << "Server: Accepted new connection on socket: " << clientSocket << std::endl;
	} catch (const std::exception& e) {
		close(clientSocket);
		std::cerr << e.what() << std::endl;
		std::runtime_error("Accepting connection failed");
	}
}

void Cluster::handleClient(int client_fd) {
	char buffer[1024] = {0};

	int bytes_received = recv(client_fd, buffer, sizeof(buffer) -1, 0);
	if (bytes_received < 0)
	{
		std::cerr << "Error reading from client" << std::endl;
		close(client_fd);
		return;
	}
	std::cout << "Received request: \n" << buffer << std::endl;
	const char* response = "HTTP/1.1 200 OK\r\n"
                          "Content-Length: 13\r\n"
                          "\r\n"
                          "Hello World!\n";
	send(client_fd, response, strlen(response), 0);
	close(client_fd);
}

void Cluster::cleanup(){
	std::cout << "wayeh" << std::endl;
}

Cluster::~Cluster(){}
