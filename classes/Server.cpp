#include "../includes/Server.hpp"

Server::Server(ServerConfiguration* config)
    : main_socket(-1), config(config)
{
    try {
        createSockets();
        std::cout << "Server: Initialized successfully\n";
    } catch (const std::exception& e) {
        std::cerr << "Server initialization failed: " << e.what() << "\n";
        cleanup();
    }
}

Server::~Server() {
    cleanup();
    std::cout << "Server: Cleaned up resources\n";
}

int Server::getSocket() const {
    return main_socket;
}

const map<int, string>& Server::getFdToPort() const {
    return fd_to_port;
}

const ServerConfiguration& Server::getConfig() const {
    return config;
}

const vector<int>& Server::getSockets() const {
    return sockets;
}

void Server::createSockets() {
    for (size_t i = 0; i < config.ports.size(); ++i) {
        struct addrinfo* info = serverInfo(config.ports[i]);
        if (!info) {
            std::cerr << "Failed to get address info for port " << config.ports[i] << "\n";
            continue;
        }
        main_socket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        if (main_socket == -1) {
            std::cerr << "Error creating socket for port " << config.ports[i] << ": " << strerror(errno) << "\n";
            freeaddrinfo(info);
            continue;
        }
        try {
            bindSocket(main_socket, info, config.ports[i]);
            listenSocket(main_socket, config.ports[i]);
            sockets.push_back(main_socket);
            fd_to_port[main_socket] = config.ports[i];
        } catch (const std::exception& e) {
            std::cerr << "Error setting up socket for port " << config.ports[i] << ": " << e.what() << "\n";
            close(main_socket);
        }
        freeaddrinfo(info);
    }
}

void Server::cleanup() {
    for (size_t i = 0; i < sockets.size(); ++i) {
        close(sockets[i]);
    }
    sockets.clear();
}

struct addrinfo* Server::serverInfo(const string& port) {
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

void Server::bindSocket(int main_socket, struct addrinfo* serverInfo, const string& port) {
    int yes = 1;
    if (setsockopt(main_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        throw std::runtime_error("setsockopt error: " + std::string(strerror(errno)));
    }
    if (bind(main_socket, serverInfo->ai_addr, serverInfo->ai_addrlen) == -1) {
        throw std::runtime_error("bind error on port " + port + ": " + std::string(strerror(errno)));
    }
}

void Server::listenSocket(int socketfd, const string& port) {
    if (listen(socketfd, 10) == -1) {
        throw std::runtime_error("listen error on port " + port + ": " + std::string(strerror(errno)));
    }
    std::cout << "Server is listening on port " << port << "\n";
}