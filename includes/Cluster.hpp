#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "Server.hpp"

class Cluster {
public:
    Cluster(const ServerConfiguration& config);
    ~Cluster();
    void run();

private:
    int epoll_fd;
    ServerConfiguration config;
    std::vector<Server*> servers;
    std::map<int, int> client_to_server;

    void createEpoll();
    void addSocketToEpoll(int fd);
    void handleEvents(struct epoll_event* events, int numEvents);
    void acceptConnections(int serverSocket);
    void handleExistingConnection(int clientSocket, uint32_t events);
    bool isServerFd(int fd);
    Server& getServerByFd(int fd);
    Server& getServerByClientFd(int fd);
    void cleanup();
};

#endif // CLUSTER_HPP