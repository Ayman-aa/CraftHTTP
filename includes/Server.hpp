#ifndef SERVER_HPP
#define SERVER_HPP

#include "ServerConfiguration.hpp"

class Server {
public:
    Server(ServerConfiguration* config);
    virtual ~Server();
    int getSocket() const;
    const map<int, string>& getFdToPort() const;
    const ServerConfiguration& getConfig() const;
    const vector<int>& getSockets() const;

private:
    int main_socket; // Renamed from 'socket' to 'main_socket'
    ServerConfiguration config;
    vector<int> sockets;
    map<int, string> fd_to_port;

    struct addrinfo* serverInfo(const string& port);
    void bindSocket(int sockfd, struct addrinfo* serverInfo,const string& port);
    void listenSocket(int socketfd,const string& port);
    void createSockets();
    void cleanup();
};

#endif // SERVER_HPP