#include "../includes/Server.hpp"


Server::Server(ServerConfiguration& config)
    : socket(-1), config(config)
{
    createSockets();
    std::cout << "full path request: " << config.locations["/"].root << std::endl;
}

Server::~Server() {
    cleanup();
    std::cout << "Server: Cleaned up resources\n";
}

int Server::getSocket() const {
    return socket;
}

const std::map<int, std::string>& Server::getFdToPort() const {
    return fd_to_port;
}

struct addrinfo* Server::serverInfo(const std::string& port)
{
    struct addrinfo hints;
    struct addrinfo* serverInfo;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL, port.c_str(), &hints, &serverInfo);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        cleanup();
        return NULL;
    }

    return serverInfo;
}

void Server::bindSocket(struct addrinfo* serverInfo, const std::string& port)
{
    int socketfd = ::socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
    if (socketfd == -1) {
        perror("socket");
        return;
    }

    int yes = 1;
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        close(socketfd);
        return;
    }

    if (bind(socketfd, serverInfo->ai_addr, serverInfo->ai_addrlen) == -1) {
        perror("bind");
        close(socketfd);
        return;
    }

    sockets.push_back(socketfd);
    fd_to_port[socketfd] = port;
}

void Server::listenSocket(int socketfd, const std::string& port)
{
    if (listen(socketfd, SOMAXCONN) == -1) {
        perror("listen");
        close(socketfd);
        return;
    }
    std::cout << "Server is listening on port " << port << std::endl;
}

void Server::createSockets()
{
    std::cout << "Server starting..." << std::endl;
    for (size_t i = 0; i < config.ports.size(); ++i) {
        struct addrinfo* serverInfo = this->serverInfo(config.ports[i]);
        if (serverInfo) {
            this->bindSocket(serverInfo, config.ports[i]);
            freeaddrinfo(serverInfo);
        }
    }

    for (size_t i = 0; i < sockets.size(); ++i) {
        this->listenSocket(sockets[i], fd_to_port[sockets[i]]);
    }

    if (!sockets.empty()) {
        socket = sockets[0];
    }

    std::cout << "Server started." << std::endl;
}

void Server::cleanup()
{
    for (size_t i = 0; i < sockets.size(); ++i) {
        close(sockets[i]);
    }
    sockets.clear();
    fd_to_port.clear();
    std::cout << "Server cleaned up." << std::endl;
}