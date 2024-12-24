#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

class FtCurl {
private:
    int sockfd;
    std::string host;
    std::string path;
    int port;

    bool parseUrl(const std::string& url) {
        size_t protocolEnd = url.find("://");
        if (protocolEnd == std::string::npos) return false;
        
        size_t hostStart = protocolEnd + 3;
        size_t pathStart = url.find("/", hostStart);
        
        if (pathStart == std::string::npos) {
            host = url.substr(hostStart);
            path = "/";
        } else {
            host = url.substr(hostStart, pathStart - hostStart);
            path = url.substr(pathStart);
        }
        
        size_t portStart = host.find(":");
        if (portStart != std::string::npos) {
            port = atoi(host.substr(portStart + 1).c_str());
            host = host.substr(0, portStart);
        } else {
            port = 80;
        }
        
        return true;
    }

public:
    FtCurl() : sockfd(-1), port(80) {}
    
    bool get(const std::string& url) {
        if (!parseUrl(url)) {
            std::cerr << "Invalid URL format" << std::endl;
            return false;
        }

        struct hostent* server = gethostbyname(host.c_str());
        if (server == NULL) {
            std::cerr << "Error resolving hostname" << std::endl;
            return false;
        }

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            std::cerr << "Error creating socket" << std::endl;
            return false;
        }

        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
        serv_addr.sin_port = htons(port);

        if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            std::cerr << "Error connecting to server" << std::endl;
            close(sockfd);
            return false;
        }

        std::string request = "GET " + path + " HTTP/1.1\r\n"
                             "Host: " + host + "\r\n"
                             "Connection: close\r\n\r\n";

        if (write(sockfd, request.c_str(), request.length()) < 0) {
            std::cerr << "Error sending request" << std::endl;
            close(sockfd);
            return false;
        }

        char buffer[4096];
        int bytes;
        while ((bytes = read(sockfd, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes] = '\0';
            std::cout << buffer;
        }

        close(sockfd);
        return true;
    }

    ~FtCurl() {
        if (sockfd != -1) {
            close(sockfd);
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <URL>" << std::endl;
        return 1;
    }

    FtCurl curl;
    return curl.get(argv[1]) ? 0 : 1;
}
