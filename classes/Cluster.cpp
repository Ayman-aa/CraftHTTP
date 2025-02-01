#include "../includes/Cluster.hpp"

static const int MAX_EVENTS = 1024;

Cluster::Cluster(Configurations &config) : config(config), epoll_fd(-1)
{
	try
	{
		createEpoll();
		initializeServers();
	} catch(const std::exception &e){
		cleanup();
		std::cerr << "Cluster initialization failed: " << e.what() << std::endl; 
	}
}

void Cluster::createEpoll()
{
	epoll_fd = epoll_create(1024);
	if(epoll_fd == -1)
		throw std::runtime_error("Error creating epoll instance: " + std::string(strerror(errno)));
}

void Cluster::initializeServers()
{
	std::vector<ServerConfiguration*> serversConfigurations = config.servers;

	for(long unsigned int i = 0; i < serversConfigurations.size(); i++)
	{
		try
		{
			Server *server = new Server(serversConfigurations[i]);
			const std::vector<int> &serverSockets = server->getSockets();

			if(serverSockets.empty())
			{
				delete server;
				throw std::runtime_error("Server: no valid sockets");
			}

			servers.push_back(server);
			for(size_t j = 0; j < serverSockets.size(); j++)
			{
				addSocketToEpoll(serverSockets[j]);
				server_fd_to_server[serverSockets[j]] = server;
			}
		} catch (const std::exception &e){
			std::cerr << e.what() << std::endl;
			throw std::runtime_error("Server: Initialization failed");
		}
	}
}

void Cluster::addSocketToEpoll(int fd)
{
	struct epoll_event event;
	event.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR;
	event.data.fd = fd;

	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
		throw std::runtime_error("Error adding socket to epoll: " + std::string(strerror(errno)));
}

void Cluster::run()
{
	struct epoll_event events[MAX_EVENTS];

	while(true)
	{
		try
		{
			processEvents(events);
		} catch(const std::exception &e){
			std::cerr << "Error in event loop: " << e.what() << std::endl;
		}
	}
}

void Cluster::processEvents(struct epoll_event *events)
{
	int numEvents = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
	if(numEvents == -1)
	{
		if(errno == EINTR)
			return;
		throw std::runtime_error("Error in epoll_wait: " + std::string(strerror(errno)));
	}

	for (int i = 0; i < numEvents; ++i)
	{
		int eventFd = events[i].data.fd;
		try
		{
			if (isServerFd(eventFd))
				acceptConnections(eventFd);
			else
				handleClientEvent(eventFd, events[i].events);
		} catch(const std::exception &e){
			std::cerr << "Error handling event for fd: " << eventFd << ": " << e.what() << std::endl;
			cleanupSocket(eventFd);
		}
	}
}

void Cluster::acceptConnections(int serverSocket)
{
	int clientSocket = accept(serverSocket, NULL, NULL);
	if(clientSocket == -1)
		throw std::runtime_error("Error accepting connection: " + std::string(strerror(errno)));
	try
	{
		addSocketToEpoll(clientSocket);
		client_to_server[clientSocket] = serverSocket;
		getServerByFd(serverSocket).connectedClients.insert(clientSocket);
		std::cout << "Server: Accepted new connection on socket " << clientSocket << std::endl;
	} catch(const std::exception &e){
		close(clientSocket);
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("Accepting connection failed");
	}
}

void Cluster::handleClientEvent(int fd, uint32_t eventsData)
{
	// Create or retrieve client
	std::map<int, ClientHandler*>::iterator it = clientsZone.find(fd);
	if (it == clientsZone.end())
		it = clientsZone.insert(std::make_pair(fd, new ClientHandler(fd, epoll_fd, getServerByClientFd(fd).getConfig(), config))).first;
	ClientHandler* client = it->second;

	if ((eventsData & EPOLLIN) && client->status == Receiving)
		handleReadEvent(client);
	else if (eventsData & EPOLLOUT)
		handleWriteEvent(client);

	if (client->status == Closed || (eventsData & EPOLLHUP) || (eventsData & EPOLLERR))
		cleanupClient(it);
}

void Cluster::handleReadEvent(ClientHandler* client) {
	client->readyToReceive();
	client->lastReceive = std::time(0);
}

void Cluster::handleWriteEvent(ClientHandler* client) {
	client->readyToSend();
}

void Cluster::cleanupClient(std::map<int, ClientHandler*>::iterator it)
{
	std::cout << "Client closed connection" << std::endl;
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, it->second->clientFd, NULL);
	close(it->second->clientFd);
	if (it->second->monitorCGI)
		kill(it->second->CGIpid, SIGKILL);
	getServerByClientFd(it->second->clientFd).connectedClients.erase(it->first);
	delete it->second;
	clientsZone.erase(it);
}

bool Cluster::isServerFd(int fd) const
{
	return server_fd_to_server.find(fd) != server_fd_to_server.end();
}

Server& Cluster::getServerByClientFd(int fd)
{
	std::map<int, int>::iterator it = client_to_server.find(fd);
	if (it != client_to_server.end()) {
		int serverFd = it->second;
		return getServerByFd(serverFd);
	}

	throw std::runtime_error("getServerByClientFd: Server not found for given file descriptor");
}

Server& Cluster::getServerByFd(int fd)
{
	std::map<int, Server*>::iterator it = server_fd_to_server.find(fd);
	if (it != server_fd_to_server.end()) {
		return *(it->second);
	}

	throw std::runtime_error("getServerByFd: Server not found for given file descriptor");
}

void Cluster::cleanupSocket(int fd)
{
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
	close(fd);
	client_to_server.erase(fd);
	std::cout << "Cleaned up socket: " << fd << std::endl;
}

void Cluster::cleanup()
{
	std::vector<Server*>::iterator it;
	for (it = servers.begin(); it != servers.end(); ++it)
		delete *it;

	servers.clear();
	
	std::map<int, int>::iterator clientIt;
	for (clientIt = client_to_server.begin(); clientIt != client_to_server.end(); ++clientIt) 
		close(clientIt->first);

	client_to_server.clear();
	server_fd_to_server.clear();

	if (epoll_fd != -1)
	{
		close(epoll_fd);
		epoll_fd = -1;
	}
	std::cout << "Cluster cleaned up" << std::endl;
}
Cluster::~Cluster() { cleanup(); }