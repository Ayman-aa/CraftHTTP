#include "includes/ServerConfiguration.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <sys/epoll.h>

using namespace std;

void handle_client(int client_fd, const ServerConfiguration& server) {
    char buffer[1024];
    int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        perror("recv");
        return;
    }
    buffer[bytes_received] = '\0';

    // Parse HTTP request (simplified)
    string request(buffer);
    istringstream request_stream(request);
    string method, path, version;
    request_stream >> method >> path >> version;

    // Determine file path
    string file_path = server.locations.at("/").root + path;
    if (file_path[file_path.size() - 1] == '/') {
        file_path += "index.html"; // Serve index.html for directory requests
    }

    // Read file content
    ifstream file(file_path.c_str());
    if (!file) {
        // File not found, send 404 response
        string response = "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found";
        send(client_fd, response.c_str(), response.size(), 0);
    } else {
        // File found, send 200 response with file content
        stringstream file_content;
        file_content << file.rdbuf();
        string content = file_content.str();
        stringstream ss;
        ss << content.size();
        string response = "HTTP/1.1 200 OK\r\nContent-Length: " + ss.str() + "\r\n\r\n" + content;
        send(client_fd, response.c_str(), response.size(), 0);
    }

    close(client_fd);
}

int main()
{
    ServerConfiguration server;
    server.ports.push_back("8080");
    server.ports.push_back("8081");
    server.host = "127.0.0.1";
    server.locations["/"].root = "/home/aaamam/Desktop/CraftHTTP"; // Correct root directory

    cout << "Server host: " << server.host << endl;
    cout << "Root of location /: " << server.locations["/"].root << endl;

    vector<int> sockets;
    map<int, string> fd_to_port;
    struct addrinfo hints, *response;
    int ok = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    for (size_t i = 0; i < server.ports.size(); ++i) {
        if (getaddrinfo(server.host.c_str(), server.ports[i].c_str(), &hints, &response) != 0) {
            perror("getaddrinfo");
            return 1;
        }

        int socketfd = socket(response->ai_family, response->ai_socktype, response->ai_protocol);
        if (socketfd == -1) {
            perror("socket");
            return 1;
        }

        if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &ok, sizeof(int)) == -1) {
            perror("setsockopt");
            return 1;
        }

        if (bind(socketfd, response->ai_addr, response->ai_addrlen) == -1) {
            perror("bind");
            return 1;
        }

        freeaddrinfo(response);

        if (listen(socketfd, 10) == -1) {
            perror("listen");
            return 1;
        }

        // Set socket to non-blocking mode
        int flags = fcntl(socketfd, F_GETFL, 0);
        if (flags == -1) {
            perror("fcntl");
            return 1;
        }
        if (fcntl(socketfd, F_SETFL, flags | O_NONBLOCK) == -1) {
            perror("fcntl");
            return 1;
        }

        sockets.push_back(socketfd);
        fd_to_port[socketfd] = server.ports[i];
        cout << "Server is listening on port " << server.ports[i] << endl;
    }

    // Create epoll instance
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        return 1;
    }

    // Add sockets to epoll instance
    struct epoll_event event;
    for (size_t i = 0; i < sockets.size(); ++i) {
        event.events = EPOLLIN;
        event.data.fd = sockets[i];
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockets[i], &event) == -1) {
            perror("epoll_ctl");
            return 1;
        }
    }

    // Event loop
    const int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS];
    while (true) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_events == -1) {
            perror("epoll_wait");
            return 1;
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

                cout << "Client connected on port " << fd_to_port[events[i].data.fd] << endl;
                handle_client(client_fd, server);
                cout << "Client disconnected" << endl;
            }
        }
    }

    for (size_t i = 0; i < sockets.size(); ++i) {
        close(sockets[i]);
    }

    close(epoll_fd);
    cout << "Server closed" << endl;

    return 0;
}