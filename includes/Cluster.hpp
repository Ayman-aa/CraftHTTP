#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "Server.hpp"
#include "ServerConfiguration.hpp"

class Cluster {
public:
    Cluster(ServerConfiguration& config);
    ~Cluster();
    void run();

private:
    int epoll_fd;
    ServerConfiguration config;
    Server* server;
    std::map<int, int> client_to_server;

    void createEpoll();
    void addSocketToEpoll(int fd);
    void handleEvents(struct epoll_event* events, int numEvents);
    void acceptConnections(int serverSocket);
    void handleClient(int client_fd);
    void serveErrorPage(int client_fd, const std::string& error_code);
    bool isServerFd(int fd);
    void cleanup();
};

#endif // CLUSTER_HPP