#ifndef SERVER_HPP
#define SERVER_HPP

#include "ServerConfiguration.hpp"

class Server {
public:
    Server(ServerConfiguration& config);
    ~Server();
    int getSocket() const;
    const std::map<int, std::string>& getFdToPort() const;

private:
    int socket;
    ServerConfiguration config;
    std::vector<int> sockets;
    std::map<int, std::string> fd_to_port;

    struct addrinfo* serverInfo(const std::string& port);
    void bindSocket(struct addrinfo* serverInfo, const std::string& port);
    void listenSocket(int socketfd, const std::string& port);
    void createSockets();
    void cleanup();
};

#endif // SERVER_HPP