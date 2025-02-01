#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "Server.hpp"
#include "ConfigurationParser.hpp"
#include "ClientHandler.hpp"
#include <sys/epoll.h>
#include <fcntl.h>
#include <exception>
#include <map>
#include <vector>

class Cluster {
public:
    Cluster(Configurations &config);
    ~Cluster();

    void run();

private:
    Configurations &config;
    int epoll_fd;
    std::vector<Server*> servers;
    std::map<int, ClientHandler*> clientsZone;
    std::map<int, Server*> server_fd_to_server;
    std::map<int, int> client_to_server;

    void createEpoll();
    void initializeServers();
    void addSocketToEpoll(int fd);

    void processEvents(struct epoll_event *events);
    void acceptConnections(int serverSocket);
    void handleClientEvent(int fd, uint32_t eventsData);
    void handleReadEvent(ClientHandler* client);
    void handleWriteEvent(ClientHandler* client);
    void cleanupClient(std::map<int, ClientHandler*>::iterator it);

    bool isServerFd(int fd) const;
    Server& getServerByClientFd(int fd);
    Server& getServerByFd(int fd);

    void cleanupSocket(int fd);
    void cleanup();
};

#endif // CLUSTER_HPP