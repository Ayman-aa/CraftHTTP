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
#include <sys/epoll.h>

using namespace std;

void serve_error_page(int client_fd, const ServerConfiguration& server, const string& error_code) {
    map<string, string>::const_iterator it = server.errorPages.find(error_code);
    if (it == server.errorPages.end()) {
        // If custom error page not found, send default error message
        stringstream ss;
        ss << "HTTP/1.1 " << error_code << " Error\r\nContent-Length: " << 13 << "\r\n\r\n" << error_code << " Error";
        string default_message = ss.str();
        send(client_fd, default_message.c_str(), default_message.size(), 0);
    } else {
        // Custom error page found, send it
        string error_page_path = it->second;
        ifstream error_file(error_page_path.c_str());
        if (!error_file) {
            // If custom error page file not found, send default error message
            stringstream ss;
            ss << "HTTP/1.1 " << error_code << " Error\r\nContent-Length: " << 13 << "\r\n\r\n" << error_code << " Error";
            string default_message = ss.str();
            send(client_fd, default_message.c_str(), default_message.size(), 0);
        } else {
            // Custom error page file found, send it
            stringstream error_content;
            error_content << error_file.rdbuf();
            string content = error_content.str();
            stringstream ss;
            ss << content.size();
            string response = "HTTP/1.1 " + error_code + " Error\r\nContent-Length: " + ss.str() + "\r\n\r\n" + content;
            send(client_fd, response.c_str(), response.size(), 0);
        }
    }
}

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

    cout << "Requested path: " << path << endl; // Log the requested path

    if (method == "GET") {
        // Handle GET request
        string file_path = server.locations.at("/").root + path;
        if (file_path[file_path.size() - 1] == '/') {
            file_path += "/index.html"; // Serve index.html for directory requests
        }

        // Read file content in chunks
        ifstream file(file_path.c_str(), ios::binary);
        if (!file) {
            // File not found, serve custom 404 error page
            serve_error_page(client_fd, server, "404");
        } else {
            // File found, send 200 response with file content
            string response = "HTTP/1.1 200 OK\r\n\r\n";
            send(client_fd, response.c_str(), response.size(), 0);

            char file_buffer[1024];
            while (file.read(file_buffer, sizeof(file_buffer)) || file.gcount() > 0) {
                //cout << "Sending " << file.gcount() << " bytes" << endl;
                //cout << "File buffer: " << file_buffer << endl;
                send(client_fd, file_buffer, file.gcount(), 0);
            }
        }
    } else if (method == "POST") {
        // Block POST request, serve custom 405 error page
        serve_error_page(client_fd, server, "405");
    } else {
        // Method not allowed, serve custom 405 error page
        serve_error_page(client_fd, server, "405");
    }

    close(client_fd);
}

int main() {
    ServerConfiguration server;
    server.ports.push_back("8080");
    server.ports.push_back("8081");
    server.host = "127.0.0.1";
    server.locations["/"].root = "/home/aaamam/Desktop/CraftHTTP/assets"; // Correct root directory

    // Define custom error pages
    server.errorPages["404"] = "/home/aaamam/Desktop/CraftHTTP/errorPages/404.html";
    server.errorPages["405"] = "/home/aaamam/Desktop/CraftHTTP/errorPages/405.html";

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