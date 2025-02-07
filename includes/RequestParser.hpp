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

/**
 * Structure representing the parsed components of a URI.
 */
struct Uri {
	std::string fullUri;      // The original decoded URI.
	std::string path;         // The path component of the URI.
	std::string fullPath;     // The full system path derived from the URI.
	std::string query;        // The query string, if any.
	std::string unparsedURI;  // The original URI before decoding.
};

/**
 * Structure representing an HTTP request message.
 */
struct RequestMessage
{
	std::string method;                               // HTTP method (GET, POST, DELETE, etc.).
	Uri uri;                                          // Parsed URI components.
	std::map<std::string, std::string> headers;       // Map of header fields and their values.
};

/**
 * RequestParser class
 *
 * Handles HTTP request parsing by processing the request line, headers,
 * and determining the effective request based on the server configuration.
 */
class RequestParser
{
	public:
		// The resolved full location based on the requested URI and server configuration.
		std::string fullLocation;
		
		// The query string extracted from the URI.
		std::string query;
		
		// Server configuration associated with the current request.
		ServerConfiguration ServerConfig;
		
		// Expected content length as specified by the Content-Length header.
		unsigned int contentLength;
		
		// The upload path determined from location settings (or default if not specified).
		std::string upload_path;

		/**
		 * Constructor initializing the RequestParser.
		 *
		 * Sets default values for header loading, CGI flags, and content length.
		 */
		RequestParser();
		
		// Parsed HTTP request message containing method, URI, and headers.
		RequestMessage message;
		
		// Flag indicating whether the HTTP headers have been completely loaded.
		bool headersLoaded;
		
		// The location configuration that matches the request URI.
		Location location;
		
		/**
		 * Loads HTTP headers from the provided content buffer.
		 *
		 * Trims leading newline characters, identifies the header/body delimiter,
		 * splits headers into individual lines, and processes each header.
		 *
		 * @param data The Content object containing raw request data.
		 * @return int Return code (typically 0 on success).
		 */
		int loadHeaders(Content &data);
		
		/**
		 * Parses the entire HTTP request.
		 *
		 * Validates the request method, ensures required headers exist,
		 * and verifies conformity with server configuration.
		 */
		void parseRequest();
		
		/**
		 * Validates and processes the request line.
		 *
		 * Extracts the HTTP method, the unparsed URI, and the HTTP version.
		 * Throws an error if the request line format is invalid.
		 *
		 * @param requestLine The raw request line string.
		 */
		void checkRequestLine(std::string& requestLine);
		
		/**
		 * Validates and processes a single HTTP header.
		 *
		 * Splits the header into key and value, trims whitespace,
		 * and adds it to the request message headers map.
		 *
		 * @param header The header string to process.
		 */
		void checkHeader(std::string &header);
		
		/**
		 * Parses the request URI.
		 *
		 * Extracts the path and query string, resolves the location using server configuration,
		 * sets up the upload path, and determines if the request targets a CGI script.
		 *
		 * @param uriStr The full URI string.
		 * @return true if there is a redirection specified in the location; false otherwise.
		 */
		bool parseUri(const std::string& uriStr);
		
		// The CGI interpreter path if the requested file requires CGI processing.
		std::string CGIpath;
		
		// Flag indicating if the requested file is a CGI script.
		bool isCGIfile;
		
		// Flag indicating if the resolved path is a directory.
		bool isDir;
		
		/**
		 * Checks the validity and accessibility of the requested path.
		 *
		 * Uses the filesystem to verify existence and read permissions,
		 * sets the directory flag if applicable, and throws an error if access is denied.
		 */
		void checkPath();

};

#endif //REQUEST_PARSER_HPP