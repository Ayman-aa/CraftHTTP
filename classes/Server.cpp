#include "../includes/Server.hpp"


Server::Server(ServerConfiguration& config)
    : socket(-1), config(config), epoll_fd(-1)
{
    start();
    cout << "full path request: " << config.locations["/"].root << endl;
}

struct addrinfo* Server::serverInfo(const std::string& port)
{
    struct addrinfo hints;
    struct addrinfo* serverInfo;

    std::memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL, port.c_str(), &hints, &serverInfo);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
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
    if (listen(socketfd, 10) == -1) {
        perror("listen");
        close(socketfd);
        return;
    }
    std::cout << "Server is listening on port " << port << std::endl;
}

void Server::start()
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

    // Create epoll instance
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(1);
    }

    // Add sockets to epoll instance
    struct epoll_event event;
    for (size_t i = 0; i < sockets.size(); ++i) {
        event.events = EPOLLIN;
        event.data.fd = sockets[i];
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockets[i], &event) == -1) {
            perror("epoll_ctl");
            exit(1);
        }
    }

    std::cout << "Server started." << std::endl;
}

void Server::run()
{
    const int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS];

    while (true) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_events == -1) {
            perror("epoll_wait");
            exit(1);
        }

        for (int i = 0; i < num_events; ++i) {
            if (events[i].events & EPOLLIN) {
                struct sockaddr_storage client_addr;
                socklen_t addr_size = sizeof(client_addr);
                int client_fd = accept(events[i].data.fd, (struct sockaddr *)&client_addr, &addr_size);

                if (client_fd == -1) {
                    perror("accept");
                    continue;
                }

                std::cout << "Client connected on port " << fd_to_port[events[i].data.fd] << std::endl;
                handleClient(client_fd);
                std::cout << "Client disconnected" << std::endl;
            }
        }
    }

    for (size_t i = 0; i < sockets.size(); ++i) {
        close(sockets[i]);
    }

    close(epoll_fd);
    std::cout << "Server closed" << std::endl;
}

void Server::handleClient(int client_fd)
{
    char buffer[1024];
    int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        perror("recv");
        return;
    }
    buffer[bytes_received] = '\0';

    // Parse HTTP request (simplified)
    std::string request(buffer);
    std::istringstream request_stream(request);
    std::string method, path, version;
    request_stream >> method >> path >> version;

    std::cout << "Requested path: " << path << std::endl; // Log the requested path

    if (method == "GET") {
        // Handle GET request
        std::string file_path = config.locations.at("/").root + path;
        if (file_path[file_path.size() - 1] == '/') {
            file_path += "index.html"; // Serve index.html for directory requests
        }
        cout << "full path request: " << file_path << endl;   
        // Read file content in chunks
        std::ifstream file(file_path.c_str(), std::ios::binary);
        if (!file) {
            // File not found, serve custom 404 error page
            serveErrorPage(client_fd, "404");
        } else {
            // File found, send 200 response with file content
            std::string response = "HTTP/1.1 200 OK\r\n\r\n";
            send(client_fd, response.c_str(), response.size(), 0);

            char file_buffer[1024];
            while (file.read(file_buffer, sizeof(file_buffer)) || file.gcount() > 0) {
                send(client_fd, file_buffer, file.gcount(), 0);
            }
        }
    } else if (method == "POST") {
        // Block POST request, serve custom 405 error page
        serveErrorPage(client_fd, "405");
    } else {
        // Method not allowed, serve custom 405 error page
        serveErrorPage(client_fd, "405");
    }

    close(client_fd);
}

void Server::serveErrorPage(int client_fd, const std::string& error_code)
{
    std::map<std::string, std::string>::const_iterator it = config.errorPages.find(error_code);
    if (it == config.errorPages.end()) {
        // If custom error page not found, send default error message
        std::stringstream ss;
        ss << "HTTP/1.1 " << error_code << " Error\r\nContent-Length: " << 13 << "\r\n\r\n" << error_code << " Error";
        std::string default_message = ss.str();
        send(client_fd, default_message.c_str(), default_message.size(), 0);
    } else {
        // Custom error page found, send it
        std::string error_page_path = it->second;
        std::ifstream error_file(error_page_path.c_str());
        if (!error_file) {
            // If custom error page file not found, send default error message
            cout << "Error page not found" << endl;
            std::stringstream ss;
            ss << "HTTP/1.1 " << error_code << " Error\r\nContent-Length: " << 13 << "\r\n\r\n" << error_code << " Error";
            std::string default_message = ss.str();
            send(client_fd, default_message.c_str(), default_message.size(), 0);
        } else {
            // Custom error page file found, send it
            std::stringstream error_content;
            error_content << error_file.rdbuf();
            std::string content = error_content.str();
            std::stringstream ss;
            ss << content.size();
            std::string response = "HTTP/1.1 " + error_code + " Error\r\nContent-Length: " + ss.str() + "\r\n\r\n" + content;
            send(client_fd, response.c_str(), response.size(), 0);
        }
    }
}