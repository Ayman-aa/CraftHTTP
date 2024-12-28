#include "../includes/ClientHandler.hpp"

ClientHandler::ClientHandler(int clientFd, int epollFd, ServerConfiguration& serverConfig, Configurations& configs)
{
    this->clientFd = clientFd;
    this->epollFd = epollFd;
    this->serverConfig = serverConfig;
    this->configs = configs;
}

void ClientHandler::recieveRequest()
{
    char buffer[1024];
    int bytes_received = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        perror("recv");
        return ;
    }
    
}