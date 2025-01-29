#pragma once

#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

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

#define URI_ALLOWED_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%"

struct Uri {
	std::string fullUri;
	std::string path;
	std::string fullPath;
	std::string query;
	std::string unparsedURI;
};

struct RequestMessage
{
	std::string method;
	Uri uri;
	std::map<std::string, std::string> headers;
};




class RequestParser
{
	public:
		std::string fullLocation;
		std::string query;
		ServerConfiguration ServerConfig;
		unsigned int contentLength;
		std::string upload_path;

		RequestParser();
		RequestMessage message;
		bool headersLoaded;
		Location location;
		// ~RequestParser();
		int	loadHeaders(Content &data);
		void parseRequest();
		void checkRequestLine(std::string& requestLine);
		void checkHeader(std::string &header);
		bool parseUri(const std::string& uriStr);  // returns true if location has a redirection
		std::string CGIpath;
		bool isCGIfile;
		bool isDir;
		void checkPath();

};

#endif //REQUEST_PARSER_HPP