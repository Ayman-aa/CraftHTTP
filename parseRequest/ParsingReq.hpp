#pragma once
#include "iostream"
#include "map"
#include "string"
#include "../config/config.hpp"
#include "../bits/Binary.hpp"
#include "../HttpError/HttpError.hpp"
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
	uint64_t contentLength;
	std::string upload_path;
    std::string cgi;

    ParsingReq();
    ServerConfig serverConfig;
    message msg;
    Location loc;

    int loadHeaders(Binary &data);
    void parseRequest();
    void parsRequestline(std::string &line);
    void parseHeaders(std::string &line);
    void checkPath();

    bool parseUri(const std::string& uriStr);
    bool haeaderGood;
    bool iscgi;
    bool isdir;
};