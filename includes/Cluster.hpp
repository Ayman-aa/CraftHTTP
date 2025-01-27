#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "Server.hpp"
#include "ConfigurationParser.hpp"
#include "ClientHandler.hpp"
#include <sys/epoll.h>
#include <fcntl.h>
#include <exception>

class Cluster {
public:
    // Constructor and Destructor
    Cluster(Configurations& config);
    ~Cluster();

    // Main server loop
    void run();

private:
    // Member variables
    Configurations &config;
    int epoll_fd;
    std::vector<Server*> servers;
    std::map<int, ClientHandler*> clientsZone;
    std::map<int, Server*> server_fd_to_server;
    std::map<int, int> client_to_server;

    // Initialization methods
    void createEpoll();
    void initializeServers();
    void addSocketToEpoll(int fd);

    // Event handling methods
    void processEvents(struct epoll_event* events);
    void acceptConnections(int serverSocket);
   // void handleClient(int client_fd);
    //void serveErrorPage(int client_fd, int error_code, Server* server);
    void handleExistingConnection(int eventFd, uint32_t eventsData);
    // Helper methods
    bool isServerFd(int fd) const;
    void cleanupSocket(int fd);
    void cleanup();
	Server& getServerByClientFd(int fd);
	Server& getServerByFd(int fd);
};

#endif // CLUSTER_HPP