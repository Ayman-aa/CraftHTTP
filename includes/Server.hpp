#ifndef SERVER_HPP
#define SERVER_HPP

#include "ServerConfiguration.hpp"

class Server {
public:
    Server(ServerConfiguration& config);
    ~Server();
    int getSocket() const;
    const map<int, string>& getFdToPort() const;

private:
    int socket;
    ServerConfiguration config;
    vector<int> sockets;
    map<int, string> fd_to_port;

    struct addrinfo* serverInfo(const string& port);
    void bindSocket(struct addrinfo* serverInfo, const string& port);
    void listenSocket(int socketfd, const string& port);
    void createSockets();
    void cleanup();
};

#endif // SERVER_HPP