#ifndef SERVER_HPP
#define SERVER_HPP

#include "ConfigurationParser.hpp"

class Server: public ConfigurationParser
{
public:
    Server(ServerConfiguration& config);
    void run();

private:
    int socket;
    ServerConfiguration config;
    std::vector<int> sockets;
    std::map<int, std::string> fd_to_port;
    int epoll_fd;

    void start();
    struct addrinfo* serverInfo(const std::string& port);
    void bindSocket(struct addrinfo* serverInfo, const std::string& port);
    void listenSocket(int socketfd, const std::string& port);
    void handleClient(int client_fd);
    void serveErrorPage(int client_fd, const std::string& error_code);
};

#endif