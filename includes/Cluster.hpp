#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "Server.hpp"
#include "ConfigurationParser.hpp"
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
    Configurations config;
    int epoll_fd;
    vector<Server*> servers;
    map<int, int> client_to_server;
    map<int, Server*> server_fd_to_server;

    // Initialization methods
    void createEpoll();
    void initializeServers();
    void addSocketToEpoll(int fd);

    // Event handling methods
    void processEvents(struct epoll_event* events);
    void acceptConnections(int serverSocket);
    void handleClient(int client_fd);
    void serveErrorPage(int client_fd, int error_code, Server* server);

    // Helper methods
    bool isServerFd(int fd) const;
    void cleanupSocket(int fd);
    void cleanup();

};

#endif // CLUSTER_HPP