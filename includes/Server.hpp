#ifndef SERVER_HPP
#define SERVER_HPP

#include "ServerConfiguration.hpp"

class Server {
public:
    Server(ServerConfiguration* config);
    virtual ~Server();
    int getSocket() const;
    const map<int, string>& getFdToPort() const;
    ServerConfiguration& getConfig();
    const vector<int>& getSockets() const;
    std::set<int> connectedClients;

private:
    int main_socket; // Renamed from 'socket' to 'main_socket'
    ServerConfiguration config;
    vector<int> sockets;
    map<int, string> fd_to_port;

    struct addrinfo* serverInfo(const string& port);
    void bindSocket(int sockFd, struct addrinfo *info, const std::string &port);
    void listenSocket(int sockFd,const string& port, int backlog);
    void setSocketOptions(int sockFd);
    void createSockets();
    void cleanup();
};

#endif // SERVER_HPP
