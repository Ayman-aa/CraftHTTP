/* -- cluster.hpp --*/

#ifndef CLUSTER_HPP
# define CLUSTER_HPP

class Cluster {
	public:
		cluster(Configuration& config);
		~cluster();
		void run();

	private:
		int epoll_fd;
		Configuration config;
		Vector<Server*> servers;
		map<int, int> client_to_server;
		map(int, Server*) server_fd_to_server;

		void createEpoll();
		void addSocketToEpoll(int fd);
		void handleEvents(struct epoll_event* events, int numEvents);
		void acceptConnections(int serverSocket);
		void handleClient(int client_fd, uint32_t events);
		void serverErrorPage(int client_fd, int error_page, Server* server);
		bool isServerFd(int fd);
		void cleanup();
};

#endif
