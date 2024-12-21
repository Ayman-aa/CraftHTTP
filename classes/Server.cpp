#include "../includes/Server.hpp"

Server::Server(ServerConfiguration& config)
    : main_socket(-1), config(config)
{
    createSockets();
}

Server::~Server() {
    cleanup();
}

int Server::getSocket() const {
    return main_socket;
}

const std::map<int, std::string>& Server::getFdToPort() const {
    return fd_to_port;
}

const ServerConfiguration& Server::getConfig() const {
    return config;
}

const std::vector<int>& Server::getSockets() const {
    return sockets;
}

void Server::createSockets() {
    for (size_t i = 0; i < config.ports.size(); ++i) {
        struct addrinfo* info = serverInfo(config.ports[i]);
        if (info) {
            int sockfd = ::socket(info->ai_family, info->ai_socktype, info->ai_protocol); // Use ::socket to avoid conflict
            if (sockfd == -1) {
                std::cerr << "Error creating socket for port " << config.ports[i] << "\n";
                continue;
            }
            bindSocket(sockfd, info, config.ports[i]);
            listenSocket(sockfd, config.ports[i]);
            sockets.push_back(sockfd);
            fd_to_port[sockfd] = config.ports[i];
            freeaddrinfo(info);
        }
    }
}

void Server::cleanup() {
    for (size_t i = 0; i < sockets.size(); ++i) {
        close(sockets[i]);
    }
    sockets.clear();
}

struct addrinfo* Server::serverInfo(const std::string& port) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(config.host.c_str(), port.c_str(), &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << "\n";
        return NULL;
    }
    return res;
}

void Server::bindSocket(int sockfd, struct addrinfo* serverInfo, const std::string& port) {
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        std::cerr << "setsockopt error\n";
        return;
    }
    if (bind(sockfd, serverInfo->ai_addr, serverInfo->ai_addrlen) == -1) {
        close(sockfd);
        std::cerr << "bind error on port " << port << "\n";
        return;
    }
}

void Server::listenSocket(int socketfd, const std::string& port) {
    if (listen(socketfd, 10) == -1) {
        std::cerr << "listen error on port " << port << "\n";
        return;
    }
    std::cout << "Server is listening on port " << port << "\n";
}