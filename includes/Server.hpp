#ifndef SERVER_HPP
#define SERVER_HPP

#include "ServerConfiguration.hpp"
#include <vector>
#include <map>
#include <string>
#include <netdb.h>

class Server {
public:
    Server(ServerConfiguration& config);
    ~Server();
    int getSocket() const;
    const std::map<int, std::string>& getFdToPort() const;
    const ServerConfiguration& getConfig() const;
    const std::vector<int>& getSockets() const;

private:
    int main_socket; // Renamed from 'socket' to 'main_socket'
    ServerConfiguration config;
    std::vector<int> sockets;
    std::map<int, std::string> fd_to_port;

    struct addrinfo* serverInfo(const std::string& port);
    void bindSocket(int sockfd, struct addrinfo* serverInfo, const std::string& port);
    void listenSocket(int socketfd, const std::string& port);
    void createSockets();
    void cleanup();
};

#endif // SERVER_HPP