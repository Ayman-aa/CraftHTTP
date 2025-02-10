/* -- cluster.hpp --*/

#ifndef CLUSTER_HPP
# define CLUSTER_HPP

#include "server.hpp"
#include "requestParser.hpp"

class Cluster {
	public:
		Cluster(Configuration& config);
		~Cluster();
		void run();

	private:
		Configuration config;
		int epoll_fd;
		vector<Server*> servers;
		map<int, int> client_to_server;
		map<int, Server*> server_fd_to_server;

		void createEpoll();
		void initializeServers();
		void addSocketToEpoll(int fd);
		void processEvents(struct epoll_event *events);
		void handleEvents(struct epoll_event* events, int numEvents);
		void acceptConnections(int serverSocket);
		//void handleClient(int client_fd, uint32_t events);
		void handleClient(int client_fd);
		void serverErrorPage(int client_fd, int error_page, Server* server);
		bool isServerFd(int fd) const;
		void cleanup();
};

#endif
