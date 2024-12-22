#include "../includes/Cluster.hpp"

Cluster::Cluster(ClusterConfiguration& configs)
    : config(configs)
{
    createEpoll();

    vector<ServerConfiguration*> serversConfigurations = config.servers;
    for (long unsigned int i = 0; i < serversConfigurations.size(); i++)
    {
        Server *server = new Server(serversConfigurations[i]);
        const vector<int>& serverSockets = server->getSockets();
        if (!serverSockets.empty()) {
            servers.push_back(server);
            for (size_t j = 0; j < serverSockets.size(); ++j) {
                addSocketToEpoll(serverSockets[j]);
                server_fd_to_server[serverSockets[j]] = server;
            }
        } else {
            cerr << "Error creating server socket\n";
            delete server;
        }
    }
}

Cluster::~Cluster() {
    cleanup();
}

void Cluster::createEpoll() {
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        cerr << "Error creating epoll instance\n";
        cleanup();
        exit(1);
    }
}

void Cluster::addSocketToEpoll(int fd) {
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR;
    event.data.fd = fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
        cerr << "Error adding socket to epoll" << endl;
        cleanup();
    }
}

void Cluster::run() {
    const int maxEvents = 1024;
    struct epoll_event events[maxEvents];

    while (true) {
        int numEvents = epoll_wait(epoll_fd, events, maxEvents, -1);
        if (numEvents == -1) {
            cerr << "Error in epoll_wait: " << strerror(errno) << "\n";
            break;
        }

        handleEvents(events, numEvents);
    }

    cleanup();
}

void Cluster::handleEvents(struct epoll_event* events, int numEvents) {
    for (int i = 0; i < numEvents; ++i) {
        int eventFd = events[i].data.fd;
        if (isServerFd(eventFd)) {
            acceptConnections(eventFd);
        } else {
            handleClient(eventFd);
        }
    }
}

void Cluster::acceptConnections(int serverSocket) {
    int clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == -1) {
        cerr << "Error accepting connection: " << strerror(errno) << "\n";
        return;
    }
    cout << "Server: Accepted new connection\n";

    addSocketToEpoll(clientSocket);
    client_to_server[clientSocket] = serverSocket;
}

void Cluster::handleClient(int client_fd)
{
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

    // Determine which server the client is connected to
    int server_fd = client_to_server[client_fd];
    Server* server = server_fd_to_server[server_fd];

    if (server == NULL) {
        cerr << "Server not found for client_fd: " << client_fd << endl;
        close(client_fd);
        return;
    }

    const ServerConfiguration& config = server->getConfig();

    // Find the appropriate location block
    map<string, Location>::const_iterator loc_it = config.locations.find(path);
    if (loc_it == config.locations.end()) {
        // If the exact path is not found, look for the root location
        loc_it = config.locations.find("/");
        if (loc_it == config.locations.end()) {
            // No location found, serve custom 404 error page
            serveErrorPage(client_fd, 404, server);
            close(client_fd);
            return;
        }
    }

    const Location& location = loc_it->second;

    if (method == "GET") {
        // Handle GET request
        string file_path = location.root + path;
        if (file_path[file_path.size() - 1] == '/') {
            file_path += "index.html"; // Serve index.html for directory requests
        }
        cout << "full path request: " << file_path << endl;   
        // Read file content in chunks
        ifstream file(file_path.c_str(), ios::binary);
        if (!file) {
            // File not found, serve custom 404 error page
            serveErrorPage(client_fd, 404, server);
        } else {
            // File found, send 200 response with file content
            string response = "HTTP/1.1 200 OK\r\n\r\n";
            send(client_fd, response.c_str(), response.size(), 0);

            char file_buffer[1024];
            while (file.read(file_buffer, sizeof(file_buffer)) || file.gcount() > 0) {
                send(client_fd, file_buffer, file.gcount(), 0);
            }
        }
    } else if (method == "POST") {
        // Block POST request, serve custom 405 error page
        serveErrorPage(client_fd, 405, server);
    } else {
        // Method not allowed, serve custom 405 error page
        serveErrorPage(client_fd, 405, server);
    }

    close(client_fd);
}

void Cluster::serveErrorPage(int client_fd, int error_code, Server* server)
{
    const ServerConfiguration& config = server->getConfig();
    map<int, string>::const_iterator it = config.errorPages.find(error_code);
    if (it == config.errorPages.end()) {
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
            cout << "Error page not found" << endl;
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
            stringstream response_stream;
            response_stream << "HTTP/1.1 " << error_code << " Error\r\nContent-Length: " << ss.str() << "\r\n\r\n" << content;
            string response = response_stream.str();
            send(client_fd, response.c_str(), response.size(), 0);
        }
    }
}
bool Cluster::isServerFd(int fd) {
    return server_fd_to_server.find(fd) != server_fd_to_server.end();
}

void Cluster::cleanup() {
    for (vector<Server*>::iterator it = servers.begin(); it != servers.end(); ++it) {
        delete *it;
    }
    servers.clear();

    if (epoll_fd != -1) {
        close(epoll_fd);
        epoll_fd = -1;
    }
}