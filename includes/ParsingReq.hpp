#pragma once

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <functional>
#include <stdexcept>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ctime>
#include <vector>
#include <map>
#include "tools.hpp"
#include "ConfigurationParser.hpp"
#include "ServerConfiguration.hpp"
#include "Content.hpp"
#include "HttpError.hpp"
#define VALID_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%"

struct uriE {
	std::string fullUri;
	std::string path;
	std::string fullPath;
	std::string query;
	std::string unparsedURI;
};
struct message
{
    std::string method;
    uriE uri;
    std::map<std::string, std::string> headers;
};

class ParsingReq
{
    public:
    std::string fullLocation;
    std::string query;
	uint32_t contentLength;
	std::string upload_path;
    std::string cgi;

    ParsingReq();
    ServerConfiguration serverConfig;
    message msg;
    Location loc;

    int loadHeaders(Content &data);
    void parseRequest();
    void parsRequestline(std::string &line);
    void parseHeaders(std::string &line);
    void checkPath();

    bool parseUri(const std::string& uriStr);
    bool haeaderGood;
    bool iscgi;
    bool isdir;
};