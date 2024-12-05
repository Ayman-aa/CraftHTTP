#include "includes/ServerConfiguration.hpp"
#include <iostream>
#include <string>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <sstream>

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
    if (file_path.back() == '/') {
        file_path += "index.html"; // Serve index.html for directory requests
    }

    // Read file content
    ifstream file(file_path);
    if (!file) {
        // File not found, send 404 response
        string response = "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found";
        send(client_fd, response.c_str(), response.size(), 0);
    } else {
        // File found, send 200 response with file content
        stringstream file_content;
        file_content << file.rdbuf();
        string content = file_content.str();
        string response = "HTTP/1.1 200 OK\r\nContent-Length: " + to_string(content.size()) + "\r\n\r\n" + content;
        send(client_fd, response.c_str(), response.size(), 0);
    }

    close(client_fd);
}

int main()
{
    ServerConfiguration server;
    server.port = "8080";
    server.host = "127.0.0.1";
    server.locations["/"].root = "/home/aaamam/Desktop/CraftHTTP"; // Correct root directory

    cout << "Server port: " << server.port << endl;
    cout << "Server host: " << server.host << endl;
    cout << "Root of location /: " << server.locations["/"].root << endl;

    int socketfd;
    struct addrinfo hints, *response;
    int ok = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(server.host.c_str(), server.port.c_str(), &hints, &response) != 0)
    {
        perror("getaddrinfo");
        return 1;
    }

    if ((socketfd = socket(response->ai_family, response->ai_socktype, response->ai_protocol)) == -1)
    {
        perror("socket");
        return 1;
    }

    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &ok, sizeof(int)) == -1)
    {
        perror("setsockopt");
        return 1;
    }

    if (bind(socketfd, response->ai_addr, response->ai_addrlen) == -1)
    {
        perror("bind");
        return 1;
    }

    freeaddrinfo(response);

    if (listen(socketfd, 10) == -1)
    {
        perror("listen");
        return 1;
    }

    cout << "Server is listening on port " << server.port << endl;

    while (true) {
        struct sockaddr_storage client_addr;
        socklen_t addr_size = sizeof(client_addr);
        int client_fd = accept(socketfd, (struct sockaddr *)&client_addr, &addr_size);
        
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        cout << "Client connected" << endl;
        handle_client(client_fd, server);
        cout << "Client disconnected" << endl;
    }

    close(socketfd);
    cout << "Server closed" << endl;

    return 0;
}